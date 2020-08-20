class Jet_PFD_NDCompass extends Jet_NDCompass {
    constructor() {
        super();
    }
    connectedCallback() {
        super.connectedCallback();
    }
    init() {
        super.init();
    }
    constructArc() {
        super.constructArc();
        if (this.aircraft == Aircraft.AS01B)
            this.constructArc_AS01B();
    }
    constructArc_AS01B() {
        this.destroyLayout();
        this.root = document.createElementNS(Avionics.SVG.NS, "svg");
        this.root.setAttribute("width", "100%");
        this.root.setAttribute("height", "100%");
        this.root.setAttribute("viewBox", "-225 -215 900 516");
        this.appendChild(this.root);
        var trsGroup = document.createElementNS(Avionics.SVG.NS, "g");
        trsGroup.setAttribute("transform", "translate(1, 160)");
        this.root.appendChild(trsGroup);
        {
            let viewBox = document.createElementNS(Avionics.SVG.NS, "svg");
            viewBox.setAttribute("x", "-225");
            viewBox.setAttribute("y", "-475");
            viewBox.setAttribute("viewBox", "-225 -550 550 600");
            trsGroup.appendChild(viewBox);
            var circleRadius = 425;
            var dashSpacing = 72;
            this.rotatingCircle = document.createElementNS(Avionics.SVG.NS, "g");
            this.rotatingCircle.setAttribute("id", "RotatingCicle");
            {
                let circleGroup = document.createElementNS(Avionics.SVG.NS, "g");
                circleGroup.setAttribute("id", "CircleGroup");
                this.rotatingCircle.appendChild(circleGroup);
                let radians = 0;
                for (let i = 0; i < dashSpacing; i++) {
                    let line = document.createElementNS(Avionics.SVG.NS, "line");
                    let bIsBig = (i % 2 == 0) ? true : false;
                    let bHasNumber = (i % 6 == 0) ? true : false;
                    let length = (bIsBig) ? 24 : 12;
                    let lineStart = 50 + circleRadius;
                    let lineEnd = 50 + circleRadius - length;
                    let degrees = (radians / Math.PI) * 180;
                    line.setAttribute("x1", "50");
                    line.setAttribute("y1", lineStart.toString());
                    line.setAttribute("x2", "50");
                    line.setAttribute("y2", lineEnd.toString());
                    line.setAttribute("transform", "rotate(" + (-degrees + 180) + " 50 50)");
                    line.setAttribute("stroke", "white");
                    line.setAttribute("stroke-width", "3");
                    if (bIsBig && bHasNumber) {
                        let text = document.createElementNS(Avionics.SVG.NS, "text");
                        text.textContent = fastToFixed(degrees / 10, 0);
                        text.setAttribute("x", "50");
                        text.setAttribute("y", (-(circleRadius - 50 - length - 30)).toString());
                        text.setAttribute("fill", "white");
                        text.setAttribute("font-size", (i % 3 == 0) ? "28" : "20");
                        text.setAttribute("font-family", "Roboto-Bold");
                        text.setAttribute("text-anchor", "middle");
                        text.setAttribute("alignment-baseline", "bottom");
                        text.setAttribute("transform", "rotate(" + degrees + " 50 50)");
                        circleGroup.appendChild(text);
                    }
                    radians += (2 * Math.PI) / dashSpacing;
                    circleGroup.appendChild(line);
                }
                this.trackingGroup = document.createElementNS(Avionics.SVG.NS, "g");
                this.trackingGroup.setAttribute("id", "trackingGroup");
                {
                    this.trackingLine = document.createElementNS(Avionics.SVG.NS, "path");
                    this.trackingLine.setAttribute("id", "trackingLine");
                    this.trackingLine.setAttribute("d", "M50 70 v " + (circleRadius - 20));
                    this.trackingLine.setAttribute("fill", "transparent");
                    this.trackingLine.setAttribute("stroke", "white");
                    this.trackingLine.setAttribute("stroke-width", "3");
                    this.trackingGroup.appendChild(this.trackingLine);
                }
                this.rotatingCircle.appendChild(this.trackingGroup);
                this.headingGroup = document.createElementNS(Avionics.SVG.NS, "g");
                this.headingGroup.setAttribute("id", "headingGroup");
                {
                    this.headingBug = document.createElementNS(Avionics.SVG.NS, "path");
                    this.headingBug.setAttribute("id", "headingBug");
                    this.headingBug.setAttribute("d", "M50 " + (50 + circleRadius) + " l -20 35 l 40 0 z");
                    this.headingBug.setAttribute("stroke", "white");
                    this.headingBug.setAttribute("stroke-width", "2");
                    this.headingGroup.appendChild(this.headingBug);
                }
                this.rotatingCircle.appendChild(this.headingGroup);
                this.selectedHeadingGroup = document.createElementNS(Avionics.SVG.NS, "g");
                this.selectedHeadingGroup.setAttribute("id", "selectedHeadingGroup");
                {
                    this.selectedHeadingLine = Avionics.SVG.computeDashLine(50, 70, (circleRadius - 5), 15, 3, "#ff00e0");
                    this.selectedHeadingLine.setAttribute("id", "selectedHeadingLine");
                    this.selectedHeadingGroup.appendChild(this.selectedHeadingLine);
                    this.selectedHeadingBug = document.createElementNS(Avionics.SVG.NS, "path");
                    this.selectedHeadingBug.setAttribute("id", "selectedHeadingBug");
                    this.selectedHeadingBug.setAttribute("d", "M50 " + (50 + circleRadius) + " h 22 v 22 h -7 l -15 -22 l -15 22 h -7 v -22 z");
                    this.selectedHeadingBug.setAttribute("stroke", "#ff00e0");
                    this.selectedHeadingBug.setAttribute("stroke-width", "2");
                    this.selectedHeadingBug.setAttribute("fill", "none");
                    this.selectedHeadingGroup.appendChild(this.selectedHeadingBug);
                }
                this.rotatingCircle.appendChild(this.selectedHeadingGroup);
                this.selectedTrackGroup = document.createElementNS(Avionics.SVG.NS, "g");
                this.selectedTrackGroup.setAttribute("id", "selectedTrackGroup");
                {
                    this.selectedTrackLine = Avionics.SVG.computeDashLine(50, 70, (circleRadius - 5), 15, 3, "#ff00e0");
                    this.selectedTrackLine.setAttribute("id", "selectedTrackLine");
                    this.selectedTrackGroup.appendChild(this.selectedTrackLine);
                    this.selectedTrackBug = document.createElementNS(Avionics.SVG.NS, "path");
                    this.selectedTrackBug.setAttribute("id", "selectedTrackBug");
                    this.selectedTrackBug.setAttribute("d", "M50 " + (50 + circleRadius) + " h -30 v -15 l 30 -15 l 30 15 v 15 z");
                    this.selectedTrackBug.setAttribute("stroke", "#ff00e0");
                    this.selectedTrackBug.setAttribute("stroke-width", "2");
                    this.selectedTrackGroup.appendChild(this.selectedTrackBug);
                }
                this.rotatingCircle.appendChild(this.selectedTrackGroup);
            }
            viewBox.appendChild(this.rotatingCircle);
        }
        {
            this.currentRefGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.currentRefGroup.setAttribute("id", "currentRefGroup");
            {
                let centerX = 230;
                let centerY = 130;
                let posX;
                posX = centerX - 50;
                this.currentRefValue = document.createElementNS(Avionics.SVG.NS, "text");
                this.currentRefValue.textContent = "266";
                this.currentRefValue.setAttribute("x", posX.toString());
                this.currentRefValue.setAttribute("y", centerY.toString());
                this.currentRefValue.setAttribute("fill", "#FF0CE2");
                this.currentRefValue.setAttribute("font-size", "36");
                this.currentRefValue.setAttribute("font-family", "Roboto-Bold");
                this.currentRefValue.setAttribute("text-anchor", "end");
                this.currentRefValue.setAttribute("alignment-baseline", "bottom");
                this.currentRefGroup.appendChild(this.currentRefValue);
                posX -= 70;
                this.currentRefMode = document.createElementNS(Avionics.SVG.NS, "text");
                this.currentRefMode.textContent = "HDG";
                this.currentRefMode.setAttribute("x", posX.toString());
                this.currentRefMode.setAttribute("y", centerY.toString());
                this.currentRefMode.setAttribute("fill", "#FF0CE2");
                this.currentRefMode.setAttribute("font-size", "26");
                this.currentRefMode.setAttribute("font-family", "Roboto-Light");
                this.currentRefMode.setAttribute("text-anchor", "end");
                this.currentRefMode.setAttribute("alignment-baseline", "bottom");
                this.currentRefGroup.appendChild(this.currentRefMode);
                posX -= 50;
                this.currentRefSelected = document.createElementNS(Avionics.SVG.NS, "text");
                this.currentRefSelected.textContent = "SEL";
                this.currentRefSelected.setAttribute("x", posX.toString());
                this.currentRefSelected.setAttribute("y", centerY.toString());
                this.currentRefSelected.setAttribute("fill", "#FF0CE2");
                this.currentRefSelected.setAttribute("font-size", "26");
                this.currentRefSelected.setAttribute("font-family", "Roboto-Light");
                this.currentRefSelected.setAttribute("text-anchor", "end");
                this.currentRefSelected.setAttribute("alignment-baseline", "bottom");
                this.currentRefGroup.appendChild(this.currentRefSelected);
                posX = centerX + 50;
                this.currentRefType = document.createElementNS(Avionics.SVG.NS, "text");
                this.currentRefType.textContent = "MAG";
                this.currentRefType.setAttribute("x", posX.toString());
                this.currentRefType.setAttribute("y", centerY.toString());
                this.currentRefType.setAttribute("fill", "#24F000");
                this.currentRefType.setAttribute("font-size", "30");
                this.currentRefType.setAttribute("font-family", "Roboto-Light");
                this.currentRefType.setAttribute("text-anchor", "start");
                this.currentRefType.setAttribute("alignment-baseline", "bottom");
                this.currentRefGroup.appendChild(this.currentRefType);
            }
            trsGroup.appendChild(this.currentRefGroup);
        }
    }
}
customElements.define("jet-pfd-nd-compass", Jet_PFD_NDCompass);
//# sourceMappingURL=NDCompass.js.map