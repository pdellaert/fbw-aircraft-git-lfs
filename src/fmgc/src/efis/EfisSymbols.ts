// Copyright (c) 2021-2022 FlyByWire Simulations
// Copyright (c) 2021-2022 Synaptic Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { FlightPlanManager } from '@fmgc/flightplanning/FlightPlanManager';
import { EfisOption, Mode, NdSymbol, NdSymbolTypeFlags, RangeSetting, rangeSettings } from '@shared/NavigationDisplay';
import { GuidanceManager } from '@fmgc/guidance/GuidanceManager';
import { Coordinates } from '@fmgc/flightplanning/data/geo';
import { Geometry } from '@fmgc/guidance/Geometry';
import { GuidanceController } from '@fmgc/guidance/GuidanceController';
import { PathVector, PathVectorType } from '@fmgc/guidance/lnav/PathVector';
import { distanceTo } from 'msfs-geo';
import { FlowEventSync } from '@shared/FlowEventSync';
import { LnavConfig } from '@fmgc/guidance/LnavConfig';
import { FlightPlanService } from '@fmgc/flightplanning/new/FlightPlanService';
import { Airport, AltitudeDescriptor, LegType, Runway, WaypointDescriptor } from 'msfs-navdata';
import { MathUtils } from '@shared/MathUtils';
import { SegmentClass } from '@fmgc/flightplanning/new/segments/SegmentClass';
import { NavigationDatabase } from '@fmgc/NavigationDatabase';
import { RunwaySurface, VorType } from '../types/fstypes/FSEnums';
import { NearbyFacilities } from './NearbyFacilities';

export class EfisSymbols {
    private blockUpdate = false;

    private guidanceController: GuidanceController;

    private guidanceManager: GuidanceManager;

    private nearby: NearbyFacilities;

    private syncer: FlowEventSync = new FlowEventSync();

    private static sides = ['L', 'R'];

    private lastMode = { L: -1, R: -1 };

    private lastRange = { L: 0, R: 0 };

    private lastEfisOption = { L: 0, R: 0 };

    private lastPlanCentre = undefined;

    private lastPpos: Coordinates = { lat: 0, long: 0 };

    private lastTrueHeading: number = -1;

    private lastNearbyFacilitiesVersion;

    private lastFpVersion;

    constructor(flightPlanManager: FlightPlanManager, guidanceController: GuidanceController) {
        this.guidanceController = guidanceController;
        this.guidanceManager = guidanceController.guidanceManager;
        this.nearby = new NearbyFacilities();
    }

    init(): void {
        this.nearby.init();
    }

