class CDUPerformancePage {
    static ShowPage(mcdu, _phase = undefined) {
        mcdu.activeSystem = 'FMGC';

        switch (_phase || mcdu.flightPhaseManager.phase) {
            case FmgcFlightPhases.PREFLIGHT: CDUPerformancePage.ShowTAKEOFFPage(mcdu); break;
            case FmgcFlightPhases.TAKEOFF: CDUPerformancePage.ShowTAKEOFFPage(mcdu); break;
            case FmgcFlightPhases.CLIMB: CDUPerformancePage.ShowCLBPage(mcdu); break;
            case FmgcFlightPhases.CRUISE: CDUPerformancePage.ShowCRZPage(mcdu); break;
            case FmgcFlightPhases.DESCENT: CDUPerformancePage.ShowDESPage(mcdu); break;
            case FmgcFlightPhases.APPROACH: CDUPerformancePage.ShowAPPRPage(mcdu); break;
            case FmgcFlightPhases.GOAROUND: CDUPerformancePage.ShowGOAROUNDPage(mcdu); break;
        }
    }
    static ShowTAKEOFFPage(mcdu) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.PerformancePageTakeoff;
        CDUPerformancePage._timer = 0;
        CDUPerformancePage._lastPhase = mcdu.flightPhaseManager.phase;
        mcdu.pageUpdate = () => {
            CDUPerformancePage._timer++;
            if (CDUPerformancePage._timer >= 50) {
                if (mcdu.flightPhaseManager.phase === CDUPerformancePage._lastPhase) {
                    CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                } else {
                    CDUPerformancePage.ShowPage(mcdu);
                }
            }
        };

        // TODO SEC F-PLN
        const targetPlan = mcdu.flightPlanService.active;

        let titleColor = "white";
        if (mcdu.flightPhaseManager.phase === FmgcFlightPhases.TAKEOFF) {
            titleColor = "green";
        }

        // check if we even have an airport
        const hasOrigin = !!targetPlan.originAirport;

        // runway
        let runway = "";
        let hasRunway = false;
        if (hasOrigin) {
            const runwayObj = targetPlan.originRunway;

            if (runwayObj) {
                runway = runwayObj.ident.substring(2);
                hasRunway = true;
            }
        }

