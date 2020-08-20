class Jet_PFD_HSIndicator extends HTMLElement {
    constructor() {
        super(...arguments);
        this.cursorOpacity = "1.0";
        this.strokeOpacity = "0.75";
        this.strokeColor = "rgb(255,255,255)";
        this.strokeSize = 6;
        this.fontSize = 25;
        this.refStartX = 0;
        this.refWidth = 0;
        this.graduationScrollPosX = 0;
        this.graduationScrollPosY = 0;
        this.nbPrimaryGraduations = 7;
        this.nbSecondaryGraduations = 1;
        this.totalGraduations = this.nbPrimaryGraduations + ((this.nbPrimaryGraduations - 1) * this.nbSecondaryGraduations);
        this.graduationSpacing = 50;
        this._showILS = false;
        this._aircraft = Aircraft.A320_NEO;
    }
    get aircraft() {
        return this._aircraft;
    }
    set aircraft(_val) {
        if (this._aircraft != _val) {
            this._aircraft = _val;
            this.construct();
        }
    }
    connectedCallback() {
        this.graduationScroller = new Avionics.Scroller(this.nbPrimaryGraduations, 10, true, 360);
        this.construct();
    }
    showILS(_val) {
        this._showILS = _val;
        if (!this._showILS) {
            this.ILSBeaconGroup.setAttribute("visibility", "hidden");
            this.ILSOffscreenGroup.setAttribute("visibility", "hidden");
        }
    }
    construct() {
        Utils.RemoveAllChildren(this);
        if (this.aircraft == Aircraft.B747_8 || this.aircraft == Aircraft.AS01B) {
            this.construct_B747_8();
        }
        else {
            this.construct_A320_Neo();
        }
    }
    construct_B747_8() {
        var posX = 0;
        var posY = 0;
        var width = 400;
        var height = 400;
        this.rootSVG = document.createElementNS(Avionics.SVG.NS, "svg");
        this.rootSVG.setAttribute("id", "ViewBox");
        this.rootSVG.setAttribute("viewBox", "0 0 " + width + " " + height);
        {
            let circleRadius = 80;
            this.rotatingCompass = document.createElementNS(Avionics.SVG.NS, "g");
            this.rotatingCompass.setAttribute("id", "Circle");
            this.rootSVG.appendChild(this.rotatingCompass);
            {
                this.rotatingCompassX = posX + width * 0.5;
                this.rotatingCompassY = posY + height * 0.5;
                {
                    let circle = document.createElementNS(Avionics.SVG.NS, "circle");
                    circle.setAttribute("cx", this.rotatingCompassX.toString());
                    circle.setAttribute("cy", this.rotatingCompassY.toString());
                    circle.setAttribute("r", circleRadius.toString());
                    circle.setAttribute("fill", "#343B51");
                    this.rotatingCompass.appendChild(circle);
                }
                let graduationsGroup = document.createElementNS(Avionics.SVG.NS, "g");
                graduationsGroup.setAttribute("id", "Graduations");
                this.rotatingCompass.appendChild(graduationsGroup);
                {
                    let angle = 0;
                    for (let i = 0; i < 72; i++) {
                        let isPrimary = (i % 2 == 0) ? true : false;
                        let length = (isPrimary) ? 3 : 2;
                        let line = document.createElementNS(Avionics.SVG.NS, "line");
                        line.setAttribute("x1", this.rotatingCompassX.toString());
                        line.setAttribute("y1", (this.rotatingCompassY - circleRadius).toString());
                        line.setAttribute("x2", this.rotatingCompassX.toString());
                        line.setAttribute("y2", (this.rotatingCompassY - circleRadius + length).toString());
                        line.setAttribute("stroke", "white");
                        line.setAttribute("stroke-width", "0.75");
                        line.setAttribute("transform", "rotate(" + angle + " " + this.rotatingCompassX + " " + this.rotatingCompassY + ")");
                        graduationsGroup.appendChild(line);
                        if (isPrimary) {
                            let fontSize = (this.fontSize * 0.20);
                            if (angle % 90 == 0)
                                fontSize = (this.fontSize * 0.3);
                            let text = document.createElementNS(Avionics.SVG.NS, "text");
                            text.textContent = (angle / 10).toString();
                            text.setAttribute("x", this.rotatingCompassX.toString());
                            text.setAttribute("y", (this.rotatingCompassY - circleRadius + length + 4).toString());
                            text.setAttribute("fill", "white");
                            text.setAttribute("font-size", fontSize.toString());
                            text.setAttribute("font-family", "Roboto-Bold");
                            text.setAttribute("text-anchor", "middle");
                            text.setAttribute("alignment-baseline", "central");
                            text.setAttribute("transform", "rotate(" + angle + " " + this.rotatingCompassX + " " + this.rotatingCompassY + ")");
                            graduationsGroup.appendChild(text);
                        }
                        angle += 360 / 72;
                    }
                }
            }
            this.selectedHeadingGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.selectedHeadingGroup.setAttribute("id", "selectedHeadingGroup");
            this.rootSVG.appendChild(this.selectedHeadingGroup);
            {
                this.selectedHeadingLine = Avionics.SVG.computeDashLine(this.rotatingCompassX, this.rotatingCompassY, (-circleRadius), 15, 1, "#ff00e0");
                this.selectedHeadingLine.setAttribute("id", "selectedHeadingLine");
                this.selectedHeadingGroup.appendChild(this.selectedHeadingLine);
                this.selectedHeadingBug = document.createElementNS(Avionics.SVG.NS, "path");
                this.selectedHeadingBug.setAttribute("id", "Heading");
                this.selectedHeadingBug.setAttribute("fill", "transparent");
                this.selectedHeadingBug.setAttribute("stroke", "#FF0CE2");
                this.selectedHeadingBug.setAttribute("stroke-width", "0.75");
                this.selectedHeadingBug.setAttribute("d", "M " + this.rotatingCompassX + " " + (this.rotatingCompassY - circleRadius) + " l-6 0 l0 -5 l3 0 l3 5 l3 -5 l3 0 l0 5 Z");
                this.selectedHeadingGroup.appendChild(this.selectedHeadingBug);
            }
            this.currentTrackGroup = document.createElementNS(Avionics.SVG.NS, "line");
            this.currentTrackGroup.setAttribute("id", "CurrentTrack");
            this.currentTrackGroup.setAttribute("x1", this.rotatingCompassX.toString());
            this.currentTrackGroup.setAttribute("y1", this.rotatingCompassY.toString());
            this.currentTrackGroup.setAttribute("x2", this.rotatingCompassX.toString());
            this.currentTrackGroup.setAttribute("y2", (this.rotatingCompassY - circleRadius).toString());
            this.currentTrackGroup.setAttribute("stroke", "white");
            this.currentTrackGroup.setAttribute("stroke-width", "0.75");
            this.rootSVG.appendChild(this.currentTrackGroup);
            let fixedElements = document.createElementNS(Avionics.SVG.NS, "g");
            fixedElements.setAttribute("id", "FixedElements");
            this.rootSVG.appendChild(fixedElements);
            {
                this.selectedHeadingText = document.createElementNS(Avionics.SVG.NS, "text");
                this.selectedHeadingText.textContent = "135H";
                this.selectedHeadingText.setAttribute("x", (this.rotatingCompassX - circleRadius * 0.225).toString());
                this.selectedHeadingText.setAttribute("y", (this.rotatingCompassY - circleRadius * 0.725).toString());
                this.selectedHeadingText.setAttribute("fill", "#FF0CE2");
                this.selectedHeadingText.setAttribute("font-size", (this.fontSize * 0.25).toString());
                this.selectedHeadingText.setAttribute("font-family", "Roboto-Bold");
                this.selectedHeadingText.setAttribute("text-anchor", "start");
                this.selectedHeadingText.setAttribute("alignment-baseline", "central");
                fixedElements.appendChild(this.selectedHeadingText);
                this.selectedHeadingTextRef = document.createElementNS(Avionics.SVG.NS, "text");
                this.selectedHeadingTextRef.textContent = "MAG";
                this.selectedHeadingTextRef.setAttribute("x", (this.rotatingCompassX + circleRadius * 0.225).toString());
                this.selectedHeadingTextRef.setAttribute("y", (this.rotatingCompassY - circleRadius * 0.725).toString());
                this.selectedHeadingTextRef.setAttribute("fill", "lime");
                this.selectedHeadingTextRef.setAttribute("font-size", (this.fontSize * 0.2).toString());
                this.selectedHeadingTextRef.setAttribute("font-family", "Roboto-Bold");
                this.selectedHeadingTextRef.setAttribute("text-anchor", "end");
                this.selectedHeadingTextRef.setAttribute("alignment-baseline", "central");
                fixedElements.appendChild(this.selectedHeadingTextRef);
                let topTriangle = document.createElementNS(Avionics.SVG.NS, "path");
                topTriangle.setAttribute("d", "M " + this.rotatingCompassX + " " + (this.rotatingCompassY - circleRadius) + " l-3.5 -5.5 l7 0 Z");
                topTriangle.setAttribute("fill", "transparent");
                topTriangle.setAttribute("stroke", "white");
                topTriangle.setAttribute("stroke-width", "0.5");
                fixedElements.appendChild(topTriangle);
            }
        }
        this.appendChild(this.rootSVG);
    }
    construct_A320_Neo() {
        this.rootSVG = document.createElementNS(Avionics.SVG.NS, "svg");
        this.rootSVG.setAttribute("id", "ViewBox");
        this.rootSVG.setAttribute("viewBox", "0 0 550 250");
        this.refStartX = 25;
        this.refWidth = 500;
        var posX = this.refStartX;
        var posY = 5;
        var width = this.refWidth;
        var height = 100;
        if (!this.rootGroup) {
            this.rootGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.rootGroup.setAttribute("id", "HS");
        }
        else {
            Utils.RemoveAllChildren(this.rootGroup);
        }
        this.rootSVG.appendChild(this.rootGroup);
        if (!this.centerSVG) {
            this.centerSVG = document.createElementNS(Avionics.SVG.NS, "svg");
            this.centerSVG.setAttribute("id", "CenterGroup");
        }
        else
            Utils.RemoveAllChildren(this.centerSVG);
        this.centerSVG.setAttribute("x", posX.toString());
        this.centerSVG.setAttribute("y", posY.toString());
        this.centerSVG.setAttribute("width", width.toString());
        this.centerSVG.setAttribute("height", height.toString());
        this.centerSVG.setAttribute("overflow", "hidden");
        this.centerSVG.setAttribute("viewBox", "0 0 " + width + " " + height);
        this.rootGroup.appendChild(this.centerSVG);
        {
            var _top = 35;
            var _left = 0;
            var _width = width;
            var _height = 80;
            var bg = document.createElementNS(Avionics.SVG.NS, "rect");
            bg.setAttribute("x", _left.toString());
            bg.setAttribute("y", _top.toString());
            bg.setAttribute("width", _width.toString());
            bg.setAttribute("height", _height.toString());
            bg.setAttribute("fill", "#343B51");
            bg.setAttribute("stroke", this.strokeColor);
            bg.setAttribute("stroke-width", this.strokeSize.toString());
            bg.setAttribute("stroke-opacity", this.strokeOpacity);
            this.centerSVG.appendChild(bg);
            var graduationGroup = document.createElementNS(Avionics.SVG.NS, "g");
            graduationGroup.setAttribute("id", "Graduations");
            {
                this.graduationScrollPosX = _left + _width * 0.5;
                this.graduationScrollPosY = _top;
                if (!this.graduations) {
                    this.graduations = [];
                    for (var i = 0; i < this.totalGraduations; i++) {
                        var line = new Avionics.SVGGraduation();
                        line.IsPrimary = (i % (this.nbSecondaryGraduations + 1)) ? false : true;
                        var lineWidth = line.IsPrimary ? 5 : 5;
                        var lineHeight = line.IsPrimary ? 25 : 12;
                        var linePosY = 0;
                        line.SVGLine = document.createElementNS(Avionics.SVG.NS, "rect");
                        line.SVGLine.setAttribute("y", linePosY.toString());
                        line.SVGLine.setAttribute("width", lineWidth.toString());
                        line.SVGLine.setAttribute("height", lineHeight.toString());
                        line.SVGLine.setAttribute("fill", "white");
                        if (line.IsPrimary) {
                            line.SVGText1 = document.createElementNS(Avionics.SVG.NS, "text");
                            line.SVGText1.setAttribute("y", (linePosY + lineHeight + 20).toString());
                            line.SVGText1.setAttribute("fill", "white");
                            line.SVGText1.setAttribute("font-size", (this.fontSize * 1.35).toString());
                            line.SVGText1.setAttribute("font-family", "Roboto-Bold");
                            line.SVGText1.setAttribute("text-anchor", "middle");
                            line.SVGText1.setAttribute("alignment-baseline", "central");
                        }
                        this.graduations.push(line);
                    }
                }
                for (var i = 0; i < this.totalGraduations; i++) {
                    var line = this.graduations[i];
                    graduationGroup.appendChild(line.SVGLine);
                    if (line.SVGText1) {
                        graduationGroup.appendChild(line.SVGText1);
                    }
                }
                this.centerSVG.appendChild(graduationGroup);
            }
            this.selectedHeadingGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.selectedHeadingGroup.setAttribute("id", "Heading");
            {
                var headingPosX = _left + _width * 0.5 - 2;
                var headingPosY = posY;
                var headingWidth = 35;
                var headingHeight = _height;
                let headingSVG = document.createElementNS(Avionics.SVG.NS, "svg");
                headingSVG.setAttribute("x", (headingPosX - headingWidth * 0.5).toString());
                headingSVG.setAttribute("y", headingPosY.toString());
                headingSVG.setAttribute("width", headingWidth.toString());
                headingSVG.setAttribute("height", headingHeight.toString());
                headingSVG.setAttribute("viewBox", "0 0 " + headingWidth + " " + headingHeight);
                {
                    let headingShape = document.createElementNS(Avionics.SVG.NS, "path");
                    headingShape.setAttribute("fill", "transparent");
                    headingShape.setAttribute("stroke", "#00F2FF");
                    headingShape.setAttribute("stroke-width", "4");
                    headingShape.setAttribute("d", "M20 24 l -12 -20 l 24 0 z");
                    headingSVG.appendChild(headingShape);
                }
                this.selectedHeadingGroup.appendChild(headingSVG);
            }
            this.centerSVG.appendChild(this.selectedHeadingGroup);
            this.currentTrackGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.currentTrackGroup.setAttribute("id", "CurrentTrack");
            {
                var trackPosX = _left + _width * 0.5;
                var trackPosY = posY + 30;
                var trackWidth = 28;
                var trackHeight = _height;
                let trackSVG = document.createElementNS(Avionics.SVG.NS, "svg");
                trackSVG.setAttribute("x", (trackPosX - trackWidth * 0.5).toString());
                trackSVG.setAttribute("y", trackPosY.toString());
                trackSVG.setAttribute("width", trackWidth.toString());
                trackSVG.setAttribute("height", trackHeight.toString());
                trackSVG.setAttribute("viewBox", "0 0 " + trackWidth + " " + trackHeight);
                {
                    let trackShape = document.createElementNS(Avionics.SVG.NS, "path");
                    trackShape.setAttribute("fill", "transparent");
                    trackShape.setAttribute("stroke", "#00FF21");
                    trackShape.setAttribute("stroke-width", "4");
                    trackShape.setAttribute("d", "M13 0 l-13 17 l13 17 l13 -17 Z");
                    trackSVG.appendChild(trackShape);
                }
                this.currentTrackGroup.appendChild(trackSVG);
            }
            this.centerSVG.appendChild(this.currentTrackGroup);
            this.ILSBeaconGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.ILSBeaconGroup.setAttribute("id", "ILSBeacon");
            {
                var ilsPosX = _left + _width * 0.5 + 2.5;
                var ilsPosY = posY + 45;
                var ilsWidth = 30;
                var ilsHeight = _height;
                let ilsSVG = document.createElementNS(Avionics.SVG.NS, "svg");
                ilsSVG.setAttribute("x", (ilsPosX - ilsWidth * 0.5).toString());
                ilsSVG.setAttribute("y", ilsPosY.toString());
                ilsSVG.setAttribute("width", ilsWidth.toString());
                ilsSVG.setAttribute("height", ilsHeight.toString());
                ilsSVG.setAttribute("viewBox", "0 0 " + ilsWidth + " " + ilsHeight);
                {
                    let ilsShape = document.createElementNS(Avionics.SVG.NS, "path");
                    ilsShape.setAttribute("fill", "transparent");
                    ilsShape.setAttribute("stroke", "#FF0CE2");
                    ilsShape.setAttribute("stroke-width", "5");
                    ilsShape.setAttribute("d", "M15 0 l0 50 M0 40 l30 0");
                    ilsSVG.appendChild(ilsShape);
                }
                this.ILSBeaconGroup.appendChild(ilsSVG);
            }
            this.centerSVG.appendChild(this.ILSBeaconGroup);
            var cursorPosX = _left + _width * 0.5;
            var cursorPosY = posY;
            var cursorWidth = 35;
            var cursorHeight = _height;
            if (!this.cursorSVG) {
                this.cursorSVG = document.createElementNS(Avionics.SVG.NS, "svg");
                this.cursorSVG.setAttribute("id", "CursorGroup");
            }
            else
                Utils.RemoveAllChildren(this.cursorSVG);
            this.cursorSVG.setAttribute("x", (cursorPosX - cursorWidth * 0.5).toString());
            this.cursorSVG.setAttribute("y", cursorPosY.toString());
            this.cursorSVG.setAttribute("width", cursorWidth.toString());
            this.cursorSVG.setAttribute("height", cursorHeight.toString());
            this.cursorSVG.setAttribute("viewBox", "0 0 " + cursorWidth + " " + cursorHeight);
            {
                let cursorShape = document.createElementNS(Avionics.SVG.NS, "path");
                cursorShape.setAttribute("fill", "yellow");
                cursorShape.setAttribute("fill-opacity", this.cursorOpacity);
                cursorShape.setAttribute("d", "M 15 2 L 25 2 L 25 53 L 15 53 L 15 2 Z");
                this.cursorSVG.appendChild(cursorShape);
            }
            this.centerSVG.appendChild(this.cursorSVG);
        }
        let rectWidth = 70;
        this.ILSOffscreenGroup = document.createElementNS(Avionics.SVG.NS, "g");
        this.ILSOffscreenGroup.setAttribute("id", "ILSOffscreen");
        this.rootSVG.appendChild(this.ILSOffscreenGroup);
        {
            let rect = document.createElementNS(Avionics.SVG.NS, "rect");
            rect.setAttribute("x", (-rectWidth * 0.5).toString());
            rect.setAttribute("y", "60");
            rect.setAttribute("width", rectWidth.toString());
            rect.setAttribute("height", "40");
            rect.setAttribute("fill", "black");
            rect.setAttribute("stroke", "white");
            rect.setAttribute("stroke-width", "3");
            this.ILSOffscreenGroup.appendChild(rect);
            this.ILSOffscreenText = document.createElementNS(Avionics.SVG.NS, "text");
            this.ILSOffscreenText.setAttribute("x", "0");
            this.ILSOffscreenText.setAttribute("y", (60 + 20).toString());
            this.ILSOffscreenText.setAttribute("fill", "#FF0CE2");
            this.ILSOffscreenText.setAttribute("font-size", (this.fontSize * 1.35).toString());
            this.ILSOffscreenText.setAttribute("font-family", "Roboto-Light");
            this.ILSOffscreenText.setAttribute("text-anchor", "middle");
            this.ILSOffscreenText.setAttribute("alignment-baseline", "central");
            this.ILSOffscreenGroup.appendChild(this.ILSOffscreenText);
        }
        this.appendChild(this.rootSVG);
    }
    update(dTime) {
        if (this.rotatingCompass)
            this.updateCircle();
        else
            this.updateRibbon();
    }
    updateRibbon() {
        var compass = SimVar.GetSimVarValue("PLANE HEADING DEGREES MAGNETIC", "degree");
        var selectedHeading = SimVar.GetSimVarValue("AUTOPILOT HEADING LOCK DIR", "degree");
        var track = SimVar.GetSimVarValue("GPS GROUND MAGNETIC TRACK", "degree");
        if (this.graduations) {
            this.graduationScroller.scroll(compass);
            var currentVal = this.graduationScroller.firstValue;
            var currentX = this.graduationScrollPosX - this.graduationScroller.offsetY * this.graduationSpacing * (this.nbSecondaryGraduations + 1);
            for (var i = 0; i < this.totalGraduations; i++) {
                var posX = currentX;
                var posY = this.graduationScrollPosY;
                this.graduations[i].SVGLine.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                if (this.graduations[i].SVGText1) {
                    var roundedVal = Math.floor(currentVal / 10);
                    this.graduations[i].SVGText1.textContent = roundedVal.toString();
                    this.graduations[i].SVGText1.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                    currentVal = this.graduationScroller.nextValue;
                }
                currentX += this.graduationSpacing;
            }
        }
        if (this.selectedHeadingGroup) {
            var autoPilotActive = Simplane.getAutoPilotHeadingSelected();
            if (autoPilotActive) {
                var delta = selectedHeading - compass;
                if (delta > 180)
                    delta = delta - 360;
                else if (delta < -180)
                    delta = delta + 360;
                var posX = delta * this.graduationSpacing * (this.nbSecondaryGraduations + 1) / this.graduationScroller.increment;
                this.selectedHeadingGroup.setAttribute("transform", "translate(" + posX.toString() + " 0)");
                this.selectedHeadingGroup.setAttribute("visibility", "visible");
            }
            else {
                this.selectedHeadingGroup.setAttribute("visibility", "hidden");
            }
        }
        if (this.currentTrackGroup) {
            var delta = track - compass;
            if (delta > 180)
                delta = delta - 360;
            else if (delta < -180)
                delta = delta + 360;
            var posX = delta * this.graduationSpacing * (this.nbSecondaryGraduations + 1) / this.graduationScroller.increment;
            this.currentTrackGroup.setAttribute("transform", "translate(" + posX.toString() + " 0)");
        }
        if (this._showILS) {
            if (this.ILSBeaconGroup && this.ILSOffscreenGroup) {
                let localizer = this.gps.radioNav.getBestILSBeacon();
                if (localizer.id > 0) {
                    var delta = localizer.course - compass;
                    if (delta > 180)
                        delta = delta - 360;
                    else if (delta < -180)
                        delta = delta + 360;
                    var posX = delta * this.graduationSpacing * (this.nbSecondaryGraduations + 1) / this.graduationScroller.increment;
                    if (posX > -(this.refWidth * 0.5) && posX < (this.refWidth * 0.5)) {
                        this.ILSBeaconGroup.setAttribute("visibility", "visible");
                        this.ILSBeaconGroup.setAttribute("transform", "translate(" + posX.toString() + " 0)");
                        this.ILSOffscreenGroup.setAttribute("visibility", "hidden");
                    }
                    else {
                        let pos;
                        if (posX <= -(this.refWidth * 0.5))
                            pos = this.refStartX + 15;
                        else
                            pos = this.refStartX + this.refWidth - 15;
                        let rounded = Math.round(localizer.course);
                        this.ILSOffscreenText.textContent = Utils.leadingZeros(rounded, 3);
                        this.ILSOffscreenGroup.setAttribute("transform", "translate(" + pos + " 0)");
                        this.ILSOffscreenGroup.setAttribute("visibility", "visible");
                        this.ILSBeaconGroup.setAttribute("visibility", "hidden");
                    }
                }
                else {
                    this.ILSOffscreenGroup.setAttribute("visibility", "hidden");
                    this.ILSBeaconGroup.setAttribute("visibility", "hidden");
                }
            }
        }
    }
    updateCircle() {
        var compass = SimVar.GetSimVarValue("PLANE HEADING DEGREES MAGNETIC", "degree");
        if (this.rotatingCompass) {
            this.rotatingCompass.setAttribute("transform", "rotate(" + (-compass) + " " + this.rotatingCompassX + " " + this.rotatingCompassY + ")");
        }
        if (this.selectedHeadingGroup) {
            var autoPilotActive = true;
            if (autoPilotActive) {
                var selectedHeading = SimVar.GetSimVarValue("AUTOPILOT HEADING LOCK DIR", "Degree");
                var delta = compass - selectedHeading;
                this.selectedHeadingGroup.setAttribute("transform", "rotate(" + (-delta) + " " + this.rotatingCompassX + " " + this.rotatingCompassY + ")");
                this.selectedHeadingGroup.setAttribute("visibility", "visible");
                this.selectedHeadingText.textContent = Math.round(selectedHeading) + "H";
                let headingLocked = Simplane.getAutoPilotHeadingLockActive();
                if (this.selectedHeadingLine)
                    this.selectedHeadingLine.classList.toggle('hide', headingLocked);
            }
            else {
                this.selectedHeadingGroup.setAttribute("visibility", "hidden");
                this.selectedHeadingText.textContent = "";
            }
        }
        if (this.currentTrackGroup) {
            var track = SimVar.GetSimVarValue("GPS GROUND MAGNETIC TRACK", "degree");
            var groundSpeed = SimVar.GetSimVarValue("GPS GROUND SPEED", "knots");
            if (groundSpeed <= 10)
                track = compass;
            var delta = compass - track;
            this.currentTrackGroup.setAttribute("transform", "rotate(" + (-delta) + " " + this.rotatingCompassX + " " + this.rotatingCompassY + ")");
        }
    }
}
customElements.define("jet-pfd-hs-indicator", Jet_PFD_HSIndicator);
//# sourceMappingURL=HSIndicator.js.map