    async update(deltaTime: number): Promise<void> {
        this.nearby.update(deltaTime);

        if (this.blockUpdate) {
            return;
        }

        // TODO use FMGC position
        const ppos = {
            lat: SimVar.GetSimVarValue('PLANE LATITUDE', 'degree latitude'),
            long: SimVar.GetSimVarValue('PLANE LONGITUDE', 'degree longitude'),
        };
        const trueHeading = SimVar.GetSimVarValue('PLANE HEADING DEGREES TRUE', 'degrees');

        const pposChanged = Avionics.Utils.computeDistance(this.lastPpos, ppos) > 2;
        if (pposChanged) {
            this.lastPpos = ppos;
        }
        const trueHeadingChanged = Avionics.Utils.diffAngle(trueHeading, this.lastTrueHeading) > 2;
        if (trueHeadingChanged) {
            this.lastTrueHeading = trueHeading;
        }

        const nearbyFacilitiesChanged = this.nearby.version !== this.lastNearbyFacilitiesVersion;
        this.lastNearbyFacilitiesVersion = this.nearby.version;
        const fpChanged = this.lastFpVersion !== FlightPlanService.version;
        this.lastFpVersion = FlightPlanService.version;
        // FIXME map reference point should be per side
        const planCentreIndex = SimVar.GetSimVarValue('L:A32NX_SELECTED_WAYPOINT', 'number');

        if (!FlightPlanService.active.hasElement(planCentreIndex)) {
            return;
        }

        let planCentre = FlightPlanService.active.elementAt(planCentreIndex);

        if (planCentre?.isDiscontinuity === true) {
            planCentre = FlightPlanService.active.elementAt(Math.max(0, (planCentreIndex - 1)));
        }

        if (planCentre?.isDiscontinuity === true) {
            throw new Error('bruh');
        }

        const termination = planCentre?.terminationWaypoint()?.location;

        if (termination) {
            this.lastPlanCentre = termination;
        }

        const planCentreChanged = termination?.lat !== this.lastPlanCentre?.lat || termination?.long !== this.lastPlanCentre?.long;

        const activeFp = FlightPlanService.active;
        // TODO temp f-pln

        const hasSuitableRunway = (airport: RawAirport): boolean => {
            for (const runway of airport.runways) {
                switch (runway.surface) {
                case RunwaySurface.Asphalt:
                case RunwaySurface.Bituminous:
                case RunwaySurface.Concrete:
                case RunwaySurface.Tarmac:
                    if (runway.length >= 1500 && runway.width >= 30) {
                        return true;
                    }
                    break;
                default:
                    break;
                }
            }
            return false;
        };

        for (const side of EfisSymbols.sides) {
            const range = rangeSettings[SimVar.GetSimVarValue(`L:A32NX_EFIS_${side}_ND_RANGE`, 'number')];
            const mode: Mode = SimVar.GetSimVarValue(`L:A32NX_EFIS_${side}_ND_MODE`, 'number');
            const efisOption = SimVar.GetSimVarValue(`L:A32NX_EFIS_${side}_OPTION`, 'Enum');

            const rangeChange = this.lastRange[side] !== range;
            this.lastRange[side] = range;
            const modeChange = this.lastMode[side] !== mode;
            this.lastMode[side] = mode;
            const efisOptionChange = this.lastEfisOption[side] !== efisOption;
            this.lastEfisOption[side] = efisOption;
            const nearbyOverlayChanged = efisOption !== EfisOption.Constraints && efisOption !== EfisOption.None && nearbyFacilitiesChanged;

            if (!pposChanged && !trueHeadingChanged && !rangeChange && !modeChange && !efisOptionChange && !nearbyOverlayChanged && !fpChanged && !planCentreChanged) {
                continue;
            }

            if (mode === Mode.PLAN && !planCentre) {
                this.syncer.sendEvent(`A32NX_EFIS_${side}_SYMBOLS`, []);
                return;
            }

            const [editAhead, editBehind, editBeside] = this.calculateEditArea(range, mode);

            // eslint-disable-next-line no-loop-func
            const withinEditArea = (ll): boolean => {
                // FIXME
                if (!termination) {
                    return true;
                }

                const dist = Avionics.Utils.computeGreatCircleDistance(mode === Mode.PLAN ? termination : ppos, ll);
                let bearing = Avionics.Utils.computeGreatCircleHeading(mode === Mode.PLAN ? termination : ppos, ll);
                if (mode !== Mode.PLAN) {
                    bearing = Avionics.Utils.clampAngle(bearing - trueHeading);
                }
                bearing = bearing * Math.PI / 180;
                const dx = dist * Math.sin(bearing);
                const dy = dist * Math.cos(bearing);
                return Math.abs(dx) < editBeside && dy > -editBehind && dy < editAhead;
            };

            const symbols: NdSymbol[] = [];

            // symbols most recently inserted always end up at the end of the array
            // we reverse the array at the end to make sure symbols are drawn in the correct order
            // eslint-disable-next-line no-loop-func
            const upsertSymbol = (symbol: NdSymbol): void => {
                if (DEBUG) {
                    console.time(`upsert symbol ${symbol.databaseId}`);
                }
                const symbolIdx = symbols.findIndex((s) => s.databaseId === symbol.databaseId);
                if (symbolIdx !== -1) {
                    const oldSymbol = symbols.splice(symbolIdx, 1)[0];
                    symbol.constraints = symbol.constraints ?? oldSymbol.constraints;
                    symbol.direction = symbol.direction ?? oldSymbol.direction;
                    symbol.length = symbol.length ?? oldSymbol.length;
                    symbol.location = symbol.location ?? oldSymbol.location;
                    symbol.type |= oldSymbol.type;
                    if (oldSymbol.radials) {
                        if (symbol.radials) {
                            symbol.radials.push(...oldSymbol.radials);
                        } else {
                            symbol.radials = oldSymbol.radials;
                        }
                    }
                    if (oldSymbol.radii) {
                        if (symbol.radii) {
                            symbol.radii.push(...oldSymbol.radii);
                        } else {
                            symbol.radii = oldSymbol.radii;
                        }
                    }
                }
                symbols.push(symbol);
            };

            // TODO ADIRs aligned (except in plan mode...?)
            if (efisOption === EfisOption.VorDmes) {
                for (const vor of this.nearby.nearbyVhfNavaids.values()) {
                    if (vor.type !== VorType.VORDME && vor.type !== VorType.VOR && vor.type !== VorType.DME && vor.type !== VorType.VORTAC && vor.type !== VorType.TACAN) {
                        continue;
                    }
                    const ll = { lat: vor.lat, long: vor.lon };
                    if (withinEditArea(ll)) {
                        upsertSymbol({
                            databaseId: vor.icao,
                            ident: vor.icao.substring(7, 12),
                            location: ll,
                            type: this.vorDmeTypeFlag(vor.type) | NdSymbolTypeFlags.EfisOption,
                        });
                    }
                }
            } else if (efisOption === EfisOption.Ndbs) {
                for (const ndb of this.nearby.nearbyNdbNavaids.values()) {
                    const ll = { lat: ndb.lat, long: ndb.lon };
                    if (withinEditArea(ll)) {
                        upsertSymbol({
                            databaseId: ndb.icao,
                            ident: ndb.icao.substring(7, 12),
                            location: ll,
                            type: NdSymbolTypeFlags.Ndb | NdSymbolTypeFlags.EfisOption,
                        });
                    }
                }
            } else if (efisOption === EfisOption.Airports) {
                for (const ap of this.nearby.nearbyAirports.values()) {
                    const ll = { lat: ap.lat, long: ap.lon };
                    if (withinEditArea(ll) && hasSuitableRunway(ap)) {
                        upsertSymbol({
                            databaseId: ap.icao,
                            ident: ap.icao.substring(7, 12),
                            location: ll,
                            type: NdSymbolTypeFlags.Airport | NdSymbolTypeFlags.EfisOption,
                        });
                    }
                }
            } else if (efisOption === EfisOption.Waypoints) {
                for (const wp of this.nearby.nearbyWaypoints.values()) {
                    const ll = { lat: wp.lat, long: wp.lon };
                    if (withinEditArea(ll)) {
                        upsertSymbol({
                            databaseId: wp.icao,
                            ident: wp.icao.substring(7, 12),
                            location: ll,
                            type: NdSymbolTypeFlags.Waypoint | NdSymbolTypeFlags.EfisOption,
                        });
                    }
                }
            }

            // TODO port over
            // for (let i = 0; i < 4; i++) {
            //     const fixInfo = this.flightPlanManager.getFixInfo(i as 0 | 1 | 2 | 3);
            //     const refFix = fixInfo?.getRefFix();
            //     if (refFix !== undefined) {
            //         upsertSymbol({
            //             databaseId: refFix.icao,
            //             ident: refFix.ident,
            //             location: refFix.infos.coordinates,
            //             type: NdSymbolTypeFlags.FixInfo,
            //             radials: fixInfo.getRadialTrueBearings(),
            //             radii: [fixInfo.getRadiusValue()],
            //         });
            //     }
            // }

            const formatConstraintAlt = (alt: number, descent: boolean, prefix: string = '') => {
                // const transAlt = activeFp?.originTransitionAltitudePilot ?? activeFp?.originTransitionAltitudeDb;
                // const transFl = activeFp?.destinationTransitionLevelPilot ?? activeFp?.destinationTransitionLevelDb;
                const transAlt = 18_000;
                const transFl = 180;

                if (descent) {
                    const fl = Math.round(alt / 100);
                    if (transFl && fl >= transFl) {
                        return `${prefix}FL${fl}`;
                    }
                } else if (transAlt && alt >= transAlt) {
                    return `${prefix}FL${Math.round(alt / 100)}`;
                }
                return `${prefix}${Math.round(alt)}`;
            };

            const formatConstraintSpeed = (speed: number, prefix: string = '') => `${prefix}${Math.floor(speed)}KT`;

            for (const [index, leg] of this.guidanceController.activeGeometry.legs.entries()) {
                if (!leg.isNull && leg.terminationWaypoint && leg.displayedOnMap) {
                    if (!('location' in leg.terminationWaypoint)) {
                        const isActive = index === this.guidanceController.activeLegIndex;

                        let type = NdSymbolTypeFlags.FlightPlan;

                        if (isActive) {
                            type |= NdSymbolTypeFlags.ActiveLegTermination;
                        }

                        if (leg.metadata.isInMissedApproach) {
                            type |= NdSymbolTypeFlags.MissedApproach;
                        }

                        const ident = leg.ident;
                        const cutIdent = leg.ident.substring(0, 4).padEnd(5, ' ');
                        const id = (Math.random() * 10_000_000).toString().substring(0, 5);

                        upsertSymbol({
                            databaseId: `X${id}${cutIdent}`,
                            ident,
                            type,
                            location: leg.terminationWaypoint,
                        });
                    }
                }
            }

            // TODO don't send the waypoint before active once FP sequencing is properly implemented
            // (currently sequences with guidance which is too early)
            // eslint-disable-next-line no-lone-blocks
            {
                for (let i = activeFp.legCount - 1; i >= (activeFp.activeLegIndex - 1) && i >= 0; i--) {
                    const wp = activeFp.elementAt(i);

                    if (wp.isDiscontinuity === true) {
                        continue;
                    }

                    // Managed by legs
                    // FIXME these should integrate with the normal algorithms to pick up contraints, not be drawn in enroute ranges, etc.
                    const legType = wp.type;
                    if (
                        legType === LegType.CA || legType === LegType.CR || legType === LegType.CI
                        || legType === LegType.FM || legType === LegType.PI
                        || legType === LegType.VA || legType === LegType.VI || legType === LegType.VM
                    ) {
                        continue;
                    }

                    if (wp.definition.waypointDescriptor === WaypointDescriptor.Airport || wp.definition.waypointDescriptor === WaypointDescriptor.Runway) {
                        // we pick these up later
                        continue;
                    }

                    // if range >= 160, don't include terminal waypoints, except at enroute boundary
                    // TODO port over
                    // if (range >= 160) {
                    //     const segment = activeFp.findSegmentByWaypointIndex(i);
                    //     if (segment.type === SegmentType.Departure) {
                    //         // keep the last waypoint from the SID as it is the enroute boundary
                    //         if (!activeFp.isLastWaypointInSegment(i)) {
                    //             continue;
                    //         }
                    //     } else if (segment.type !== SegmentType.Enroute) {
                    //         continue;
                    //     }
                    // }

                    if ((!wp.isXF() && !wp.isHX()) || !withinEditArea(wp.terminationWaypoint().location)) {
                        continue;
                    }

                    let type = NdSymbolTypeFlags.FlightPlan;
                    const constraints = [];
                    let direction;

                    const isCourseReversal = wp.type === LegType.HA
                        || wp.type === LegType.HF
                        || wp.type === LegType.HM
                        || wp.additionalData.legType === LegType.PI;

                    if (i === activeFp.activeLegIndex) {
                        type |= NdSymbolTypeFlags.ActiveLegTermination;
                    } else if (isCourseReversal && i > (activeFp.activeLegIndex + 1) && range <= 80 && !LnavConfig.DEBUG_FORCE_INCLUDE_COURSE_REVERSAL_VECTORS) {
                        if (wp.definition.turnDirection === 'L') {
                            type |= NdSymbolTypeFlags.CourseReversalLeft;
                        } else {
                            type |= NdSymbolTypeFlags.CourseReversalRight;
                        }
                        direction = wp.definition.magneticCourse; // TODO true
                    }

                    if (i >= activeFp.firstMissedApproachLeg) {
                        type |= NdSymbolTypeFlags.MissedApproach;
                    }

                    if (wp.definition.altitudeDescriptor > 0 && wp.definition.altitudeDescriptor < 6) {
                    // TODO vnav to predict
                        type |= NdSymbolTypeFlags.ConstraintUnknown;
                    }

                    if (efisOption === EfisOption.Constraints) {
                        const descent = wp.segment.class === SegmentClass.Arrival;
                        switch (wp.definition.altitudeDescriptor) {
                        case AltitudeDescriptor.AtAlt1:
                            constraints.push(formatConstraintAlt(wp.definition.altitude1, descent));
                            break;
                        case AltitudeDescriptor.AtOrAboveAlt1:
                            constraints.push(formatConstraintAlt(wp.definition.altitude1, descent, '+'));
                            break;
                        case AltitudeDescriptor.AtOrBelowAlt1:
                            constraints.push(formatConstraintAlt(wp.definition.altitude1, descent, '-'));
                            break;
                        case AltitudeDescriptor.BetweenAlt1Alt2:
                            constraints.push(formatConstraintAlt(wp.definition.altitude1, descent, '-'));
                            constraints.push(formatConstraintAlt(wp.definition.altitude2, descent, '+'));
                            break;
                        default:
                            // FIXME do the rest
                            break;
                        }

                        if (wp.definition.speed > 0) {
                            constraints.push(formatConstraintSpeed(wp.definition.speed));
                        }
                    }

                    upsertSymbol({
                        databaseId: wp.terminationWaypoint().databaseId,
                        ident: wp.ident,
                        location: wp.terminationWaypoint().location,
                        type,
                        constraints: constraints.length > 0 ? constraints : undefined,
                        direction,
                    });
                }
            }

            const airports: [Airport, Runway][] = [
                [activeFp.originAirport, activeFp.originRunway],
                [activeFp.destinationAirport, activeFp.destinationRunway],
            ];
            for (const [airport, runway] of airports) {
                if (!airport) {
                    continue;
                }
                if (runway) {
                    if (withinEditArea(runway.thresholdLocation)) {
                        upsertSymbol({
                            databaseId: airport.databaseId,
                            ident: NavigationDatabase.formatLongRunwayIdent(airport.ident, runway.ident),
                            location: runway.thresholdLocation,
                            direction: runway.bearing,
                            length: runway.length / MathUtils.DIV_METRES_TO_NAUTICAL_MILES,
                            type: NdSymbolTypeFlags.Runway,
                        });
                    }
                } else if (withinEditArea(airport.location)) {
                    upsertSymbol({
                        databaseId: airport.databaseId,
                        ident: airport.ident,
                        location: airport.location,
                        type: NdSymbolTypeFlags.Airport,
                    });
                }
            }

            // Pseudo waypoints

            for (const pwp of this.guidanceController.currentPseudoWaypoints.filter((it) => it)) {
                upsertSymbol({
                    databaseId: `W      ${pwp.ident}`,
                    ident: pwp.ident,
                    location: pwp.efisSymbolLla,
                    type: pwp.efisSymbolFlag,
                });
            }

            const wordsPerSymbol = 6;
            const maxSymbols = 640 / wordsPerSymbol;
            if (symbols.length > maxSymbols) {
                symbols.splice(0, symbols.length - maxSymbols);
                this.guidanceController.efisStateForSide[side].dataLimitReached = true;
            } else {
                this.guidanceController.efisStateForSide[side].dataLimitReached = false;
            }

            this.syncer.sendEvent(`A32NX_EFIS_${side}_SYMBOLS`, symbols);

            // make sure we don't run too often
            this.blockUpdate = true;
            setTimeout(() => {
                this.blockUpdate = false;
            }, 200);
        }
    }

