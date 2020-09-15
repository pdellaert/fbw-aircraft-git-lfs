class CDUIRSInit {
    static ShowPage(mcdu, lon) {
        mcdu.clearDisplay();
        mcdu.setTitle('IRS INIT');
        const checkAligned = SimVar.GetSimVarValue("L:A320_Neo_ADIRS_STATE", "Number");
        const emptyIRSGpsString = "--°--.--/---°--.--"
        const arrowupdwn = "↑↓";
        let larrowupdwn = arrowupdwn;
        let rarrowupdwn = "";
        if (lon) {
            rarrowupdwn = arrowupdwn;
            larrowupdwn = "";
        }
        let statusIRS1;
        let statusIRS2;
        let statusIRS3;
        // Ref coordinates are taken based on origin airport
        const airportCoordinates = mcdu.flightPlanManager.getOrigin().infos.coordinates;
        const originAirportLat = CDUInitPage.ConvertDDToDMS(airportCoordinates['lat'], false);
        const originAirportLon = CDUInitPage.ConvertDDToDMS(airportCoordinates['long'], true);
        const currentGPSLat = CDUInitPage.ConvertDDToDMS(SimVar.GetSimVarValue("GPS POSITION LAT", "degree latitude"), false);
        const currentGPSLon = CDUInitPage.ConvertDDToDMS(SimVar.GetSimVarValue("GPS POSITION LON", "degree longitude"), true);
        let alignMsg = "ALIGN ON REF →[color]blue";
        let GPSPosAlign = ["--°--.--", "--°--.--", " "];
        if (SimVar.GetSimVarValue("L:A320_Neo_ADIRS_KNOB_1", "Enum") || SimVar.GetSimVarValue("L:A320_Neo_ADIRS_KNOB_2", "Enum") || SimVar.GetSimVarValue("L:A320_Neo_ADIRS_KNOB_3", "Enum")) {
            GPSPosAlign = ['{IrsInitFont}' + currentGPSLat['deg'] + '°{IrsInitFontEnd}' + currentGPSLat['min'] + '.' + Math.ceil(Number(currentGPSLat['sec'] / 100)) + '[s-text]{IrsInitFont}' + currentGPSLat['dir'] + "{IrsInitFontEnd} [color]green", '{IrsInitFont}' + currentGPSLon['deg'] + '°{IrsInitFontEnd}' + currentGPSLon['min'] + '.' + Math.ceil(Number(currentGPSLon['sec'] / 100)) + '[s-text]{IrsInitFont}' + currentGPSLon['dir'] + "{IrsInitFontEnd} [color]green", ""];
            if (!SimVar.GetSimVarValue("A32NX_ADIRS_PFD_ALIGNED_FIRST", "boolean") && !SimVar.GetSimVarValue("L:A320_Neo_ADIRS_IN_ALIGN", "Bool") && checkAligned == 1) {
                alignMsg = "CONFIRM ALIGN* [color]red";
            }
        }
        let IRSAlignOnPos = "{IrsInitFont}" + currentGPSLat['deg'] + '°{IrsInitFontEnd}' + currentGPSLat['min'] + '.' + Math.ceil(Number(currentGPSLat['sec'] / 100)) + '[s-text]{IrsInitFont}' + currentGPSLat['dir'] + '/{IrsInitFontEnd}{IrsInitFont}' + currentGPSLon['deg'] + '°{IrsInitFontEnd}' + currentGPSLon['min'] + '.' + Math.ceil(Number(currentGPSLon['sec'] / 100)) + '{IrsInitFont}' +  currentGPSLon['dir'] + '{IrsInitFontEnd}';
        let IRS1GpsString = emptyIRSGpsString;
        if (SimVar.GetSimVarValue("L:A320_Neo_ADIRS_KNOB_1", "Enum") == 1) {
            if (checkAligned == 2) {
                statusIRS1 = "IRS1 ALIGNED ON GPS";
            } else {
                statusIRS1 = "IRS1 ALIGNING ON GPS";
            }
            IRS1GpsString = IRSAlignOnPos + "[color]green";
        } else {
            statusIRS1 = "IRS1 OFF";
        }
        let IRS2GpsString = emptyIRSGpsString;
        if (SimVar.GetSimVarValue("L:A320_Neo_ADIRS_KNOB_2", "Enum") == 1) {
            if (checkAligned == 2) {
                statusIRS2 = "IRS2 ALIGNED ON GPS";
            } else {
                statusIRS2 = "IRS2 ALIGNING ON GPS";
            }
            IRS2GpsString = IRSAlignOnPos + "[color]green";
        } else {
            statusIRS2 = "IRS2 OFF";
        }
        let IRS3GpsString = emptyIRSGpsString;
        if (SimVar.GetSimVarValue("L:A320_Neo_ADIRS_KNOB_3", "Enum") == 1) {
            if (checkAligned == 2) {
                statusIRS3 = "IRS3 ALIGNED ON GPS";
            } else {
                statusIRS3 = "IRS3 ALIGNING ON GPS";
            }
            IRS3GpsString = IRSAlignOnPos + "[color]green";
        } else {
            statusIRS3 = "IRS3 OFF";
        }

        mcdu.setTemplate([
            ["IRS INIT"],
            ["LAT" + larrowupdwn , rarrowupdwn + "LONG", "REFERENCE",],
            ['{IrsInitFont}' + originAirportLat['deg'] + '°{IrsInitFontEnd}' + originAirportLat['min'] + '.' + Math.ceil(Number(originAirportLat['sec'] / 100)) + '[s-text]{IrsInitFont}' + originAirportLat['dir'] + "{IrsInitFontEnd} [color]blue", '{IrsInitFont}' + originAirportLon['deg'] + '°{IrsInitFontEnd}' + originAirportLon['min'] + '.' + Math.ceil(Number(originAirportLon['sec'] / 100)) + '[s-text]{IrsInitFont}' + originAirportLon['dir'] + "{IrsInitFontEnd} [color]blue", mcdu.flightPlanManager.getOrigin().ident + " [color]green"],
            ["LAT", "LONG", "GPS POSITION"],
            GPSPosAlign,
            ["", "", statusIRS1],
            ["", "", IRS1GpsString],
            ["", "", statusIRS2],
            ["", "", IRS2GpsString],
            ["", "", statusIRS3],
            ["", "", IRS3GpsString],
            [],
            ["<RETURN", alignMsg]
        ]);
        
        // IRS Font is different we loop over keywords to set correct font since at the moment we cannot adapt FMCMainDisplay.js to allow for extra keywords
        mcdu._lineElements.forEach(function (ele) {
            ele.forEach(function (el) {
                if (el != null) {
                let newHtml = el;
                if (newHtml != null) {
                    newHtml = newHtml.innerHTML.replace(/{IrsInitFont}/g, '<span class=\'irs-text\'>');
                    newHtml = newHtml.replace(/{IrsInitFontEnd}/g, '</span>');
                    el.innerHTML = newHtml;
                }
            }})
        });

        mcdu.onLeftInput[0] = () => {
            lon = false;
        };

        mcdu.onRightInput[0] = () => {
            lon = true;
        };

        mcdu.onLeftInput[5] = () => {
            CDUInitPage.ShowPage1(mcdu);
        };

        mcdu.onRightInput[5] = () => {
            if (!SimVar.GetSimVarValue("A32NX_ADIRS_PFD_ALIGNED_FIRST", "Bool") && !SimVar.GetSimVarValue("L:A320_Neo_ADIRS_IN_ALIGN", "Bool")) {
                SimVar.SetSimVarValue("L:A320_Neo_ADIRS_IN_ALIGN", "Bool", 1);
            } else {
                // Ref alignment not currently implemented
            }
        };


        // This page auto-refreshes based on source material. Function will stop auto-refreshing when page has been changed.
        autoRefresh();
        function autoRefresh() {
            setTimeout(() => {
                if (mcdu.getTitle() == 'IRS INIT') {
                    CDUIRSInit.ShowPage(mcdu, lon = lon);
                }
            }, 2000);
        }

    }
}
