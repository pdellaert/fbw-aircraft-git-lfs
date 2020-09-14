class A320_Neo_SAI extends BaseAirliners {
    get templateID() { return "A320_Neo_SAI"; }
    connectedCallback() {
        super.connectedCallback();
        this.addIndependentElementContainer(new NavSystemElementContainer("Altimeter", "Altimeter", new A320_Neo_SAI_Altimeter()));
        this.addIndependentElementContainer(new NavSystemElementContainer("Airspeed", "Airspeed", new A320_Neo_SAI_Airspeed()));
        this.addIndependentElementContainer(new NavSystemElementContainer("Horizon", "Horizon", new A320_Neo_SAI_Attitude()));
        this.addIndependentElementContainer(new NavSystemElementContainer("SelfTest", "SelfTest", new A320_Neo_SAI_SelfTest()));
        this.addIndependentElementContainer(new NavSystemElementContainer("Pressure", "Pressure", new A320_Neo_SAI_Pressure()));
    }
    Update() {
        super.Update();
    }
}

class A320_Neo_SAI_Airspeed extends NavSystemElement {
  constructor() {
      super();
  }
  init(root) {
      this.airspeedElement = this.gps.getChildById("Airspeed");
  }
  onEnter() {
  }
  isReady() {
      return true;
  }
  onUpdate(_deltaTime) {
      this.airspeedElement.update(_deltaTime);
  }
  onExit() {
  }
  onEvent(_event) {
  }
}
class A320_Neo_SAI_AirspeedIndicator extends HTMLElement {
  constructor() {
      super(...arguments);
      this.greenColor = "green";
      this.yellowColor = "yellow";
      this.redColor = "red";
      this.fontSize = 25;
      this.cursorScrollPosX = 0;
      this.cursorScrollPosY = 0;
      this.cursorScrollNbTexts = 3;
      this.cursorScrollSpacing = 30;
      this.graduationScrollPosX = 0;
      this.graduationScrollPosY = 0;
      this.graduationSpacing = 10;
      this.graduationMinValue = 30;
      this.nbPrimaryGraduations = 11;
      this.nbSecondaryGraduations = 3;
      this.totalGraduations = this.nbPrimaryGraduations + ((this.nbPrimaryGraduations - 1) * this.nbSecondaryGraduations);
  }
  connectedCallback() {
      this.graduationScroller = new Avionics.Scroller(this.nbPrimaryGraduations, 20);
      this.construct();
  }
  construct() {
      Utils.RemoveAllChildren(this);
      this.cursorScroller = new Avionics.Scroller(this.cursorScrollNbTexts, 1, false, 10);
      this.rootSVG = document.createElementNS(Avionics.SVG.NS, "svg");
      this.rootSVG.setAttribute("id", "ViewBox");
      this.rootSVG.setAttribute("viewBox", "0 0 250 500");
      var width = 40;
      var height = 250;
      var posX = width * 0.5;
      var posY = 0;
      if (!this.rootGroup) {
          this.rootGroup = document.createElementNS(Avionics.SVG.NS, "g");
          this.rootGroup.setAttribute("id", "Airspeed");
      }
      else {
          Utils.RemoveAllChildren(this.rootGroup);
      }
      var topMask = document.createElementNS(Avionics.SVG.NS, "path");
      topMask.setAttribute("d", "M0 0 l118 0 l0 30 q-118 2 -118 50 Z");
      topMask.setAttribute("fill", "black");
      this.rootGroup.appendChild(topMask);
      var bottomMask = document.createElementNS(Avionics.SVG.NS, "path");
      bottomMask.setAttribute("d", "M0 250 l118 0 l0 -35 q-118 -2 -118 -50 Z");
      bottomMask.setAttribute("fill", "black");
      this.rootGroup.appendChild(bottomMask);
      if (!this.centerSVG) {
          this.centerSVG = document.createElementNS(Avionics.SVG.NS, "svg");
          this.centerSVG.setAttribute("id", "CenterGroup");
      }
      else
          Utils.RemoveAllChildren(this.centerSVG);
      this.centerSVG.setAttribute("x", (posX - width * 0.5).toString());
      this.centerSVG.setAttribute("y", posY.toString());
      this.centerSVG.setAttribute("width", width.toString());
      this.centerSVG.setAttribute("height", height.toString());
      this.centerSVG.setAttribute("viewBox", "0 0 " + width + " " + height);
      {
          var _top = 0;
          var _left = 0;
          var _width = width;
          var _height = height;
          var bg = document.createElementNS(Avionics.SVG.NS, "rect");
          bg.setAttribute("x", _left.toString());
          bg.setAttribute("y", _top.toString());
          bg.setAttribute("width", _width.toString());
          bg.setAttribute("height", _height.toString());
          bg.setAttribute("fill", "black");
          this.centerSVG.appendChild(bg);
          if (this.airspeeds) {
              var arcGroup = document.createElementNS(Avionics.SVG.NS, "g");
              arcGroup.setAttribute("id", "Arcs");
              {
                  this.arcs = [];
                  var _arcWidth = 18;
                  var _arcPosX = _left + _width + 3;
                  var _arcStartPosY = _top + _height * 0.5;
                  var arcHeight = this.arcToSVG(this.airspeeds.greenEnd - this.airspeeds.greenStart);
                  var arcPosY = _arcStartPosY - this.arcToSVG(this.airspeeds.greenStart) - arcHeight;
                  var arc = document.createElementNS(Avionics.SVG.NS, "rect");
                  arc.setAttribute("x", _arcPosX.toString());
                  arc.setAttribute("y", arcPosY.toString());
                  arc.setAttribute("width", _arcWidth.toString());
                  arc.setAttribute("height", arcHeight.toString());
                  arc.setAttribute("fill", this.greenColor);
                  this.arcs.push(arc);
                  var arcHeight = this.arcToSVG(this.airspeeds.yellowEnd - this.airspeeds.yellowStart);
                  var arcPosY = _arcStartPosY - this.arcToSVG(this.airspeeds.yellowStart) - arcHeight;
                  var arc = document.createElementNS(Avionics.SVG.NS, "rect");
                  arc.setAttribute("x", _arcPosX.toString());
                  arc.setAttribute("y", arcPosY.toString());
                  arc.setAttribute("width", _arcWidth.toString());
                  arc.setAttribute("height", arcHeight.toString());
                  arc.setAttribute("fill", this.yellowColor);
                  this.arcs.push(arc);
                  var arcHeight = this.arcToSVG(this.airspeeds.redEnd - this.airspeeds.redStart);
                  var arcPosY = _arcStartPosY - this.arcToSVG(this.airspeeds.redStart) - arcHeight;
                  var arc = document.createElementNS(Avionics.SVG.NS, "rect");
                  arc.setAttribute("x", _arcPosX.toString());
                  arc.setAttribute("y", arcPosY.toString());
                  arc.setAttribute("width", _arcWidth.toString());
                  arc.setAttribute("height", arcHeight.toString());
                  arc.setAttribute("fill", this.redColor);
                  this.arcs.push(arc);
                  var arcHeight = this.arcToSVG(this.airspeeds.whiteEnd - this.airspeeds.whiteStart);
                  var arcPosY = _arcStartPosY - this.arcToSVG(this.airspeeds.whiteStart) - arcHeight;
                  var arc = document.createElementNS(Avionics.SVG.NS, "rect");
                  arc.setAttribute("x", (_arcPosX + _arcWidth * 0.5).toString());
                  arc.setAttribute("y", arcPosY.toString());
                  arc.setAttribute("width", (_arcWidth * 0.5).toString());
                  arc.setAttribute("height", arcHeight.toString());
                  arc.setAttribute("fill", "white");
                  this.arcs.push(arc);
                  for (var i = 0; i < this.arcs.length; i++) {
                      arcGroup.appendChild(this.arcs[i]);
                  }
                  this.centerSVG.appendChild(arcGroup);
              }
          }
          var graduationGroup = document.createElementNS(Avionics.SVG.NS, "g");
          graduationGroup.setAttribute("id", "Graduations");
          {
              this.graduationScrollPosX = _left + _width;
              this.graduationScrollPosY = _top + _height * 0.5;
              this.graduations = [];
              for (var i = 0; i < this.totalGraduations; i++) {
                  var line = new Avionics.SVGGraduation();
                  var mod = i % (this.nbSecondaryGraduations + 1);
                  line.IsPrimary = (mod == 0) ? true : false;
                  var lineWidth = (mod == 2) ? 10 : 4;
                  var lineHeight = (mod == 2) ? 2 : 2;
                  var linePosX = -lineWidth;
                  line.SVGLine = document.createElementNS(Avionics.SVG.NS, "rect");
                  line.SVGLine.setAttribute("x", linePosX.toString());
                  line.SVGLine.setAttribute("width", lineWidth.toString());
                  line.SVGLine.setAttribute("height", lineHeight.toString());
                  line.SVGLine.setAttribute("fill", "white");
                  if (mod == 0) {
                      line.SVGText1 = document.createElementNS(Avionics.SVG.NS, "text");
                      line.SVGText1.setAttribute("x", (linePosX - 2).toString());
                      line.SVGText1.setAttribute("fill", "white");
                      line.SVGText1.setAttribute("font-size", (this.fontSize * 0.65).toString());
                      line.SVGText1.setAttribute("font-family", "Roboto-Bold");
                      line.SVGText1.setAttribute("text-anchor", "end");
                      line.SVGText1.setAttribute("alignment-baseline", "central");
                  }
                  this.graduations.push(line);
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
      }
      this.rootGroup.appendChild(this.centerSVG);
      var cursorPosX = _left + _width;
      var cursorPosY = _top + _height * 0.5;
      var cursorWidth = 12;
      var cursorHeight = 18;
      if (!this.cursorSVG) {
          this.cursorSVG = document.createElementNS(Avionics.SVG.NS, "svg");
          this.cursorSVG.setAttribute("id", "CursorGroup");
      }
      else
          Utils.RemoveAllChildren(this.cursorSVG);
      this.cursorSVG.setAttribute("x", cursorPosX.toString());
      this.cursorSVG.setAttribute("y", (cursorPosY - cursorHeight * 0.5).toString());
      this.cursorSVG.setAttribute("width", cursorWidth.toString());
      this.cursorSVG.setAttribute("height", cursorHeight.toString());
      this.cursorSVG.setAttribute("viewBox", "0 0 " + cursorWidth + " " + cursorHeight);
      {
          var rect = document.createElementNS(Avionics.SVG.NS, "rect");
          rect.setAttribute("x", "0");
          rect.setAttribute("y", "0");
          rect.setAttribute("width", cursorWidth.toString());
          rect.setAttribute("height", cursorHeight.toString());
          rect.setAttribute("fill", "black");
          this.cursorSVG.appendChild(rect);
          if (!this.cursorSVGShape)
              this.cursorSVGShape = document.createElementNS(Avionics.SVG.NS, "path");
          this.cursorSVGShape.setAttribute("d", "M 0 " + (cursorHeight * 0.5) + " L" + cursorWidth + " 0 L" + cursorWidth + " " + cursorHeight + " Z");
          this.cursorSVGShape.setAttribute("fill", "yellow");
          this.cursorSVG.appendChild(this.cursorSVGShape);
          this.rootGroup.appendChild(this.cursorSVG);
      }
      var topBg = document.createElementNS(Avionics.SVG.NS, "rect");
      topBg.setAttribute("x", _left.toString());
      topBg.setAttribute("y", (_top - 5).toString());
      topBg.setAttribute("width", "125");
      topBg.setAttribute("height", "35");
      topBg.setAttribute("fill", "black");
      this.rootGroup.appendChild(topBg);
      var bottomBg = document.createElementNS(Avionics.SVG.NS, "rect");
      bottomBg.setAttribute("x", _left.toString());
      bottomBg.setAttribute("y", (_top + _height - 35).toString());
      bottomBg.setAttribute("width", "125");
      bottomBg.setAttribute("height", "40");
      bottomBg.setAttribute("fill", "black");
      this.rootGroup.appendChild(bottomBg);
      this.rootSVG.appendChild(this.rootGroup);
      this.appendChild(this.rootSVG);
  }
  update(dTime) {
      var indicatedSpeed = Simplane.getIndicatedSpeed();
      this.updateArcScrolling(indicatedSpeed);
      this.updateGraduationScrolling(indicatedSpeed);
      this.updateCursorScrolling(indicatedSpeed);
  }
  arcToSVG(_value) {
      var pixels = (_value * this.graduationSpacing * (this.nbSecondaryGraduations + 1)) / 10;
      return pixels;
  }
  updateGraduationScrolling(_speed) {
      if (this.graduations) {
          if (_speed < this.graduationMinValue)
              _speed = this.graduationMinValue;
          this.graduationScroller.scroll(_speed);
          var currentVal = this.graduationScroller.firstValue;
          var currentY = this.graduationScrollPosY + this.graduationScroller.offsetY * this.graduationSpacing * (this.nbSecondaryGraduations + 1);
          for (var i = 0; i < this.totalGraduations; i++) {
              var posX = this.graduationScrollPosX;
              var posY = currentY;
              if ((currentVal < this.graduationMinValue) || (currentVal == this.graduationMinValue && !this.graduations[i].SVGText1)) {
                  this.graduations[i].SVGLine.setAttribute("visibility", "hidden");
                  if (this.graduations[i].SVGText1) {
                      this.graduations[i].SVGText1.setAttribute("visibility", "hidden");
                  }
              }
              else {
                  this.graduations[i].SVGLine.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                  if (this.graduations[i].SVGText1) {
                      this.graduations[i].SVGText1.textContent = currentVal.toString();
                      this.graduations[i].SVGText1.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                  }
              }
              if (this.graduations[i].SVGText1)
                  currentVal = this.graduationScroller.nextValue;
              currentY -= this.graduationSpacing;
          }
      }
  }
  updateArcScrolling(_speed) {
      if (this.arcs) {
          var offset = this.arcToSVG(_speed);
          for (var i = 0; i < this.arcs.length; i++) {
              this.arcs[i].setAttribute("transform", "translate(0 " + offset.toString() + ")");
          }
      }
  }
  updateCursorScrolling(_speed) {
      if (this.cursorSVGScrollTexts) {
          if (_speed < 20) {
              this.cursorSVGMainText.textContent = "--";
              for (var i = 0; i < this.cursorSVGScrollTexts.length; i++) {
                  this.cursorSVGScrollTexts[i].textContent = "";
              }
          }
          else {
              var integral = Math.trunc(_speed / 10);
              if (integral > 1)
                  this.cursorSVGMainText.textContent = integral.toString();
              this.cursorScroller.scroll(_speed);
              var currentVal = this.cursorScroller.firstValue;
              var currentY = this.cursorScrollPosY + this.cursorScroller.offsetY * this.cursorScrollSpacing;
              for (var i = 0; i < this.cursorSVGScrollTexts.length; i++) {
                  var posX = this.cursorScrollPosX;
                  var posY = currentY;
                  this.cursorSVGScrollTexts[i].textContent = currentVal.toString();
                  this.cursorSVGScrollTexts[i].setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                  currentY -= this.cursorScrollSpacing;
                  currentVal = this.cursorScroller.nextValue;
              }
          }
      }
  }
}
customElements.define('a320-neo-sai-airspeed-indicator', A320_Neo_SAI_AirspeedIndicator);

class A320_Neo_SAI_Altimeter extends NavSystemElement {
  constructor() {
      super();
  }
  init(root) {
      this.altimeterElement = this.gps.getChildById("Altimeter");
  }
  onEnter() {
  }
  isReady() {
      return true;
      ;
  }
  onUpdate(_deltaTime) {
      this.altimeterElement.update(_deltaTime);
  }
  onExit() {
  }
  onEvent(_event) {
      switch (_event) {
          case "BARO_INC":
              SimVar.SetSimVarValue("K:KOHLSMAN_INC", "number", 1);
              break;
          case "BARO_DEC":
              SimVar.SetSimVarValue("K:KOHLSMAN_DEC", "number", 1);
              break;
      }
  }
}
class A320_Neo_SAI_AltimeterIndicator extends HTMLElement {
    constructor() {
        super(...arguments);
        this.fontSize = 25;
        this.cursorScrollPosX = 0;
        this.cursorScrollPosY = 0;
        this.cursorScrollNbTexts = 5;
        this.cursorScrollSpacing = 48;
        this.graduationScrollPosX = 0;
        this.graduationScrollPosY = 0;
        this.nbPrimaryGraduations = 7;
        this.nbSecondaryGraduations = 4;
        this.totalGraduations = this.nbPrimaryGraduations + ((this.nbPrimaryGraduations - 1) * this.nbSecondaryGraduations);
        this.graduationSpacing = 14;
    }
    connectedCallback() {
        this.cursorScroller = new Avionics.Scroller(this.cursorScrollNbTexts, 20, true, 100);
        this.graduationScroller = new Avionics.Scroller(this.nbPrimaryGraduations, 500, true);
        this.construct();
    }
    construct() {
        Utils.RemoveAllChildren(this);
        this.rootSVG = document.createElementNS(Avionics.SVG.NS, "svg");
        this.rootSVG.setAttribute("id", "ViewBox");
        this.rootSVG.setAttribute("viewBox", "0 0 250 500");
        var width = 60;
        var height = 250;
        var posX = 40 + width * 0.5;
        var posY = 0;
        if (!this.rootGroup) {
            this.rootGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.rootGroup.setAttribute("id", "Altimeter");
        }
        else {
            Utils.RemoveAllChildren(this.rootGroup);
        }
        if (!this.centerSVG) {
            this.centerSVG = document.createElementNS(Avionics.SVG.NS, "svg");
            this.centerSVG.setAttribute("id", "CenterGroup");
        }
        else
            Utils.RemoveAllChildren(this.centerSVG);
        var topMask = document.createElementNS(Avionics.SVG.NS, "path");
        topMask.setAttribute("d", "M0 0 l0 30 q118 2 118 50 l0 -80 Z");
        topMask.setAttribute("fill", "black");
        this.rootGroup.appendChild(topMask);
        var bottomMask = document.createElementNS(Avionics.SVG.NS, "path");
        bottomMask.setAttribute("d", "M0 250 l0 -35 q118 -2 118 -50 l0 85 Z");
        bottomMask.setAttribute("fill", "black");
        this.rootGroup.appendChild(bottomMask);
        this.centerSVG.setAttribute("x", posX.toString());
        this.centerSVG.setAttribute("y", posY.toString());
        this.centerSVG.setAttribute("width", width.toString());
        this.centerSVG.setAttribute("height", height.toString());
        this.centerSVG.setAttribute("viewBox", "0 0 " + width + " " + height);
        this.centerSVG.setAttribute("overflow", "hidden");
        {
            var _top = 0;
            var _left = 0;
            var _width = width;
            var _height = height;
            var bg = document.createElementNS(Avionics.SVG.NS, "rect");
            bg.setAttribute("x", _left.toString());
            bg.setAttribute("y", _top.toString());
            bg.setAttribute("width", _width.toString());
            bg.setAttribute("height", _height.toString());
            bg.setAttribute("fill", "black");
            this.centerSVG.appendChild(bg);
            this.graduationScrollPosX = _left;
            this.graduationScrollPosY = _top + _height * 0.5;
            this.graduations = [];
            for (var i = 0; i < this.totalGraduations; i++) {
                var line = new Avionics.SVGGraduation();
                line.IsPrimary = true;
                if (this.nbSecondaryGraduations > 0 && (i % (this.nbSecondaryGraduations + 1)))
                    line.IsPrimary = false;
                var lineWidth = line.IsPrimary ? 4 : 12;
                line.SVGLine = document.createElementNS(Avionics.SVG.NS, "rect");
                line.SVGLine.setAttribute("x", "0");
                line.SVGLine.setAttribute("width", lineWidth.toString());
                line.SVGLine.setAttribute("height", "2");
                line.SVGLine.setAttribute("fill", "white");
                if (line.IsPrimary) {
                    line.SVGText1 = document.createElementNS(Avionics.SVG.NS, "text");
                    line.SVGText1.setAttribute("x", (lineWidth + 2).toString());
                    line.SVGText1.setAttribute("fill", "white");
                    line.SVGText1.setAttribute("font-size", (this.fontSize * 0.85).toString());
                    line.SVGText1.setAttribute("font-family", "Roboto-Bold");
                    line.SVGText1.setAttribute("text-anchor", "start");
                    line.SVGText1.setAttribute("alignment-baseline", "central");
                }
                this.graduations.push(line);
            }
            var graduationGroup = document.createElementNS(Avionics.SVG.NS, "g");
            graduationGroup.setAttribute("id", "graduationGroup");
            for (var i = 0; i < this.totalGraduations; i++) {
                var line = this.graduations[i];
                graduationGroup.appendChild(line.SVGLine);
                if (line.SVGText1)
                    graduationGroup.appendChild(line.SVGText1);
                if (line.SVGText2)
                    graduationGroup.appendChild(line.SVGText2);
            }
            this.centerSVG.appendChild(graduationGroup);
        }
        this.rootGroup.appendChild(this.centerSVG);
        _left = posX - width * 0.5;
        var cursorPosX = _left + 5;
        var cursorPosY = _top + _height * 0.5;
        var cursorWidth = width * 1.25;
        var cursorHeight = 39;
        if (!this.cursorSVG) {
            this.cursorSVG = document.createElementNS(Avionics.SVG.NS, "svg");
            this.cursorSVG.setAttribute("id", "CursorGroup");
        }
        else
            Utils.RemoveAllChildren(this.cursorSVG);
        this.cursorSVG.setAttribute("x", cursorPosX.toString());
        this.cursorSVG.setAttribute("y", (cursorPosY - cursorHeight * 0.5).toString());
        this.cursorSVG.setAttribute("width", cursorWidth.toString());
        this.cursorSVG.setAttribute("height", cursorHeight.toString());
        this.cursorSVG.setAttribute("viewBox", "0 4 " + cursorWidth + " " + cursorHeight);
        {
            var trs = document.createElementNS(Avionics.SVG.NS, "g");
            trs.setAttribute("transform", "scale(0.6)");
            this.cursorSVG.appendChild(trs);
            if (!this.cursorSVGShape)
                this.cursorSVGShape = document.createElementNS(Avionics.SVG.NS, "path");
            this.cursorSVGShape.setAttribute("fill", "black");
            this.cursorSVGShape.setAttribute("d", "M0 22 L65 22 L65 8 L140 8 L140 70 L65 70 L65 56 L0 56 Z");
            this.cursorSVGShape.setAttribute("stroke", "yellow");
            this.cursorSVGShape.setAttribute("stroke-width", "0.85");
            trs.appendChild(this.cursorSVGShape);
            var _cursorPosX = 0;
            var _cursorPosY = cursorHeight * 0.5 + 18;
            if (!this.cursorSVGMainText)
                this.cursorSVGMainText = document.createElementNS(Avionics.SVG.NS, "text");
            this.cursorSVGMainText.setAttribute("width", _width.toString());
            this.cursorSVGMainText.setAttribute("fill", "green");
            this.cursorSVGMainText.setAttribute("font-size", (this.fontSize * 1.4).toString());
            this.cursorSVGMainText.setAttribute("font-family", "Roboto-Bold");
            this.cursorSVGMainText.setAttribute("text-anchor", "end");
            this.cursorSVGMainText.setAttribute("alignment-baseline", "central");
            this.cursorSVGMainText.setAttribute("transform", "translate(" + (_cursorPosX + 65).toString() + " " + _cursorPosY.toString() + ")");
            trs.appendChild(this.cursorSVGMainText);
            this.cursorScrollPosX = _cursorPosX + 118;
            this.cursorScrollPosY = _cursorPosY;
            this.cursorSVGScrollTexts = [];
            for (var i = 0; i < this.cursorScrollNbTexts; i++) {
                var text = document.createElementNS(Avionics.SVG.NS, "text");
                text.setAttribute("width", _width.toString());
                text.setAttribute("fill", "green");
                text.setAttribute("font-size", (this.fontSize * 1.6).toString());
                text.setAttribute("font-family", "Roboto-Bold");
                text.setAttribute("text-anchor", "end");
                text.setAttribute("alignment-baseline", "central");
                this.cursorSVGScrollTexts.push(text);
            }
            for (var i = 0; i < this.cursorSVGScrollTexts.length; i++) {
                trs.appendChild(this.cursorSVGScrollTexts[i]);
            }
            this.rootGroup.appendChild(this.cursorSVG);
        }
        var topBg = document.createElementNS(Avionics.SVG.NS, "rect");
        topBg.setAttribute("x", (_left - 40).toString());
        topBg.setAttribute("y", (_top - 5).toString());
        topBg.setAttribute("width", "125");
        topBg.setAttribute("height", "35");
        topBg.setAttribute("fill", "black");
        this.rootGroup.appendChild(topBg);
        var bottomBg = document.createElementNS(Avionics.SVG.NS, "rect");
        bottomBg.setAttribute("x", (_left - 40).toString());
        bottomBg.setAttribute("y", (_top + _height - 35).toString());
        bottomBg.setAttribute("width", "125");
        bottomBg.setAttribute("height", "40");
        bottomBg.setAttribute("fill", "black");
        this.rootGroup.appendChild(bottomBg);
        this.rootSVG.appendChild(this.rootGroup);
        this.appendChild(this.rootSVG);
    }
    update(_dTime) {
        var altitude = SimVar.GetSimVarValue("INDICATED ALTITUDE:2", "feet");
        this.updateGraduationScrolling(altitude);
        this.updateCursorScrolling(altitude);
    }
    updateGraduationScrolling(_altitude) {
        if (this.graduations) {
            this.graduationScroller.scroll(_altitude);
            var currentVal = this.graduationScroller.firstValue;
            var currentY = this.graduationScrollPosY + this.graduationScroller.offsetY * this.graduationSpacing * (this.nbSecondaryGraduations + 1);
            var firstRoundValueY = currentY;
            for (var i = 0; i < this.totalGraduations; i++) {
                var posX = this.graduationScrollPosX;
                var posY = Math.round(currentY);
                this.graduations[i].SVGLine.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                if (this.graduations[i].SVGText1) {
                    var roundedVal = 0;
                    roundedVal = Math.floor(Math.abs(currentVal));
                    var integral = Math.floor(roundedVal / 100);
                    this.graduations[i].SVGText1.textContent = Utils.leadingZeros(integral, 3);
                    this.graduations[i].SVGText1.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                    if (this.graduations[i].SVGText2)
                        this.graduations[i].SVGText2.setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                    firstRoundValueY = posY;
                    currentVal = this.graduationScroller.nextValue;
                }
                currentY -= this.graduationSpacing;
            }
            if (this.graduationBarSVG) {
                this.graduationBarSVG.setAttribute("transform", "translate(0 " + firstRoundValueY + ")");
            }
        }
    }
    updateCursorScrolling(_altitude) {
        if (this.cursorSVGScrollTexts) {
            var integral = Math.trunc(_altitude / 100);
            if (Math.abs(integral) > 0)
                this.cursorSVGMainText.textContent = integral.toString();
            else
                this.cursorSVGMainText.textContent = "";
            this.cursorScroller.scroll(_altitude);
            var currentVal = this.cursorScroller.firstValue;
            var currentY = this.cursorScrollPosY + this.cursorScroller.offsetY * this.cursorScrollSpacing;
            for (var i = 0; i < this.cursorSVGScrollTexts.length; i++) {
                var posX = this.cursorScrollPosX;
                var posY = Math.round(currentY);
                if (currentVal == 0)
                    this.cursorSVGScrollTexts[i].textContent = "00";
                else
                    this.cursorSVGScrollTexts[i].textContent = currentVal.toString();
                this.cursorSVGScrollTexts[i].setAttribute("transform", "translate(" + posX.toString() + " " + posY.toString() + ")");
                currentY -= this.cursorScrollSpacing;
                currentVal = this.cursorScroller.nextValue;
            }
        }
    }
}
customElements.define('a320-neo-sai-altimeter-indicator', A320_Neo_SAI_AltimeterIndicator);

class A320_Neo_SAI_Attitude extends NavSystemElement {
  init(root) {
      this.attitudeElement = this.gps.getChildById("Horizon");
      this.attitudeElement.setAttribute("is-backup", "true");
      if (this.gps) {
          var aspectRatio = this.gps.getAspectRatio();
          this.attitudeElement.setAttribute("aspect-ratio", aspectRatio.toString());
      }
  }
  onEnter() {
  }
  onUpdate(_deltaTime) {
      var xyz = Simplane.getOrientationAxis();
      if (xyz) {
          this.attitudeElement.setAttribute("pitch", (xyz.pitch / Math.PI * 180).toString());
          this.attitudeElement.setAttribute("bank", (xyz.bank / Math.PI * 180).toString());
          this.attitudeElement.setAttribute("slip_skid", Simplane.getInclinometer().toString());
      }
  }
  onExit() {
  }
  onEvent(_event) {
  }
}
class A320_Neo_SAI_AttitudeIndicator extends HTMLElement {
  constructor() {
      super();
      this.backgroundVisible = true;
      this.bankSizeRatio = -8.25;
      this.bankSizeRatioFactor = 1.0;
  }
  static get observedAttributes() {
      return [
          "pitch",
          "bank",
          "slip_skid",
          "background",
      ];
  }
  connectedCallback() {
      this.construct();
  }
  construct() {
      Utils.RemoveAllChildren(this);
      this.bankSizeRatioFactor = 0.62;
      {
          this.horizon_root = document.createElementNS(Avionics.SVG.NS, "svg");
          this.horizon_root.setAttribute("width", "100%");
          this.horizon_root.setAttribute("height", "100%");
          this.horizon_root.setAttribute("viewBox", "-200 -200 400 300");
          this.horizon_root.setAttribute("x", "-100");
          this.horizon_root.setAttribute("y", "-100");
          this.horizon_root.setAttribute("overflow", "visible");
          this.horizon_root.setAttribute("style", "position:absolute; z-index: -3; width: 100%; height:100%;");
          this.horizon_root.setAttribute("transform", "translate(0, 100)");
          this.appendChild(this.horizon_root);
          this.horizonTopColor = "#3D9FFF";
          this.horizonBottomColor = "#905A45";
          this.horizonTop = document.createElementNS(Avionics.SVG.NS, "rect");
          this.horizonTop.setAttribute("fill", (this.backgroundVisible) ? this.horizonTopColor : "transparent");
          this.horizonTop.setAttribute("x", "-1000");
          this.horizonTop.setAttribute("y", "-1000");
          this.horizonTop.setAttribute("width", "2000");
          this.horizonTop.setAttribute("height", "2000");
          this.horizon_root.appendChild(this.horizonTop);
          this.bottomPart = document.createElementNS(Avionics.SVG.NS, "g");
          this.horizon_root.appendChild(this.bottomPart);
          this.horizonBottom = document.createElementNS(Avionics.SVG.NS, "rect");
          this.horizonBottom.setAttribute("fill", (this.backgroundVisible) ? this.horizonBottomColor : "transparent");
          this.horizonBottom.setAttribute("x", "-1500");
          this.horizonBottom.setAttribute("y", "0");
          this.horizonBottom.setAttribute("width", "3000");
          this.horizonBottom.setAttribute("height", "3000");
          this.bottomPart.appendChild(this.horizonBottom);
          let separator = document.createElementNS(Avionics.SVG.NS, "rect");
          separator.setAttribute("fill", "#e0e0e0");
          separator.setAttribute("x", "-1500");
          separator.setAttribute("y", "-3");
          separator.setAttribute("width", "3000");
          separator.setAttribute("height", "6");
          this.bottomPart.appendChild(separator);
      }
      {
          let pitchContainer = document.createElement("div");
          pitchContainer.setAttribute("id", "Pitch");
          pitchContainer.style.top = "-14%";
          pitchContainer.style.left = "-10%";
          pitchContainer.style.width = "120%";
          pitchContainer.style.height = "120%";
          pitchContainer.style.position = "absolute";
          pitchContainer.style.transform = "scale(1.35)";
          this.appendChild(pitchContainer);
          this.pitch_root = document.createElementNS(Avionics.SVG.NS, "svg");
          this.pitch_root.setAttribute("width", "100%");
          this.pitch_root.setAttribute("height", "100%");
          this.pitch_root.setAttribute("viewBox", "-200 -200 400 300");
          this.pitch_root.setAttribute("overflow", "visible");
          this.pitch_root.setAttribute("style", "position:absolute; z-index: -2;");
          pitchContainer.appendChild(this.pitch_root);
          {
              this.pitch_root_group = document.createElementNS(Avionics.SVG.NS, "g");
              this.pitch_root.appendChild(this.pitch_root_group);
              var x = -115;
              var y = -122;
              var w = 230;
              var h = 275;
              let attitudePitchContainer = document.createElementNS(Avionics.SVG.NS, "svg");
              attitudePitchContainer.setAttribute("width", w.toString());
              attitudePitchContainer.setAttribute("height", h.toString());
              attitudePitchContainer.setAttribute("x", x.toString());
              attitudePitchContainer.setAttribute("y", y.toString());
              attitudePitchContainer.setAttribute("viewBox", x + " " + y + " " + w + " " + h);
              attitudePitchContainer.setAttribute("overflow", "hidden");
              this.pitch_root_group.appendChild(attitudePitchContainer);
              {
                  this.attitude_pitch = document.createElementNS(Avionics.SVG.NS, "g");
                  attitudePitchContainer.appendChild(this.attitude_pitch);
                  let maxDash = 80;
                  let fullPrecisionLowerLimit = -20;
                  let fullPrecisionUpperLimit = 20;
                  let halfPrecisionLowerLimit = -30;
                  let halfPrecisionUpperLimit = 45;
                  let unusualAttitudeLowerLimit = -30;
                  let unusualAttitudeUpperLimit = 50;
                  let bigWidth = 50;
                  let bigHeight = 3;
                  let mediumWidth = 30;
                  let mediumHeight = 3;
                  let smallWidth = 10;
                  let smallHeight = 2;
                  let fontSize = 20;
                  let angle = -maxDash;
                  let nextAngle;
                  let width;
                  let height;
                  let text;
                  while (angle <= maxDash) {
                      if (angle % 10 == 0) {
                          width = bigWidth;
                          height = bigHeight;
                          text = true;
                          if (angle >= fullPrecisionLowerLimit && angle < fullPrecisionUpperLimit) {
                              nextAngle = angle + 2.5;
                          }
                          else if (angle >= halfPrecisionLowerLimit && angle < halfPrecisionUpperLimit) {
                              nextAngle = angle + 5;
                          }
                          else {
                              nextAngle = angle + 10;
                          }
                      }
                      else {
                          if (angle % 5 == 0) {
                              width = mediumWidth;
                              height = mediumHeight;
                              text = false;
                              if (angle >= fullPrecisionLowerLimit && angle < fullPrecisionUpperLimit) {
                                  nextAngle = angle + 2.5;
                              }
                              else {
                                  nextAngle = angle + 5;
                              }
                          }
                          else {
                              width = smallWidth;
                              height = smallHeight;
                              nextAngle = angle + 2.5;
                              text = false;
                          }
                      }
                      if (angle != 0) {
                          let rect = document.createElementNS(Avionics.SVG.NS, "rect");
                          rect.setAttribute("fill", "white");
                          rect.setAttribute("x", (-width / 2).toString());
                          rect.setAttribute("y", (this.bankSizeRatio * angle - height / 2).toString());
                          rect.setAttribute("width", width.toString());
                          rect.setAttribute("height", height.toString());
                          this.attitude_pitch.appendChild(rect);
                          if (text) {
                              let leftText = document.createElementNS(Avionics.SVG.NS, "text");
                              leftText.textContent = Math.abs(angle).toString();
                              leftText.setAttribute("x", ((-width / 2) - 5).toString());
                              leftText.setAttribute("y", (this.bankSizeRatio * angle - height / 2 + fontSize / 2).toString());
                              leftText.setAttribute("text-anchor", "end");
                              leftText.setAttribute("font-size", (fontSize * 1.2).toString());
                              leftText.setAttribute("font-family", "Roboto-Bold");
                              leftText.setAttribute("fill", "white");
                              this.attitude_pitch.appendChild(leftText);
                          }
                          if (angle < unusualAttitudeLowerLimit) {
                              let chevron = document.createElementNS(Avionics.SVG.NS, "path");
                              let path = "M" + -smallWidth / 2 + " " + (this.bankSizeRatio * nextAngle - bigHeight / 2) + " l" + smallWidth + "  0 ";
                              path += "L" + bigWidth / 2 + " " + (this.bankSizeRatio * angle - bigHeight / 2) + " l" + -smallWidth + " 0 ";
                              path += "L0 " + (this.bankSizeRatio * nextAngle + 20) + " ";
                              path += "L" + (-bigWidth / 2 + smallWidth) + " " + (this.bankSizeRatio * angle - bigHeight / 2) + " l" + -smallWidth + " 0 Z";
                              chevron.setAttribute("d", path);
                              chevron.setAttribute("fill", "red");
                              this.attitude_pitch.appendChild(chevron);
                          }
                          if (angle >= unusualAttitudeUpperLimit && nextAngle <= maxDash) {
                              let chevron = document.createElementNS(Avionics.SVG.NS, "path");
                              let path = "M" + -smallWidth / 2 + " " + (this.bankSizeRatio * angle - bigHeight / 2) + " l" + smallWidth + "  0 ";
                              path += "L" + (bigWidth / 2) + " " + (this.bankSizeRatio * nextAngle + bigHeight / 2) + " l" + -smallWidth + " 0 ";
                              path += "L0 " + (this.bankSizeRatio * angle - 20) + " ";
                              path += "L" + (-bigWidth / 2 + smallWidth) + " " + (this.bankSizeRatio * nextAngle + bigHeight / 2) + " l" + -smallWidth + " 0 Z";
                              chevron.setAttribute("d", path);
                              chevron.setAttribute("fill", "red");
                              this.attitude_pitch.appendChild(chevron);
                          }
                      }
                      angle = nextAngle;
                  }
              }
          }
      }
      {
          let attitudeContainer = document.createElement("div");
          attitudeContainer.setAttribute("id", "Attitude");
          attitudeContainer.style.top = "-14%";
          attitudeContainer.style.left = "-10%";
          attitudeContainer.style.width = "120%";
          attitudeContainer.style.height = "120%";
          attitudeContainer.style.position = "absolute";
          attitudeContainer.style.transform = "scale(1.35)";
          this.appendChild(attitudeContainer);
          this.attitude_root = document.createElementNS(Avionics.SVG.NS, "svg");
          this.attitude_root.setAttribute("width", "100%");
          this.attitude_root.setAttribute("height", "100%");
          this.attitude_root.setAttribute("viewBox", "-200 -200 400 300");
          this.attitude_root.setAttribute("overflow", "visible");
          this.attitude_root.setAttribute("style", "position:absolute; z-index: 0");
          attitudeContainer.appendChild(this.attitude_root);
          {
              this.attitude_bank = document.createElementNS(Avionics.SVG.NS, "g");
              this.attitude_root.appendChild(this.attitude_bank);
              let topTriangle = document.createElementNS(Avionics.SVG.NS, "path");
              topTriangle.setAttribute("d", "M0 -180 l-7.5 -10 l15 0 Z");
              topTriangle.setAttribute("fill", "white");
              topTriangle.setAttribute("stroke", "white");
              topTriangle.setAttribute("stroke-width", "1");
              topTriangle.setAttribute("stroke-opacity", "1");
              this.attitude_bank.appendChild(topTriangle);
              let smallDashesAngle = [-50, -40, -30, -20, -10, 10, 20, 30, 40, 50];
              let smallDashesHeight = [18, 18, 18, 11, 11, 11, 11, 18, 18, 18];
              let radius = 175;
              for (let i = 0; i < smallDashesAngle.length; i++) {
                  let dash = document.createElementNS(Avionics.SVG.NS, "line");
                  dash.setAttribute("x1", "0");
                  dash.setAttribute("y1", (-radius).toString());
                  dash.setAttribute("x2", "0");
                  dash.setAttribute("y2", (-radius - smallDashesHeight[i]).toString());
                  dash.setAttribute("fill", "none");
                  dash.setAttribute("stroke", "white");
                  dash.setAttribute("stroke-width", "2");
                  dash.setAttribute("transform", "rotate(" + smallDashesAngle[i] + ",0,0)");
                  this.attitude_bank.appendChild(dash);
              }
              let arc = document.createElementNS(Avionics.SVG.NS, "path");
              arc.setAttribute("d", "M-88 -150 q88 -48 176 0");
              arc.setAttribute("fill", "transparent");
              arc.setAttribute("stroke", "white");
              arc.setAttribute("stroke-width", "1.5");
              this.attitude_bank.appendChild(arc);
          }
          {
              let cursors = document.createElementNS(Avionics.SVG.NS, "g");
              this.attitude_root.appendChild(cursors);
              let leftUpper = document.createElementNS(Avionics.SVG.NS, "path");
              leftUpper.setAttribute("d", "M-90 0 l0 -6 l55 0 l0 28 l-5 0 l0 -22 l-40 0 Z");
              leftUpper.setAttribute("fill", "black");
              leftUpper.setAttribute("stroke", "yellow");
              leftUpper.setAttribute("stroke-width", "0.7");
              leftUpper.setAttribute("stroke-opacity", "1.0");
              cursors.appendChild(leftUpper);
              let rightUpper = document.createElementNS(Avionics.SVG.NS, "path");
              rightUpper.setAttribute("d", "M90 0 l0 -6 l-55 0 l0 28 l5 0 l0 -22 l40 0 Z");
              rightUpper.setAttribute("fill", "black");
              rightUpper.setAttribute("stroke", "yellow");
              rightUpper.setAttribute("stroke-width", "0.7");
              rightUpper.setAttribute("stroke-opacity", "1.0");
              cursors.appendChild(rightUpper);
              let centerRect = document.createElementNS(Avionics.SVG.NS, "rect");
              centerRect.setAttribute("x", "-4");
              centerRect.setAttribute("y", "-8");
              centerRect.setAttribute("height", "8");
              centerRect.setAttribute("width", "8");
              centerRect.setAttribute("stroke", "yellow");
              centerRect.setAttribute("stroke-width", "3");
              cursors.appendChild(centerRect);
              this.slipSkidTriangle = document.createElementNS(Avionics.SVG.NS, "path");
              this.slipSkidTriangle.setAttribute("d", "M0 -170 l-13 20 l26 0 Z");
              this.slipSkidTriangle.setAttribute("fill", "black");
              this.slipSkidTriangle.setAttribute("stroke", "white");
              this.slipSkidTriangle.setAttribute("stroke-width", "1");
              this.attitude_root.appendChild(this.slipSkidTriangle);
              this.slipSkid = document.createElementNS(Avionics.SVG.NS, "path");
              this.slipSkid.setAttribute("d", "M-20 -140 L-16 -146 L16 -146 L20 -140 Z");
              this.slipSkid.setAttribute("fill", "black");
              this.slipSkid.setAttribute("stroke", "white");
              this.slipSkid.setAttribute("stroke-width", "1");
              this.attitude_root.appendChild(this.slipSkid);
          }
      }
      this.applyAttributes();
  }
  attributeChangedCallback(name, oldValue, newValue) {
      if (oldValue == newValue)
          return;
      switch (name) {
          case "pitch":
              this.pitch = parseFloat(newValue);
              break;
          case "bank":
              this.bank = parseFloat(newValue);
              break;
          case "slip_skid":
              this.slipSkidValue = parseFloat(newValue);
              break;
          case "background":
              if (newValue == "false")
                  this.backgroundVisible = false;
              else
                  this.backgroundVisible = true;
              break;
          default:
              return;
      }
      this.applyAttributes();
  }
  applyAttributes() {
      if (this.bottomPart)
          this.bottomPart.setAttribute("transform", "rotate(" + this.bank + ", 0, 0) translate(0," + (this.pitch * this.bankSizeRatio) + ")");
      if (this.pitch_root_group)
          this.pitch_root_group.setAttribute("transform", "rotate(" + this.bank + ", 0, 0)");
      if (this.attitude_pitch)
          this.attitude_pitch.setAttribute("transform", "translate(0," + (this.pitch * this.bankSizeRatio * this.bankSizeRatioFactor) + ")");
      if (this.slipSkid)
          this.slipSkid.setAttribute("transform", "rotate(" + this.bank + ", 0, 0) translate(" + (this.slipSkidValue * 40) + ", 0)");
      if (this.slipSkidTriangle)
          this.slipSkidTriangle.setAttribute("transform", "rotate(" + this.bank + ", 0, 0)");
      if (this.horizonTop) {
          if (this.backgroundVisible) {
              this.horizonTop.setAttribute("fill", this.horizonTopColor);
              this.horizonBottom.setAttribute("fill", this.horizonBottomColor);
          }
          else {
              this.horizonTop.setAttribute("fill", "transparent");
              this.horizonBottom.setAttribute("fill", "transparent");
          }
      }
  }
}
customElements.define('a320-neo-sai-attitude-indicator', A320_Neo_SAI_AttitudeIndicator);


class A320_Neo_SAI_Pressure extends NavSystemElement {
    init(root) {
        this.pressureElement = this.gps.getChildById("Pressure");
    }
    onEnter() {
    }
    isReady() {
        return true;
    }
    onUpdate(_deltaTime) {
        this.pressureElement.update(_deltaTime);
    }
    onExit() {
    }
    onEvent(_event) {
    }
}
class A320_Neo_SAI_PressureIndicator extends HTMLElement {
    connectedCallback() {
        this.construct();
    }
    construct() {
        Utils.RemoveAllChildren(this);
        this.rootSVG = document.createElementNS(Avionics.SVG.NS, "svg");
        this.rootSVG.setAttribute("id", "ViewBox");
        this.rootSVG.setAttribute("viewBox", "0 0 200 30");
        if (!this.rootGroup) {
            this.rootGroup = document.createElementNS(Avionics.SVG.NS, "g");
            this.rootGroup.setAttribute("id", "Pressure");
        }
        else {
            Utils.RemoveAllChildren(this.rootGroup);
        }

        if (!this.pressureSVG) {
            this.pressureSVG = document.createElementNS(Avionics.SVG.NS, "text");
        }
        this.pressureSVG.textContent = "";
        this.pressureSVG.setAttribute("x", "0");
        this.pressureSVG.setAttribute("y", "10");
        this.pressureSVG.setAttribute("fill", "cyan");
        this.pressureSVG.setAttribute("font-size", "27");
        this.pressureSVG.setAttribute("font-family", "Roboto");
        this.pressureSVG.setAttribute("text-anchor", "start");
        this.pressureSVG.setAttribute("alignment-baseline", "central");

        this.rootGroup.appendChild(this.pressureSVG);
        this.rootSVG.appendChild(this.rootGroup);
        this.appendChild(this.rootSVG);
    }
    update() {
        if (this.pressureSVG) {
            const pressureInHg = SimVar.GetSimVarValue("KOHLSMAN SETTING HG:2", "inches of mercury");
            if (pressureInHg !== this.lastPressureValue) {
                this.lastPressureValue = pressureInHg;
                const pressureHpa = SimVar.GetSimVarValue("KOHLSMAN SETTING HG:2", "millibar");
                this.pressureSVG.textContent = fastToFixed(pressureHpa, 0) + "/" + pressureInHg.toFixed(2);
            }
        }
    }
}
customElements.define('a320-neo-sai-pressure-indicator', A320_Neo_SAI_PressureIndicator);

class A320_Neo_SAI_SelfTest extends NavSystemElement {
    init(root) {
        this._lastTime = Date.now();
        this.selfTestElement = this.gps.getChildById("SelfTest");
    }
    onEnter() {
    }
    isReady() {
        return true;
    }
    onUpdate(_deltaTime) {

        // Delta time mitigation strategy
        const curTime = Date.now();
        const localDeltaTime = curTime - this._lastTime;
        this._lastTime = curTime;

        const ac_pwr = SimVar.GetSimVarValue("L:ACPowerAvailable", "bool");
        const dc_pwr = SimVar.GetSimVarValue("L:DCPowerAvailable", "bool");
        const cold_dark = SimVar.GetSimVarValue('L:A32NX_COLD_AND_DARK_SPAWN', 'Bool');
        const complete = this.selfTestElement.complete;

        if ((ac_pwr || dc_pwr) && !complete) {
            this.selfTestElement.update(localDeltaTime);
        }
        if (!ac_pwr && !dc_pwr) {
            // TODO: More realistic/advanced Behaviour when power is lost
            this.selfTestElement.resetTimer();
        }

        if (!cold_dark && ac_pwr && dc_pwr) {
            // TODO: better way of doing this not on loop
            this.selfTestElement.finishTest();
        }

    }
    onExit() {
    }
    onEvent(_event) {
    }
}
class A320_Neo_SAI_SelfTestTimer extends HTMLElement {

    connectedCallback() {
        this.construct();
    }

    construct() {
        Utils.RemoveAllChildren(this);
        const boxHeight = 7;
        const boxWidthSmall = 15;
        const boxWidth = 18;
        const boxWidthInit = 30;
        const boxRow1 = 32.5;
        const boxRow2 = 46.5;
        const boxRow3 = 64;
        const txt_off = 6;

        this.start_time = 73;
        this.complete = false;
        this.testTimer = Math.floor(Math.random() * 3) + this.start_time;

        this.hide_inst_div = document.querySelector("#SelfTestHider");
        this.hide_inst_div.style.display = "none";

        this.selfTestDiv = document.createElement("div");
        this.selfTestDiv.id = "SelfTestDiv";
        this.selfTestDiv.setAttribute("border", "none");
        this.selfTestDiv.setAttribute("position", "absolute");
        this.selfTestDiv.setAttribute("display", "block");
        this.selfTestDiv.setAttribute("top", "0%");
        this.selfTestDiv.setAttribute("left", "0%");
        this.selfTestDiv.setAttribute("width", "100%");
        this.selfTestDiv.setAttribute("height", "100%");
        this.appendChild(this.selfTestDiv);

        this.selfTestSVG = document.createElementNS(Avionics.SVG.NS, "svg");
        this.selfTestSVG.setAttribute("id", "SelfTestSVG");
        this.selfTestSVG.setAttribute("viewBox", "0 0 600 600");
        this.selfTestSVG.style.position = "absolute";
        this.selfTestSVG.style.top = "0%";
        this.selfTestSVG.style.left = "0%";
        this.selfTestSVG.style.width = "100%";
        this.selfTestSVG.style.height = "100%";
        this.selfTestDiv.appendChild(this.selfTestSVG);

        const st_spd = document.createElementNS(Avionics.SVG.NS, "rect");
        st_spd.setAttribute("id", "SpeedTest");
        st_spd.setAttribute("fill", "#afbb3a");
        st_spd.setAttribute("x", "8%");
        st_spd.setAttribute("y", boxRow2 + "%");
        st_spd.setAttribute("width", boxWidthSmall + "%");
        st_spd.setAttribute("height", boxHeight + "%");
        this.selfTestSVG.appendChild(st_spd);

        const st_spd_txt = document.createElementNS(Avionics.SVG.NS, "text");
        st_spd_txt.setAttribute("id", "SpeedTestTxt");
        st_spd_txt.textContent = "SPD";
        st_spd_txt.setAttribute("font", "Roboto");
        st_spd_txt.setAttribute("font-weight", "900");
        st_spd_txt.setAttribute("font-size", "42px");
        st_spd_txt.setAttribute("fill", "#1f242d");
        st_spd_txt.setAttribute("x", "8.75%");
        st_spd_txt.setAttribute("y", boxRow2 + txt_off + "%");
        this.selfTestSVG.appendChild(st_spd_txt);

        const st_alt = document.createElementNS(Avionics.SVG.NS, "rect");
        st_alt.setAttribute("id", "AltTest");
        st_alt.setAttribute("fill", "#afbb3a");
        st_alt.setAttribute("x", "70%");
        st_alt.setAttribute("y", boxRow2 + "%");
        st_alt.setAttribute("width", boxWidth + "%");
        st_alt.setAttribute("height", boxHeight + "%");
        this.selfTestSVG.appendChild(st_alt);

        const st_alt_txt = document.createElementNS(Avionics.SVG.NS, "text");
        st_alt_txt.setAttribute("id", "AltTestTxt")
        st_alt_txt.textContent = "ALT";
        st_alt_txt.setAttribute("font", "Roboto");
        st_alt_txt.setAttribute("font-weight", "900");
        st_alt_txt.setAttribute("font-size", "42px");
        st_alt_txt.setAttribute("fill", "#1f242d");
        st_alt_txt.setAttribute("x", "72.5%");
        st_alt_txt.setAttribute("y", boxRow2 + txt_off + "%");
        this.selfTestSVG.appendChild(st_alt_txt);

        const st_tmr = document.createElementNS(Avionics.SVG.NS, "rect");
        st_tmr.setAttribute("id", "TmrTest")
        st_tmr.setAttribute("fill", "#afbb3a");
        st_tmr.setAttribute("x", "30%");
        st_tmr.setAttribute("y", boxRow3 + "%");
        st_tmr.setAttribute("width", boxWidthInit + "%");
        st_tmr.setAttribute("height", boxHeight + "%");
        this.selfTestSVG.appendChild(st_tmr);

        this.st_tmr_txt = document.createElementNS(Avionics.SVG.NS, "text");
        this.st_tmr_txt.setAttribute("id", "TmrTestTxt")
        this.st_tmr_txt.textContent = "INIT "+ Math.ceil(this.testTimer)  +"s";
        this.st_tmr_txt.setAttribute("font", "Roboto");
        this.st_tmr_txt.setAttribute("font-weight", "900");
        this.st_tmr_txt.setAttribute("font-size", "42px");
        this.st_tmr_txt.setAttribute("fill", "#1f242d");
        this.st_tmr_txt.setAttribute("x", "31%");
        this.st_tmr_txt.setAttribute("y", boxRow3 + txt_off + "%");
        this.selfTestSVG.appendChild(this.st_tmr_txt);

        const st_att = document.createElementNS(Avionics.SVG.NS, "rect");
        st_att.setAttribute("id", "AttTest")
        st_att.setAttribute("fill", "#afbb3a");
        st_att.setAttribute("x", "36%");
        st_att.setAttribute("y", boxRow1 + "%");
        st_att.setAttribute("width", boxWidth + "%");
        st_att.setAttribute("height", boxHeight + "%");
        this.selfTestSVG.appendChild(st_att);

        const st_att_txt = document.createElementNS(Avionics.SVG.NS, "text");
        st_att_txt.setAttribute("id", "AttTestTxt")
        st_att_txt.textContent = "ATT";
        st_att_txt.setAttribute("font", "Roboto");
        st_att_txt.setAttribute("font-weight", "900");
        st_att_txt.setAttribute("font-size", "42px");
        st_att_txt.setAttribute("fill", "#1f242d");
        st_att_txt.setAttribute("x", "38%");
        st_att_txt.setAttribute("y", boxRow1 + txt_off + "%");
        this.selfTestSVG.appendChild(st_att_txt);

    }
    update(dTime) {
        if (this.complete) return;
        if (this.testTimer >= 0) {
            this.testTimer -= dTime / 1000;
        }

        if (this.testTimer > 9) {
            this.st_tmr_txt.textContent = "INIT " + Math.ceil(this.testTimer) + "s";
        }
        else {
            this.st_tmr_txt.textContent = "INIT 0" + Math.ceil(this.testTimer) + "s";
        }
        if (this.testTimer <= 0) {
            this.finishTest();
        }
    }

    resetTimer() {
        if (this.testTimer > this.start_time) return;
        this.testTimer = Math.floor(Math.random() * 3) + this.start_time;
        this.complete = false;
        this.hide_inst_div.style.display = "none";
        this.selfTestDiv.style.display = "block";
    }
    finishTest() {
        if (this.complete) return;
        this.testTimer = 0;
        this.selfTestDiv.style.display = "none";
        this.hide_inst_div.style.display = "block";
        this.complete = true;
    }
}

customElements.define('a320-neo-sai-self-test', A320_Neo_SAI_SelfTestTimer);

registerInstrument("a320-neo-sai", A320_Neo_SAI);
//# sourceMappingURL=A320_Neo_SAI.js.map