    private generatePathVectorSymbol(vector: PathVector): NdSymbol {
        let typeVectorPart: number;
        if (vector.type === PathVectorType.Line) {
            typeVectorPart = NdSymbolTypeFlags.FlightPlanVectorLine;
        } else if (vector.type === PathVectorType.Arc) {
            typeVectorPart = NdSymbolTypeFlags.FlightPlanVectorArc;
        } else if (vector.type === PathVectorType.DebugPoint) {
            typeVectorPart = NdSymbolTypeFlags.FlightPlanVectorDebugPoint;
        }

        // FIXME https://cdn.discordapp.com/attachments/845070631644430359/911876826169741342/brabs.gif
        const id = Math.round(Math.random() * 10_000).toString();

        const symbol: NdSymbol = {
            databaseId: id,
            ident: vector.type === PathVectorType.DebugPoint ? vector.annotation : id,
            type: NdSymbolTypeFlags.ActiveFlightPlanVector | typeVectorPart,
            location: vector.startPoint,
        };

        if (vector.type === PathVectorType.Line) {
            symbol.lineEnd = vector.endPoint;
        }

        if (vector.type === PathVectorType.Arc) {
            symbol.arcEnd = vector.endPoint;
            symbol.arcRadius = distanceTo(vector.startPoint, vector.centrePoint);
            symbol.arcSweepAngle = vector.sweepAngle;
        }

        return symbol;
    }

