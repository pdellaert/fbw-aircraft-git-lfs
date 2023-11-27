// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

const MAX_FIX_ROW = 5;

const Markers = {
    FPLN_DISCONTINUITY: ["---F-PLN DISCONTINUITY--"],
    END_OF_FPLN:        ["------END OF F-PLN------"],
    NO_ALTN_FPLN:       ["-----NO ALTN F-PLN------"],
    END_OF_ALTN_FPLN:   ["---END OF ALTN F-PLN----"],
    TOO_STEEP_PATH:     ["-----TOO STEEP PATH-----"]
};

const Altitude = Object.freeze({
    Empty: "\xa0\xa0\xa0\xa0\xa0",
    NoPrediction: "-----",
})
const Speed = Object.freeze({
    Empty: "\xa0\xa0\xa0",
    NoPrediction: "---",
})
const Time = Object.freeze({
    Empty: "\xa0\xa0\xa0\xa0",
    NoPrediction: "----",
})

class CDUFlightPlanPage {

    static ShowPage(mcdu, offset = 0, forPlan = 0) {

        // INIT
        function addLskAt(index, delay, callback) {
            mcdu.leftInputDelay[index] = (typeof delay === 'function') ? delay : () => delay;
            mcdu.onLeftInput[index] = callback;
        }

        function addRskAt(index, delay, callback) {
            mcdu.rightInputDelay[index] = (typeof delay === 'function') ? delay : () => delay;
            mcdu.onRightInput[index] = callback;
        }

        //mcdu.flightPlanManager.updateWaypointDistances(false /* approach */);
        //mcdu.flightPlanManager.updateWaypointDistances(true /* approach */);
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.FlightPlanPage;
        mcdu.returnPageCallback = () => {
            CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
        };
        mcdu.activeSystem = 'FMGC';

        // regular update due to showing dynamic data on this page
        mcdu.page.SelfPtr = setTimeout(() => {
            if (mcdu.page.Current === mcdu.page.FlightPlanPage) {
                CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
            }
        }, mcdu.PageTimeout.Medium);
        mcdu.onUnload = () => {
            mcdu.efisInterface.setSecRelatedPageOpen(false);
        }

        const flightPhase = mcdu.flightPhaseManager.phase;
        const isFlying = flightPhase >= FmgcFlightPhases.TAKEOFF && flightPhase != FmgcFlightPhases.DONE;

        let showFrom = false;
        // TODO FIXME: Correct FMS lateral position calculations and move logic from F-PLN A
        // 22-70-00:11
        const adirLat = ADIRS.getLatitude();
        const adirLong = ADIRS.getLongitude();
        const ppos = (adirLat.isNormalOperation() && adirLong.isNormalOperation()) ? {
            lat: ADIRS.getLatitude().value,
            long: ADIRS.getLongitude().value,
        } : {
            lat: NaN,
            long: NaN
        };

        const forActiveOrTemporary = forPlan === 0;
        const targetPlan = forActiveOrTemporary ? mcdu.flightPlanService.activeOrTemporary : mcdu.flightPlanService.secondary(1);
        const planAccentColor = forActiveOrTemporary ? mcdu.flightPlanService.hasTemporary ? 'yellow' : 'green' : 'white';

        let headerText;
        if (forActiveOrTemporary) {
            if (mcdu.flightPlanService.hasTemporary) {
                headerText = `{yellow}{sp}TMPY{end}`;
            } else {
                headerText = `{sp}`;
            }
        } else {
            headerText = `{sp}{sp}{sp}{sp}{sp}{sp}{sp}{sp}{sp}{sp}{sp}SEC`;
        }

        let flightNumberText = '';
        if (forActiveOrTemporary) {
            flightNumberText = SimVar.GetSimVarValue("ATC FLIGHT NUMBER", "string", "FMC");
        }

        // TODO FIXME: Move from F-PLN A
        const utcTime = SimVar.GetGlobalVarValue("ZULU TIME", "seconds");

        if (mcdu.flightPlanService.active.originAirport) {
            if (!isFlying) {
                // TODO
                // fpm._waypointReachedAt = utcTime;
            }
        }

        const waypointsAndMarkers = [];
        const first = Math.max(0, targetPlan.activeLegIndex - 1);

        // VNAV
        const fmsGeometryProfile = mcdu.guidanceController.vnavDriver.mcduProfile;
        const fmsPseudoWaypoints = mcdu.guidanceController.currentPseudoWaypoints;

        /** @type {Map<number, VerticalWaypointPrediction>} */
        let vnavPredictionsMapByWaypoint = null;
        if (fmsGeometryProfile && fmsGeometryProfile.isReadyToDisplay) {
            vnavPredictionsMapByWaypoint = fmsGeometryProfile.waypointPredictions;
        }

        let cumulativeDistance = 0;
        // Primary F-PLAN

        // In this loop, we insert pseudowaypoints between regular waypoints and compute the distances between the previous and next (pseudo-)waypoint.
        for (let i = first; i < targetPlan.legCount; i++) {
            const inMissedApproach = i >= targetPlan.firstMissedApproachLegIndex;
            const isActiveLeg = i === targetPlan.activeLegIndex && forActiveOrTemporary;
            const isFromLeg = i === targetPlan.activeLegIndex - 1 && forActiveOrTemporary;

            const wp = targetPlan.allLegs[i];

            if (wp.isDiscontinuity) {
                waypointsAndMarkers.push({ marker: Markers.FPLN_DISCONTINUITY, fpIndex: i, inAlternate: false, inMissedApproach });
                continue;
            }

            // No PWP on FROM leg
            const pseudoWaypointsOnLeg = fmsPseudoWaypoints.filter((it) => it.displayedOnMcdu && it.alongLegIndex === i);
            pseudoWaypointsOnLeg.sort((a, b) => a.distanceFromStart - b.distanceFromStart);

            for (let j = 0; j < pseudoWaypointsOnLeg.length; j++) {
                const pwp = pseudoWaypointsOnLeg[j];
                const distanceFromLastLine = pwp.distanceFromStart - cumulativeDistance;
                cumulativeDistance = pwp.distanceFromStart;

                if (!isFromLeg) {
                    waypointsAndMarkers.push({ pwp, fpIndex: i, inMissedApproach, distanceFromLastLine, isActive: isActiveLeg && j === 0 })
                }
            }

            if (i >= targetPlan.activeLegIndex && wp.definition.type === 'HM') {
                waypointsAndMarkers.push({ holdResumeExit: wp, fpIndex: i, inMissedApproach, isActive: isActiveLeg });
            }

            const distanceFromLastLine = wp.calculated ? (wp.calculated.cumulativeDistanceWithTransitions - cumulativeDistance) : 0;
            cumulativeDistance = wp.calculated ? wp.calculated.cumulativeDistanceWithTransitions : cumulativeDistance;

            waypointsAndMarkers.push({ wp, fpIndex: i, inAlternate: false, inMissedApproach, distanceFromLastLine, isActive: isActiveLeg && pseudoWaypointsOnLeg.length === 0 });

            if (i === targetPlan.lastIndex) {
                waypointsAndMarkers.push({ marker: Markers.END_OF_FPLN, fpIndex: i, inAlternate: false, inMissedApproach });
            }
        }

        // Primary ALTN F-PLAN
        if (targetPlan.alternateDestinationAirport) {
            for (let i = 0; i < targetPlan.alternateFlightPlan.legCount; i++) {
                const inMissedApproach = i >= targetPlan.alternateFlightPlan.firstMissedApproachLegIndex;

                const wp = targetPlan.alternateFlightPlan.allLegs[i];

                if (wp.isDiscontinuity) {
                    waypointsAndMarkers.push({ marker: Markers.FPLN_DISCONTINUITY, fpIndex: i, inAlternate: true });
                    continue;
                }

                // TODO port over (fms-v2)
                if (i >= targetPlan.alternateFlightPlan.activeLegIndex && wp.definition.type === 'HM') {
                    waypointsAndMarkers.push({ holdResumeExit: wp, fpIndex: i, inAlternate: true });
                }

                const distanceFromLastLine = wp.calculated ? (wp.calculated.cumulativeDistanceWithTransitions - cumulativeDistance) : 0;
                cumulativeDistance = wp.calculated ? wp.calculated.cumulativeDistanceWithTransitions : cumulativeDistance;

                waypointsAndMarkers.push({ wp, fpIndex: i, inAlternate: true, inMissedApproach, distanceFromLastLine });

                if (i === targetPlan.alternateFlightPlan.lastIndex) {
                    waypointsAndMarkers.push({ marker: Markers.END_OF_ALTN_FPLN, fpIndex: i, inAlternate: true });
                }
            }
        } else if (targetPlan.legCount > 0) {
            waypointsAndMarkers.push({ marker: Markers.NO_ALTN_FPLN, fpIndex: targetPlan.legCount + 1, inAlternate: true });
        }

        const tocIndex = waypointsAndMarkers.findIndex(({ pwp }) => pwp && pwp.ident === '(T/C)');

        // Render F-PLAN Display

        // fprow:   1      | 2     | 3 4   | 5 6   | 7 8   | 9 10  | 11 12   |
        // display: SPD/ALT| R0    | R1    | R2    | R3    | R4    | DEST    | SCRATCHPAD
        // functions:      | F[0]  | F[1]  | F[2]  | F[3]  | F[4]  | F[5]    |
        //                 | FROM  | TO    |
        let rowsCount = 5;

        if (waypointsAndMarkers.length === 0) {
            rowsCount = 0;
            mcdu.setTemplate([
                [`{left}{small}{sp}FROM{end}${headerText}{end}{right}{small}${flightNumberText}{sp}{sp}{sp}{end}{end}`],
                ...emptyFplnPage(forPlan)
            ]);
            mcdu.onLeftInput[0] = () => CDULateralRevisionPage.ShowPage(mcdu, undefined, undefined, forPlan);
            return;
        } else if (waypointsAndMarkers.length >= 5) {
            rowsCount = 5;
        } else {
            rowsCount = waypointsAndMarkers.length;
        }

        let useTransitionAltitude = false;

        // Only examine first 5 (or less) waypoints/markers
        const scrollWindow = [];
        for (let rowI = 0, winI = offset; rowI < rowsCount; rowI++, winI++) {
            winI = winI % (waypointsAndMarkers.length);

            const {
                /** @type {import('fbw-a32nx/src/systems/fmgc/src/flightplanning/new/legs/FlightPlanLeg').FlightPlanElement} */
                wp,
                pwp,
                marker,
                /** @type {import('fbw-a32nx/src/systems/fmgc/src/flightplanning/new/legs/FlightPlanLeg').FlightPlanElement} */
                holdResumeExit,
                fpIndex,
                inAlternate,
                inMissedApproach,
                distanceFromLastLine,
                isActive
            } = waypointsAndMarkers[winI];

            const legAccentColor = (inAlternate || inMissedApproach) ? "cyan" : planAccentColor;

            const wpPrev = targetPlan.maybeElementAt(fpIndex - 1);
            const wpNext = targetPlan.maybeElementAt(fpIndex + 1);
            const wpActive = (fpIndex >= targetPlan.activeLegIndex);

            // Bearing/Track
            let bearingTrack = "";
            const maybeBearingTrackTo = pwp ? targetPlan.maybeElementAt(fpIndex) : wp;
            const bearingTrackTo = maybeBearingTrackTo ? maybeBearingTrackTo : wpNext;
            switch (rowI) {
                case 1:
                    const trueBearing = SimVar.GetSimVarValue("L:A32NX_EFIS_L_TO_WPT_BEARING", "Degrees");
                    if (isActive && trueBearing >= 0) {
                        bearingTrack = `BRG${trueBearing.toFixed(0).padStart(3,"0")}\u00b0`;
                    }
                    break;
                case 2:
                    bearingTrack = formatTrack(wpPrev, bearingTrackTo);
                    break;
            }

            const constraintType = wp ? CDUVerticalRevisionPage.constraintType(mcdu, fpIndex, targetPlan.index, inAlternate) : WaypointConstraintType.Unknown;
            if (constraintType === WaypointConstraintType.CLB) {
                useTransitionAltitude = true;
            } else if (constraintType === WaypointConstraintType.DES) {
                useTransitionAltitude = false;
            } else if (tocIndex >= 0) {
                // FIXME Guess because VNAV doesn't tell us whether altitudes are climb or not \o/
                useTransitionAltitude = winI <= tocIndex;
            } // else we stick with the last time we were sure...

            if (wp && wp.isDiscontinuity === false) {
                // Waypoint
                if (offset === 0) {
                    showFrom = true;
                }

                let ident = wp.ident;
                let isOverfly = wp.definition.overfly;
                const isFromLeg = !inAlternate && fpIndex === targetPlan.alternateFlightPlan.activeLegIndex - 1;

                let verticalWaypoint = null;
                // TODO: Alternate predictions
                if (!inAlternate && vnavPredictionsMapByWaypoint) {
                    verticalWaypoint = vnavPredictionsMapByWaypoint.get(fpIndex);
                }

                // Color
                let color;
                if (isActive) {
                    color = "white";
                } else {
                    const inMissedApproach = targetPlan.index === Fmgc.FlightPlanIndex.Active && fpIndex >= targetPlan.firstMissedApproachLegIndex;

                    if (inMissedApproach || inAlternate) {
                        color = 'cyan';
                    } else {
                        color = planAccentColor;
                    }
                }

                // Time
                let timeCell = Time.NoPrediction;
                let timeColor = "white";
                if (verticalWaypoint && isFinite(verticalWaypoint.secondsFromPresent)) {
                    const utcTime = SimVar.GetGlobalVarValue("ZULU TIME", "seconds");

                    timeCell = `${isFromLeg ? "{big}" : "{small}"}${isFlying
                        ? FMCMainDisplay.secondsToUTC(utcTime + verticalWaypoint.secondsFromPresent)
                        : FMCMainDisplay.secondsTohhmm(verticalWaypoint.secondsFromPresent)}{end}`;

                    timeColor = color;
                } else if (!inAlternate && fpIndex === targetPlan.originLegIndex) {
                    timeCell = "{big}0000{end}";
                    timeColor = color;
                }

                // Fix Header
                const fixAnnotation = wp.annotation;

                // Distance
                let distance = "";
                // Active waypoint is live distance, others are distances in the flight plan
                if (isActive) {
                    if (Number.isFinite(mcdu.guidanceController.activeLegAlongTrackCompletePathDtg)) {
                        distance = Math.round(Math.max(0, Math.min(9999, mcdu.guidanceController.activeLegAlongTrackCompletePathDtg))).toFixed(0);
                    }
                } else {
                    distance = Math.round(Math.max(0, Math.min(9999, distanceFromLastLine))).toFixed(0);
                }

                let fpa = '';
                if (wp.definition.verticalAngle !== undefined) {
                    fpa = (Math.round(wp.definition.verticalAngle * 10) / 10).toFixed(1);
                }

                let altColor = "white";
                let spdColor = "white";

                // Should show empty speed prediction for waypoint after hold
                let speedConstraint = wp.type === "HM" ? Speed.Empty : Speed.NoPrediction;
                let speedPrefix = "";

                if (targetPlan.index !== Fmgc.FlightPlanIndex.Temporary && wp.type !== "HM") {
                    if (!inAlternate && fpIndex === targetPlan.originLegIndex) {
                        speedConstraint = Number.isFinite(mcdu.v1Speed) ? `{big}${Math.round(mcdu.v1Speed)}{end}` : Speed.NoPrediction;
                        spdColor = Number.isFinite(mcdu.v1Speed) ? color : "white";
                    } else if (isFromLeg) {
                        speedConstraint = Speed.Empty;
                    } else if (verticalWaypoint && verticalWaypoint.speed) {
                        speedConstraint = `{small}${verticalWaypoint.speed < 1 ? formatMachNumber(verticalWaypoint.speed) : Math.round(verticalWaypoint.speed)}{end}`;

                        if (verticalWaypoint.speedConstraint) {
                            speedPrefix = `${verticalWaypoint.isSpeedConstraintMet ? "{magenta}" : "{amber}"}*{end}`;
                        }
                        spdColor = color;
                    } else if (wp.hasPilotEnteredSpeedConstraint()) {
                        speedConstraint = Math.round(wp.pilotEnteredSpeedConstraint.speed);
                        spdColor = "magenta";
                    } else if (wp.hasDatabaseSpeedConstraint()) {
                        speedConstraint = `{small}${Math.round(wp.definition.speed)}{end}`;
                        spdColor = "magenta";
                    }
                }

                speedConstraint = speedPrefix + speedConstraint;

                // Altitude
                const hasAltConstraint = legHasAltConstraint(wp);
                let altitudeConstraint = Altitude.NoPrediction;
                let altSize = "big";
                if (inAlternate && fpIndex === targetPlan.alternateFlightPlan.destinationLegIndex) {
                    if (targetPlan.alternateFlightPlan.destinationRunway && Number.isFinite(targetPlan.alternateFlightPlan.destinationRunway.thresholdCrossingHeight)) {
                        altitudeConstraint = formatAlt(targetPlan.alternateFlightPlan.destinationRunway.thresholdCrossingHeight);
                        altColor = color;
                        altSize = "small";
                    } else if (targetPlan.alternateFlightPlan.destinationAirport && Number.isFinite(targetPlan.alternateFlightPlan.destinationAirport.location.alt)) {
                        altitudeConstraint = formatAlt(targetPlan.alternateFlightPlan.destinationAirport.location.alt);
                        altColor = color
                        altSize = "small";
                    }
                } else if (inAlternate && fpIndex === targetPlan.alternateFlightPlan.originLegIndex) {
                    if (targetPlan.alternateFlightPlan.originRunway && Number.isFinite(targetPlan.alternateFlightPlan.originRunway.location.alt)) {
                        altitudeConstraint = formatAlt(targetPlan.alternateFlightPlan.originRunway.location.alt);
                        altColor = color;
                    } else if (targetPlan.alternateFlightPlan.originAirport && Number.isFinite(targetPlan.alternateFlightPlan.originAirport.location.alt)) {
                        altitudeConstraint = formatAlt(targetPlan.alternateFlightPlan.originAirport.location.alt);
                        altColor = color;
                    }
                } else if (!inAlternate && fpIndex === targetPlan.destinationLegIndex) {
                    if (targetPlan.destinationRunway && Number.isFinite(targetPlan.destinationRunway.thresholdCrossingHeight)) {
                        altitudeConstraint = formatAlt(targetPlan.destinationRunway.thresholdCrossingHeight);
                        altColor = color;
                        altSize = "small";
                    } else if (targetPlan.destinationAirport && Number.isFinite(targetPlan.destinationAirport.location.alt)) {
                        altitudeConstraint = formatAlt(targetPlan.destinationAirport.location.alt);
                        altColor = color
                        altSize = "small";
                    }
                } else if (!inAlternate && fpIndex === targetPlan.originLegIndex) {
                    if (targetPlan.originRunway && Number.isFinite(targetPlan.originRunway.location.alt)) {
                        altitudeConstraint = formatAlt(targetPlan.originRunway.location.alt);
                        altColor = color;
                    } else if (targetPlan.originAirport && Number.isFinite(targetPlan.originAirport.location.alt)) {
                        altitudeConstraint = formatAlt(targetPlan.originAirport.location.alt);
                        altColor = color;
                    }
                } else if (targetPlan.index !== Fmgc.FlightPlanIndex.Temporary) {
                    if (hasAltConstraint && !isFromLeg) {
                        if (verticalWaypoint && verticalWaypoint.altitude) {
                            altitudeConstraint = `{big}${verticalWaypoint.isAltitudeConstraintMet ? "{magenta}*{end}" : "{amber}*{end}"}{end}${formatAltitudeOrLevel(mcdu, verticalWaypoint.altitude, useTransitionAltitude).padStart(5, "\xa0")}`;
                            altColor = color;
                            altSize = "small";
                        } else {
                            altitudeConstraint = formatAltConstraint(mcdu, wp.altitudeConstraint, useTransitionAltitude);
                            altColor = "magenta";
                            altSize = wp.hasPilotEnteredAltitudeConstraint() ? "big" : "small";
                        }
                    // Waypoint with no alt constraint.
                    // In this case `altitudeConstraint is actually just the predictedAltitude`
                    } else if ((isFromLeg || !hasAltConstraint) && verticalWaypoint && verticalWaypoint.altitude) {
                        altitudeConstraint = formatAltitudeOrLevel(mcdu, verticalWaypoint.altitude, useTransitionAltitude);
                        altColor = color;
                        altSize = isFromLeg ? "big" : "small";
                    }
                }

                // forced turn indication if next leg is not a course reversal
                if (wpNext && legTurnIsForced(wpNext) && !legTypeIsCourseReversal(wpNext)) {
                    if (wpNext.turnDirection === 1) {
                        ident += "{";
                    } else {
                        ident += "}";
                    }

                    // the overfly symbol is not shown in this case
                    isOverfly = false;
                }

                scrollWindow[rowI] = {
                    fpIndex,
                    inAlternate: inAlternate,
                    active: wpActive,
                    ident: ident,
                    color,
                    distance,
                    fpa,
                    spdColor,
                    speedConstraint,
                    altColor,
                    altSize,
                    altitudeConstraint,
                    timeCell,
                    timeColor,
                    fixAnnotation: fixAnnotation ? fixAnnotation : "",
                    bearingTrack,
                    isOverfly,
                };

                if (fpIndex !== targetPlan.destinationLegIndex) {
                    addLskAt(rowI,
                        (value) => {
                            if (value === "") {
                                return mcdu.getDelaySwitchPage();
                            }
                            return mcdu.getDelayBasic();
                        },
                        (value, scratchpadCallback) => {
                            switch (value) {
                                case "":
                                    CDULateralRevisionPage.ShowPage(mcdu, wp, fpIndex, forPlan, inAlternate);
                                    break;
                                case FMCMainDisplay.clrValue:
                                    CDUFlightPlanPage.clearElement(mcdu, fpIndex, offset, forPlan, inAlternate, scratchpadCallback);
                                    break;
                                case FMCMainDisplay.ovfyValue:
                                    mcdu.toggleWaypointOverfly(fpIndex, () => {
                                        CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
                                    });
                                    break;
                                default:
                                    if (value.length > 0) {
                                        mcdu.insertWaypoint(value, forPlan, inAlternate, fpIndex, true, (success) => {
                                            if (!success) {
                                                scratchpadCallback();
                                            }
                                            CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
                                        }, !mcdu.flightPlanService.hasTemporary);
                                    }
                                    break;
                            }
                        });
                } else {
                    addLskAt(rowI, () => mcdu.getDelaySwitchPage(),
                        (value, scratchpadCallback) => {
                            if (value === "") {
                                CDULateralRevisionPage.ShowPage(mcdu, wp, fpIndex, forPlan, inAlternate);
                            } else if (value.length > 0) {
                                mcdu.insertWaypoint(value, forPlan, inAlternate, fpIndex, true, (success) => {
                                    if (!success) {
                                        scratchpadCallback();
                                    }
                                    CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
                                }, true);
                            }
                        });
                }

                addRskAt(rowI, () => mcdu.getDelaySwitchPage(),
                    (value, scratchpadCallback) => {
                        if (value === "") {
                            CDUVerticalRevisionPage.ShowPage(mcdu, wp, fpIndex, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
                        } else {
                            CDUVerticalRevisionPage.setConstraints(mcdu, wp, fpIndex, verticalWaypoint, value, scratchpadCallback, offset, forPlan, inAlternate);
                        }
                    });

            } else if (pwp) {
                const baseColor = forActiveOrTemporary ? mcdu.flightPlanService.hasTemporary ? "yellow" : "green" : "white";
                const color = isActive ? "white" : baseColor;

                // TODO: PWP should not be shown while predictions are recomputed or in a temporary flight plan,
                // but if I don't show them, the flight plan jumps around because the offset is no longer correct if the number of items in the flight plan changes.
                // Like this, they are still there, but have dashes for predictions.
                const shouldHidePredictions = !fmsGeometryProfile || !fmsGeometryProfile.isReadyToDisplay || !pwp.flightPlanInfo;

                let timeCell = Time.NoPrediction;
                let timeColor = "white";
                if (!shouldHidePredictions && Number.isFinite(pwp.flightPlanInfo.secondsFromPresent)) {
                    const utcTime = SimVar.GetGlobalVarValue("ZULU TIME", "seconds");
                    timeColor = color;

                    timeCell = isFlying
                        ? `${FMCMainDisplay.secondsToUTC(utcTime + pwp.flightPlanInfo.secondsFromPresent)}[s-text]`
                        : `${FMCMainDisplay.secondsTohhmm(pwp.flightPlanInfo.secondsFromPresent)}[s-text]`;
                }

                let speed = Speed.NoPrediction;
                let spdColor = "white";
                if (!shouldHidePredictions && Number.isFinite(pwp.flightPlanInfo.speed)) {
                    speed = `{small}${pwp.flightPlanInfo.speed < 1 ? formatMachNumber(pwp.flightPlanInfo.speed) : Math.round(pwp.flightPlanInfo.speed).toFixed(0)}{end}`;
                    spdColor = color;
                }

                let altitudeConstraint = Altitude.NoPrediction;
                let altColor = "white";
                if (!shouldHidePredictions && Number.isFinite(pwp.flightPlanInfo.altitude)) {
                    altitudeConstraint = formatAltitudeOrLevel(mcdu, pwp.flightPlanInfo.altitude, useTransitionAltitude);
                    altColor = color;
                }

                let distance = undefined;
                if (!shouldHidePredictions) {
                    distance = isActive
                        ? mcdu.guidanceController.activeLegAlongTrackCompletePathDtg - pwp.distanceFromLegTermination
                        : distanceFromLastLine;
                }

                scrollWindow[rowI] = {
                    fpIndex: fpIndex,
                    active: isActive,
                    ident: pwp.mcduIdent || pwp.ident,
                    color,
                    distance: distance !== undefined ? Math.round(Math.max(0, Math.min(9999, distance))).toFixed(0) : "",
                    spdColor,
                    speedConstraint: speed,
                    altColor,
                    altSize: "small",
                    altitudeConstraint,
                    timeCell,
                    timeColor,
                    fixAnnotation: `{${color}}${pwp.mcduHeader || ''}{end}`,
                    bearingTrack,
                    isOverfly: false,
                };

                addLskAt(rowI, 0, (value, scratchpadCallback) => {
                    if (value === FMCMainDisplay.clrValue) {
                        // TODO
                        mcdu.setScratchpadMessage(NXSystemMessages.notAllowed);
                    }
                });
            } else if (marker) {
                // Marker
                scrollWindow[rowI] = waypointsAndMarkers[winI];
                addLskAt(rowI, 0, (value, scratchpadCallback) => {
                    if (value === FMCMainDisplay.clrValue) {
                        CDUFlightPlanPage.clearElement(mcdu, fpIndex, offset, forPlan, inAlternate, scratchpadCallback);
                        return;
                    }

                    mcdu.insertWaypoint(value, forPlan, inAlternate, fpIndex, true, (success) => {
                        if (!success) {
                            scratchpadCallback();
                        }
                        CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
                    }, !mcdu.flightPlanService.hasTemporary);
                });
            } else if (holdResumeExit && holdResumeExit.isDiscontinuity === false) {
                const isNext = fpIndex === (targetPlan.activeLegIndex + 1);

                let color = legAccentColor;
                if (isActive) {
                    color = "white";
                }

                const decelReached = isActive || isNext && mcdu.holdDecelReached;
                const holdSpeed = fpIndex === mcdu.holdIndex && mcdu.holdSpeedTarget > 0 ? mcdu.holdSpeedTarget.toFixed(0) : '\xa0\xa0\xa0';
                const turnDirection = holdResumeExit.definition.turnDirection;
                // prompt should only be shown once entering decel for hold (3 - 20 NM before hold)
                const immExit = decelReached && !holdResumeExit.holdImmExit;
                const resumeHold = decelReached && holdResumeExit.holdImmExit;

                scrollWindow[rowI] = {
                    fpIndex,
                    holdResumeExit,
                    color,
                    immExit,
                    resumeHold,
                    holdSpeed,
                    turnDirection,
                };

                addLskAt(rowI, 0, (value, scratchpadCallback) => {
                    if (value === FMCMainDisplay.clrValue) {
                        CDUFlightPlanPage.clearElement(mcdu, fpIndex, offset, forPlan, inAlternate, scratchpadCallback);
                        return;
                    }

                    CDUHoldAtPage.ShowPage(mcdu, fpIndex, forPlan, inAlternate);
                    scratchpadCallback();
                });

                addRskAt(rowI, 0, (value, scratchpadCallback) => {
                    // IMM EXIT, only active once reaching decel
                    if (isActive) {
                        mcdu.fmgcMesssagesListener.triggerToAllSubscribers('A32NX_IMM_EXIT', fpIndex, immExit);
                        setTimeout(() => {
                            CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
                        }, 500);
                    } else if (decelReached) {
                        CDUFlightPlanPage.clearElement(mcdu, fpIndex, offset, forPlan, inAlternate, scratchpadCallback);
                        return;
                    }
                    scratchpadCallback();
                });
            }
        }

        // Pass current waypoint data to FMGC
        if (scrollWindow[1]) {
            mcdu.efisInterface.setPlanCentre(targetPlan.index, scrollWindow[1].fpIndex, scrollWindow[1].inAlternate);
        } else if (scrollWindow[0]) {
            mcdu.efisInterface.setPlanCentre(targetPlan.index, scrollWindow[0].fpIndex, scrollWindow[0].inAlternate);
        } else {
            mcdu.efisInterface.setPlanCentre(targetPlan.index, first + offset, false);
        }

        mcdu.efisInterface.setAlternateLegVisible(scrollWindow.some(row => row.inAlternate), forPlan);
        mcdu.efisInterface.setMissedLegVisible(targetPlan && scrollWindow.some(row => row.fpIndex >= targetPlan.firstMissedApproachLegIndex), forPlan);
        mcdu.efisInterface.setAlternateMissedLegVisible(targetPlan && targetPlan.alternateFlightPlan && scrollWindow.some(row => row.fpIndex >= targetPlan.alternateFlightPlan.firstMissedApproachLegIndex), forPlan);
        mcdu.efisInterface.setSecRelatedPageOpen(!forActiveOrTemporary);

        // Render scrolling data to text >> add ditto marks

        let firstWp = scrollWindow.length;
        const scrollText = [];
        for (let rowI = 0; rowI < scrollWindow.length; rowI++) {
            const { marker: cMarker, holdResumeExit: cHold, speedConstraint: cSpd, altitudeConstraint: cAlt, ident: cIdent } = scrollWindow[rowI];
            let spdRpt = false;
            let altRpt = false;
            let showFix = true;
            let showDist = true;
            let showNm = false;

            if (cHold) {
                const { color, immExit, resumeHold, holdSpeed, turnDirection } = scrollWindow[rowI];
                scrollText[(rowI * 2)] = ["", `{amber}${immExit ? 'IMM\xa0\xa0' : ''}${resumeHold ? 'RESUME\xa0' : ''}{end}`, 'HOLD\xa0\xa0\xa0\xa0'];
                scrollText[(rowI * 2) + 1] = [`{${color}}HOLD ${turnDirection}{end}`, `{amber}${immExit ? 'EXIT*' : ''}${resumeHold ? 'HOLD*' : ''}{end}`, `\xa0{${color}}{small}{white}SPD{end}\xa0${holdSpeed}{end}{end}`];
            } else if (!cMarker) { // Waypoint
                if (rowI > 0) {
                    const { marker: pMarker, pwp: pPwp, holdResumeExit: pHold, speedConstraint: pSpd, altitudeConstraint: pAlt} = scrollWindow[rowI - 1];
                    if (!pMarker && !pPwp && !pHold) {
                        firstWp = Math.min(firstWp, rowI);
                        if (rowI === firstWp) {
                            showNm = true;
                        }
                        if (cSpd !== Speed.NoPrediction && cSpd === pSpd) {
                            spdRpt = true;
                        }

                        if (cAlt !== Altitude.NoPrediction && cAlt === pAlt) {
                            altRpt = true;
                        }
                    // If previous row is a marker, clear all headers unless it's a speed limit
                    } else if (!pHold) {
                        showDist = false;
                        showFix = cIdent === "(LIM)";
                    }
                }

                scrollText[(rowI * 2)] = renderFixHeader(scrollWindow[rowI], showNm, showDist, showFix);
                scrollText[(rowI * 2) + 1] = renderFixContent(scrollWindow[rowI], spdRpt, altRpt);

            // Marker
            } else {
                scrollText[(rowI * 2)] = [];
                scrollText[(rowI * 2) + 1] = cMarker;
            }
        }

        // Destination (R6)

        const destText = [];
        if (mcdu.flightPlanService.hasTemporary) {
            destText[0] = [" ", " "];
            destText[1] = ["{ERASE[color]amber", "INSERT*[color]amber"];

            addLskAt(5, 0, async () => {
                mcdu.eraseTemporaryFlightPlan(() => {
                    CDUFlightPlanPage.ShowPage(mcdu, 0, forPlan);
                });
            });
            addRskAt(5, 0, async () => {
                mcdu.insertTemporaryFlightPlan(() => {
                    CDUFlightPlanPage.ShowPage(mcdu, 0, forPlan);
                });
            });
        } else {
            let destCell = "----";
            if (targetPlan.destinationAirport) {
                destCell = targetPlan.destinationAirport.ident;

                if (targetPlan.destinationRunway) {
                    destCell += targetPlan.destinationRunway.ident.substring(2);
                }
            }
            let destTimeCell = "----";
            let destDistCell = "----";
            let destEFOBCell = "-----";

            if (targetPlan.destinationAirport) {
                if (CDUInitPage.fuelPredConditionsMet(mcdu) && mcdu._fuelPredDone) {
                    mcdu.tryUpdateRouteTrip(isFlying);
                }

                const destDist = mcdu.guidanceController.alongTrackDistanceToDestination

                if (Number.isFinite(destDist)) {
                    destDistCell = Math.round(destDist).toFixed(0);
                }

                if (fmsGeometryProfile && fmsGeometryProfile.isReadyToDisplay) {
                    const destEfob = fmsGeometryProfile.getRemainingFuelAtDestination();

                    if (Number.isFinite(destEfob)) {
                        destEFOBCell = (NXUnits.poundsToUser(destEfob) / 1000).toFixed(1);
                    }

                    const timeRemaining = fmsGeometryProfile.getTimeToDestination();
                    if (Number.isFinite(timeRemaining)) {
                        const utcTime = SimVar.GetGlobalVarValue("ZULU TIME", "seconds");

                        destTimeCell = isFlying
                            ? FMCMainDisplay.secondsToUTC(utcTime + timeRemaining)
                            : FMCMainDisplay.secondsTohhmm(timeRemaining);
                    }
                }
            }

            destText[0] = ["\xa0DEST", "DIST\xa0\xa0EFOB", isFlying ? "\xa0UTC{sp}{sp}{sp}{sp}" : "TIME{sp}{sp}{sp}{sp}"];
            destText[1] = [destCell, `{small}${destDistCell.padStart(4, '\xa0')}\xa0${destEFOBCell.padStart(5, '\xa0')}{end}`, `{small}${destTimeCell}{end}{sp}{sp}{sp}{sp}`];

            addLskAt(5, () => mcdu.getDelaySwitchPage(),
                () => {
                    CDULateralRevisionPage.ShowPage(mcdu, targetPlan.destinationLeg, targetPlan.destinationLegIndex, forPlan);
                });

            addRskAt(5, () => mcdu.getDelaySwitchPage(),
                () => {
                    CDUVerticalRevisionPage.ShowPage(mcdu, targetPlan.destinationLeg, targetPlan.destinationLegIndex, undefined, undefined, undefined, undefined, forPlan, false);
                });
        }

        // scrollText pad to 10 rows
        while (scrollText.length < 10) {
            scrollText.push([""]);
        }
        const allowScroll = waypointsAndMarkers.length > 4;
        if (allowScroll) {
            mcdu.onAirport = () => { // Only called if > 4 waypoints
                const isOnFlightPlanPage = mcdu.page.Current === mcdu.page.FlightPlanPage;
                const destinationAirportOffset = waypointsAndMarkers.length - 5;
                const allowCycleToOriginAirport = mcdu.flightPhaseManager.phase === FmgcFlightPhases.PREFLIGHT;
                if (offset === destinationAirportOffset && allowCycleToOriginAirport && isOnFlightPlanPage) { // only show origin if still on ground
                    // Go back to top of flight plan page to show origin airport.
                    offset = 0;
                } else {
                    offset = destinationAirportOffset; // if in air only dest is available.
                }
                CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
            };
            mcdu.onDown = () => { // on page down decrement the page offset.
                if (offset > 0) { // if page not on top
                    offset--;
                } else { // else go to the bottom
                    offset = waypointsAndMarkers.length - 1;
                }
                CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
            };
            mcdu.onUp = () => {
                if (offset < waypointsAndMarkers.length - 1) { // if page not on bottom
                    offset++;
                } else { // else go on top
                    offset = 0;
                }
                CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
            };
        }
        mcdu.setArrows(allowScroll, allowScroll, true, true);
        scrollText[0][1] = "SPD/ALT\xa0\xa0\xa0";
        scrollText[0][2] = isFlying ? "\xa0UTC{sp}{sp}{sp}{sp}" : "TIME{sp}{sp}{sp}{sp}";
        mcdu.setTemplate([
            [`{left}{small}{sp}${showFrom ? "FROM" : "{sp}{sp}{sp}{sp}"}{end}${headerText}{end}{right}{small}${flightNumberText}{sp}{sp}{sp}{end}{end}`],
            ...scrollText,
            ...destText
        ]);
    }

    static async clearElement(mcdu, fpIndex, offset, forPlan, forAlternate, scratchpadCallback) {
        if (fpIndex === Fmgc.FlightPlanIndex.Active && mcdu.flightPlanService.hasTemporary) {
            mcdu.setScratchpadMessage(NXSystemMessages.notAllowed);
            scratchpadCallback();
            return;
        }

        // TODO maybe move this to FMS logic ?
        if (fpIndex === Fmgc.FlightPlanIndex.Active && fpIndex <= mcdu.flightPlanService.activeLegIndex) {
            // 22-72-00:67
            // Stop clearing TO or FROM waypoints when NAV is engaged
            if (mcdu.navModeEngaged()) {
                mcdu.setScratchpadMessage(NXSystemMessages.notAllowedInNav);
                scratchpadCallback();
                return;
            }
        }

        try {
            await mcdu.flightPlanService.deleteElementAt(fpIndex, forPlan, forAlternate);
        } catch (e) {
            console.error(e);
            mcdu.setScratchpadMessage(NXFictionalMessages.internalError);
            scratchpadCallback();
        }

        CDUFlightPlanPage.ShowPage(mcdu, offset, forPlan);
    }
}

function renderFixTableHeader(isFlying) {
    return [
        `{sp}\xa0FROM`,
        "SPD/ALT\xa0\xa0\xa0",
        isFlying ? "\xa0UTC{sp}{sp}{sp}{sp}" : "TIME{sp}{sp}{sp}{sp}"
    ];
}

function renderFixHeader(rowObj, showNm = false, showDist = true, showFix = true) {
    const { fixAnnotation, color, distance, bearingTrack, fpa } = rowObj;
    let right = showDist ? `{${color}}${distance}{end}` : '';
    if (fpa) {
        right += `{white}${fpa}°{end}`;
    } else if (showNm) {
        right += `{${color}}NM{end}\xa0\xa0\xa0`;
    } else {
        right += '\xa0\xa0\xa0\xa0\xa0';
    }
    return [
        `${(showFix) ? fixAnnotation.padEnd(7, "\xa0").padStart(8, "\xa0") : ""}`,
        right,
        `{${color}}${bearingTrack}{end}\xa0`,
    ];
}

function renderFixContent(rowObj, spdRepeat = false, altRepeat = false) {
    const {ident, isOverfly, color, spdColor, speedConstraint, altColor, altSize, altitudeConstraint, timeCell, timeColor} = rowObj;

    return [
        `${ident}${isOverfly ? FMCMainDisplay.ovfyValue : ""}[color]${color}`,
        `{${spdColor}}${spdRepeat ? "\xa0\"\xa0" : speedConstraint}{end}{${altColor}}{${altSize}}/${altRepeat ? "\xa0\xa0\xa0\"\xa0\xa0" : altitudeConstraint.padStart(6, "\xa0")}{end}{end}`,
        `${timeCell}{sp}{sp}{sp}{sp}[color]${timeColor}`
    ];
}

function emptyFplnPage(forPlan) {
    return [
        ["", "SPD/ALT{sp}{sp}{sp}", "TIME{sp}{sp}{sp}{sp}"],
        [`PPOS[color]${forPlan === 0 ? 'green' : 'white'}`, "{sp}{sp}{sp}/ -----", "----{sp}{sp}{sp}{sp}"],
        [""],
        ["---F-PLN DISCONTINUITY---"],
        [""],
        ["------END OF F-PLN-------"],
        [""],
        ["-----NO ALTN F-PLN-------"],
        [""],
        [""],
        ["\xa0DEST", "DIST\xa0\xa0EFOB", "TIME{sp}{sp}{sp}{sp}"],
        ["-------", "----\xa0---.-[s-text]", "----{sp}{sp}{sp}{sp}[s-text]"]
    ];
}

function legTypeIsCourseReversal(wp) {
    switch (wp.additionalData.legType) {
        case 12: // HA
        case 13: // HF
        case 14: // HM
        case 16: // PI
            return true;
        default:
    }
    return false;
}

function legTurnIsForced(wp) {
    // forced turns are only for straight legs
    return (wp.turnDirection === 1 /* Left */ || wp.turnDirection === 2 /* Right */)
        // eslint-disable-next-line semi-spacing
        && wp.additionalData.legType !== 1 /* AF */ && wp.additionalData.legType !== 17 /* RF */;
}

function formatMachNumber(rawNumber) {
    return (Math.round(100 * rawNumber) / 100).toFixed(2).slice(1);
}

/**
 * @param {FlightPlanLeg} leg
 * @return {boolean}
 */
function legHasAltConstraint(leg) {
    return leg.hasPilotEnteredAltitudeConstraint() || leg.hasDatabaseAltitudeConstraint();
}

/**
 * Formats an altitude as an altitude or flight level for display.
 * @param {*} mcdu Reference to the MCDU instance
 * @param {number} altitudeToFormat  The altitude in feet.
 * @param {boolean} useTransAlt Whether to use transition altitude, otherwise transition level is used.
 * @returns {string} The formatted altitude/level.
 */
function formatAltitudeOrLevel(mcdu, alt, useTransAlt) {
    const activePlan = mcdu.flightPlanService.active;

    let isFl = false;
    if (useTransAlt) {
        const transAlt = activePlan.performanceData.transitionAltitude.get();
        isFl = transAlt !== undefined && alt > transAlt;
    } else {
        const transLevel = activePlan.performanceData.transitionLevel.get();
        isFl = transLevel !== undefined && alt >= (transLevel * 100);
    }

    if (isFl) {
        return `FL${(alt / 100).toFixed(0).padStart(3,"0")}`;
    }

    return formatAlt(alt);
}

function formatTrack(from, to) {
    // TODO: Does this show something for non-waypoint terminated legs?
    if (!from || !from.definition || !from.definition.waypoint || !from.definition.waypoint.location || !to || !to.definition || !to.definition.waypoint || to.definition.type === "HM") {
        return "";
    }

    const magVar = Facilities.getMagVar(from.definition.waypoint.location.lat, from.definition.waypoint.location.long);
    const tr = Avionics.Utils.computeGreatCircleHeading(from.definition.waypoint.location, to.definition.waypoint.location);
    const track = A32NX_Util.trueToMagnetic(tr, magVar);
    return `TRK${track.toFixed(0).padStart(3,"0")}\u00b0`;
}

/**
 * Formats a numberical altitude to a string to be displayed in the altitude column. Does not format FLs, use {@link formatAltitudeOrLevel} for this purpose
 * @param {Number} alt The altitude to format
 * @returns {String} The formatted altitude string
 */
function formatAlt(alt) {
    return (Math.round(alt / 10) * 10).toFixed(0);
}

function formatAltConstraint(mcdu, constraint, useTransAlt) {
    // Altitude constraint types "G" and "H" are not shown in the flight plan
    switch (constraint.type) {
        case '@': // at
            return formatAltitudeOrLevel(mcdu, constraint.altitude1, useTransAlt);
        case '+': // atOrAbove
            return "+" + formatAltitudeOrLevel(mcdu, constraint.altitude1, useTransAlt);
        case '-': // atOrBelow
            return "-" + formatAltitudeOrLevel(mcdu, constraint.altitude1, useTransAlt);
        case 'B': // range
            return "WINDOW";
    }

    return '';
}