        // v speeds
        let v1 = "---";
        let vR = "---";
        let v2 = "---";
        let v1Check = "{small}\xa0\xa0\xa0{end}";
        let vRCheck = "{small}\xa0\xa0\xa0{end}";
        let v2Check = "{small}\xa0\xa0\xa0{end}";
        if (mcdu.flightPhaseManager.phase < FmgcFlightPhases.TAKEOFF) {
            v1 = "{amber}___{end}";
            if (mcdu.unconfirmedV1Speed) {
                v1Check = `{small}{cyan}${("" + mcdu.unconfirmedV1Speed).padEnd(3)}{end}{end}`;
            } else if (mcdu.v1Speed) {
                v1 = `{cyan}${("" + mcdu.v1Speed).padEnd(3)}{end}`;
            }
            mcdu.onLeftInput[0] = (value, scratchpadCallback) => {
                if (value === "") {
                    if (mcdu.unconfirmedV1Speed) {
                        mcdu.v1Speed = mcdu.unconfirmedV1Speed;
                        mcdu.unconfirmedV1Speed = undefined;
                    } else {
                        // not real: v-speed helper
                        if (mcdu.flaps && !isFinite(mcdu.zeroFuelWeight)) {
                            mcdu.setScratchpadMessage(NXSystemMessages.initializeWeightOrCg);
                        } else if (mcdu.flaps && isFinite(mcdu.zeroFuelWeight)) {
                            mcdu.setScratchpadText(mcdu._getV1Speed().toString());
                        } else {
                            mcdu.setScratchpadMessage(NXSystemMessages.formatError);
                            scratchpadCallback();
                        }
                    }
                    CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                } else {
                    if (mcdu.trySetV1Speed(value)) {
                        CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                    } else {
                        scratchpadCallback();
                    }
                }
            };
            vR = "{amber}___{end}";
            if (mcdu.unconfirmedVRSpeed) {
                vRCheck = `{small}{cyan}${("" + mcdu.unconfirmedVRSpeed).padEnd(3)}{end}{end}`;
            } else if (mcdu.vRSpeed) {
                vR = `{cyan}${("" + mcdu.vRSpeed).padEnd(3)}{end}`;
            }
            mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
                if (value === "") {
                    if (mcdu.unconfirmedVRSpeed) {
                        mcdu.vRSpeed = mcdu.unconfirmedVRSpeed;
                        mcdu.unconfirmedVRSpeed = undefined;
                    } else {
                        if (mcdu.flaps && !isFinite(mcdu.zeroFuelWeight)) {
                            mcdu.setScratchpadMessage(NXSystemMessages.initializeWeightOrCg);
                        } else if (mcdu.flaps && isFinite(mcdu.zeroFuelWeight)) {
                            mcdu.setScratchpadText(mcdu._getVRSpeed().toString());
                        } else {
                            mcdu.setScratchpadMessage(NXSystemMessages.formatError);
                            scratchpadCallback();
                        }
                    }
                    CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                } else {
                    if (mcdu.trySetVRSpeed(value)) {
                        CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                    } else {
                        scratchpadCallback();
                    }
                }
            };
            v2 = "{amber}___{end}";
            if (mcdu.unconfirmedV2Speed) {
                v2Check = `{small}{cyan}${("" + mcdu.unconfirmedV2Speed).padEnd(3)}{end}{end}`;
            } else if (mcdu.v2Speed) {
                v2 = `{cyan}${("" + mcdu.v2Speed).padEnd(3)}{end}`;
            }
            mcdu.onLeftInput[2] = (value, scratchpadCallback) => {
                if (value === "") {
                    if (mcdu.unconfirmedV2Speed) {
                        mcdu.v2Speed = mcdu.unconfirmedV2Speed;
                        mcdu.unconfirmedV2Speed = undefined;
                    } else {
                        if (mcdu.flaps && !isFinite(mcdu.zeroFuelWeight)) {
                            mcdu.setScratchpadMessage(NXSystemMessages.initializeWeightOrCg);
                        } else if (mcdu.flaps && isFinite(mcdu.zeroFuelWeight)) {
                            mcdu.setScratchpadText(mcdu._getV2Speed().toString());
                        } else {
                            mcdu.setScratchpadMessage(NXSystemMessages.formatError);
                            scratchpadCallback();
                        }
                    }
                    CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                } else {
                    if (mcdu.trySetV2Speed(value)) {
                        CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                    } else {
                        scratchpadCallback();
                    }
                }
            };
        } else {
            v1 = "\xa0\xa0\xa0";
            vR = "\xa0\xa0\xa0";
            v2 = "\xa0\xa0\xa0";
            if (mcdu.v1Speed) {
                v1 = `{green}${("" + mcdu.v1Speed).padEnd(3)}{end}`;
            }
            if (mcdu.vRSpeed) {
                vR = `{green}${("" + mcdu.vRSpeed).padEnd(3)}{end}`;
            }
            if (mcdu.v2Speed) {
                v2 = `{green}${("" + mcdu.v2Speed).padEnd(3)}{end}`;
            }
            mcdu.onLeftInput[0] = (value, scratchpadCallback) => {
                if (value !== "") {
                    mcdu.setScratchpadMessage(NXSystemMessages.notAllowed);
                    scratchpadCallback();
                }
            };
            mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
                if (value !== "") {
                    mcdu.setScratchpadMessage(NXSystemMessages.notAllowed);
                    scratchpadCallback();
                }
            };
            mcdu.onLeftInput[2] = (value, scratchpadCallback) => {
                if (value !== "") {
                    mcdu.setScratchpadMessage(NXSystemMessages.notAllowed);
                    scratchpadCallback();
                }
            };
        }

        // transition altitude - remains editable during take off
        let transAltCell = "";
        if (hasOrigin) {
            transAltCell = "[\xa0".padEnd(4, "\xa0") + "]";

            const transAlt = targetPlan.performanceData.transitionAltitude.get();
            const transAltitudeIsFromDatabase = targetPlan.performanceData.transitionAltitudeIsFromDatabase;

            if (transAlt !== undefined) {
                transAltCell = `{cyan}${transAlt}{end}`;
                if (transAltitudeIsFromDatabase) {
                    transAltCell += "[s-text]";
                }
            }

            mcdu.onLeftInput[3] = (value, scratchpadCallback) => {
                if (mcdu.trySetTakeOffTransAltitude(value)) {
                    CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                } else {
                    scratchpadCallback();
                }
            };
        }

        // thrust reduction / acceleration altitude
        const altitudeColour = hasOrigin ? (mcdu.flightPhaseManager.phase >= FmgcFlightPhases.TAKEOFF ? "green" : "cyan") : "white";

        const plan = mcdu.flightPlanService.active;
        const thrRed = plan.performanceData.thrustReductionAltitude.get();
        const thrRedPilot = plan.performanceData.thrustReductionAltitudeIsPilotEntered.get();
        const acc = plan.performanceData.accelerationAltitude.get();
        const accPilot = plan.performanceData.accelerationAltitudeIsPilotEntered.get();
        const eoAcc = plan.performanceData.engineOutAccelerationAltitude.get();
        const eoAccPilot = plan.performanceData.engineOutAccelerationAltitudeIsPilotEntered.get();

        const thrRedAcc = `{${thrRedPilot ? 'big' : 'small'}}${thrRed !== undefined ? thrRed.toFixed(0).padStart(5, '\xa0') : '-----'}{end}/{${accPilot ? 'big' : 'small'}}${acc !== undefined ? acc.toFixed(0).padEnd(5, '\xa0') : '-----'}{end}`;

        mcdu.onLeftInput[4] = (value, scratchpadCallback) => {
            if (mcdu.trySetThrustReductionAccelerationAltitude(value)) {
                CDUPerformancePage.ShowTAKEOFFPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        // eng out acceleration altitude
        const engOut = `{${eoAccPilot ? 'big' : 'small'}}${eoAcc !== undefined ? eoAcc.toFixed(0).padStart(5, '\xa0') : '-----'}{end}`;
        mcdu.onRightInput[4] = (value, scratchpadCallback) => {
            if (mcdu.trySetEngineOutAcceleration(value)) {
                CDUPerformancePage.ShowTAKEOFFPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        // center column
        let flpRetrCell = "---";
        let sltRetrCell = "---";
        let cleanCell = "---";
        if (isFinite(mcdu.zeroFuelWeight)) {
            const flapSpeed = mcdu.computedVfs;
            if (flapSpeed !== 0) {
                flpRetrCell = `{green}${flapSpeed.toFixed(0)}{end}`;
            }
            const slatSpeed = mcdu.computedVss;
            if (slatSpeed !== 0) {
                sltRetrCell = `{green}${slatSpeed.toFixed(0)}{end}`;
            }
            const cleanSpeed = mcdu.computedVgd;
            if (cleanSpeed !== 0) {
                cleanCell = `{green}${cleanSpeed.toFixed(0)}{end}`;
            }
        }
        // takeoff shift
        let toShiftCell = "{inop}----{end}\xa0";
        if (hasOrigin && hasRunway) {
            toShiftCell = "{inop}{small}[M]{end}[\xa0\xa0]*{end}";
            // TODO store and show TO SHIFT
        }

        // flaps / trim horizontal stabilizer
        let flapsThs = "[]/[\xa0\xa0\xa0][color]cyan";
        // The following line uses a special Javascript concept that is signed
        // zeroes. In Javascript -0 is strictly equal to 0, so for most cases we
        // don't care about that difference. But here, we use that fact to show
        // the pilot the precise value they entered: DN0.0 or UP0.0. The only
        // way to figure that difference out is using Object.is, as
        // Object.is(+0, -0) returns false. Alternatively we could use a helper
        // variable (yuck) or encode it using a very small, but negative value
        // such as -0.001.
        const formattedThs = mcdu.ths !== null
            ? (mcdu.ths >= 0 && !Object.is(mcdu.ths, -0) ? `UP${Math.abs(mcdu.ths).toFixed(1)}` : `DN${Math.abs(mcdu.ths).toFixed(1)}`)
            : '';
        if (mcdu.flightPhaseManager.phase < FmgcFlightPhases.TAKEOFF) {
            const flaps = mcdu.flaps !== null ? mcdu.flaps : "[]";
            const ths = formattedThs ? formattedThs : "[\xa0\xa0\xa0]";
            flapsThs = `${flaps}/${ths}[color]cyan`;
            mcdu.onRightInput[2] = (value, scratchpadCallback) => {
                if (mcdu.trySetFlapsTHS(value)) {
                    CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                } else {
                    scratchpadCallback();
                }
            };
        } else {
            const flaps = mcdu.flaps !== null ? mcdu.flaps : "";
            const ths = formattedThs ? formattedThs : "\xa0\xa0\xa0\xa0\xa0";
            flapsThs = `${flaps}/${ths}[color]green`;
        }

        // flex takeoff temperature
        let flexTakeOffTempCell = "[\xa0\xa0]°[color]cyan";
        if (mcdu.flightPhaseManager.phase < FmgcFlightPhases.TAKEOFF) {
            if (isFinite(mcdu.perfTOTemp)) {
                if (mcdu._toFlexChecked) {
                    flexTakeOffTempCell = `${mcdu.perfTOTemp.toFixed(0)}°[color]cyan`;
                } else {
                    flexTakeOffTempCell = `{small}${mcdu.perfTOTemp.toFixed(0)}{end}${flexTakeOffTempCell}[color]cyan`;
                }
            }
            mcdu.onRightInput[3] = (value, scratchpadCallback) => {
                if (mcdu._toFlexChecked) {
                    if (mcdu.setPerfTOFlexTemp(value)) {
                        CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                    } else {
                        scratchpadCallback();
                    }
                } else {
                    if (value === "" || mcdu.setPerfTOFlexTemp(value)) {
                        mcdu._toFlexChecked = true;
                        CDUPerformancePage.ShowTAKEOFFPage(mcdu);
                    } else {
                        scratchpadCallback();
                    }
                }
            };
        } else {
            if (isFinite(mcdu.perfTOTemp)) {
                flexTakeOffTempCell = `${mcdu.perfTOTemp.toFixed(0)}°[color]green`;
            } else {
                flexTakeOffTempCell = "";
            }
        }

        let next = "NEXT\xa0";
        let nextPhase = "PHASE>";
        if ((mcdu.unconfirmedV1Speed || mcdu.unconfirmedVRSpeed || mcdu.unconfirmedV2Speed || !mcdu._toFlexChecked) && mcdu.flightPhaseManager.phase < FmgcFlightPhases.TAKEOFF) {
            next = "CONFIRM\xa0";
            nextPhase = "TO DATA*";
            mcdu.onRightInput[5] = (value) => {
                mcdu.v1Speed = mcdu.unconfirmedV1Speed ? mcdu.unconfirmedV1Speed : mcdu.v1Speed;
                mcdu.vRSpeed = mcdu.unconfirmedVRSpeed ? mcdu.unconfirmedVRSpeed : mcdu.vRSpeed;
                mcdu.v2Speed = mcdu.unconfirmedV2Speed ? mcdu.unconfirmedV2Speed : mcdu.v2Speed;
                mcdu.unconfirmedV1Speed = undefined;
                mcdu.unconfirmedVRSpeed = undefined;
                mcdu.unconfirmedV2Speed = undefined;
                mcdu._toFlexChecked = true;
                CDUPerformancePage.ShowTAKEOFFPage(mcdu);
            };
        } else {
            mcdu.rightInputDelay[5] = () => {
                return mcdu.getDelaySwitchPage();
            };
            mcdu.onRightInput[5] = (value) => {
                CDUPerformancePage.ShowCLBPage(mcdu);
            };
        }

        mcdu.setTemplate([
            ["TAKE OFF RWY\xa0{green}" + runway.padStart(3, "\xa0") + "{end}[color]" + titleColor],
            ["\xa0V1\xa0\xa0FLP RETR", ""],
            [v1 + v1Check + "\xa0F=" + flpRetrCell, ""],
            ["\xa0VR\xa0\xa0SLT RETR", "TO SHIFT\xa0"],
            [vR + vRCheck + "\xa0S=" + sltRetrCell, toShiftCell],
            ["\xa0V2\xa0\xa0\xa0\xa0\xa0CLEAN", "FLAPS/THS"],
            [v2 + v2Check + "\xa0O=" + cleanCell, flapsThs],
            ["TRANS ALT", "FLEX TO TEMP"],
            [`{cyan}${transAltCell}{end}`, flexTakeOffTempCell],
            ["THR\xa0RED/ACC", "ENG\xa0OUT\xa0ACC"],
            [`{${altitudeColour}}${thrRedAcc}{end}`, `{${altitudeColour}}${engOut}{end}`],
            ["\xa0UPLINK[color]inop", next],
            ["<TO DATA[color]inop", nextPhase]
        ]);
    }
    static ShowCLBPage(mcdu, confirmAppr = false) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.PerformancePageClb;
        CDUPerformancePage._timer = 0;
        CDUPerformancePage._lastPhase = mcdu.flightPhaseManager.phase;
        mcdu.pageUpdate = () => {
            CDUPerformancePage._timer++;
            if (CDUPerformancePage._timer >= 100) {
                if (mcdu.flightPhaseManager.phase === CDUPerformancePage._lastPhase) {
                    CDUPerformancePage.ShowCLBPage(mcdu);
                } else {
                    CDUPerformancePage.ShowPage(mcdu);
                }
            }
        };
        const isPhaseActive = mcdu.flightPhaseManager.phase === FmgcFlightPhases.CLIMB;
        const titleColor = isPhaseActive ? "green" : "white";
        const isSelected = Simplane.getAutoPilotAirspeedSelected();
        const actModeCell = isSelected ? "SELECTED" : "MANAGED";
        const costIndexCell = isFinite(mcdu.costIndex) ? mcdu.costIndex.toFixed(0) + "[color]cyan" : "[][color]cyan";
        let managedSpeedCell = "";
        if (isPhaseActive) {
            if (mcdu.managedSpeedTarget === mcdu.managedSpeedClimb) {
                managedSpeedCell = "{small}" + mcdu.managedSpeedClimb.toFixed(0) + "/" + mcdu.managedSpeedClimbMach.toFixed(2).replace("0.", ".") + "{end}";
            } else if (Simplane.getAutoPilotMachModeActive() || SimVar.GetSimVarValue("K:AP_MANAGED_SPEED_IN_MACH_ON", "Bool")) {
                managedSpeedCell = "{small}" + mcdu.managedSpeedClimbMach.toFixed(2).replace("0.", ".") + "{end}";
            } else {
                managedSpeedCell = "{small}" + mcdu.managedSpeedTarget.toFixed(0) + "{end}";
            }
        } else {
            managedSpeedCell = (isSelected ? "*" : "") + mcdu.managedSpeedClimb > mcdu.managedSpeedLimit ? mcdu.managedSpeedLimit.toFixed(0) : mcdu.managedSpeedClimb.toFixed(0);

            mcdu.onLeftInput[3] = (value, scratchpadCallback) => {
                if (mcdu.trySetPreSelectedClimbSpeed(value)) {
                    CDUPerformancePage.ShowCLBPage(mcdu);
                } else {
                    scratchpadCallback();
                }
            };
        }
        const [selectedSpeedTitle, selectedSpeedCell] = CDUPerformancePage.getSelectedTitleAndValue(isPhaseActive, isSelected, mcdu.preSelectedClbSpeed);
        mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
            if (mcdu.tryUpdateCostIndex(value)) {
                CDUPerformancePage.ShowCLBPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };
        const timeLabel = mcdu.flightPhaseManager.phase >= FmgcFlightPhases.TAKEOFF ? "UTC" : "TIME";
        const bottomRowLabels = ["\xa0PREV", "NEXT\xa0"];
        const bottomRowCells = ["<PHASE", "PHASE>"];
        mcdu.leftInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        if (isPhaseActive) {
            if (confirmAppr) {
                bottomRowLabels[0] = "\xa0CONFIRM[color]amber";
                bottomRowCells[0] = "*APPR PHASE[color]amber";
                mcdu.onLeftInput[5] = async () => {
                    if (mcdu.flightPhaseManager.tryGoInApproachPhase()) {
                        CDUPerformancePage.ShowAPPRPage(mcdu);
                    }
                };
            } else {
                bottomRowLabels[0] = "\xa0ACTIVATE[color]cyan";
                bottomRowCells[0] = "{APPR PHASE[color]cyan";
                mcdu.onLeftInput[5] = () => {
                    CDUPerformancePage.ShowCLBPage(mcdu, true);
                };
            }
        } else {
            mcdu.onLeftInput[5] = () => {
                CDUPerformancePage.ShowTAKEOFFPage(mcdu);
            };
        }
        mcdu.rightInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onRightInput[5] = () => {
            CDUPerformancePage.ShowCRZPage(mcdu);
        };
        mcdu.setTemplate([
            ["CLB[color]" + titleColor],
            ["ACT MODE", "EFOB", timeLabel],
            [actModeCell + "[color]green", "6.0[color]green", "----[color]green"],
            ["\xa0CI"],
            [costIndexCell + "[color]cyan"],
            ["\xa0MANAGED"],
            ["\xa0" + managedSpeedCell + "[color]green"],
            ["\xa0" + selectedSpeedTitle],
            ["\xa0" + selectedSpeedCell + "[color]cyan"],
            [""],
            [""],
            bottomRowLabels,
            bottomRowCells
        ]);
    }
    static ShowCRZPage(mcdu, confirmAppr = false) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.PerformancePageCrz;
        CDUPerformancePage._timer = 0;
        CDUPerformancePage._lastPhase = mcdu.flightPhaseManager.phase;
        mcdu.pageUpdate = () => {
            CDUPerformancePage._timer++;
            if (CDUPerformancePage._timer >= 100) {
                if (mcdu.flightPhaseManager.phase === CDUPerformancePage._lastPhase) {
                    CDUPerformancePage.ShowCRZPage(mcdu);
                } else {
                    CDUPerformancePage.ShowPage(mcdu);
                }
            }
        };
        const isPhaseActive = mcdu.flightPhaseManager.phase === FmgcFlightPhases.CRUISE;
        const titleColor = isPhaseActive ? "green" : "white";
        const isSelected = Simplane.getAutoPilotAirspeedSelected();
        const isFlying = false;
        const actModeCell = isSelected ? "SELECTED" : "MANAGED";
        const costIndexCell = isFinite(mcdu.costIndex) ? mcdu.costIndex.toFixed(0) + "[color]cyan" : "[][color]cyan";
        let managedSpeedCell = "";
        const managedSpeed = isPhaseActive ? mcdu.managedSpeedTarget : mcdu.managedSpeedCruise;
        if (isFinite(managedSpeed)) {
            managedSpeedCell = (isSelected ? "*" : "") + managedSpeed.toFixed(0);
        }
        const [selectedSpeedTitle, selectedSpeedCell] = CDUPerformancePage.getSelectedTitleAndValue(isPhaseActive, isSelected, mcdu.preSelectedCrzSpeed);
        mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
            if (mcdu.tryUpdateCostIndex(value)) {
                CDUPerformancePage.ShowCRZPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };
        let timeLabel = "TIME";
        if (isFlying) {
            timeLabel = "UTC";
        }
        const bottomRowLabels = ["\xa0PREV", "NEXT\xa0"];
        const bottomRowCells = ["<PHASE", "PHASE>"];
        mcdu.leftInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        if (isPhaseActive) {
            if (confirmAppr) {
                bottomRowLabels[0] = "\xa0CONFIRM[color]amber";
                bottomRowCells[0] = "*APPR PHASE[color]amber";
                mcdu.onLeftInput[5] = async () => {
                    if (mcdu.flightPhaseManager.tryGoInApproachPhase()) {
                        CDUPerformancePage.ShowAPPRPage(mcdu);
                    }
                };
            } else {
                bottomRowLabels[0] = "\xa0ACTIVATE[color]cyan";
                bottomRowCells[0] = "{APPR PHASE[color]cyan";
                mcdu.onLeftInput[5] = () => {
                    CDUPerformancePage.ShowCRZPage(mcdu, true);
                };
            }
        } else {
            mcdu.onLeftInput[3] = (value, scratchpadCallback) => {
                if (mcdu.trySetPreSelectedCruiseSpeed(value)) {
                    CDUPerformancePage.ShowCRZPage(mcdu);
                } else {
                    scratchpadCallback();
                }
            };
            mcdu.onLeftInput[5] = () => {
                CDUPerformancePage.ShowCLBPage(mcdu);
            };
        }
        mcdu.rightInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onRightInput[5] = () => {
            CDUPerformancePage.ShowDESPage(mcdu);
        };
        mcdu.setTemplate([
            ["CRZ[color]" + titleColor],
            ["ACT MODE", "EFOB", timeLabel],
            [actModeCell + "[color]green", "6.0[color]green", "----[color]green"],
            ["\xa0CI"],
            [costIndexCell + "[color]cyan"],
            ["\xa0MANAGED"],
            ["\xa0" + managedSpeedCell + "[color]green"],
            ["\xa0" + selectedSpeedTitle],
            ["\xa0" + selectedSpeedCell + "[color]cyan"],
            ["", "DES CABIN RATE>"],
            ["", "-350FT/MIN[color]green"],
            bottomRowLabels,
            bottomRowCells
        ]);
    }
    static ShowDESPage(mcdu, confirmAppr = false) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.PerformancePageDes;
        CDUPerformancePage._timer = 0;
        CDUPerformancePage._lastPhase = mcdu.flightPhaseManager.phase;
        mcdu.pageUpdate = () => {
            CDUPerformancePage._timer++;
            if (CDUPerformancePage._timer >= 100) {
                if (mcdu.flightPhaseManager.phase === CDUPerformancePage._lastPhase) {
                    CDUPerformancePage.ShowDESPage(mcdu);
                } else {
                    CDUPerformancePage.ShowPage(mcdu);
                }
            }
        };
        const isPhaseActive = mcdu.flightPhaseManager.phase === FmgcFlightPhases.DESCENT;
        const titleColor = isPhaseActive ? "green" : "white";
        const isFlying = false;
        const isSelected = Simplane.getAutoPilotAirspeedSelected();
        const actModeCell = isSelected ? "SELECTED" : "MANAGED";
        const costIndexCell = isFinite(mcdu.costIndex) ? mcdu.costIndex.toFixed(0) + "[color]cyan" : "[][color]cyan";
        let managedSpeedCell = "";
        const managedSpeed = isPhaseActive ? mcdu.managedSpeedTarget : mcdu.managedSpeedDescend;
        if (isFinite(managedSpeed)) {
            managedSpeedCell = (isSelected ? "*" : "") + managedSpeed.toFixed(0);
        }
        const [selectedSpeedTitle, selectedSpeedCell] = CDUPerformancePage.getSelectedTitleAndValue(isPhaseActive, isSelected, mcdu.preSelectedDesSpeed);
        let timeLabel = "TIME";
        if (isFlying) {
            timeLabel = "UTC";
        }
        const bottomRowLabels = ["\xa0PREV", "NEXT\xa0"];
        const bottomRowCells = ["<PHASE", "PHASE>"];
        mcdu.leftInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        if (isPhaseActive) {
            if (confirmAppr) {
                bottomRowLabels[0] = "\xa0CONFIRM[color]amber";
                bottomRowCells[0] = "*APPR PHASE[color]amber";
                mcdu.onLeftInput[5] = async () => {
                    if (mcdu.flightPhaseManager.tryGoInApproachPhase()) {
                        CDUPerformancePage.ShowAPPRPage(mcdu);
                    }
                };
            } else {
                bottomRowLabels[0] = "\xa0ACTIVATE[color]cyan";
                bottomRowCells[0] = "{APPR PHASE[color]cyan";
                mcdu.onLeftInput[5] = () => {
                    CDUPerformancePage.ShowDESPage(mcdu, true);
                };
            }
        } else {
            mcdu.onLeftInput[3] = (value, scratchpadCallback) => {
                if (mcdu.trySetPreSelectedDescentSpeed(value)) {
                    CDUPerformancePage.ShowDESPage(mcdu);
                } else {
                    scratchpadCallback();
                }
            };
            mcdu.onLeftInput[5] = () => {
                CDUPerformancePage.ShowCRZPage(mcdu);
            };
        }
        mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
            if (mcdu.tryUpdateCostIndex(value)) {
                CDUPerformancePage.ShowDESPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };
        mcdu.rightInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onRightInput[5] = () => {
            CDUPerformancePage.ShowAPPRPage(mcdu);
        };
        mcdu.setTemplate([
            ["DES[color]" + titleColor],
            ["ACT MODE", "EFOB", timeLabel],
            [actModeCell + "[color]green", "6.0[color]green", "----[color]green"],
            ["\xa0CI"],
            [costIndexCell + "[color]cyan"],
            ["\xa0MANAGED"],
            ["\xa0" + managedSpeedCell + "[color]green"],
            ["\xa0" + selectedSpeedTitle],
            ["\xa0" + selectedSpeedCell + "[color]cyan"],
            [""],
            [""],
            bottomRowLabels,
            bottomRowCells
        ]);
    }

    static ShowAPPRPage(mcdu) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.PerformancePageAppr;

        const plan = mcdu.flightPlanService.active;

        CDUPerformancePage._timer = 0;
        CDUPerformancePage._lastPhase = mcdu.flightPhaseManager.phase;
        mcdu.pageUpdate = () => {
            CDUPerformancePage._timer++;
            if (CDUPerformancePage._timer >= 100) {
                if (mcdu.flightPhaseManager.phase === CDUPerformancePage._lastPhase) {
                    CDUPerformancePage.ShowAPPRPage(mcdu);
                }
            }
        };

        const closeToDest = plan.destinationAirport && 0 <= 180; // TODO port over (fms-v2)

        let qnhCell = "[\xa0\xa0][color]cyan";
        if (isFinite(mcdu.perfApprQNH)) {
            if (mcdu.perfApprQNH < 500) {
                qnhCell = mcdu.perfApprQNH.toFixed(2) + "[color]cyan";
            } else {
                qnhCell = mcdu.perfApprQNH.toFixed(0) + "[color]cyan";
            }
        } else if (closeToDest) {
            qnhCell = "____[color]amber";
        }
        mcdu.onLeftInput[0] = (value, scratchpadCallback) => {
            if (mcdu.setPerfApprQNH(value)) {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        let tempCell = "{cyan}[\xa0]°{end}";
        if (isFinite(mcdu.perfApprTemp)) {
            tempCell = "{cyan}" + (mcdu.perfApprTemp >= 0 ? "+" : "-") + ("" + Math.abs(mcdu.perfApprTemp).toFixed(0)).padStart(2).replace(/ /g, "\xa0") + "°{end}";
        } else if (closeToDest) {
            tempCell = "{amber}___°{end}";
        }
        mcdu.onLeftInput[1] = (value, scratchpadCallback) => {
            if (mcdu.setPerfApprTemp(value)) {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };
        let magWindHeadingCell = "[\xa0]";
        if (isFinite(mcdu.perfApprWindHeading)) {
            magWindHeadingCell = ("" + mcdu.perfApprWindHeading.toFixed(0)).padStart(3, 0);
        }
        let magWindSpeedCell = "[\xa0]";
        if (isFinite(mcdu.perfApprWindSpeed)) {
            magWindSpeedCell = mcdu.perfApprWindSpeed.toFixed(0).padStart(3, "0");
        }
        mcdu.onLeftInput[2] = (value, scratchpadCallback) => {
            if (mcdu.setPerfApprWind(value)) {
                mcdu.updateTowerHeadwind();
                mcdu.updatePerfSpeeds();
                CDUPerformancePage.ShowAPPRPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        let transAltCell = "\xa0".repeat(5);
        const hasDestination = !!plan.destinationAirport;

        if (hasDestination) {
            const transitionLevel = plan.performanceData.transitionLevel.get();

            if (transitionLevel !== undefined) {
                transAltCell = (transitionLevel * 100).toFixed(0).padEnd(5, "\xa0");

                if (plan.performanceData.transitionLevelIsFromDatabase.get()) {
                    transAltCell = `{small}${transAltCell}{end}`;
                }
            } else {
                transAltCell = "[\xa0]".padEnd(5, "\xa0");
            }
        }
        mcdu.onLeftInput[3] = (value, scratchpadCallback) => {
            if (mcdu.setPerfApprTransAlt(value)) {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        let vappCell = "---";
        let vlsCell = "---";
        let flpRetrCell = "---";
        let sltRetrCell = "---";
        let cleanCell = "---";
        if (isFinite(mcdu.zeroFuelWeight) && mcdu.approachSpeeds && mcdu.approachSpeeds.valid) {
            vappCell = `{cyan}{small}${mcdu.approachSpeeds.vapp.toFixed(0)}{end}{end}`;
            vlsCell = `{green}${mcdu.approachSpeeds.vls.toFixed(0)}{end}`;
            flpRetrCell = `{green}${mcdu.approachSpeeds.f.toFixed(0)}{end}`;
            sltRetrCell = `{green}${mcdu.approachSpeeds.s.toFixed(0)}{end}`;
            cleanCell = `{green}${mcdu.approachSpeeds.gd.toFixed(0)}{end}`;
        }
        if (isFinite(mcdu.vApp)) { // pilot override
            vappCell = `{cyan}${mcdu.vApp.toFixed(0).padStart(3, "\xa0")}{end}`;
        }
        mcdu.onLeftInput[4] = (value, scratchpadCallback) => {
            if (mcdu.setPerfApprVApp(value)) {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };
        mcdu.onRightInput[4] = () => {
            mcdu.setPerfApprFlaps3(!mcdu.perfApprFlaps3);
            mcdu.updatePerfSpeeds();
            CDUPerformancePage.ShowAPPRPage(mcdu);
        };

        let baroCell = "[\xa0\xa0\xa0]";
        if (isFinite(mcdu.perfApprMDA)) {
            baroCell = mcdu.perfApprMDA.toFixed(0);
        }
        mcdu.onRightInput[1] = (value, scratchpadCallback) => {
            if (mcdu.setPerfApprMDA(value) && mcdu.setPerfApprDH(FMCMainDisplay.clrValue)) {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        const approach = plan.approach;
        const isILS = approach && approach.type === 5;
        let radioLabel = "";
        let radioCell = "";
        if (isILS) {
            radioLabel = "RADIO";
            if (isFinite(mcdu.perfApprDH)) {
                radioCell = mcdu.perfApprDH.toFixed(0);
            } else if (mcdu.perfApprDH === "NO DH") {
                radioCell = "NO DH";
            } else {
                radioCell = "[\xa0]";
            }
            mcdu.onRightInput[2] = (value, scratchpadCallback) => {
                if (mcdu.setPerfApprDH(value) && mcdu.setPerfApprMDA(FMCMainDisplay.clrValue)) {
                    CDUPerformancePage.ShowAPPRPage(mcdu);
                } else {
                    scratchpadCallback();
                }
            };
        }

        const bottomRowLabels = ["\xa0PREV", "NEXT\xa0"];
        const bottomRowCells = ["<PHASE", "PHASE>"];
        let titleColor = "white";
        if (mcdu.flightPhaseManager.phase === FmgcFlightPhases.APPROACH) {
            bottomRowLabels[0] = "";
            bottomRowCells[0] = "";
            titleColor = "green";
        } else {
            if (mcdu.flightPhaseManager.phase === FmgcFlightPhases.GOAROUND) {
                mcdu.leftInputDelay[5] = () => {
                    return mcdu.getDelaySwitchPage();
                };
                mcdu.onLeftInput[5] = (value) => {
                    CDUPerformancePage.ShowGOAROUNDPage(mcdu);
                };
            } else {
                mcdu.leftInputDelay[5] = () => {
                    return mcdu.getDelaySwitchPage();
                };
                mcdu.onLeftInput[5] = (value) => {
                    CDUPerformancePage.ShowDESPage(mcdu);
                };
            }
        }
        if (mcdu.flightPhaseManager.phase === FmgcFlightPhases.GOAROUND) {
            bottomRowLabels[1] = "";
            bottomRowCells[1] = "";
        } else {
            mcdu.rightInputDelay[5] = () => {
                return mcdu.getDelaySwitchPage();
            };
            mcdu.onRightInput[5] = (value) => {
                CDUPerformancePage.ShowGOAROUNDPage(mcdu);
            };
        }

        let titleCell = `${"\xa0".repeat(5)}{${titleColor}}APPR{end}\xa0`;
        if (approach && approach.name) {
            titleCell += `{green}${approach.name}{end}` + "\xa0".repeat(24 - 10 - approach.name.length);
        } else {
            titleCell += "\xa0".repeat(24 - 10);
        }

        mcdu.setTemplate([
            /* t  */[titleCell],
            /* 1l */["QNH"],
            /* 1L */[qnhCell],
            /* 2l */["TEMP", "BARO"],
            /* 2L */[`${tempCell}${"\xa0".repeat(6)}O=${cleanCell}`, baroCell + "[color]cyan"],
            /* 3l */["MAG WIND", radioLabel],
            /* 3L */[`{cyan}${magWindHeadingCell}°/${magWindSpeedCell}{end}\xa0\xa0S=${sltRetrCell}`, radioCell + "[color]cyan"],
            /* 4l */["TRANS ALT"],
            /* 4L */[`{cyan}${transAltCell}{end}${"\xa0".repeat(5)}F=${flpRetrCell}`],
            /* 5l */["VAPP\xa0\xa0\xa0VLS", "LDG CONF\xa0"],
            /* 5L */[`${vappCell}${"\xa0".repeat(4)}${vlsCell}`, mcdu.perfApprFlaps3 ? "{cyan}CONF3/{end}{small}FULL{end}*" : "{cyan}FULL/{end}{small}CONF3{end}*"],
            /* 6l */bottomRowLabels,
            /* 6L */bottomRowCells,
        ]);
    }

    static ShowGOAROUNDPage(mcdu, confirmAppr = false) {
        mcdu.clearDisplay();
        mcdu.page.Current = mcdu.page.PerformancePageGoAround;
        CDUPerformancePage._timer = 0;
        CDUPerformancePage._lastPhase = mcdu.flightPhaseManager.phase;
        mcdu.pageUpdate = () => {
            CDUPerformancePage._timer++;
            if (CDUPerformancePage._timer >= 100) {
                if (mcdu.flightPhaseManager.phase === CDUPerformancePage._lastPhase) {
                    CDUPerformancePage.ShowGOAROUNDPage(mcdu);
                } else {
                    CDUPerformancePage.ShowPage(mcdu);
                }
            }
        };

        const haveDestination = mcdu.flightPlanService.active.destinationAirport !== undefined;

        const titleColor = mcdu.flightPhaseManager.phase === FmgcFlightPhases.GOAROUND ? "green" : "white";
        const altitudeColour = haveDestination ? (mcdu.flightPhaseManager.phase >= FmgcFlightPhases.GOAROUND ? "green" : "cyan") : "white";

        const plan = mcdu.flightPlanService.active;
        const thrRed = plan.performanceData.missedThrustReductionAltitude.get();
        const thrRedPilot = plan.performanceData.missedThrustReductionAltitudeIsPilotEntered.get();
        const acc = plan.performanceData.missedAccelerationAltitude.get();
        const accPilot = plan.performanceData.missedAccelerationAltitudeIsPilotEntered.get();
        const eoAcc = plan.performanceData.missedEngineOutAccelerationAltitude.get();
        const eoAccPilot = plan.performanceData.missedEngineOutAccelerationAltitudeIsPilotEntered.get();

        const thrRedAcc = `{${thrRedPilot ? 'big' : 'small'}}${thrRed !== undefined ? thrRed.toFixed(0).padStart(5, '\xa0') : '-----'}{end}/{${accPilot ? 'big' : 'small'}}${acc !== undefined ? acc.toFixed(0).padEnd(5, '\xa0') : '-----'}{end}`;
        const engOut = `{${eoAccPilot ? 'big' : 'small'}}${eoAcc !== undefined ? eoAcc.toFixed(0).padStart(5, '\xa0') : '-----'}{end}`;

        mcdu.onLeftInput[4] = (value, scratchpadCallback) => {
            if (mcdu.trySetThrustReductionAccelerationAltitudeGoaround(value)) {
                CDUPerformancePage.ShowGOAROUNDPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        mcdu.onRightInput[4] = (value, scratchpadCallback) => {
            if (mcdu.trySetEngineOutAccelerationAltitudeGoaround(value)) {
                CDUPerformancePage.ShowGOAROUNDPage(mcdu);
            } else {
                scratchpadCallback();
            }
        };

        let flpRetrCell = "---";
        let sltRetrCell = "---";
        let cleanCell = "---";
        if (isFinite(mcdu.zeroFuelWeight)) {
            const flapSpeed = mcdu.computedVfs;
            if (isFinite(flapSpeed)) {
                flpRetrCell = `{green}${flapSpeed.toFixed(0).padEnd(3, '\xa0')}{end}`;
            }
            const slatSpeed = mcdu.computedVss;
            if (isFinite(slatSpeed)) {
                sltRetrCell = `{green}${slatSpeed.toFixed(0).padEnd(3, '\xa0')}{end}`;
            }
            const cleanSpeed = mcdu.computedVgd;
            if (isFinite(cleanSpeed)) {
                cleanCell = `{green}${cleanSpeed.toFixed(0).padEnd(3, '\xa0')}{end}`;
            }
        }

        const bottomRowLabels = ["\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0", "\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0"];
        const bottomRowCells = ["\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0", "\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0"];
        if (mcdu.flightPhaseManager.phase === FmgcFlightPhases.GOAROUND) {
            if (confirmAppr) {
                bottomRowLabels[0] = "\xa0{amber}CONFIRM{amber}\xa0\xa0\xa0\xa0";
                bottomRowCells[0] = "{amber}*APPR\xa0PHASE{end}\xa0";
                mcdu.leftInputDelay[5] = () => {
                    return mcdu.getDelaySwitchPage();
                };
                mcdu.onLeftInput[5] = async () => {
                    if (mcdu.flightPhaseManager.tryGoInApproachPhase()) {
                        CDUPerformancePage.ShowAPPRPage(mcdu);
                    }
                };
            } else {
                bottomRowLabels[0] = "\xa0{cyan}ACTIVATE{end}\xa0\xa0\xa0";
                bottomRowCells[0] = "{cyan}{APPR\xa0PHASE{end}\xa0";
                mcdu.leftInputDelay[5] = () => {
                    return mcdu.getDelaySwitchPage();
                };
                mcdu.onLeftInput[5] = () => {
                    CDUPerformancePage.ShowGOAROUNDPage(mcdu, true);
                };
            }
            bottomRowLabels[1] = "\xa0\xa0\xa0\xa0\xa0\xa0\xa0{white}NEXT{end}\xa0";
            bottomRowCells[1] = "\xa0\xa0\xa0\xa0\xa0\xa0{white}PHASE>{end}";
            mcdu.rightInputDelay[5] = () => {
                return mcdu.getDelaySwitchPage();
            };
            mcdu.onRightInput[5] = () => {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            };
        } else {
            bottomRowLabels[0] = "\xa0{white}PREV{end}\xa0\xa0\xa0\xa0\xa0\xa0\xa0";
            bottomRowCells[0] = "{white}<PHASE{end}\xa0\xa0\xa0\xa0\xa0\xa0";
            mcdu.leftInputDelay[5] = () => {
                return mcdu.getDelaySwitchPage();
            };
            mcdu.onLeftInput[5] = () => {
                CDUPerformancePage.ShowAPPRPage(mcdu);
            };
        }

        mcdu.setTemplate([
            [`{${titleColor}}\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0GO\xa0AROUND\xa0\xa0\xa0\xa0\xa0\xa0{end}`],
            ["", "", "\xa0\xa0\xa0\xa0\xa0FLP\xa0RETR\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0"],
            ["", "", `\xa0\xa0\xa0\xa0\xa0\xa0\xa0F=${flpRetrCell}\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0`],
            ["", "", "\xa0\xa0\xa0\xa0\xa0SLT RETR\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0"],
            ["", "", `\xa0\xa0\xa0\xa0\xa0\xa0\xa0S=${sltRetrCell}\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0`],
            ["", "", "\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0CLEAN\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0"],
            ["", "", `\xa0\xa0\xa0\xa0\xa0\xa0\xa0O=${cleanCell}\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0`],
            [""],
            [""],
            ["", "", "THR\xa0RED/ACC\xa0\xa0ENG\xa0OUT\xa0ACC"],
            ["", "", `{${altitudeColour}}${thrRedAcc}\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0${engOut}{end}`],
            ["", "", bottomRowLabels.join("")],
            ["", "", bottomRowCells.join("")],
        ]);
    }

    static getSelectedTitleAndValue(_isPhaseActive, _isSelected, _preSel) {
        if (_isPhaseActive) {
            return _isSelected ? ["SELECTED", "" + Math.round(Simplane.getAutoPilotMachModeActive() ? SimVar.GetGameVarValue('FROM MACH TO KIAS', 'number', Simplane.getAutoPilotMachHoldValue()) : Simplane.getAutoPilotAirspeedHoldValue())] : ["", ""];
        } else {
            return ["PRESEL", isFinite(_preSel) ? "" + _preSel : "*[ ]"];
        }
    }
}
CDUPerformancePage._timer = 0;
