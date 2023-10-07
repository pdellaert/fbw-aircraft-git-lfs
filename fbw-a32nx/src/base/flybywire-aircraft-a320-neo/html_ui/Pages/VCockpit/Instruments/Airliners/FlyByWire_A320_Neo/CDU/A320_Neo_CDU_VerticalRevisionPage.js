// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

/** This must match WaypointConstraintType from TS */
const WaypointConstraintType = Object.freeze({
    Unknown: 0,
    CLB: 1,
    DES: 2,
});

class CDUVerticalRevisionPage {
    /**
     * @param mcdu
     * @param {FlightPlanLeg} waypoint
     * @param verticalWaypoint
     * @param confirmSpeed
     * @param confirmAlt
     * @param confirmCode
     */
    static ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, confirmSpeed = undefined, confirmAlt = undefined, confirmCode = undefined, forPlan = Fmgc.FlightPlanIndex.Active, inAlternate = false) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.VerticalRevisionPage;

        /** @type {BaseFlightPlan} */
        const targetPlan = mcdu.flightPlan(forPlan, inAlternate);

        const confirmConstraint = Number.isFinite(confirmSpeed) || Number.isFinite(confirmAlt);
        const constraintType = CDUVerticalRevisionPage.constraintType(mcdu, wpIndex, forPlan, inAlternate);
        const isOrigin = wpIndex === 0;
        const isDestination = wpIndex === targetPlan.destinationLegIndex;

        let waypointIdent = "---";
        if (waypoint) {
            waypointIdent = waypoint.ident;
            if (isDestination) {
                const destinationRunway = targetPlan.destinationRunway;

                if (destinationRunway) {
                    // TODO this is broken currently, need to see if this is just the leg ident r actual airport + runway? (fms-v2)
                    waypointIdent += Avionics.Utils.formatRunway(destinationRunway.ident);
                }
            }
        }

        let coordinates = "---";
        if (waypoint.isXF()) {
            coordinates = CDUPilotsWaypoint.formatLatLong(waypoint.terminationWaypoint().location);
        }

        const showSpeedLim = mcdu._fuelPredDone || isOrigin || isDestination || constraintType !== WaypointConstraintType.Unknown;
        // the conditions other than isDestination are a workaround for no ToC
        const showDesSpeedLim = showSpeedLim && (isDestination ||
            constraintType === WaypointConstraintType.DES ||
            (mcdu.flightPhaseManager.phase > FmgcFlightPhases.CRUISE &&
                mcdu.flightPhaseManager.phase < FmgcFlightPhases.GOAROUND));

        let speedLimitTitle = "";
        let speedLimitCell = "";
        if (showDesSpeedLim) {
            speedLimitTitle = "\xa0DES SPD LIM";
            if (mcdu.descentSpeedLimit !== undefined) {
                speedLimitCell = `{magenta}{${mcdu.descentSpeedLimitPilot ? 'big' : 'small'}}${mcdu.descentSpeedLimit.toFixed(0).padStart(3, "0")}/${this.formatFl(mcdu.descentSpeedLimitAlt, targetPlan.performanceData.transitionLevel.get() * 100)}{end}{end}`;
            } else {
                speedLimitCell = "{cyan}*[ ]/[   ]{end}";
            }
        } else if (showSpeedLim) {
            speedLimitTitle = "\xa0CLB SPD LIM";
            if (mcdu.climbSpeedLimit !== undefined) {
                speedLimitCell = `{magenta}{${mcdu.climbSpeedLimitPilot ? 'big' : 'small'}}${mcdu.climbSpeedLimit.toFixed(0).padStart(3, "0")}/${this.formatFl(mcdu.climbSpeedLimitAlt, targetPlan.performanceData.transitionAltitude.get())}{end}{end}`;
            } else {
                speedLimitCell = "{cyan}*[ ]/[   ]{end}";
            }
        }

        let speedConstraint = 0;
        if (waypoint.definition.speed > 10) {
            speedConstraint = waypoint.definition.speed.toFixed(0);
        }

        const transAltLevel = constraintType === WaypointConstraintType.DES ? targetPlan.performanceData.transitionLevel.get() * 100 : targetPlan.performanceData.transitionAltitude.get();
        let altitudeConstraint = "";
        switch (waypoint.definition.altitudeDescriptor) {
            case '@': {
                altitudeConstraint = this.formatFl(Math.round(waypoint.definition.altitude1), transAltLevel);
                break;
            }
            case '+': {
                altitudeConstraint = "+" + this.formatFl(Math.round(waypoint.definition.altitude1), transAltLevel);
                break;
            }
            case '-': {
                altitudeConstraint = "-" + this.formatFl(Math.round(waypoint.definition.altitude1), transAltLevel);
                break;
            }
            case 'B': {
                if (waypoint.definition.altitude1 < waypoint.definition.altitude2) {
                    altitudeConstraint = "+" + this.formatFl(Math.round(waypoint.definition.altitude1), transAltLevel)
                        + "/-" + this.formatFl(Math.round(waypoint.definition.altitude2), transAltLevel);
                } else {
                    altitudeConstraint = "+" + this.formatFl(Math.round(waypoint.definition.altitude2), transAltLevel)
                        + "/-" + this.formatFl(Math.round(waypoint.definition.altitude1), transAltLevel);
                }
                break;
            }
        }

        let r3Title = "ALT CSTR\xa0";
        let r3Cell = "{cyan}[\xa0\xa0\xa0\xa0]*{end}";
        let l3Title = "\xa0SPD CSTR";
        let l3Cell = "{cyan}*[\xa0\xa0\xa0]{end}";
        let l4Title = "MACH/START WPT[color]inop";
        let l4Cell = `\xa0{inop}[\xa0]/{small}${waypointIdent}{end}{end}`;
        let r4Title = "";
        let r4Cell = "";
        let r5Cell = "";

        if (isDestination) {
            const hasGsIntercept = targetPlan.approach && (targetPlan.approach.type === 5 /* ILS */ || targetPlan.approach.type === 6 /* GLS */);
            const gsIntercept = hasGsIntercept ? targetPlan.glideslopeIntercept() : 0;

            if (hasGsIntercept && gsIntercept > 0) {
                r3Title = "G/S INTCP\xa0";
                r3Cell = `{green}{small}${gsIntercept.toFixed(0)}{end}{end}`;
            } else {
                r3Title = "";
                r3Cell = "";
            }

            const closeToDest = false; // TODO fms-v2: port liveDistanceTo (use VNAV?)
            // const closeToDest = mcdu.flightPlanManager.getDestination() && mcdu.flightPlanManager.getDestination().liveDistanceTo <= 180;
            l4Title = "\xa0QNH";
            if (isFinite(mcdu.perfApprQNH)) {
                if (mcdu.perfApprQNH < 500) {
                    l4Cell = `{cyan}${mcdu.perfApprQNH.toFixed(2)}{end}`;
                } else {
                    l4Cell = `{cyan}${mcdu.perfApprQNH.toFixed(0)}{end}`;
                }
            } else if (closeToDest) {
                l4Cell = "{amber}____{end}";
            } else {
                l4Cell = "{cyan}[\xa0\xa0]{end}";
            }
            mcdu.onLeftInput[3] = (value, scratchpadCallback) => {
                if (mcdu.setPerfApprQNH(value)) {
                    CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, confirmSpeed, confirmAlt, confirmCode, forPlan, inAlternate);
                } else {
                    scratchpadCallback();
                }
            };

            l3Title = "";
            l3Cell = "";
            r5Cell = "";
        } else {
            if (altitudeConstraint) {
                r3Cell = `{magenta}${altitudeConstraint}{end}`;
            }
            if (speedConstraint) {
                l3Cell = `{magenta}${speedConstraint}{end}`;
            }

            [r4Title, r4Cell] = this.formatAltErrorTitleAndValue(waypoint, verticalWaypoint);

            if (mcdu._cruiseEntered && mcdu._cruiseFlightLevel && (mcdu.flightPhaseManager.phase < FmgcFlightPhases.DESCENT || mcdu.flightPhaseManager.phase > FmgcFlightPhases.GOAROUND)) {
                r5Cell = "STEP ALTS>";
            }
        }

        mcdu.setTemplate([
            ["VERT REV {small}AT{end}{green} " + waypointIdent + "{end}"],
            [],
            [""],
            [speedLimitTitle, ""],
            [speedLimitCell, "RTA>[color]inop"],
            [l3Title, r3Title],
            [l3Cell, r3Cell],
            [l4Title, r4Title],
            [l4Cell, r4Cell],
            [""],
            ["<WIND/TEMP", r5Cell],
            [""],
            [confirmConstraint ? "{amber}*CLB{end}" : "<RETURN", confirmConstraint ? "{amber}DES*{end}" : "", confirmConstraint ? "{amber}{small}OR{end}{end}" : ""]
        ]);

        mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
            if (!showSpeedLim) {
                scratchpadCallback();
                return;
            }

            if (value === FMCMainDisplay.clrValue) {
                if (showDesSpeedLim) {
                    if (mcdu.descentSpeedLimitPilot) {
                        mcdu.descentSpeedLimit = 250;
                        mcdu.descentSpeedLimitAlt = 10000;
                    } else {
                        mcdu.descentSpeedLimit = undefined;
                        mcdu.descentSpeedLimitAlt = undefined;
                    }
                    mcdu.descentSpeedLimitPilot = false;
                } else {
                    if (mcdu.climbSpeedLimitPilot) {
                        mcdu.climbSpeedLimit = 250;
                        mcdu.climbSpeedLimitAlt = 10000;
                    } else {
                        mcdu.climbSpeedLimit = undefined;
                        mcdu.climbSpeedLimitAlt = undefined;
                    }
                    mcdu.climbSpeedLimitPilot = false;
                }
                CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
                return;
            }

            const matchResult = value.match(/^([0-9]{1,3})\/(((FL)?([0-9]{1,3}))|([0-9]{4,5}))$/);
            if (matchResult === null) {
                mcdu.setScratchpadMessage(NXSystemMessages.formatError);
                scratchpadCallback();
                return;
            }

            const speed = parseInt(matchResult[1]);
            let alt = matchResult[5] !== undefined ? parseInt(matchResult[5]) * 100 : parseInt(matchResult[6]);

            if (speed < 90 || speed > 350 || alt > 45000) {
                mcdu.setScratchpadMessage(NXSystemMessages.entryOutOfRange);
                scratchpadCallback();
                return;
            }

            alt = Math.round(alt / 10) * 10;

            if (showDesSpeedLim) {
                mcdu.descentSpeedLimit = speed;
                mcdu.descentSpeedLimitAlt = alt;
                mcdu.descentSpeedLimitPilot = true;
            } else {
                mcdu.climbSpeedLimit = speed;
                mcdu.climbSpeedLimitAlt = alt;
                mcdu.climbSpeedLimitPilot = true;
            }

            CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
        }; // SPD LIM
        mcdu.onRightInput[1] = () => {}; // RTA
        mcdu.onLeftInput[2] = async (value, scratchpadCallback) => {
            if (value === FMCMainDisplay.clrValue) {
                await mcdu.flightPlanService.setSpeedAt(wpIndex, -1, false, forPlan, inAlternate);

                mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

                this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, forPlan, inAlternate);
                return;
            }

            if (value.match(/^[0-9]{1,3}$/ === null)) {
                mcdu.setScratchpadMessage(NXSystemMessages.formatError);
                scratchpadCallback();
                return;
            }

            const speed = parseInt(value);

            if (speed < 90 || speed > 350) {
                mcdu.setScratchpadMessage(NXSystemMessages.entryOutOfRange);
                scratchpadCallback();
                return;
            }

            if (constraintType === WaypointConstraintType.Unknown) {
                CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, speed, undefined, undefined, forPlan, inAlternate);
                return;
            }

            await mcdu.flightPlanService.setSpeedAt(wpIndex, speed, constraintType === WaypointConstraintType.DES, forPlan, inAlternate);

            mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

            this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
        }; // SPD CSTR
        mcdu.onRightInput[2] = async (value, scratchpadCallback) => {
            if (value === FMCMainDisplay.clrValue) {
                await mcdu.flightPlanService.setAltitudeDescriptionAt(wpIndex, 0, false, forPlan, inAlternate);
                await mcdu.flightPlanService.setAltitudeAt(wpIndex, 0, forPlan, false, inAlternate);

                mcdu.updateConstraints();
                mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

                this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);

                return;
            }

            const matchResult = value.match(/^([+-])?(((FL)?([0-9]{1,3}))|([0-9]{4,5}))$/);

            if (matchResult === null) {
                mcdu.setScratchpadMessage(NXSystemMessages.formatError);
                scratchpadCallback();
                return;
            }

            const altitude = matchResult[5] !== undefined ? parseInt(matchResult[5]) * 100 : parseInt(matchResult[6]);
            const code = matchResult[1] === undefined ? '@' : (matchResult[1] === '-' ? '-' : '+');

            if (altitude > 45000) {
                mcdu.setScratchpadMessage(NXSystemMessages.entryOutOfRange);
                scratchpadCallback();
                return;
            }

            if (constraintType === WaypointConstraintType.Unknown) {
                CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, undefined, altitude, code, forPlan, inAlternate,);
                return;
            }

            await mcdu.flightPlanService.setAltitudeDescriptionAt(wpIndex, code, false, forPlan, inAlternate);
            await mcdu.flightPlanService.setAltitudeAt(wpIndex, altitude, false, forPlan, inAlternate);

            mcdu.updateConstraints();
            mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

            this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
        }; // ALT CSTR
        mcdu.onLeftInput[4] = () => {
            //TODO: show appropriate wind page based on waypoint
            CDUWindPage.Return = () => {
                CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
            };
            CDUWindPage.ShowPage(mcdu);
        }; // WIND
        mcdu.onRightInput[4] = () => {
            if (!mcdu._cruiseEntered || !mcdu._cruiseFlightLevel) {
                return;
            }
            CDUStepAltsPage.Return = () => {
                CDUVerticalRevisionPage.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
            };
            CDUStepAltsPage.ShowPage(mcdu);
        }; // STEP ALTS
        if (!confirmConstraint) {
            mcdu.onLeftInput[5] = () => {
                CDUFlightPlanPage.ShowPage(mcdu);
            };
        } else {
            mcdu.onLeftInput[5] = async () => {
                if (Number.isFinite(confirmSpeed)) {
                    await mcdu.flightPlanService.setSpeedAt(wpIndex, confirmSpeed, false, forPlan, inAlternate);

                    mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

                    this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
                }

                if (Number.isFinite(confirmAlt)) {
                    await mcdu.flightPlanService.setAltitudeDescriptionAt(wpIndex, confirmCode, false, forPlan, inAlternate);
                    await mcdu.flightPlanService.setAltitudeAt(wpIndex, confirmAlt, false, forPlan, inAlternate);

                    mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

                    this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
                }
            };

            mcdu.onRightInput[5] = async () => {
                if (Number.isFinite(confirmSpeed)) {
                    await mcdu.flightPlanService.setSpeedAt(wpIndex, confirmSpeed, true, forPlan, inAlternate);

                    mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

                    this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
                }
                if (Number.isFinite(confirmAlt)) {
                    await mcdu.flightPlanService.setAltitudeDescriptionAt(wpIndex, confirmCode, true, forPlan, inAlternate);
                    await mcdu.flightPlanService.setAltitudeAt(wpIndex, confirmAlt, forPlan, true, inAlternate);

                    mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

                    this.ShowPage(mcdu, waypoint, wpIndex, verticalWaypoint, verticalWaypoint, undefined, undefined, undefined, forPlan, inAlternate);
                }
            };
        }
    }

    static formatFl(constraint, transAlt) {
        if (transAlt >= 100 && constraint > transAlt) {
            return "FL" + Math.round(constraint / 100);
        }
        return constraint;
    }

    /**
     * @param mcdu
     * @param legIndex {number}
     * @param forPlan {number}
     * @param inAlternate {boolean}
     * @returns {number|(function(*, *): (*))|*|WaypointConstraintType|VerticalWaypointType}
     */
    static constraintType(mcdu, legIndex, forPlan, inAlternate) {
        const planAtIndex = mcdu.flightPlanService.get(forPlan);
        const plan = inAlternate ? planAtIndex.alternateFlightPlan : planAtIndex;

        const leg = plan.legElementAt(legIndex);

        if (leg.constraintType !== 3 /* Unknown */) {
            return leg.constraintType;
        }

        return plan.autoConstraintTypeForLegIndex(legIndex);
    }

    // constraints can be set directly by LSK on f-pln page
    static async setConstraints(mcdu, leg, legIndex, verticalWaypoint, value, scratchpadCallback, offset = 0, forPlan = 0, inAlternate = false) {
        const matchResult = value.match(/^(([0-9]{1,3})\/?)?(\/([+-])?(((FL)?([0-9]{1,3}))|([0-9]{4,5})))?$/);
        if (matchResult === null) {
            mcdu.setScratchpadMessage(NXSystemMessages.formatError);
            scratchpadCallback();
            return;
        }

        let speed;
        let alt;

        if (matchResult[2] !== undefined) {
            speed = parseInt(matchResult[2]);
        }

        const code = matchResult[4] === undefined ? '@' : (matchResult[4] === '-' ? '-' : '+');

        if (matchResult[8] !== undefined) {
            alt = parseInt(matchResult[8]) * 100;
        }

        if (matchResult[9] !== undefined) {
            alt = parseInt(matchResult[9]);
        }

        if ((speed !== undefined && (speed < 90 || speed > 350)) || (alt !== undefined && alt > 45000)) {
            mcdu.setScratchpadMessage(NXSystemMessages.entryOutOfRange);
            scratchpadCallback();
            return;
        }

        const type = CDUVerticalRevisionPage.constraintType(mcdu, legIndex, forPlan, inAlternate);

        if (type === WaypointConstraintType.Unknown) {
            CDUVerticalRevisionPage.ShowPage(mcdu, leg, legIndex, verticalWaypoint, speed, alt, code, forPlan, inAlternate);
            return;
        }

        if (speed !== undefined) {
            await mcdu.flightPlanService.setSpeedAt(legIndex, speed, type === WaypointConstraintType.DES, forPlan, inAlternate);

            mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

            CDUFlightPlanPage.ShowPage(mcdu, offset);
        }

        if (alt !== undefined) {
            await mcdu.flightPlanService.setAltitudeDescriptionAt(legIndex, code, type === WaypointConstraintType.DES, forPlan, inAlternate);
            await mcdu.flightPlanService.setAltitudeAt(legIndex, alt, type === WaypointConstraintType.DES, forPlan, inAlternate);

            mcdu.guidanceController.vnavDriver.invalidateFlightPlanProfile();

            CDUFlightPlanPage.ShowPage(mcdu, offset);
        }
    }

    static formatAltErrorTitleAndValue(waypoint, verticalWaypoint) {
        const empty = ["", ""];

        if (!waypoint || !verticalWaypoint) {
            return empty;
        }

        // No constraint
        if (waypoint.legAltitudeDescription === 0 || verticalWaypoint.isAltitudeConstraintMet) {
            return empty;
        }

        // Weird prediction error
        if (!isFinite(verticalWaypoint.altError)) {
            return empty;
        }

        let formattedAltError = (Math.round(verticalWaypoint.altError / 10) * 10).toFixed(0);
        if (verticalWaypoint.altError > 0) {
            formattedAltError = "+" + formattedAltError;
        }

        return ["ALT ERROR\xa0", "{green}{small}" + formattedAltError + "{end}{end}"];
    }
}
