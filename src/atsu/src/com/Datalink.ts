//  Copyright (c) 2021 FlyByWire Simulations
//  SPDX-License-Identifier: GPL-3.0

import { NXDataStore } from '@shared/persistence';
import { AtsuStatusCodes } from '../AtsuStatusCodes';
import { Atsu } from '../ATSU';
import { CpdlcMessage } from '../messages/CpdlcMessage';
import { AtsuMessage, AtsuMessageNetwork, AtsuMessageType } from '../messages/AtsuMessage';
import { AtisMessage, AtisType } from '../messages/AtisMessage';
import { MetarMessage } from '../messages/MetarMessage';
import { TafMessage } from '../messages/TafMessage';
import { WeatherMessage } from '../messages/WeatherMessage';
import { HoppieConnector } from './HoppieConnector';
import { NXApiConnector } from './NXApiConnector';

export class Datalink {
    private overallDelay = 0;

    private waitedTimeHoppie = 0;

    private waitedTimeNXApi = 0;

    private firstPollHoppie = true;

    private enqueueReceivedMessages(parent: Atsu, messages: AtsuMessage[]): void {
        messages.forEach((message) => {
            // ignore empty messages (happens sometimes in CPDLC with buggy ATC software)
            if (message.Message.length !== 0) {
                this.estimateTransmissionTime();
                setTimeout(() => parent.registerMessages([message]), this.overallDelay);
            }
        });
    }

    constructor(parent: Atsu) {
        // copy the datalink transmission time data
        switch (NXDataStore.get('CONFIG_DATALINK_TRANSMISSION_TIME', 'FAST')) {
        case 'REAL':
            SimVar.SetSimVarValue('L:A32NX_CONFIG_DATALINK_TIME', 'number', 0);
            break;
        case 'FAST':
            SimVar.SetSimVarValue('L:A32NX_CONFIG_DATALINK_TIME', 'number', 2);
            break;
        default:
            SimVar.SetSimVarValue('L:A32NX_CONFIG_DATALINK_TIME', 'number', 1);
            break;
        }

        setInterval(() => {
            if (SimVar.GetSimVarValue('L:A32NX_HOPPIE_ACTIVE', 'number') !== 1) {
                parent.atc.resetAtc();
            }

            // update the internal timer
            if (this.overallDelay <= 200) {
                this.overallDelay = 0;
            } else {
                this.overallDelay -= 200;
            }

            if (HoppieConnector.pollInterval() <= this.waitedTimeHoppie) {
                HoppieConnector.poll().then((retval) => {
                    if (retval[0] === AtsuStatusCodes.Ok) {
                        // delete all data in the first call (Hoppie stores old data)
                        if (!this.firstPollHoppie) {
                            this.enqueueReceivedMessages(parent, retval[1]);
                        }
                        this.firstPollHoppie = false;
                    }
                });
                this.waitedTimeHoppie = 0;
            } else {
                this.waitedTimeHoppie += 5000;
            }

            if (NXApiConnector.pollInterval() <= this.waitedTimeNXApi) {
                NXApiConnector.poll().then((retval) => {
                    if (retval[0] === AtsuStatusCodes.Ok) {
                        this.enqueueReceivedMessages(parent, retval[1]);
                    }
                });
                this.waitedTimeNXApi = 0;
            } else {
                this.waitedTimeNXApi += 5000;
            }
        }, 5000);
    }

    private estimateTransmissionTime(): void {
        let timeout = 0;

        switch (SimVar.GetSimVarValue('L:A32NX_CONFIG_DATALINK_TIME', 'number')) {
        // realistic transmission
        case 0:
            timeout = 30;
            break;
        // fast transmission
        case 2:
            timeout = 10;
            break;
        // instant transmission
        default:
            timeout = 2;
            break;
        }

        // update the timeout and overall delay
        timeout += Math.floor(Math.random() * timeout * 0.5);
        timeout *= 1000;
        this.overallDelay += timeout;
    }

    private async receiveWeatherData(requestMetar: boolean, icaos: string[], index: number, message: WeatherMessage): Promise<AtsuStatusCodes> {
        let retval = AtsuStatusCodes.Ok;

        if (index < icaos.length) {
            if (requestMetar === true) {
                retval = await NXApiConnector.receiveMetar(icaos[index], message).then(() => this.receiveWeatherData(requestMetar, icaos, index + 1, message));
            } else {
                retval = await NXApiConnector.receiveTaf(icaos[index], message).then(() => this.receiveWeatherData(requestMetar, icaos, index + 1, message));
            }
        }

        return retval;
    }

    public async receiveWeather(requestMetar: boolean, icaos: string[]): Promise<[AtsuStatusCodes, WeatherMessage | undefined]> {
        this.estimateTransmissionTime();

        return new Promise((resolve, _reject) => {
            setTimeout(() => {
                let message = undefined;
                if (requestMetar === true) {
                    message = new MetarMessage();
                } else {
                    message = new TafMessage();
                }

                this.receiveWeatherData(requestMetar, icaos, 0, message).then((code) => {
                    if (code !== AtsuStatusCodes.Ok) {
                        resolve([AtsuStatusCodes.ComFailed, undefined]);
                    }
                    resolve([AtsuStatusCodes.Ok, message]);
                });
            }, this.overallDelay);
        });
    }

    public async isStationAvailable(callsign: string): Promise<AtsuStatusCodes> {
        return HoppieConnector.isStationAvailable(callsign);
    }

    public async receiveAtis(icao: string, type: AtisType): Promise<[AtsuStatusCodes, WeatherMessage | undefined]> {
        this.estimateTransmissionTime();

        return new Promise((resolve, _reject) => {
            setTimeout(() => {
                const message = new AtisMessage();
                NXApiConnector.receiveAtis(icao, type, message).then(() => resolve([AtsuStatusCodes.Ok, message]));
            }, this.overallDelay);
        });
    }

    public async sendMessage(message: AtsuMessage, force: boolean): Promise<AtsuStatusCodes> {
        this.estimateTransmissionTime();

        return new Promise((resolve, _reject) => {
            setTimeout(() => {
                if (message.Type < AtsuMessageType.AOC) {
                    if (message.Network === AtsuMessageNetwork.FBW) {
                        NXApiConnector.sendTelexMessage(message).then((code) => resolve(code));
                    } else {
                        HoppieConnector.sendTelexMessage(message, force).then((code) => resolve(code));
                    }
                } else if (message.Type === AtsuMessageType.DCL) {
                    HoppieConnector.sendTelexMessage(message, force).then((code) => resolve(code));
                } else if (message.Type < AtsuMessageType.ATC) {
                    HoppieConnector.sendCpdlcMessage(message as CpdlcMessage, force).then((code) => resolve(code));
                } else {
                    resolve(AtsuStatusCodes.UnknownMessage);
                }
            }, this.overallDelay);
        });
    }
}
