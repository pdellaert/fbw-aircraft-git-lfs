class FMCMainDisplay extends BaseAirliners {
    constructor() {
        super(...arguments);
        this.currentFlightPlanWaypointIndex = -1;
        this._title = undefined;
        this._pageCurrent = undefined;
        this._pageCount = undefined;
        this._labels = [];
        this._lines = [];
        this._inOut = undefined;
        this.onLeftInput = [];
        this.onRightInput = [];
        this.leftInputDelay = [];
        this.rightInputDelay = [];
        this.costIndex = 0;
        this.lastUserInput = "";
        this.isDisplayingErrorMessage = false;
        this.isDisplayingTypeTwoMessage = false;
        this.routeIndex = 0;
        this.coRoute = "";
        this.tmpOrigin = "";
        this.transitionAltitude = 10000;
        this.perfTOTemp = 20;
        this._overridenFlapApproachSpeed = NaN;
        this._overridenSlatApproachSpeed = NaN;
        this._routeFinalFuelWeight = 0;
        this._routeFinalFuelTime = 30;
        this._routeFinalFuelTimeDefault = 30;
        this._routeReservedWeight = 0;
        this._routeReservedPercent = 5;
        this.takeOffWeight = NaN;
        this.landingWeight = NaN;
        this.averageWind = 0;
        this.perfApprQNH = NaN;
        this.perfApprTemp = NaN;
        this.perfApprWindHeading = NaN;
        this.perfApprWindSpeed = NaN;
        this.perfApprTransAlt = NaN;
        this.vApp = NaN;
        this.perfApprMDA = NaN;
        this.perfApprDH = NaN;
        this._lockConnectIls = false;
        this._apNavIndex = 1;
        this._apLocalizerOn = false;
        this._canSwitchToNav = false;
        this._vhf1Frequency = 0;
        this._vhf2Frequency = 0;
        this._vor1Frequency = 0;
        this._vor1Course = 0;
        this._vor2Frequency = 0;
        this._vor2Course = 0;
        this._ilsFrequency = 0;
        this._ilsFrequencyPilotEntered = false;
        this._ilsCourse = 0;
        this._adf1Frequency = 0;
        this._adf2Frequency = 0;
        this._rcl1Frequency = 0;
        this._pre2Frequency = 0;
        this._atc1Frequency = 0;
        this._radioNavOn = false;
        this._debug = 0;
        this._checkFlightPlan = 0;

        this._windDirections = {
            TAILWIND : "TL",
            HEADWIND : "HD"
        };
        this._fuelPlanningPhases = {
            PLANNING : 1,
            IN_PROGRESS : 2,
            COMPLETED : 3
        };
        this._zeroFuelWeightZFWCGEntered = false;
        this._taxiEntered = false;
        this._windDir = this._windDirections.HEADWIND;
        this._DistanceToAlt = 0;
        this._routeAltFuelWeight = 0;
        this._routeAltFuelTime = 0;
        this._routeTripFuelWeight = 0;
        this._routeTripTime = 0;
        this._defaultTaxiFuelWeight = 0.2;
        this._rteRsvPercentOOR = false;
        this._rteReservedEntered = false;
        this._rteFinalCoeffecient = 0;
        this._rteFinalEntered = false;
        this._routeAltFuelEntered = false;
        this._minDestFob = 0;
        this._minDestFobEntered = false;
        this._defaultRouteFinalTime = 45;
        this._fuelPredDone = false;
        this._fuelPlanningPhase = this._fuelPlanningPhases.PLANNING;
        this._blockFuelEntered = false;
        /* CPDLC Fields */
        this._cpdlcAtcCenter = "";
        this.tropo = "";
    }

    onUpdate(_deltaTime) {
        super.onUpdate(_deltaTime);
    }

    getTitle() {
        if (this._title === undefined) {
            this._title = this._titleElement.textContent;
        }
        return this._title;
    }

    setTitle(content) {
        let color = content.split("[color]")[1];
        if (!color) {
            color = "white";
        }
        this._title = content.split("[color]")[0];
        this._titleElement.classList.remove("white", "cyan", "yellow", "green", "amber", "red", "magenta", "inop");
        this._titleElement.classList.add(color);
        this._titleElement.textContent = this._title;
    }

    setTitleLeft(content) {
        if (!content) {
            this._titleLeftElement.textContent = "";
            return;
        }
        let color = content.split("[color]")[1];
        if (!color) {
            color = "white";
        }
        this._titleLeft = content.split("[color]")[0];
        this._titleLeftElement.classList.remove("white", "blue", "yellow", "green", "red", "magenta", "inop");
        this._titleLeftElement.classList.add(color);
        this._titleLeftElement.textContent = this._titleLeft;
    }

    setPageCurrent(value) {
        if (typeof (value) === "number") {
            this._pageCurrent = value;
        } else if (typeof (value) === "string") {
            this._pageCurrent = parseInt(value);
        }
        this._pageCurrentElement.textContent = (this._pageCurrent > 0 ? this._pageCurrent : "") + "";
    }

    setPageCount(value) {
        if (typeof (value) === "number") {
            this._pageCount = value;
        } else if (typeof (value) === "string") {
            this._pageCount = parseInt(value);
        }
        this._pageCountElement.textContent = (this._pageCount > 0 ? this._pageCount : "") + "";
        if (this._pageCount === 0) {
            this.getChildById("page-slash").textContent = "";
        } else {
            this.getChildById("page-slash").textContent = "/";
        }
    }

    setLabel(label, row, col = -1) {
        if (col >= this._labelElements[row].length) {
            return;
        }
        if (!this._labels[row]) {
            this._labels[row] = [];
        }
        if (!label) {
            label = "";
        }
        if (col === -1) {
            for (let i = 0; i < this._labelElements[row].length; i++) {
                this._labels[row][i] = "";
                this._labelElements[row][i].textContent = "";
            }
            col = 0;
        }
        if (label === "__FMCSEPARATOR") {
            label = "------------------------";
        }
        if (label !== "") {
            if (label.indexOf("[b-text]") !== -1) {
                label = label.replace("[b-text]", "");
                this._lineElements[row][col].classList.remove("s-text");
                this._lineElements[row][col].classList.add("msg-text");
            } else {
                this._lineElements[row][col].classList.remove("msg-text");
            }

            let color = label.split("[color]")[1];
            if (!color) {
                color = "white";
            }
            const e = this._labelElements[row][col];
            e.classList.remove("white", "cyan", "yellow", "green", "amber", "red", "magenta", "inop");
            e.classList.add(color);
            label = label.split("[color]")[0];
        }
        this._labels[row][col] = label;
        this._labelElements[row][col].textContent = label;
    }

    setLine(content, row, col = -1) {
        if (col >= this._lineElements[row].length) {
            return;
        }
        if (!content) {
            content = "";
        }
        if (!this._lines[row]) {
            this._lines[row] = [];
        }
        if (col === -1) {
            for (let i = 0; i < this._lineElements[row].length; i++) {
                this._lines[row][i] = "";
                this._lineElements[row][i].textContent = "";
            }
            col = 0;
        }
        if (content === "__FMCSEPARATOR") {
            content = "------------------------";
        }
        if (content !== "") {
            if (content.indexOf("[s-text]") !== -1) {
                content = content.replace("[s-text]", "");
                this._lineElements[row][col].classList.add("s-text");
            } else {
                this._lineElements[row][col].classList.remove("s-text");
            }
            let color = content.split("[color]")[1];
            if (!color) {
                color = "white";
            }
            const e = this._lineElements[row][col];
            e.classList.remove("white", "cyan", "yellow", "green", "amber", "red", "magenta", "inop");
            e.classList.add(color);
            content = content.split("[color]")[0];
        }
        this._lines[row][col] = content;
        this._lineElements[row][col].textContent = this._lines[row][col];
    }

    get inOut() {
        return this.getInOut();
    }

    getInOut() {
        if (this._inOut === undefined) {
            this._inOut = this._inOutElement.textContent;
        }
        return this._inOut;
    }

    set inOut(v) {
        this.setInOut(v);
    }

    setInOut(content) {
        this._inOut = content;
        this._inOutElement.textContent = this._inOut;
    }

    setTemplate(template, large = false) {
        if (template[0]) {
            this.setTitle(template[0][0]);
            this.setPageCurrent(template[0][1]);
            this.setPageCount(template[0][2]);
            this.setTitleLeft(template[0][3]);
        }
        for (let i = 0; i < 6; i++) {
            let tIndex = 2 * i + 1;
            if (template[tIndex]) {
                if (large) {
                    if (template[tIndex][1] !== undefined) {
                        this.setLine(template[tIndex][0], i, 0);
                        this.setLine(template[tIndex][1], i, 1);
                        this.setLine(template[tIndex][2], i, 2);
                        this.setLine(template[tIndex][3], i, 3);
                    } else {
                        this.setLine(template[tIndex][0], i, -1);
                    }
                } else {
                    if (template[tIndex][1] !== undefined) {
                        this.setLabel(template[tIndex][0], i, 0);
                        this.setLabel(template[tIndex][1], i, 1);
                        this.setLabel(template[tIndex][2], i, 2);
                        this.setLabel(template[tIndex][3], i, 3);
                    } else {
                        this.setLabel(template[tIndex][0], i, -1);
                    }
                }
            }
            tIndex = 2 * i + 2;
            if (template[tIndex]) {
                if (template[tIndex][1] !== undefined) {
                    this.setLine(template[tIndex][0], i, 0);
                    this.setLine(template[tIndex][1], i, 1);
                    this.setLine(template[tIndex][2], i, 2);
                    this.setLine(template[tIndex][3], i, 3);
                } else {
                    this.setLine(template[tIndex][0], i, -1);
                }
            }
        }
        if (template[13]) {
            this.setInOut(template[13][0]);
        }
        SimVar.SetSimVarValue("L:AIRLINER_MCDU_CURRENT_FPLN_WAYPOINT", "number", this.currentFlightPlanWaypointIndex);
    }

    get cruiseFlightLevel() {
        return this._cruiseFlightLevel;
    }

    set cruiseFlightLevel(fl) {
        this._cruiseFlightLevel = Math.round(fl);
        SimVar.SetSimVarValue("L:AIRLINER_CRUISE_ALTITUDE", "number", this._cruiseFlightLevel * 100);
    }

    clearUserInput() {
        if (!this.isDisplayingErrorMessage && !this.isDisplayingTypeTwoMessage) {
            this.lastUserInput = this.inOut;
            this.inOut = "";
            this._inOutElement.className = "white";
        }
        return this.lastUserInput;
    }

    tryClearOldUserInput() {
        if (!this.isDisplayingErrorMessage && !this.isDisplayingTypeTwoMessage) {
            this.lastUserInput = "";
        }
        this.tryShowMessage();
    }

    _showTypeOneMessage(message, color = false) {
        if (!this.isDisplayingErrorMessage && !this.isDisplayingTypeTwoMessage && this.inOut) {
            this.lastUserInput = this.inOut;
        }
        this.isDisplayingErrorMessage = true;
        this.inOut = message;
        this._inOutElement.className = color ? "amber" : "white";
    }

    setCruiseFlightLevelAndTemperature(input) {
        if (input === A320_Neo_CDU_MainDisplay.clrValue) {
            this.cruiseFlightLevel = undefined;
            this.cruiseTemperature = undefined;
            return true;
        }
        const flString = input.split("/")[0].replace("FL", "");
        const tempString = input.split("/")[1];
        const onlyTemp = flString.length === 0;

        if (tempString) {
            const temp = parseFloat(tempString);
            if (isFinite(temp)) {
                if (temp > -270 && temp < 100) {
                    this.cruiseTemperature = temp;
                } else {
                    if (onlyTemp) {
                        this.addNewMessage(NXSystemMessages.entryOutOfRange);
                        return false;
                    }
                }
            } else {
                if (onlyTemp) {
                    this.addNewMessage(NXSystemMessages.notAllowed);
                    return false;
                }
            }
        }
        if (flString) {
            if (this.trySetCruiseFl(parseFloat(flString))) {
                if (SimVar.GetSimVarValue("L:A32NX_CRZ_ALT_SET_INITIAL", "bool") === 1 && SimVar.GetSimVarValue("L:A32NX_GOAROUND_PASSED", "bool") === 1) {
                    SimVar.SetSimVarValue("L:A32NX_NEW_CRZ_ALT", "number", this.cruiseFlightLevel);
                } else {
                    SimVar.SetSimVarValue("L:A32NX_CRZ_ALT_SET_INITIAL", "bool", 1);
                }
                return true;
            }
            return false;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    tryUpdateCostIndex(costIndex) {
        const value = parseInt(costIndex);
        if (isFinite(value)) {
            if (value >= 0) {
                if (value < 1000) {
                    this.costIndex = value;
                    return true;
                } else {
                    this.addNewMessage(NXSystemMessages.entryOutOfRange);
                    return false;
                }
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    /**
     * Any tropopause altitude up to 60,000 ft is able to be entered
     * @param {string | number} tropo Format: NNNN or NNNNN Leading 0’s must be included. Entry is rounded to the nearest 10 ft
     * @return {boolean} Whether tropopause could be set or not
     */
    tryUpdateTropo(tropo) {
        const _tropo = typeof tropo === 'number' ? tropo.toString() : tropo;
        if (_tropo.match(/^(?=(\D*\d){4,5}\D*$)/g)) {
            const value = parseInt(_tropo.padEnd(5, '0'));
            if (isFinite(value)) {
                if (value >= 0 && value <= 60000) {
                    const valueRounded = Math.round(value / 10) * 10;
                    this.tropo = valueRounded.toString();
                    return true;
                }
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    ensureCurrentFlightPlanIsTemporary(callback = EmptyCallback.Boolean) {
        if (this.flightPlanManager.getCurrentFlightPlanIndex() === 0) {
            this.flightPlanManager.copyCurrentFlightPlanInto(1, () => {
                this.flightPlanManager.setCurrentFlightPlanIndex(1, (result) => {
                    SimVar.SetSimVarValue("L:FMC_FLIGHT_PLAN_IS_TEMPORARY", "number", 1);
                    SimVar.SetSimVarValue("L:MAP_SHOW_TEMPORARY_FLIGHT_PLAN", "number", 1);
                    callback(result);
                });
            });
        } else {
            callback(true);
        }
    }

    tryUpdateFromTo(fromTo, callback = EmptyCallback.Boolean) {
        if (fromTo === A320_Neo_CDU_MainDisplay.clrValue) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return callback(false);
        }
        const from = fromTo.split("/")[0];
        const to = fromTo.split("/")[1];
        this.dataManager.GetAirportByIdent(from).then((airportFrom) => {
            if (airportFrom) {
                this.dataManager.GetAirportByIdent(to).then((airportTo) => {
                    if (airportTo) {
                        this.eraseTemporaryFlightPlan(() => {
                            this.flightPlanManager.clearFlightPlan(() => {
                                this.flightPlanManager.setOrigin(airportFrom.icao, () => {
                                    this.tmpOrigin = airportFrom.ident;
                                    this.flightPlanManager.setDestination(airportTo.icao, () => {
                                        this.tmpOrigin = airportTo.ident;
                                        callback(true);
                                    });
                                });
                            });
                        });
                    } else {
                        this.addNewMessage(NXSystemMessages.notInDatabase);
                        callback(false);
                    }
                });
            } else {
                this.addNewMessage(NXSystemMessages.notInDatabase);
                callback(false);
            }
        });
    }

    /**
     * Computes distance between destination and alternate destination
     */
    tryUpdateDistanceToAlt() {
        this._DistanceToAlt = Avionics.Utils.computeGreatCircleDistance(
            this.flightPlanManager.getDestination().infos.coordinates,
            this.altDestination.infos.coordinates
        );
    }

    isAltFuelInRange(fuel) {
        return 0 < fuel && fuel < (this.blockFuel - this._routeTripFuelWeight);
    }

    async trySetRouteAlternateFuel(altFuel) {
        if (altFuel === A320_Neo_CDU_MainDisplay.clrValue) {
            this._routeAltFuelEntered = false;
            return true;
        }
        const value = parseFloat(altFuel) * this._conversionWeight;
        if (isFinite(value)) {
            if (this.isAltFuelInRange(value)) {
                this._routeAltFuelEntered = true;
                this._routeAltFuelWeight = value;
                this._routeAltFuelTime = minutesTohhmm(A32NX_FuelPred.computeUserAltTime(this._routeAltFuelWeight * 1000, 290));
                return true;
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
                return false;
            }
        }
        this.addNewMessage(NXSystemMessages.formatError);
        return false;
    }

    isMinDestFobInRange(fuel) {
        return 0 <= fuel && fuel <= 80.0;
    }

    async trySetMinDestFob(fuel) {
        if (fuel === A320_Neo_CDU_MainDisplay.clrValue) {
            this._minDestFobEntered = false;
            return true;
        }
        const value = parseFloat(fuel) * this._conversionWeight;
        if (isFinite(value)) {
            if (this.isMinDestFobInRange(value)) {
                this._minDestFobEntered = true;
                if (value < this._minDestFob) {
                    this.addNewMessage(NXSystemMessages.checkMinDestFob);
                }
                this._minDestFob = value;
                return true;
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
            }
        }
        this.addNewMessage(NXSystemMessages.formatError);
        return false;
    }

    async tryUpdateAltDestination(altDestIdent) {
        if (altDestIdent === "NONE" || altDestIdent === A320_Neo_CDU_MainDisplay.clrValue) {
            this.altDestination = undefined;
            this._DistanceToAlt = 0;
            return true;
        }
        const airportAltDest = await this.dataManager.GetAirportByIdent(altDestIdent);
        if (airportAltDest) {
            this.altDestination = airportAltDest;
            this.tryUpdateDistanceToAlt();
            return true;
        }
        this.addNewMessage(NXSystemMessages.notInDatabase);
        return false;
    }

    /**
     * Updates the Fuel weight cell to tons. Uses a place holder FL120 for 30 min
     */
    tryUpdateRouteFinalFuel() {
        if (this._routeFinalFuelTime <= 0) {
            this._routeFinalFuelTime = this._defaultRouteFinalTime;
        }
        this._routeFinalFuelWeight = A32NX_FuelPred.computeHoldingTrackFF(this.zeroFuelWeight, 120) / 1000;
        this._rteFinalCoeffecient = A32NX_FuelPred.computeHoldingTrackFF(this.zeroFuelWeight, 120) / 30;
    }

    /**
     * Updates the alternate fuel and time values using a place holder FL of 330 until that can be set
     */
    tryUpdateRouteAlternate() {
        if (this._DistanceToAlt < 20) {
            this._routeAltFuelWeight = 0;
            this._routeAltFuelTime = 0;
        } else {
            const placeholderFl = 120;
            let airDistance = 0;
            if (this._windDir === this._windDirections.TAILWIND) {
                airDistance = A32NX_FuelPred.computeAirDistance(Math.round(this._DistanceToAlt), this.averageWind);
            } else if (this._windDir === this._windDirections.HEADWIND) {
                airDistance = A32NX_FuelPred.computeAirDistance(Math.round(this._DistanceToAlt), -this.averageWind);
            }

            const deviation = (this.zeroFuelWeight + this._routeFinalFuelWeight - A32NX_FuelPred.refWeight) * A32NX_FuelPred.computeNumbers(airDistance, placeholderFl, A32NX_FuelPred.computations.CORRECTIONS, true);
            if ((20 < airDistance && airDistance < 200) && (100 < placeholderFl && placeholderFl < 290)) { //This will always be true until we can setup alternate routes
                this._routeAltFuelWeight = (A32NX_FuelPred.computeNumbers(airDistance, placeholderFl, A32NX_FuelPred.computations.FUEL, true) + deviation) / 1000;
                this._routeAltFuelTime = A32NX_FuelPred.computeNumbers(airDistance, placeholderFl, A32NX_FuelPred.computations.TIME, true);
            }
        }
    }

    /**
     * Attempts to calculate trip information. Is dynamic in that it will use liveDistanceTo the destination rather than a
     * static distance. Works down to 20NM airDistance and FL100 Up to 3100NM airDistance and FL390, anything out of those ranges and values
     * won't be updated.
     */
    tryUpdateRouteTrip(dynamic = false) {
        let airDistance = 0;
        const groundDistance = dynamic ? this.flightPlanManager.getDestination().liveDistanceTo : this.flightPlanManager.getDestination().cumulativeDistanceInFP;
        if (this._windDir === this._windDirections.TAILWIND) {
            airDistance = A32NX_FuelPred.computeAirDistance(groundDistance, this.averageWind);
        } else if (this._windDir === this._windDirections.HEADWIND) {
            airDistance = A32NX_FuelPred.computeAirDistance(groundDistance, -this.averageWind);
        }

        let altToUse = this.cruiseFlightLevel;
        // Use the cruise level for calculations otherwise after cruise use descent altitude down to 10,000 feet.
        if (this.currentFlightPhase >= FlightPhase.FLIGHT_PHASE_DESCENT) {
            altToUse = SimVar.GetSimVarValue("PLANE ALTITUDE", 'Feet') / 100;
        }

        if ((20 <= airDistance && airDistance <= 3100) && (100 <= altToUse && altToUse <= 390)) {
            const deviation = (this.zeroFuelWeight + this._routeFinalFuelWeight + this._routeAltFuelWeight - A32NX_FuelPred.refWeight) * A32NX_FuelPred.computeNumbers(airDistance, altToUse, A32NX_FuelPred.computations.CORRECTIONS, false);

            this._routeTripFuelWeight = (A32NX_FuelPred.computeNumbers(airDistance, altToUse, A32NX_FuelPred.computations.FUEL, false) + deviation) / 1000;
            this._routeTripTime = A32NX_FuelPred.computeNumbers(airDistance, altToUse, A32NX_FuelPred.computations.TIME, false);
        }
    }

    tryUpdateMinDestFob() {
        this._minDestFob = this._routeAltFuelWeight + this.getRouteFinalFuelWeight();
    }

    tryUpdateTOW() {
        this.takeOffWeight = this.getGW() - this.taxiFuelWeight;
    }

    tryUpdateLW() {
        this.landingWeight = this.takeOffWeight - this._routeTripFuelWeight;
    }

    /**
     * Computes extra fuel
     * @param {boolean}useFOB - States whether to use the FOB rather than block fuel when computing extra fuel
     * @returns {number}
     */
    tryGetExtraFuel(useFOB = false) {
        if (useFOB) {
            return this.getFOB() - this.getTotalTripFuelCons() - this._minDestFob - this.taxiFuelWeight;
        } else {
            return this.blockFuel - this.getTotalTripFuelCons() - this._minDestFob - this.taxiFuelWeight;
        }
    }

    /**
     * EXPERIMENTAL
     * Attempts to calculate the extra time
     */
    tryGetExtraTime(useFOB = false) {
        if (this.tryGetExtraFuel(useFOB) <= 0) {
            return 0;
        }
        const tempWeight = this.getGW() - this._minDestFob;
        const tempFFCoefficient = A32NX_FuelPred.computeHoldingTrackFF(tempWeight, 180) / 30;
        return (this.tryGetExtraFuel(useFOB) * 1000) / tempFFCoefficient;
    }

    getRouteAltFuelWeight() {
        return this._routeAltFuelWeight;
    }

    getRouteAltFuelTime() {
        return this._routeAltFuelTime;
    }

    setOriginRunwayIndex(runwayIndex, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            this.flightPlanManager.setDepartureProcIndex(-1, () => {
                this.flightPlanManager.setOriginRunwayIndex(runwayIndex, () => {
                    return callback(true);
                });
            });
        });
    }

    setRunwayIndex(runwayIndex, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            const routeOriginInfo = this.flightPlanManager.getOrigin().infos;
            if (!this.flightPlanManager.getOrigin()) {
                this.addNewMessage(NXFictionalMessages.noOriginSet);
                return callback(false);
            } else if (runwayIndex === -1) {
                this.flightPlanManager.setDepartureRunwayIndex(-1, () => {
                    this.flightPlanManager.setOriginRunwayIndex(-1, () => {
                        return callback(true);
                    });
                });
            } else if (routeOriginInfo instanceof AirportInfo) {
                if (routeOriginInfo.oneWayRunways[runwayIndex]) {
                    this.flightPlanManager.setDepartureRunwayIndex(runwayIndex, () => {
                        return callback(true);
                    });
                }
            } else {
                this.addNewMessage(NXSystemMessages.notInDatabase);
                callback(false);
            }
        });
    }

    setDepartureIndex(departureIndex, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            const currentRunway = this.flightPlanManager.getDepartureRunway();
            this.flightPlanManager.setDepartureProcIndex(departureIndex, () => {
                if (currentRunway) {
                    const departure = this.flightPlanManager.getDeparture();
                    const departureRunwayIndex = departure.runwayTransitions.findIndex(t => {
                        return t.name.indexOf(currentRunway.designation) !== -1;
                    });
                    if (departureRunwayIndex >= -1) {
                        return this.flightPlanManager.setDepartureRunwayIndex(departureRunwayIndex, () => {
                            return callback(true);
                        });
                    }
                }
                return callback(true);
            });
        });
    }

    removeDeparture() {
        this.flightPlanManager.removeDeparture();
        return true;
    }

    setApproachTransitionIndex(transitionIndex, callback = EmptyCallback.Boolean) {
        const arrivalIndex = this.flightPlanManager.getArrivalProcIndex();
        this.ensureCurrentFlightPlanIsTemporary(() => {
            this.flightPlanManager.setApproachTransitionIndex(transitionIndex, () => {
                this.flightPlanManager.setArrivalProcIndex(arrivalIndex, () => {
                    callback(true);
                });
            });
        });
    }

    setArrivalProcIndex(arrivalIndex, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            this.flightPlanManager.setArrivalProcIndex(arrivalIndex, () => {
                callback(true);
            });
        });
    }

    setArrivalIndex(arrivalIndex, transitionIndex, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            this.flightPlanManager.setArrivalEnRouteTransitionIndex(transitionIndex, () => {
                this.flightPlanManager.setArrivalProcIndex(arrivalIndex, () => {
                    callback(true);
                });
            });
        });
    }

    removeArrival() {
        this.flightPlanManager.removeDeparture();
        return true;
    }

    setApproachIndex(approachIndex, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            this.flightPlanManager.setApproachIndex(approachIndex, () => {
                const frequency = this.flightPlanManager.getApproachNavFrequency();
                if (isFinite(frequency)) {
                    const freq = Math.round(frequency * 100) / 100;
                    if (this.connectIlsFrequency(freq)) {
                        this._ilsFrequencyPilotEntered = false;
                        SimVar.SetSimVarValue("L:FLIGHTPLAN_APPROACH_ILS", "number", freq);
                        const approach = this.flightPlanManager.getApproach();
                        if (approach && approach.name && approach.name.indexOf("ILS") !== -1) {
                            const runway = this.flightPlanManager.getApproachRunway();
                            if (runway) {
                                SimVar.SetSimVarValue("L:FLIGHTPLAN_APPROACH_COURSE", "number", runway.direction);
                            }
                        }
                    }
                }
                callback(true);
            });
        });
    }

    updateFlightNo(flightNo, callback = EmptyCallback.Boolean) {
        if (flightNo.length > 7) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return callback(false);
        }

        SimVar.SetSimVarValue("ATC FLIGHT NUMBER", "string", flightNo, "FMC").then(() => {
            NXApi.connectTelex(flightNo)
                .then(() => {
                    callback(true);
                })
                .catch((err) => {
                    if (err !== NXApi.disabledError) {
                        this.addNewMessage(NXFictionalMessages.fltNbrInUse);
                        return callback(false);
                    }

                    return callback(true);
                });
        });
    }

    updateCoRoute(coRoute, callback = EmptyCallback.Boolean) {
        if (coRoute.length > 2) {
            if (coRoute.length < 10) {
                if (coRoute === "NONE") {
                    this.coRoute = undefined;
                } else {
                    this.coRoute = coRoute;
                }
                return callback(true);
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return callback(false);
    }

    getTotalTripTime() {
        return this._routeTripTime;
    }

    getTotalTripFuelCons() {
        return this._routeTripFuelWeight;
    }

    getOrSelectVORsByIdent(ident, callback) {
        this.dataManager.GetVORsByIdent(ident).then((navaids) => {
            if (!navaids || navaids.length === 0) {
                this.addNewMessage(NXSystemMessages.notInDatabase);
                return callback(undefined);
            }
            if (navaids.length === 1) {
                return callback(navaids[0]);
            }
            A320_Neo_CDU_SelectWptPage.ShowPage(this, navaids, callback);
        });
    }
    getOrSelectNDBsByIdent(ident, callback) {
        this.dataManager.GetNDBsByIdent(ident).then((navaids) => {
            if (!navaids || navaids.length === 0) {
                this.addNewMessage(NXSystemMessages.notInDatabase);
                return callback(undefined);
            }
            if (navaids.length === 1) {
                return callback(navaids[0]);
            }
            A320_Neo_CDU_SelectWptPage.ShowPage(this, navaids, callback);
        });
    }

    activateDirectToWaypoint(waypoint, callback = EmptyCallback.Void) {
        const waypoints = this.flightPlanManager.getWaypoints();
        const indexInFlightPlan = waypoints.findIndex(w => {
            return w.icao === waypoint.icao;
        });
        let i = 1;
        const removeWaypointMethod = (callback = EmptyCallback.Void) => {
            if (i < indexInFlightPlan) {
                this.flightPlanManager.removeWaypoint(1, i === indexInFlightPlan - 1, () => {
                    i++;
                    removeWaypointMethod(callback);
                });
            } else {
                callback();
            }
        };
        removeWaypointMethod(() => {
            this.flightPlanManager.activateDirectTo(waypoint.infos.icao, callback);
        });
    }

    insertWaypoint(newWaypointTo, index, callback = EmptyCallback.Boolean) {
        this.ensureCurrentFlightPlanIsTemporary(async () => {
            this.getOrSelectWaypointByIdent(newWaypointTo, (waypoint) => {
                if (!waypoint) {
                    this.addNewMessage(NXSystemMessages.notInDatabase);
                    return callback(false);
                }
                this.flightPlanManager.addWaypoint(waypoint.icao, index, () => {
                    return callback(true);
                });
            });
        });
    }

    async insertWaypointsAlongAirway(lastWaypointIdent, index, airwayName, callback = EmptyCallback.Boolean) {
        const referenceWaypoint = this.flightPlanManager.getWaypoint(index - 1);
        const lastWaypointIdentPadEnd = lastWaypointIdent.padEnd(5, " ");
        if (referenceWaypoint) {
            const infos = referenceWaypoint.infos;
            if (infos instanceof WayPointInfo) {
                await referenceWaypoint.infos.UpdateAirways(); // Sometimes the waypoint is initialized without waiting to the airways array to be filled
                const airway = infos.airways.find(a => {
                    return a.name === airwayName;
                });
                if (airway) {
                    const firstIndex = airway.icaos.indexOf(referenceWaypoint.icao);
                    const lastWaypointIcao = airway.icaos.find(icao => icao.substring(7, 12) === lastWaypointIdentPadEnd);
                    const lastIndex = airway.icaos.indexOf(lastWaypointIcao);
                    if (firstIndex >= 0) {
                        if (lastIndex >= 0) {
                            let inc = 1;
                            if (lastIndex < firstIndex) {
                                inc = -1;
                            }
                            index -= 1;
                            const count = Math.abs(lastIndex - firstIndex);
                            for (let i = 1; i < count + 1; i++) { // 9 -> 6
                                const syncInsertWaypointByIcao = async (icao, idx) => {
                                    return new Promise(resolve => {
                                        console.log("add icao:" + icao + " @ " + idx);
                                        this.flightPlanManager.addWaypoint(icao, idx, () => {
                                            const waypoint = this.flightPlanManager.getWaypoint(idx);
                                            waypoint.infos.airwayIn = airwayName;
                                            if (i < count) {
                                                waypoint.infos.airwayOut = airwayName;
                                            }
                                            console.log("icao:" + icao + " added");
                                            resolve();
                                        });
                                    });
                                };

                                await syncInsertWaypointByIcao(airway.icaos[firstIndex + i * inc], index + i);
                            }
                            callback(true);
                            return;
                        }
                        this.addNewMessage(NXFictionalMessages.secondIndexNotFound);
                        return callback(false);
                    }
                    this.addNewMessage(NXFictionalMessages.firstIndexNotFound);
                    return callback(false);
                }
                this.addNewMessage(NXFictionalMessages.noRefWpt);
                return callback(false);
            }
            this.addNewMessage(NXFictionalMessages.noWptInfos);
            return callback(false);
        }
        this.addNewMessage(NXFictionalMessages.noRefWpt);
        return callback(false);
    }

    // Copy airway selections from temporary to active flightplan
    async copyAirwaySelections() {
        const temporaryFPWaypoints = this.flightPlanManager.getWaypoints(1);
        const activeFPWaypoints = this.flightPlanManager.getWaypoints(0);
        for (let i = 0; i < activeFPWaypoints.length; i++) {
            if (activeFPWaypoints[i].infos && temporaryFPWaypoints[i] && activeFPWaypoints[i].icao === temporaryFPWaypoints[i].icao && temporaryFPWaypoints[i].infos) {
                activeFPWaypoints[i].infos.airwayIn = temporaryFPWaypoints[i].infos.airwayIn;
                activeFPWaypoints[i].infos.airwayOut = temporaryFPWaypoints[i].infos.airwayOut;
            }
        }
    }

    removeWaypoint(index, callback = EmptyCallback.Void) {
        this.ensureCurrentFlightPlanIsTemporary(() => {
            this.flightPlanManager.removeWaypoint(index, true, callback);
        });
    }

    eraseTemporaryFlightPlan(callback = EmptyCallback.Void) {
        this.flightPlanManager.setCurrentFlightPlanIndex(0, () => {
            SimVar.SetSimVarValue("L:FMC_FLIGHT_PLAN_IS_TEMPORARY", "number", 0);
            SimVar.SetSimVarValue("L:MAP_SHOW_TEMPORARY_FLIGHT_PLAN", "number", 0);
            callback();
        });
    }

    insertTemporaryFlightPlan(callback = EmptyCallback.Void) {
        if (this.flightPlanManager.getCurrentFlightPlanIndex() === 1) {
            this.flightPlanManager.copyCurrentFlightPlanInto(0, () => {
                this.flightPlanManager.setCurrentFlightPlanIndex(0, () => {
                    SimVar.SetSimVarValue("L:FMC_FLIGHT_PLAN_IS_TEMPORARY", "number", 0);
                    SimVar.SetSimVarValue("L:MAP_SHOW_TEMPORARY_FLIGHT_PLAN", "number", 0);
                    callback();
                });
            });
        }
    }

    checkVSpeedDisagree(mcdu) {
        return mcdu.v1Speed && mcdu.vRSpeed && mcdu.v2Speed && mcdu.v1Speed <= mcdu.vRSpeed && mcdu.vRSpeed <= mcdu.v2Speed;
    }

    trySetV1Speed(s) {
        if (!/^\d+$/.test(s)) {
            this.addNewMessage(NXSystemMessages.formatError);
            return false;
        }
        const v = parseInt(s);
        if (isFinite(v)) {
            if (v >= 90 && v < 1000) {
                this.v1Speed = v;
                SimVar.SetSimVarValue("L:AIRLINER_V1_SPEED", "Knots", this.v1Speed);
                if ((v > SimVar.GetSimVarValue("L:AIRLINER_VR_SPEED", "Knots") || v > SimVar.GetSimVarValue("L:AIRLINER_V2_SPEED", "Knots")) && SimVar.GetSimVarValue("L:AIRLINER_VR_SPEED", "Knots") !== -1 && SimVar.GetSimVarValue("L:AIRLINER_V2_SPEED", "Knots") !== -1) {
                    this.addNewMessage(NXSystemMessages.vToDisagree, this.checkVSpeedDisagree);
                }
                return true;
            }
            this.addNewMessage(NXSystemMessages.entryOutOfRange);
            return false;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetVRSpeed(s) {
        if (!/^\d+$/.test(s)) {
            this.addNewMessage(NXSystemMessages.formatError);
            return false;
        }
        const v = parseInt(s);
        if (isFinite(v)) {
            if (v >= 90 && v < 1000) {
                this.vRSpeed = v;
                SimVar.SetSimVarValue("L:AIRLINER_VR_SPEED", "Knots", this.vRSpeed);
                if ((v < SimVar.GetSimVarValue("L:AIRLINER_V1_SPEED", "Knots") || v > SimVar.GetSimVarValue("L:AIRLINER_V2_SPEED", "Knots")) && SimVar.GetSimVarValue("L:AIRLINER_V1_SPEED", "Knots") !== -1 && SimVar.GetSimVarValue("L:AIRLINER_V2_SPEED", "Knots") !== -1) {
                    this.addNewMessage(NXSystemMessages.vToDisagree, this.checkVSpeedDisagree);
                }
                return true;
            }
            this.addNewMessage(NXSystemMessages.entryOutOfRange);
            return false;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetV2Speed(s) {
        if (!/^\d+$/.test(s)) {
            this.addNewMessage(NXSystemMessages.formatError);
            return false;
        }
        const v = parseInt(s);
        if (isFinite(v)) {
            if (v >= 90 && v < 1000) {
                this.v2Speed = v;
                SimVar.SetSimVarValue("L:AIRLINER_V2_SPEED", "Knots", this.v2Speed);
                if ((v < SimVar.GetSimVarValue("L:AIRLINER_V1_SPEED", "Knots") || v < SimVar.GetSimVarValue("L:AIRLINER_VR_SPEED", "Knots")) && SimVar.GetSimVarValue("L:AIRLINER_V1_SPEED", "Knots") !== -1 && SimVar.GetSimVarValue("L:AIRLINER_VR_SPEED", "Knots") !== -1) {
                    this.addNewMessage(NXSystemMessages.vToDisagree, this.checkVSpeedDisagree);
                }
                return true;
            }
            this.addNewMessage(NXSystemMessages.entryOutOfRange);
            return false;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetTransAltitude(s) {
        if (!/^\d+$/.test(s)) {
            this.addNewMessage(NXSystemMessages.formatError);
            return false;
        }
        const v = parseInt(s);
        if (isFinite(v) && v > 0) {
            this.transitionAltitude = v;
            SimVar.SetSimVarValue("L:AIRLINER_TRANS_ALT", "Number", v);
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetThrustReductionAccelerationAltitude(s) {
        let thrRed = NaN;
        let accAlt = NaN;
        if (s) {
            const sSplit = s.split("/");
            thrRed = parseInt(sSplit[0]);
            accAlt = parseInt(sSplit[1]);
        }
        if (isFinite(thrRed) || isFinite(accAlt)) {
            if (isFinite(thrRed)) {
                this.thrustReductionAltitude = thrRed;
                SimVar.SetSimVarValue("L:AIRLINER_THR_RED_ALT", "Number", this.thrustReductionAltitude);
            }
            if (isFinite(accAlt)) {
                this.accelerationAltitude = accAlt;
                SimVar.SetSimVarValue("L:AIRLINER_ACC_ALT", "Number", this.accelerationAltitude);
            }
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetThrustReductionAccelerationAltitudeGoaround(s) {
        let thrRed = NaN;
        let accAlt = NaN;
        if (s) {
            const sSplit = s.split("/");
            thrRed = parseInt(sSplit[0]);
            accAlt = parseInt(sSplit[1]);
        }
        if ((isFinite(thrRed) || isFinite(accAlt)) && thrRed <= accAlt) {
            if (isFinite(thrRed)) {
                this.thrustReductionAltitudeGoaround = thrRed;
                SimVar.SetSimVarValue("L:AIRLINER_THR_RED_ALT_GOAROUND", "Number", this.thrustReductionAltitudeGoaround);
            }
            if (isFinite(accAlt)) {
                this.accelerationAltitudeGoaround = accAlt;
                SimVar.SetSimVarValue("L:AIRLINER_ACC_ALT_GOAROUND", "Number", this.accelerationAltitudeGoaround);
            }
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetEngineOutAcceleration(s) {
        const engOutAcc = parseInt(s);
        if (isFinite(engOutAcc)) {
            this.engineOutAccelerationGoaround = engOutAcc;
            SimVar.SetSimVarValue("L:AIRLINER_ENG_OUT_ACC_ALT_GOAROUND", "Number", this.engineOutAccelerationGoaround);
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getFlapSpeed() {
        const phase = Simplane.getCurrentFlightPhase();
        const flapsHandleIndex = Simplane.getFlapsHandleIndex();
        let flapSpeed = 100;
        if (flapsHandleIndex === 1) {
            let slatSpeed = 0;
            if (phase === FlightPhase.FLIGHT_PHASE_TAKEOFF || phase === FlightPhase.FLIGHT_PHASE_CLIMB || phase === FlightPhase.FLIGHT_PHASE_GOAROUND) {
                slatSpeed = Simplane.getStallSpeedPredicted(flapsHandleIndex - 1) * 1.25;
            } else if (phase === FlightPhase.FLIGHT_PHASE_DESCENT || phase === FlightPhase.FLIGHT_PHASE_APPROACH) {
                slatSpeed = this.getSlatApproachSpeed();
            }
            return slatSpeed;
        }
        if (flapsHandleIndex === 2 || flapsHandleIndex === 3) {
            if (phase === FlightPhase.FLIGHT_PHASE_TAKEOFF || phase === FlightPhase.FLIGHT_PHASE_CLIMB || phase === FlightPhase.FLIGHT_PHASE_GOAROUND) {
                flapSpeed = Simplane.getStallSpeedPredicted(flapsHandleIndex - 1) * 1.26;
            } else if (phase === FlightPhase.FLIGHT_PHASE_DESCENT || phase === FlightPhase.FLIGHT_PHASE_APPROACH) {
                flapSpeed = this.getFlapApproachSpeed();
            }
        }
        return flapSpeed;
    }

    getCleanTakeOffSpeed() {
        const dWeight = (this.getWeight() - 42) / (75 - 42);
        return 204 + 40 * dWeight;
    }

    updateCleanTakeOffSpeed() {
        const toGreenDotSpeed = this.getCleanTakeOffSpeed();
        if (isFinite(toGreenDotSpeed)) {
            SimVar.SetSimVarValue("L:AIRLINER_TO_GREEN_DOT_SPD", "Number", toGreenDotSpeed);
        }
    }

    setPerfTOFlexTemp(s) {
        const value = parseFloat(s);
        if (isFinite(value) && value > -270 && value < 150) {
            this.perfTOTemp = value;
            SimVar.SetSimVarValue("L:AIRLINER_TO_FLEX_TEMP", "Number", this.perfTOTemp);
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getCrzManagedSpeed() {
        let dCI = this.costIndex / 999;
        dCI = dCI * dCI;
        let speed = 290 * (1 - dCI) + 310 * dCI;
        if (SimVar.GetSimVarValue("PLANE ALTITUDE", "feet") < 10000) {
            speed = Math.min(speed, 250);
        }
        return Math.min(speed, this.getSpeedConstraint(false));
    }

    getDesManagedSpeed() {
        const dCI = this.costIndex / 999;
        const flapsHandleIndex = Simplane.getFlapsHandleIndex();
        if (flapsHandleIndex !== 0) {
            return this.getFlapSpeed();
        }
        let speed = 288 * (1 - dCI) + 300 * dCI;
        if (SimVar.GetSimVarValue("PLANE ALTITUDE", "feet") < 10000) {
            speed = Math.min(speed, 250);
        }
        return Math.min(speed, this.getSpeedConstraint(false));
    }

    getFlapApproachSpeed() {
        if (isFinite(this._overridenFlapApproachSpeed)) {
            return this._overridenFlapApproachSpeed;
        }
        const dWeight = SimVar.GetSimVarValue("TOTAL WEIGHT", "kilograms") / 1000;
        switch (true) {
            case (dWeight <= 50):
                return 131;
            case (dWeight <= 55):
                return Math.ceil(131 + 1.2 * (dWeight - 50));
            case (dWeight <= 60):
                return Math.ceil(137 + 1.4 * (dWeight - 55));
            case (dWeight <= 65):
                return Math.ceil(144 + dWeight - 60);
            case (dWeight <= 70):
                return Math.ceil(149 + 1.2 * (dWeight - 65));
            case (dWeight <= 75):
                return Math.ceil(155 + dWeight - 70);
            default:
                return Math.ceil(160 + 1.20 * (dWeight - 75));
        }
    }

    getSlatApproachSpeed() {
        if (isFinite(this._overridenSlatApproachSpeed)) {
            return this._overridenSlatApproachSpeed;
        }
        const dWeight = SimVar.GetSimVarValue("TOTAL WEIGHT", "kilograms") / 1000;
        switch (true) {
            case (dWeight <= 45):
                return Math.ceil(152 + 1.8 * (dWeight - 40));
            case (dWeight <= 50):
                return Math.ceil(161 + 1.6 * (dWeight - 45));
            case (dWeight <= 55):
                return Math.ceil(169 + 1.8 * (dWeight - 50));
            case (dWeight <= 60):
                return Math.ceil(178 + 1.6 * (dWeight - 55));
            default:
                return Math.ceil(186 + 1.4 * (dWeight - 60));
        }
    }

    getCleanApproachSpeed() {
        let dWeight = (this.getWeight() - 42) / (75 - 42);
        dWeight = Math.min(Math.max(dWeight, 0), 1);
        const base = Math.max(172, this.getVLS() + 5);
        return base + 40 * dWeight;
    }

    updateCleanApproachSpeed() {
        const apprGreenDotSpeed = this.getCleanApproachSpeed();
        if (isFinite(apprGreenDotSpeed)) {
            SimVar.SetSimVarValue("L:AIRLINER_APPR_GREEN_DOT_SPD", "Number", apprGreenDotSpeed);
        }
    }

    isTaxiFuelInRange(taxi) {
        return 0 <= taxi && taxi <= 9.9;
    }

    /**
     * Attempts to predict required block fuel for trip
     * @returns {boolean}
     */
    tryFuelPlanning() {
        if (this._fuelPlanningPhase === this._fuelPlanningPhases.IN_PROGRESS) {
            this._blockFuelEntered = true;
            this._fuelPlanningPhase = this._fuelPlanningPhases.COMPLETED;
            return true;
        }
        const tempRouteFinalFuelTime = this._routeFinalFuelTime;
        this.tryUpdateRouteFinalFuel();
        this.tryUpdateRouteAlternate();
        this.tryUpdateRouteTrip();
        this.tryUpdateMinDestFob();

        this._routeFinalFuelTime = tempRouteFinalFuelTime;
        this._routeFinalFuelWeight = (this._routeFinalFuelTime * this._rteFinalCoeffecient) / 1000;

        this.blockFuel = this.getTotalTripFuelCons() + this._minDestFob + this.taxiFuelWeight;
        this._fuelPlanningPhase = this._fuelPlanningPhases.IN_PROGRESS;
        return true;
    }

    async trySetTaxiFuelWeight(s) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this.taxiFuelWeight = this._defaultTaxiFuelWeight;
            this._taxiEntered = false;
            return true;
        }
        if (!/[0-9]+(\.[0-9][0-9]?)?/.test(s)) {
            this.addNewMessage(NXSystemMessages.formatError);
            return false;
        }
        const value = parseFloat(s) * this._conversionWeight;
        if (isFinite(value) && value >= 0) {
            if (this.isTaxiFuelInRange(value)) {
                this._taxiEntered = true;
                this.taxiFuelWeight = value;
                return true;
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
                return false;
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getRouteFinalFuelWeight() {
        if (isFinite(this._routeFinalFuelWeight)) {
            this._routeFinalFuelWeight = (this._routeFinalFuelTime * this._rteFinalCoeffecient) / 1000;
            return this._routeFinalFuelWeight;
        }
    }

    getRouteFinalFuelTime() {
        return this._routeFinalFuelTime;
    }

    isFinalFuelInRange(fuel) {
        return 0 <= fuel && fuel <= 100;
    }

    isFinalTimeInRange(time) {
        const convertedTime = hhmmToMinutes(time.padStart(4,"0"));
        return 0 <= convertedTime && convertedTime <= 90;
    }

    /**
     * This method is used to set initial Final Time for when INIT B is making predictions
     * @param {String} s - containing time value
     * @returns {boolean}
     */
    async trySetRouteFinalTime(s) {
        if (s) {
            if (s === A320_Neo_CDU_MainDisplay.clrValue) {
                this._routeFinalFuelTime = this._routeFinalFuelTimeDefault;
                return true;
            }
            const rteFinalTime = s.split("/")[1];
            if (rteFinalTime !== undefined) {
                if (this.isFinalTimeInRange(rteFinalTime)) {
                    this._routeFinalFuelTime = hhmmToMinutes(rteFinalTime.padStart(4,"0"));
                    return true;
                } else {
                    this.addNewMessage(NXSystemMessages.entryOutOfRange);
                    return false;
                }
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    /**
     *
     * @param {string} s
     * @returns {Promise<boolean>}
     */
    async trySetRouteFinalFuel(s) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this._routeFinalFuelTime = this._routeFinalFuelTimeDefault;
            this._rteFinalEntered = false;
            return true;
        }
        if (s) {
            this._rteFinalEntered = true;
            const rteFinalWeight = parseFloat(s.split("/")[0]) / this._conversionWeight;
            const rteFinalTime = s.split("/")[1];
            if (rteFinalTime === undefined) {
                if (this.isFinalFuelInRange(rteFinalWeight)) {
                    this._routeFinalFuelWeight = rteFinalWeight;
                    this._routeFinalFuelTime = (rteFinalWeight * 1000) / this._rteFinalCoeffecient;
                    return true;
                } else {
                    this.addNewMessage(NXSystemMessages.entryOutOfRange);
                    return false;
                }
            } else {
                if (this.isFinalTimeInRange(rteFinalTime)) {
                    this._routeFinalFuelTime = hhmmToMinutes(rteFinalTime.padStart(4,"0"));
                    this._routeFinalFuelWeight = (this._routeFinalFuelTime * this._rteFinalCoeffecient) / 1000;
                    return true;
                } else {
                    this.addNewMessage(NXSystemMessages.entryOutOfRange);
                    return false;
                }
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getRouteReservedWeight() {
        if (isFinite(this._routeReservedWeight) && this._routeReservedWeight !== 0) {
            return this._routeReservedWeight;
        } else {
            return this._routeReservedPercent * this.blockFuel / 100;
        }
    }

    getRouteReservedPercent() {
        if (isFinite(this._routeReservedWeight) && isFinite(this.blockFuel) && this._routeReservedWeight !== 0) {
            return this._routeReservedWeight / this.blockFuel * 100;
        }
        return this._routeReservedPercent;
    }

    trySetRouteReservedPercent(s) {
        if (s) {
            if (s === A320_Neo_CDU_MainDisplay.clrValue) {
                this._rteReservedEntered = false;
                this._routeReservedWeight = 0;
                this._routeReservedPercent = 5;
                return true;
            }
            const rteRsvPercent = parseFloat(s.split("/")[1]);
            if (!this.isRteRsvPercentInRange(rteRsvPercent)) {
                this._rteRsvPercentOOR = true;
                this.addNewMessage(NXSystemMessages.notAllowed);
                return false;
            }
            this._rteRsvPercentOOR = false;
            if (isFinite(rteRsvPercent)) {
                this._routeReservedWeight = NaN;
                this._routeReservedPercent = rteRsvPercent;
                return true;
            }
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    /**
     * Checks input and passes to trySetCruiseFl()
     * @param input
     * @returns {boolean} input passed checks
     */
    trySetCruiseFlCheckInput(input) {
        if (input === A320_Neo_CDU_MainDisplay.clrValue) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
        const flString = input.replace("FL", "");
        if (!flString) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
        return this.trySetCruiseFl(parseFloat(flString));
    }

    /**
     * Sets new Cruise FL if all conditions good
     * @param fl {number} Altitude or FL
     * @returns {boolean} input passed checks
     */
    trySetCruiseFl(fl) {
        if (!isFinite(fl)) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
        if (fl >= 1000) {
            fl = Math.floor(fl / 100);
        }
        if (fl > MAX_CRUISE_FL) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
        const phase = Simplane.getCurrentFlightPhase();
        const selFl = Math.floor(Math.max(0, Simplane.getAutoPilotSelectedAltitudeLockValue("feet")) / 100);
        if (fl < selFl && phase === FlightPhase.FLIGHT_PHASE_CLIMB) {
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
        if (fl > Math.floor(Simplane.getAltitude() / 100) && phase > FlightPhase.FLIGHT_PHASE_CRUISE) {
            this.addNewMessage(NXSystemMessages.entryOutOfRange);
            return false;
        }
        if (fl < selFl && fl < 10 && phase === FlightPhase.FLIGHT_PHASE_CLIMB || phase === FlightPhase.FLIGHT_PHASE_CRUISE) {
            this.cruiseFlightLevel = selFl;
            this._cruiseEntered = true;
            this.updateConstraints();
            return true;
        }
        if (fl > 0 && fl <= MAX_CRUISE_FL) {
            this.cruiseFlightLevel = fl;
            this._cruiseEntered = true;
            this.updateConstraints();
            return true;
        }
        this.addNewMessage(NXSystemMessages.entryOutOfRange);
        return false;
    }

    isRteRsvPercentInRange(value) {
        return value > 0 && value < 15;
    }

    trySetRouteReservedFuel(s) {
        if (s) {
            if (s === A320_Neo_CDU_MainDisplay.clrValue) {
                this._rteReservedEntered = false;
                this._routeReservedWeight = 0;
                this._routeReservedPercent = 5;
                this._rteRsvPercentOOR = false;
                return true;
            }
            const rteRsvWeight = parseFloat(s.split("/")[0]) / this._conversionWeight;
            const rteRsvPercent = parseFloat(s.split("/")[1]);
            if (!this.isRteRsvPercentInRange(rteRsvPercent)) {
                this._rteRsvPercentOOR = true;
                return true;
            }
            this._rteRsvPercentOOR = false;
            this._rteReservedEntered = true;
            if (isFinite(rteRsvWeight)) {
                this._routeReservedWeight = rteRsvWeight;
                this._routeReservedPercent = 0;
                if (this.isRteRsvPercentInRange(this.getRouteReservedPercent())) { // Bit of a hacky method due previous tight coupling of weight and percentage calculations
                    return true;
                } else {
                    this.trySetRouteReservedFuel(A320_Neo_CDU_MainDisplay.clrValue);
                    this._rteRsvPercentOOR = true;
                    return false;
                }
            } else if (isFinite(rteRsvPercent)) {
                this._routeReservedWeight = NaN;
                this._routeReservedPercent = rteRsvPercent;
                return true;
            }
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    updateTakeOffTrim() {
        let d = (this.zeroFuelWeightMassCenter - 13) / (33 - 13);
        d = Math.min(Math.max(d, -0.5), 1);
        let dW = (this.getWeight(true) - 400) / (800 - 400);
        dW = Math.min(Math.max(dW, 0), 1);
        const minTrim = 3.5 * dW + 1.5 * (1 - dW);
        const maxTrim = 8.6 * dW + 4.3 * (1 - dW);
        this.takeOffTrim = minTrim * d + maxTrim * (1 - d);
    }

    getZeroFuelWeight(useLbs = false) {
        if (useLbs) {
            return this.zeroFuelWeight * 2.204623;
        }
        return this.zeroFuelWeight;
    }

    setZeroFuelWeight(s, callback = EmptyCallback.Boolean, useLbs = false) {
        let value = parseFloat(s);
        if (isFinite(value)) {
            if (useLbs) {
                value = value / 2.204623;
            }
            this.zeroFuelWeight = value;
            this.updateTakeOffTrim();
            return callback(true);
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        callback(false);
    }

    setZeroFuelCG(s, callback = EmptyCallback.Boolean) {
        const value = parseFloat(s);
        if (isFinite(value) && value > 0 && value < 100) {
            this.zeroFuelWeightMassCenter = value;
            this.updateTakeOffTrim();
            return callback(true);
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        callback(false);
    }

    isZFWInRange(zfw) {
        return 35.0 <= zfw && zfw <= 80.0; //TODO figure out how to handle LBs and KG input
    }

    isZFWCGInRange(zfwcg) {
        return (8.0 <= zfwcg && zfwcg <= 50.0);
    }

    tryEditZeroFuelWeightZFWCG(zfw = 0, zfwcg = 0, useLbs = false) {
        if (zfw > 0) {
            if (this.isZFWInRange(zfw)) {
                if (useLbs) {
                    zfw = zfw / 2.204623;
                }
                this.setZeroFuelWeight(zfw.toString());
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
                return false;
            }
        }
        if (zfwcg > 0) {
            if (this.isZFWCGInRange(zfwcg)) {
                this.setZeroFuelCG(zfwcg.toString());
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
                return false;
            }
        }
        this.updateTakeOffTrim();
        this.updateCleanTakeOffSpeed();
        this.updateCleanApproachSpeed();
        return true;
    }

    async trySetZeroFuelWeightZFWCG(s, useLbs = false) {
        let zfw = 0;
        let zfwcg = 0;
        if (s) {
            if (s.includes("/")) {
                const sSplit = s.split("/");
                zfw = parseFloat(sSplit[0]) / this._conversionWeight;
                zfwcg = parseFloat(sSplit[1]);
            } else {
                zfw = parseFloat(s) / this._conversionWeight;
            }
        }
        if (zfw > 0 && zfwcg > 0) {
            if (this.isZFWInRange(zfw) && this.isZFWCGInRange(zfwcg)) {
                this._zeroFuelWeightZFWCGEntered = true;
                if (useLbs) {
                    zfw = zfw / 2.204623;
                }

                this.setZeroFuelWeight(zfw.toString());
                this.setZeroFuelCG(zfwcg.toString());

                this.updateTakeOffTrim();
                this.updateCleanTakeOffSpeed();
                this.updateCleanApproachSpeed();
                return true;
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
                return false;
            }
        }
        if (this._zeroFuelWeightZFWCGEntered) {
            return this.tryEditZeroFuelWeightZFWCG(zfw, zfwcg, useLbs);
        }
        this.addNewMessage(NXSystemMessages.formatError);
        return false;
    }

    /**
     *
     * @param useLbs states whether to return the weight back in tons or pounds
     * @returns {*}
     */
    getFOB(useLbs = false) {
        if (useLbs) {
            return SimVar.GetSimVarValue("FUEL TOTAL QUANTITY WEIGHT", "pound");
        } else {
            return (SimVar.GetSimVarValue("FUEL TOTAL QUANTITY WEIGHT", "pound") * 0.453592) / 1000;
        }
    }

    /**
     * retrieves GW in Tons
     * @returns {number}
     */
    getGW() {
        return SimVar.GetSimVarValue("TOTAL WEIGHT", "kg") / 1000;
    }

    getCG() {
        return SimVar.GetSimVarValue("CG PERCENT", "Percent over 100") * 100;
    }

    /**
     *
     * @returns {number} Returns estimated fuel on board when arriving at the destination
     */
    getDestEFOB(useFOB = false) {
        if (useFOB) {
            return this.getFOB() - this._routeTripFuelWeight - this.taxiFuelWeight;
        } else {
            return this.blockFuel - this._routeTripFuelWeight - this.taxiFuelWeight;
        }
    }

    /**
     * @returns {number} Returns EFOB when arriving at the alternate dest
     */
    getAltEFOB(useFOB = false) {
        return this.getDestEFOB(useFOB) - this._routeAltFuelWeight;
    }

    isBlockFuelInRange(fuel) {
        return 0 <= fuel && fuel <= 80;
    }

    trySetBlockFuel(s, useLbs = false) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this.blockFuel = 0.0;
            this._blockFuelEntered = false;
            this._fuelPredDone = false;
            this._fuelPlanningPhase = this._fuelPlanningPhases.PLANNING;
            return true;
        }
        let value = parseFloat(s) / this._conversionWeight;
        if (isFinite(value) && this.isBlockFuelInRange(value)) {
            if (this.isBlockFuelInRange(value)) {
                if (useLbs) {
                    value = value / 2.204623;
                }
                this.blockFuel = value;
                this.updateTakeOffTrim();
                this._blockFuelEntered = true;
                return true;
            } else {
                this.addNewMessage(NXSystemMessages.entryOutOfRange);
                return false;
            }

        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getWeight(useLbs = false) {
        let w = this.zeroFuelWeight + this.blockFuel;
        if (useLbs) {
            w *= 2.204623;
        }
        return w;
    }

    async trySetTakeOffWeightLandingWeight(s) {
        let tow = NaN;
        let lw = NaN;
        if (s) {
            const sSplit = s.split("/");
            tow = parseFloat(sSplit[0]);
            lw = parseFloat(sSplit[1]);
        }
        if (isFinite(tow) || isFinite(lw)) {
            if (isFinite(tow)) {
                this.takeOffWeight = tow;
            }
            if (isFinite(lw)) {
                this.landingWeight = lw;
            }
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    isAvgWindInRange(wind) {
        return 0 <= wind && wind <= 250;
    }

    async trySetAverageWind(s) {
        const validDelims = ["HD", "H", "-", "TL", "T", "+"];
        const matchedIndex = validDelims.findIndex(element => s.includes(element));

        if (matchedIndex >= 0) {
            const wind = parseFloat(s.split(validDelims[matchedIndex])[1]);

            this._windDir = matchedIndex <= 2 ? this._windDirections.HEADWIND : this._windDirections.TAILWIND;

            if (isFinite(wind)) {
                if (this.isAvgWindInRange(wind)) {
                    this.averageWind = wind;
                    return true;
                } else {
                    this.addNewMessage(NXSystemMessages.entryOutOfRange);
                    return false;
                }
            } else {
                this.addNewMessage(NXSystemMessages.formatError);
                return false;
            }
        }
    }

    trySetPreSelectedClimbSpeed(s) {
        const v = parseFloat(s);
        if (isFinite(v)) {
            this.preSelectedClbSpeed = v;
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetPreSelectedCruiseSpeed(s) {
        const v = parseFloat(s);
        if (isFinite(v)) {
            this.preSelectedCrzSpeed = v;
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    trySetPreSelectedDescentSpeed(s) {
        const v = parseFloat(s);
        if (isFinite(v)) {
            this.preSelectedDesSpeed = v;
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    setPerfApprQNH(s) {
        const value = parseFloat(s);
        const QNH_REGEX = /[0-9]{2}.[0-9]{2}/;

        if (QNH_REGEX.test(value)) {
            this.perfApprQNH = value;
            return true;
        } else if (isFinite(value)) {
            this.perfApprQNH = value;
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    setPerfApprTemp(s) {
        const value = parseFloat(s);
        if (isFinite(value) && value > -270 && value < 150) {
            this.perfApprTemp = value;
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    setPerfApprWind(s) {
        let heading = NaN;
        let speed = NaN;
        if (s) {
            const sSplit = s.split("/");
            heading = parseFloat(sSplit[0]);
            speed = parseFloat(sSplit[1]);
        }
        if ((isFinite(heading) && heading >= 0 && heading < 360) || (isFinite(speed) && speed > 0)) {
            if (isFinite(heading)) {
                this.perfApprWindHeading = heading;
            }
            if (isFinite(speed)) {
                this.perfApprWindSpeed = speed;
            }
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    setPerfApprTransAlt(s) {
        if (!/^\d+$/.test(s)) {
            this.addNewMessage(NXSystemMessages.formatError);
            return false;
        }
        const v = parseInt(s);
        if (isFinite(v) && v > 0) {
            this.perfApprTransAlt = v;
            SimVar.SetSimVarValue("L:AIRLINER_APPR_TRANS_ALT", "Number", v);
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getVApp() {
        if (isFinite(this.vApp)) {
            return this.vApp;
        }
        if (isFinite(this.perfApprWindSpeed) && isFinite(this.perfApprWindHeading)) {
            return Math.ceil(this.getVLS() + NXSpeedsUtils.addWindComponent(this._towerHeadwind / 3));
        }
        return Math.ceil(this.getVLS() + NXSpeedsUtils.addWindComponent());
    }

    setPerfApprVApp(s) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this.vApp = NaN;
        }
        const value = parseFloat(s);
        if (isFinite(value) && value > 0) {
            this.vApp = value;
            return true;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    getVLS() {
        const dWeight = SimVar.GetSimVarValue("TOTAL WEIGHT", "kilograms") / 1000;
        const cg = this.zeroFuelWeightMassCenter;
        if (((isNaN(cg)) ? 24 : cg) < 25) {
            switch (true) {
                case (dWeight <= 50):
                    return 116;
                case (dWeight >= 75):
                    return Math.ceil(139 + .8 * (dWeight - 75));
                case (dWeight <= 55):
                    return Math.ceil(116 + .8 * (dWeight - 50));
                case (dWeight <= 70):
                    return Math.ceil(120 + dWeight - 55);
                default:
                    return Math.ceil(135 + .8 * (dWeight - 70));
            }
        }
        switch (true) {
            case (dWeight <= 50):
                return 116;
            case (dWeight >= 75):
                return Math.ceil(139 + .8 * (dWeight - 75));
            case (dWeight <= 55):
                return Math.ceil(116 + .6 * (dWeight - 50));
            default:
                return Math.ceil(119 + dWeight - 55);
        }
    }

    setPerfApprMDA(s) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this.perfApprMDA = NaN;
            SimVar.SetSimVarValue("L:AIRLINER_MINIMUM_DESCENT_ALTITUDE", "feet", 0);
            return true;
        } else {
            const value = parseFloat(s);
            if (isFinite(value)) {
                this.perfApprMDA = value;
                SimVar.SetSimVarValue("L:AIRLINER_MINIMUM_DESCENT_ALTITUDE", "feet", this.perfApprMDA);
                return true;
            }
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
    }

    setPerfApprDH(s) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this.perfApprDH = NaN;
            SimVar.SetSimVarValue("L:AIRLINER_DECISION_HEIGHT", "feet", -1);
            return true;
        } else if (s === "NO" || s === "NO DH" || s === "NODH") {
            if (Simplane.getAutoPilotApproachType() === 4) {
                this.perfApprDH = "NO DH";
                SimVar.SetSimVarValue("L:AIRLINER_DECISION_HEIGHT", "feet", -2);
                return true;
            } else {
                this.addNewMessage(NXSystemMessages.notAllowed);
                return false;
            }
        } else {
            const value = parseFloat(s);
            if (isFinite(value)) {
                if (value >= 0 && value <= 700) {
                    this.perfApprDH = value;
                    SimVar.SetSimVarValue("L:AIRLINER_DECISION_HEIGHT", "feet", this.perfApprDH);
                    return true;
                } else {
                    this.addNewMessage(NXSystemMessages.entryOutOfRange);
                    return false;
                }
            }
            this.addNewMessage(NXSystemMessages.notAllowed);
            return false;
        }
    }

    getIsFlying() {
        return this.currentFlightPhase >= FlightPhase.FLIGHT_PHASE_TAKEOFF;
    }

    async tryGoInApproachPhase() {
        if (this.currentFlightPhase === FlightPhase.FLIGHT_PHASE_CLIMB) {
            this.currentFlightPhase = FlightPhase.FLIGHT_PHASE_APPROACH;
            Coherent.call("GENERAL_ENG_THROTTLE_MANAGED_MODE_SET", ThrottleMode.AUTO);
            SimVar.SetSimVarValue("L:A32NX_GOAROUND_PASSED", "bool", 0);
            return true;
        }
        if (this.currentFlightPhase === FlightPhase.FLIGHT_PHASE_CRUISE) {
            this.currentFlightPhase = FlightPhase.FLIGHT_PHASE_APPROACH;
            Coherent.call("GENERAL_ENG_THROTTLE_MANAGED_MODE_SET", ThrottleMode.AUTO);
            SimVar.SetSimVarValue("L:A32NX_GOAROUND_PASSED", "bool", 0);
            return true;
        }
        if (this.currentFlightPhase === FlightPhase.FLIGHT_PHASE_DESCENT) {
            this.currentFlightPhase = FlightPhase.FLIGHT_PHASE_APPROACH;
            Coherent.call("GENERAL_ENG_THROTTLE_MANAGED_MODE_SET", ThrottleMode.AUTO);
            SimVar.SetSimVarValue("L:A32NX_GOAROUND_PASSED", "bool", 0);
            return true;
        }
        if (this.currentFlightPhase === FlightPhase.FLIGHT_PHASE_GOAROUND) {
            this.currentFlightPhase = FlightPhase.FLIGHT_PHASE_APPROACH;
            Coherent.call("GENERAL_ENG_THROTTLE_MANAGED_MODE_SET", ThrottleMode.AUTO);
            SimVar.SetSimVarValue("L:A32NX_GOAROUND_PASSED", "bool", 0);
            return true;
        }
        if (this.currentFlightPhase === FlightPhase.FLIGHT_PHASE_APPROACH) {
            SimVar.SetSimVarValue("L:A32NX_GOAROUND_PASSED", "bool", 0);
            return true;
        }
        return false;
    }

    connectIlsFrequency(_freq) {
        if (_freq >= 108 && _freq <= 111.95 && RadioNav.isHz50Compliant(_freq)) {
            switch (this.radioNav.mode) {
                case NavMode.FOUR_SLOTS: {
                    this.ilsFrequency = _freq;
                    break;
                }
                case NavMode.TWO_SLOTS: {
                    this.vor1Frequency = _freq;
                    break;
                }
            }
            this.connectIls();
            return true;
        }
        return false;
    }

    connectIls() {
        if (this.isRadioNavActive()) {
            return;
        }
        if (this._lockConnectIls) {
            return;
        }
        this._lockConnectIls = true;
        setTimeout(() => {
            this._lockConnectIls = false;
        }, 1000);
        switch (this.radioNav.mode) {
            case NavMode.FOUR_SLOTS: {
                if (Math.abs(this.radioNav.getILSActiveFrequency(1) - this.ilsFrequency) > 0.005) {
                    this.radioNav.setILSActiveFrequency(1, this.ilsFrequency);
                }
                break;
            }
            case NavMode.TWO_SLOTS: {
                if (Math.abs(this.radioNav.getVORActiveFrequency(1) - this.vor1Frequency) > 0.005) {
                    this.radioNav.setVORActiveFrequency(1, this.vor1Frequency);
                }
                break;
            }
            default:
                console.error("Unknown RadioNav operating mode");
                break;
        }
    }

    setIlsFrequency(s) {
        if (s === A320_Neo_CDU_MainDisplay.clrValue) {
            this.ilsFrequency = 0;
            this.radioNav.setILSActiveFrequency(1, 0);
            this._ilsFrequencyPilotEntered = false;
            return true;
        }
        const v = parseFloat(s);
        if (isFinite(v)) {
            const freq = Math.round(v * 100) / 100;
            if (this.connectIlsFrequency(freq)) {
                this._ilsFrequencyPilotEntered = true;
                return true;
            }
            this.addNewMessage(NXSystemMessages.entryOutOfRange);
            return false;
        }
        this.addNewMessage(NXSystemMessages.notAllowed);
        return false;
    }

    initRadioNav(_boot) {
        if (this.isPrimary) {
            console.log("Init RadioNav");
            {
                if (_boot) {
                    this.vhf1Frequency = this.radioNav.getVHFActiveFrequency(this.instrumentIndex, 1);
                    this.vhf2Frequency = this.radioNav.getVHFActiveFrequency(this.instrumentIndex, 2);
                } else {
                    if (Math.abs(this.radioNav.getVHFActiveFrequency(this.instrumentIndex, 1) - this.vhf1Frequency) > 0.005) {
                        this.radioNav.setVHFActiveFrequency(this.instrumentIndex, 1, this.vhf1Frequency);
                    }
                    if (Math.abs(this.radioNav.getVHFActiveFrequency(this.instrumentIndex, 2) - this.vhf2Frequency) > 0.005) {
                        this.radioNav.setVHFActiveFrequency(this.instrumentIndex, 2, this.vhf2Frequency);
                    }
                }
            }
            {
                if (Math.abs(this.radioNav.getVORActiveFrequency(1) - this.vor1Frequency) > 0.005) {
                    this.radioNav.setVORActiveFrequency(1, this.vor1Frequency);
                }
                if (this.vor1Course >= 0) {
                    SimVar.SetSimVarValue("K:VOR1_SET", "number", this.vor1Course);
                }
                this.connectIls();
            }
            {
                if (Math.abs(this.radioNav.getVORActiveFrequency(2) - this.vor2Frequency) > 0.005) {
                    this.radioNav.setVORActiveFrequency(2, this.vor2Frequency);
                }
                if (this.vor2Course >= 0) {
                    SimVar.SetSimVarValue("K:VOR2_SET", "number", this.vor2Course);
                }
                if (Math.abs(this.radioNav.getILSActiveFrequency(2) - 0) > 0.005) {
                    this.radioNav.setILSActiveFrequency(2, 0);
                }
            }
            {
                if (_boot) {
                    this.adf1Frequency = this.radioNav.getADFActiveFrequency(1);
                    this.adf2Frequency = this.radioNav.getADFActiveFrequency(2);
                } else {
                    if (Math.abs(this.radioNav.getADFActiveFrequency(1) - this.adf1Frequency) > 0.005) {
                        SimVar.SetSimVarValue("K:ADF_COMPLETE_SET", "Frequency ADF BCD32", Avionics.Utils.make_adf_bcd32(this.adf1Frequency * 1000)).then(() => {
                        });
                    }
                    if (Math.abs(this.radioNav.getADFActiveFrequency(2) - this.adf2Frequency) > 0.005) {
                        SimVar.SetSimVarValue("K:ADF2_COMPLETE_SET", "Frequency ADF BCD32", Avionics.Utils.make_adf_bcd32(this.adf2Frequency * 1000)).then(() => {
                        });
                    }
                }
            }
            {
                if (this.atc1Frequency > 0) {
                    SimVar.SetSimVarValue("K:XPNDR_SET", "Frequency BCD16", Avionics.Utils.make_xpndr_bcd16(this.atc1Frequency));
                } else {
                    this.atc1Frequency = SimVar.GetSimVarValue("TRANSPONDER CODE:1", "number");
                }
            }
        }
    }

    updateRadioNavState() {
        if (this.isPrimary) {
            const radioNavOn = this.isRadioNavActive();
            if (radioNavOn !== this._radioNavOn) {
                this._radioNavOn = radioNavOn;
                if (!radioNavOn) {
                    this.initRadioNav(false);
                }
                if (this.refreshPageCallback) {
                    this.refreshPageCallback();
                }
            }
            let apNavIndex = 1;
            let gpsDriven = true;
            const apprHold = SimVar.GetSimVarValue("AUTOPILOT APPROACH HOLD", "Bool");
            if (apprHold) {
                if (this.canSwitchToNav()) {
                    let navid = 0;
                    const ils = this.radioNav.getBestILSBeacon();
                    if (ils.id > 0) {
                        navid = ils.id;
                    } else {
                        const vor = this.radioNav.getBestVORBeacon();
                        if (vor.id > 0) {
                            navid = vor.id;
                        }
                    }
                    if (navid > 0) {
                        apNavIndex = navid;
                        const hasFlightplan = Simplane.getAutopilotGPSActive();
                        const apprCaptured = Simplane.getAutoPilotAPPRCaptured();
                        if (apprCaptured || !hasFlightplan) {
                            gpsDriven = false;
                        }
                    }
                }
            }
            if (apNavIndex !== this._apNavIndex) {
                SimVar.SetSimVarValue("K:AP_NAV_SELECT_SET", "number", apNavIndex);
                this._apNavIndex = apNavIndex;
            }
            const curState = SimVar.GetSimVarValue("GPS DRIVES NAV1", "Bool");
            if (curState !== gpsDriven) {
                SimVar.SetSimVarValue("K:TOGGLE_GPS_DRIVES_NAV1", "Bool", 0);
            }
        }
    }

    canSwitchToNav() {
        if (!this._canSwitchToNav) {
            const altitude = Simplane.getAltitudeAboveGround();
            if (altitude >= 500) {
                this._canSwitchToNav = true;
            }
        }
        return this._canSwitchToNav;
    }

    isRadioNavActive() {
        return this.radioNav.getRADIONAVActive((this.isPrimary) ? 1 : 2);
    }

    get vhf1Frequency() {
        return this._vhf1Frequency;
    }

    get vhf2Frequency() {
        return this._vhf2Frequency;
    }

    get vor1Frequency() {
        return this._vor1Frequency;
    }

    get vor1Course() {
        return this._vor1Course;
    }

    get vor2Frequency() {
        return this._vor2Frequency;
    }

    get vor2Course() {
        return this._vor2Course;
    }

    get ilsFrequency() {
        return this._ilsFrequency;
    }

    get ilsCourse() {
        return this._ilsCourse;
    }

    get adf1Frequency() {
        return this._adf1Frequency;
    }

    get adf2Frequency() {
        return this._adf2Frequency;
    }

    get rcl1Frequency() {
        return this._rcl1Frequency;
    }

    get pre2Frequency() {
        return this._pre2Frequency;
    }

    get atc1Frequency() {
        return this._atc1Frequency;
    }

    set vhf1Frequency(_frq) {
        this._vhf1Frequency = _frq;
    }

    set vhf2Frequency(_frq) {
        this._vhf2Frequency = _frq;
    }

    set vor1Frequency(_frq) {
        this._vor1Frequency = _frq;
        SimVar.SetSimVarValue("L:FMC_VOR_FREQUENCY:1", "Hz", _frq * 1000000);
    }

    set vor1Course(_crs) {
        this._vor1Course = _crs;
    }

    set vor2Frequency(_frq) {
        this._vor2Frequency = _frq;
        SimVar.SetSimVarValue("L:FMC_VOR_FREQUENCY:2", "Hz", _frq * 1000000);
    }

    set vor2Course(_crs) {
        this._vor2Course = _crs;
    }

    set ilsFrequency(_frq) {
        this._ilsFrequency = _frq;
    }

    set ilsCourse(_crs) {
        this._ilsCourse = _crs;
    }

    set adf1Frequency(_frq) {
        this._adf1Frequency = _frq;
    }

    set adf2Frequency(_frq) {
        this._adf2Frequency = _frq;
    }

    set rcl1Frequency(_frq) {
        this._rcl1Frequency = _frq;
    }

    set pre2Frequency(_frq) {
        this._pre2Frequency = _frq;
    }

    set atc1Frequency(_frq) {
        this._atc1Frequency = _frq;
    }

    handlePreviousInputState() {
        if (this.inOut === A320_Neo_CDU_MainDisplay.clrValue) {
            this.inOut = "";
        }
        if (this.isDisplayingErrorMessage || this.isDisplayingTypeTwoMessage) {
            this.inOut = this.lastUserInput;
            this._inOutElement.className = "white";
            this.isDisplayingErrorMessage = false;
            this.isDisplayingTypeTwoMessage = false;
        }
    }

    Init() {
        super.Init();
        this.dataManager = new FMCDataManager(this);
        this.tempCurve = new Avionics.Curve();
        this.tempCurve.interpolationFunction = Avionics.CurveTool.NumberInterpolation;
        this.tempCurve.add(-10 * 3.28084, 21.50);
        this.tempCurve.add(0, 15.00);
        this.tempCurve.add(10 * 3.28084, 8.50);
        this.tempCurve.add(20 * 3.28084, 2.00);
        this.tempCurve.add(30 * 3.28084, -4.49);
        this.tempCurve.add(40 * 3.28084, -10.98);
        this.tempCurve.add(50 * 3.28084, -17.47);
        this.tempCurve.add(60 * 3.28084, -23.96);
        this.tempCurve.add(70 * 3.28084, -30.45);
        this.tempCurve.add(80 * 3.28084, -36.94);
        this.tempCurve.add(90 * 3.28084, -43.42);
        this.tempCurve.add(100 * 3.28084, -49.90);
        this.tempCurve.add(150 * 3.28084, -56.50);
        this.tempCurve.add(200 * 3.28084, -56.50);
        this.tempCurve.add(250 * 3.28084, -51.60);
        this.tempCurve.add(300 * 3.28084, -46.64);
        this.tempCurve.add(400 * 3.28084, -22.80);
        this.tempCurve.add(500 * 3.28084, -2.5);
        this.tempCurve.add(600 * 3.28084, -26.13);
        this.tempCurve.add(700 * 3.28084, -53.57);
        this.tempCurve.add(800 * 3.28084, -74.51);
        let mainFrame = this.getChildById("Electricity");
        if (mainFrame == null) {
            mainFrame = this;
        }
        this.generateHTMLLayout(mainFrame);
        this._titleLeftElement = this.getChildById("title-left");
        this._titleElement = this.getChildById("title");
        this._pageCurrentElement = this.getChildById("page-current");
        this._pageCountElement = this.getChildById("page-count");
        this._labelElements = [];
        this._lineElements = [];
        for (let i = 0; i < 6; i++) {
            this._labelElements[i] = [
                this.getChildById("label-" + i + "-left"),
                this.getChildById("label-" + i + "-right"),
                this.getChildById("label-" + i + "-center")
            ];
            this._lineElements[i] = [
                this.getChildById("line-" + i + "-left"),
                this.getChildById("line-" + i + "-right"),
                this.getChildById("line-" + i + "-center")
            ];
        }
        this._inOutElement = this.getChildById("in-out");
        this._inOutElement.style.removeProperty("color");
        this._inOutElement.className = "white";
        this.onMenu = () => {
            FMCMainDisplayPages.MenuPage(this);
        };
        this.onLetterInput = (l) => {
            this.handlePreviousInputState();
            this.inOut += l;
        };
        this.onSp = () => {
            this.handlePreviousInputState();
            this.inOut += " ";
        };
        this.onDel = () => {
            this.handlePreviousInputState();
            if (this.inOut.length > 0) {
                this.inOut = this.inOut.slice(0, -1);
            }
        };
        this.onDiv = () => {
            this.handlePreviousInputState();
            this.inOut += "/";
        };
        this.onClr = () => {
            if (this.inOut === "") {
                this.inOut = A320_Neo_CDU_MainDisplay.clrValue;
            } else if (this.inOut === A320_Neo_CDU_MainDisplay.clrValue) {
                this.inOut = "";
            } else if (this.isDisplayingErrorMessage || this.isDisplayingTypeTwoMessage) {
                this.tryRemoveMessage();
                this.inOut = this.lastUserInput;
                this._inOutElement.className = "white";
                this.isDisplayingErrorMessage = false;
                this.isDisplayingTypeTwoMessage = false;
            } else {
                this.inOut = this.inOut.slice(0, -1);
            }
            this.tryShowMessage();
        };
        this.cruiseFlightLevel = SimVar.GetGameVarValue("AIRCRAFT CRUISE ALTITUDE", "feet");
        this.cruiseFlightLevel /= 100;
        SimVar.SetSimVarValue("L:FLIGHTPLAN_USE_DECEL_WAYPOINT", "number", 1);
        this.flightPlanManager.onCurrentGameFlightLoaded(() => {
            this.flightPlanManager.updateFlightPlan(() => {
                this.flightPlanManager.updateCurrentApproach(() => {
                    const frequency = this.flightPlanManager.getApproachNavFrequency();
                    if (isFinite(frequency)) {
                        const freq = Math.round(frequency * 100) / 100;
                        if (this.connectIlsFrequency(freq)) {
                            this._ilsFrequencyPilotEntered = false;
                            SimVar.SetSimVarValue("L:FLIGHTPLAN_APPROACH_ILS", "number", freq);
                            const approach = this.flightPlanManager.getApproach();
                            if (approach && approach.name && approach.name.indexOf("ILS") !== -1) {
                                const runway = this.flightPlanManager.getApproachRunway();
                                if (runway) {
                                    SimVar.SetSimVarValue("L:FLIGHTPLAN_APPROACH_COURSE", "number", runway.direction);
                                }
                            }
                        }
                    }
                });
                const callback = () => {
                    this.flightPlanManager.createNewFlightPlan();
                    SimVar.SetSimVarValue("L:AIRLINER_V1_SPEED", "Knots", NaN);
                    SimVar.SetSimVarValue("L:AIRLINER_V2_SPEED", "Knots", NaN);
                    SimVar.SetSimVarValue("L:AIRLINER_VR_SPEED", "Knots", NaN);
                    const cruiseAlt = Math.floor(this.flightPlanManager.cruisingAltitude / 100);
                    console.log("FlightPlan Cruise Override. Cruising at FL" + cruiseAlt + " instead of default FL" + this.cruiseFlightLevel);
                    if (cruiseAlt > 0) {
                        this.cruiseFlightLevel = cruiseAlt;
                    }
                };
                const arrivalIndex = this.flightPlanManager.getArrivalProcIndex();
                if (arrivalIndex >= 0) {
                    this.flightPlanManager.setArrivalProcIndex(arrivalIndex, callback);
                } else {
                    callback();
                }
            });
        });
        this.updateFuelVars();
        this.thrustReductionAltitude = 1500;
        SimVar.SetSimVarValue("L:AIRLINER_THR_RED_ALT", "Number", this.thrustReductionAltitude);
        this.PageTimeout = {
            Prog: 5000,
            Dyn: 1500
        };
        this.page = {
            SelfPtr: false,
            Current: 0,
            Clear: 0,
            AirportsMonitor: 1,
            AirwaysFromWaypointPage: 2,
            // AirwaysFromWaypointPageGetAllRows: 3,
            AvailableArrivalsPage: 4,
            AvailableArrivalsPageVias: 5,
            AvailableDeparturesPage: 6,
            AvailableFlightPlanPage: 7,
            DataIndexPage1: 8,
            DataIndexPage2: 9,
            DirectToPage: 10,
            FlightPlanPage: 11,
            FuelPredPage: 12,
            GPSMonitor: 13,
            HoldAtPage: 14,
            IdentPage: 15,
            InitPageA: 16,
            InitPageB: 17,
            IRSInit: 18,
            IRSMonitor: 19,
            IRSStatus: 20,
            IRSStatusFrozen: 21,
            LateralRevisionPage: 22,
            MenuPage: 23,
            NavaidPage: 24,
            NavRadioPage: 25,
            NewWaypoint: 26,
            PerformancePageTakeoff: 27,
            PerformancePageClb: 28,
            PerformancePageCrz: 29,
            PerformancePageDes: 30,
            PerformancePageAppr: 31,
            PerformancePageGoAround: 32,
            PilotsWaypoint: 33,
            PosFrozen: 34,
            PositionMonitorPage: 35,
            ProgressPage: 36,
            ProgressPageReport: 37,
            ProgressPagePredictiveGPS: 38,
            SelectedNavaids: 39,
            SelectWptPage: 40,
            VerticalRevisionPage: 41,
            WaypointPage: 42,
            AOCInit: 43,
            AOCInit2: 44,
            AOCOfpData: 45,
            AOCOfpData2: 46,
            ClimbWind: 47,
            CruiseWind: 48,
            DescentWind: 49,
        };
    }

    /**
     * Used for switching pages
     * @returns {number} delay in ms between 150 and 200
     */
    getDelaySwitchPage() {
        return 150 + 50 * Math.random();
    }

    /**
     * Used for basic inputs e.g. alternate airport, ci, fl, temp, constraints, ...
     * @returns {number} delay in ms between 300 and 400
     */
    getDelayBasic() {
        return 300 + 100 * Math.random();
    }

    /**
     * Used for e.g. loading time fore pages
     * @returns {number} delay in ms between 600 and 800
     */
    getDelayMedium() {
        return 600 + 200 * Math.random();
    }

    /**
     * Used for intense calculation
     * @returns {number} delay in ms between 900 and 12000
     */
    getDelayHigh() {
        return 900 + 300 * Math.random();
    }

    /**
     * Used for changes to the flight plan
     * @returns {number} dynamic delay in ms between ~300 and up to +2000 (depending on additional conditions)
     */
    getDelayRouteChange() {
        if (this._zeroFuelWeightZFWCGEntered && this._blockFuelEntered) {
            return Math.pow(this.flightPlanManager.getWaypointsCount(), 2) + (this.flightPlanManager.getDestination().cumulativeDistanceInFP) / 10 + Math.random() * 300;
        } else {
            return 300 + this.flightPlanManager.getWaypointsCount() * Math.random() + this.flightPlanManager.getDestination().cumulativeDistanceInFP * Math.random();
        }
    }

    /**
     * Used for calculation time for fuel pred page
     * @returns {number} dynamic delay in ms between 2000ms and 4000ms
     */
    getDelayFuelPred() {
        return 225 * this.flightPlanManager.getWaypointsCount() + (this.flightPlanManager.getDestination().cumulativeDistanceInFP / 2);
    }

    /**
     * Used to load wind data into fms
     * @returns {number} dynamic delay in ms dependent on amount of waypoints
     */
    getDelayWindLoad() {
        return Math.pow(this.flightPlanManager.getWaypointsCount(), 2);
    }

    /**
     * Tries to delete a pages timeout
     */
    tryDeleteTimeout() {
        if (this.page.SelfPtr) {
            clearTimeout(this.page.SelfPtr);
            this.page.SelfPtr = false;
        }
    }

    updateZfwVars() {
        const totalWeight = SimVar.GetSimVarValue("TOTAL WEIGHT", "kilograms") / 1000;
        const blockFuel = SimVar.GetSimVarValue("FUEL TOTAL QUANTITY", "gallons") * SimVar.GetSimVarValue("FUEL WEIGHT PER GALLON", "kilograms") / 1000;
        this.zeroFuelWeight = totalWeight - blockFuel;
        this.zeroFuelWeightMassCenter = SimVar.GetSimVarValue("CG PERCENT", "percent");
    }

    updateFuelVars() {
        this.blockFuel = SimVar.GetSimVarValue("FUEL TOTAL QUANTITY", "gallons") * SimVar.GetSimVarValue("FUEL WEIGHT PER GALLON", "kilograms") / 1000;
        this.updateZfwVars();
    }

    generateHTMLLayout(parent) {
        while (parent.children.length > 0) {
            parent.removeChild(parent.children[0]);
        }
        const header = document.createElement("div");
        header.id = "header";

        const titleLeft = document.createElement("div");
        titleLeft.classList.add("s-text");
        titleLeft.id = "title-left";
        parent.appendChild(titleLeft);

        const title = document.createElement("span");
        title.id = "title";
        header.appendChild(title);

        parent.appendChild(header);

        const page = document.createElement("div");
        page.id = "page-info";
        page.classList.add("s-text");

        const pageCurrent = document.createElement("span");
        pageCurrent.id = "page-current";

        const pageSlash = document.createElement("span");
        pageSlash.id = "page-slash";
        pageSlash.textContent = "/";

        const pageCount = document.createElement("span");
        pageCount.id = "page-count";

        page.appendChild(pageCurrent);
        page.appendChild(pageSlash);
        page.appendChild(pageCount);
        parent.appendChild(page);

        for (let i = 0; i < 6; i++) {
            const label = document.createElement("div");
            label.classList.add("label", "s-text");
            const labelLeft = document.createElement("span");
            labelLeft.id = "label-" + i + "-left";
            labelLeft.classList.add("fmc-block", "label", "label-left");
            const labelRight = document.createElement("span");
            labelRight.id = "label-" + i + "-right";
            labelRight.classList.add("fmc-block", "label", "label-right");
            const labelCenter = document.createElement("span");
            labelCenter.id = "label-" + i + "-center";
            labelCenter.classList.add("fmc-block", "label", "label-center");
            label.appendChild(labelLeft);
            label.appendChild(labelRight);
            label.appendChild(labelCenter);
            parent.appendChild(label);
            const line = document.createElement("div");
            line.classList.add("line");
            const lineLeft = document.createElement("span");
            lineLeft.id = "line-" + i + "-left";
            lineLeft.classList.add("fmc-block", "line", "line-left");
            const lineRight = document.createElement("span");
            lineRight.id = "line-" + i + "-right";
            lineRight.classList.add("fmc-block", "line", "line-right");
            const lineCenter = document.createElement("span");
            lineCenter.id = "line-" + i + "-center";
            lineCenter.classList.add("fmc-block", "line", "line-center");
            line.appendChild(lineLeft);
            line.appendChild(lineRight);
            line.appendChild(lineCenter);
            parent.appendChild(line);
        }
        const footer = document.createElement("div");
        footer.classList.add("line");
        const inout = document.createElement("span");
        inout.id = "in-out";
        footer.appendChild(inout);
        parent.appendChild(footer);
    }

    setAPSelectedSpeed(_speed, _aircraft) {
        if (isFinite(_speed)) {
            if (Simplane.getAutoPilotMachModeActive()) {
                const mach = SimVar.GetGameVarValue("FROM KIAS TO MACH", "number", _speed);
                Coherent.call("AP_MACH_VAR_SET", 1, mach);
                SimVar.SetSimVarValue("K:AP_MANAGED_SPEED_IN_MACH_ON", "number", 1);
                return;
            }
            Coherent.call("AP_SPD_VAR_SET", 1, _speed);
            SimVar.SetSimVarValue("K:AP_MANAGED_SPEED_IN_MACH_OFF", "number", 1);
        }
    }

    setAPManagedSpeed(_speed, _aircraft) {
        if (isFinite(_speed)) {
            if (Simplane.getAutoPilotMachModeActive()) {
                let mach = SimVar.GetGameVarValue("FROM KIAS TO MACH", "number", _speed);
                const cruiseMach = SimVar.GetGameVarValue("AIRCRAFT CRUISE MACH", "mach");
                mach = Math.min(mach, cruiseMach);
                Coherent.call("AP_MACH_VAR_SET", 2, mach);
                SimVar.SetSimVarValue("K:AP_MANAGED_SPEED_IN_MACH_ON", "number", 1);
                return;
            }
            Coherent.call("AP_SPD_VAR_SET", 2, _speed);
            SimVar.SetSimVarValue("K:AP_MANAGED_SPEED_IN_MACH_OFF", "number", 1);
        }
    }
}
