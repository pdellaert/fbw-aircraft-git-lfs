class CDUAtcDepartReq {
    static ShowPage1(mcdu) {
        mcdu.clearDisplay();

        if (mcdu.pdcMessage === undefined) {
            mcdu.pdcMessage = new Atsu.PdcMessage();
        }

        let flightNo = "______[color]amber";
        let fromTo = "____|____[color]amber";
        const atis = new CDU_SingleValueField(mcdu,
            "string",
            mcdu.pdcMessage.Atis,
            {
                clearable: true,
                emptyValue: "_[color]amber",
                suffix: "[color]cyan",
                maxLength: 1,
                isValid: ((value) => {
                    return /^[A-Z()]*$/.test(value) === true;
                })
            },
            (value) => {
                mcdu.pdcMessage.Atis = value;
                CDUAtcDepartReq.ShowPage1(mcdu);
            }
        );
        const gate = new CDU_SingleValueField(mcdu,
            "string",
            mcdu.pdcMessage.Gate,
            {
                clearable: true,
                emptyValue: "[\xa0\xa0\xa0\xa0][color]cyan",
                suffix: "[color]cyan",
                maxLength: 4
            },
            (value) => {
                mcdu.pdcMessage.Gate = value;
                CDUAtcDepartReq.ShowPage1(mcdu);
            }
        );
        const freetext = new CDU_SingleValueField(mcdu,
            "string",
            mcdu.pdcMessage.Freetext0,
            {
                clearable: 0 === mcdu.pdcMessage.Freetext1.length,
                emptyValue: "[\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0][color]cyan",
                suffix: "[color]white",
                maxLength: 22
            },
            (value) => {
                mcdu.pdcMessage.Freetext0 = value;
                CDUAtcDepartReq.ShowPage1(mcdu);
            }
        );

        if (mcdu.atsuManager.flightNumber().length !== 0 && mcdu.flightPlanManager.getOrigin() !== null) {
            mcdu.pdcMessage.Callsign = mcdu.atsuManager.flightNumber();
            flightNo = mcdu.pdcMessage.Callsign + "[color]green";
        }
        if (mcdu.flightPlanManager.getDestination() && mcdu.flightPlanManager.getDestination().ident) {
            mcdu.pdcMessage.Origin = mcdu.flightPlanManager.getOrigin().ident;
            mcdu.pdcMessage.Destination = mcdu.flightPlanManager.getDestination().ident;
            fromTo = mcdu.pdcMessage.Origin + "/" + mcdu.pdcMessage.Destination + "[color]cyan";
        }

        // check if all required information are available to prepare the PDC message
        let reqDisplButton = "REQ DISPL\xa0[color]cyan";
        if ("" !== mcdu.pdcMessage.Callsign && "" !== mcdu.pdcMessage.Origin && "" !== mcdu.pdcMessage.Destination && 1 === mcdu.pdcMessage.Atis.length) {
            reqDisplButton = "REQ DISPL*[color]cyan";
        }

        mcdu.setTemplate([
            ["DEPART REQUEST"],
            ["ATC FLT NBR", "A/C TYPE"],
            [flightNo, "A20N[color]cyan"],
            ["FROM/TO"],
            [fromTo],
            ["GATE", "ATIS"],
            [gate, atis],
            ["---------FREE TEXT---------"],
            [freetext],
            ["", "MORE\xa0"],
            ["", "FREE TEXT>[color]white"],
            ["\xa0ATC MENU", "ATC DEPART\xa0[color]cyan"],
            ["<RETURN", reqDisplButton]
        ]);

        mcdu.rightInputDelay[4] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onRightInput[4] = () => {
            if (0 !== mcdu.pdcMessage.Freetext0.length) {
                CDUAtcDepartReq.ShowPage2(mcdu);
            } else {
                mcdu.addNewMessage(NXSystemMessages.mandatoryFields);
            }
        };

        mcdu.leftInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onLeftInput[5] = () => {
            CDUAtcMenu.ShowPage2(mcdu);
        };

        mcdu.rightInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onRightInput[5] = () => {
            if ("" === mcdu.pdcMessage.Callsign || "" === mcdu.pdcMessage.Origin || "" === mcdu.pdcMessage.Destination || 1 !== mcdu.pdcMessage.Atis.length) {
                mcdu.addNewMessage(NXSystemMessages.mandatoryFields);
                return;
            }

            // publish the message
            mcdu.atsuManager.sendMessage(mcdu.pdcMessage).then((code) => {
                if (code === Atsu.AtsuStatusCodes.Ok) {
                    mcdu.pdcMessage = undefined;
                } else {
                    mcdu.addNewAtsuMessage(code);
                }

                CDUAtcDepartReq.ShowPage1(mcdu, store);
            });

            CDUAtcDepartReq.ShowPage1(mcdu);
        };
    }

    static ShowPage2(mcdu) {
        mcdu.clearDisplay();

        const addionalLineTemplate = [
            ["FREE TEXT"],
            [""],
            [""],
            [""],
            [""],
            [""],
            [""],
            [""],
            [""],
            [""],
            [""],
            ["\xa0DEPART REQUEST"],
            ["<RETURN"]
        ];

        // find the first empty line
        let firstEmptyLineIndex = -1;
        if (0 === mcdu.pdcMessage.Freetext5.length) {
            firstEmptyLineIndex = 4;
        }
        if (0 === mcdu.pdcMessage.Freetext4.length) {
            firstEmptyLineIndex = 3;
        }
        if (0 === mcdu.pdcMessage.Freetext3.length) {
            firstEmptyLineIndex = 2;
        }
        if (0 === mcdu.pdcMessage.Freetext2.length) {
            firstEmptyLineIndex = 1;
        }
        if (0 === mcdu.pdcMessage.Freetext1.length) {
            firstEmptyLineIndex = 0;
        }

        switch (firstEmptyLineIndex) {
            case -1:
            case 4:
                const line4 = new CDU_SingleValueField(mcdu,
                    "string",
                    mcdu.pdcMessage.Freetext5,
                    {
                        clearable: 0 !== mcdu.pdcMessage.Freetext5.length,
                        emptyValue: "[\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0][color]cyan",
                        suffix: "[color]white",
                        maxLength: 22
                    },
                    (value) => {
                        mcdu.pdcMessage.Freetext5 = value;
                        CDUAtcDepartReq.ShowPage2(mcdu);
                    }
                );
                addionalLineTemplate[10] = [line4];
            case 3:
                const line3 = new CDU_SingleValueField(mcdu,
                    "string",
                    mcdu.pdcMessage.Freetext4,
                    {
                        clearable: 0 === mcdu.pdcMessage.Freetext5.length,
                        emptyValue: "[\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0][color]cyan",
                        suffix: "[color]white",
                        maxLength: 22
                    },
                    (value) => {
                        mcdu.pdcMessage.Freetext4 = value;
                        CDUAtcDepartReq.ShowPage2(mcdu);
                    }
                );
                addionalLineTemplate[8] = [line3];
            case 2:
                const line2 = new CDU_SingleValueField(mcdu,
                    "string",
                    mcdu.pdcMessage.Freetext3,
                    {
                        clearable: 0 === mcdu.pdcMessage.Freetext4.length,
                        emptyValue: "[\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0][color]cyan",
                        suffix: "[color]white",
                        maxLength: 22
                    },
                    (value) => {
                        mcdu.pdcMessage.Freetext3 = value;
                        CDUAtcDepartReq.ShowPage2(mcdu);
                    }
                );
                addionalLineTemplate[6] = [line2];
            case 1:
                const line1 = new CDU_SingleValueField(mcdu,
                    "string",
                    mcdu.pdcMessage.Freetext2,
                    {
                        clearable: 0 === mcdu.pdcMessage.Freetext3.length,
                        emptyValue: "[\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0][color]cyan",
                        suffix: "[color]white",
                        maxLength: 22
                    },
                    (value) => {
                        mcdu.pdcMessage.Freetext2 = value;
                        CDUAtcDepartReq.ShowPage2(mcdu);
                    }
                );
                addionalLineTemplate[4] = [line1];
            default:
                const line0 = new CDU_SingleValueField(mcdu,
                    "string",
                    mcdu.pdcMessage.Freetext1,
                    {
                        clearable: 0 === mcdu.pdcMessage.Freetext2.length,
                        emptyValue: "[\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0\xa0][color]cyan",
                        suffix: "[color]white",
                        maxLength: 22
                    },
                    (value) => {
                        mcdu.pdcMessage.Freetext1 = value;
                        CDUAtcDepartReq.ShowPage2(mcdu);
                    }
                );
                addionalLineTemplate[2] = [line0];
                break;
        }

        // define the template
        mcdu.setTemplate(addionalLineTemplate);

        mcdu.leftInputDelay[5] = () => {
            return mcdu.getDelaySwitchPage();
        };
        mcdu.onLeftInput[5] = () => {
            CDUAtcDepartReq.ShowPage1(mcdu);
        };
    }
}