    private vorDmeTypeFlag(type: VorType): NdSymbolTypeFlags {
        switch (type) {
        case VorType.VORDME:
        case VorType.VORTAC:
            return NdSymbolTypeFlags.VorDme;
        case VorType.VOR:
            return NdSymbolTypeFlags.Vor;
        case VorType.DME:
        case VorType.TACAN:
            return NdSymbolTypeFlags.Dme;
        default:
            return 0;
        }
    }

    private findPointFromEndOfPath(path: Geometry, distanceFromEnd: NauticalMiles): Coordinates | undefined {
        let accumulator = 0;

        // FIXME take transitions into account on newer FMSs
        for (const [, leg] of path.legs) {
            accumulator += leg.distance;

            if (accumulator > distanceFromEnd) {
                const distanceFromEndOfLeg = distanceFromEnd - (accumulator - leg.distance);

                return leg.getPseudoWaypointLocation(distanceFromEndOfLeg);
            }
        }

        // console.error(`[VNAV/findPointFromEndOfPath] ${distanceFromEnd.toFixed(2)}nm is larger than the total lateral path.`);

        return undefined;
    }

    private calculateEditArea(range: RangeSetting, mode: Mode): [number, number, number] {
        switch (mode) {
        case Mode.ARC:
            if (range <= 10) {
                return [10.5, 3.5, 8.3];
            }
            if (range <= 20) {
                return [20.5, 7, 16.6];
            }
            if (range <= 40) {
                return [40.5, 14, 33.2];
            }
            if (range <= 80) {
                return [80.5, 28, 66.4];
            }
            if (range <= 160) {
                return [160.5, 56, 132.8];
            }
            return [320.5, 112, 265.6];
        case Mode.ROSE_NAV:
            if (range <= 10) {
                return [7.6, 7.1, 7.1];
            }
            if (range <= 20) {
                return [14.7, 14.2, 14.2];
            }
            if (range <= 40) {
                return [28.9, 28.4, 28.4];
            }
            if (range <= 80) {
                return [57.3, 56.8, 56.8];
            }
            if (range <= 160) {
                return [114.1, 113.6, 113.6];
            }
            return [227.7, 227.2, 227.2];
        case Mode.PLAN:
            if (range <= 10) {
                return [7, 7, 7];
            }
            if (range <= 20) {
                return [14, 14, 14];
            }
            if (range <= 40) {
                return [28, 28, 28];
            }
            if (range <= 80) {
                return [56, 56, 56];
            }
            if (range <= 160) {
                return [112, 112, 112];
            }
            return [224, 224, 224];
        default:
            return [0, 0, 0];
        }
    }
}
