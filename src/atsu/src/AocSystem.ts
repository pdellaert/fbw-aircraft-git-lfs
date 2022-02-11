//  Copyright (c) 2022 FlyByWire Simulations
//  SPDX-License-Identifier: GPL-3.0

import { NXApiConnector } from '@atsu/com/NXApiConnector';
import { AtsuStatusCodes } from './AtsuStatusCodes';
import { AtsuMessageDirection, AtsuMessage, AtsuMessageType } from './messages/AtsuMessage';
import { WeatherMessage } from './messages/WeatherMessage';
import { AtisType } from './messages/AtisMessage';
import { Datalink } from './com/Datalink';

/**
 * Defines the AOC manager
 */
export class AocSystem {
    private datalink: Datalink | undefined = undefined;

    private messageQueue: AtsuMessage[] = [];

    constructor(datalink: Datalink) {
        this.datalink = datalink;
    }

    public static async connect(flightNo: string): Promise<AtsuStatusCodes> {
        return NXApiConnector.connect(flightNo);
    }

    public static async disconnect(): Promise<AtsuStatusCodes> {
        return NXApiConnector.disconnect();
    }

    public static isRelevantMessage(message: AtsuMessage): boolean {
        return message.Type < AtsuMessageType.AOC;
    }

    public async sendMessage(message: AtsuMessage): Promise<AtsuStatusCodes> {
        if (AocSystem.isRelevantMessage(message)) {
            return this.datalink.sendMessage(message, false);
        }
        return AtsuStatusCodes.UnknownMessage;
    }

    public removeMessage(uid: number): boolean {
        const index = this.messageQueue.findIndex((element) => element.UniqueMessageID === uid);
        if (index !== -1) {
            this.messageQueue.splice(index, 1);
        }
        return index !== -1;
    }

    public async receiveWeather(requestMetar: boolean, icaos: string[]): Promise<[AtsuStatusCodes, WeatherMessage | undefined]> {
        return this.datalink.receiveWeather(requestMetar, icaos);
    }

    public async receiveAtis(icao: string, type: AtisType): Promise<[AtsuStatusCodes, WeatherMessage | undefined]> {
        return this.datalink.receiveAtis(icao, type);
    }

    public messageRead(uid: number): boolean {
        const index = this.messageQueue.findIndex((element) => element.UniqueMessageID === uid);
        if (index !== -1 && this.messageQueue[index].Direction === AtsuMessageDirection.Input) {
            if (this.messageQueue[index].Confirmed === false) {
                const cMsgCnt = SimVar.GetSimVarValue('L:A32NX_COMPANY_MSG_COUNT', 'Number');
                SimVar.SetSimVarValue('L:A32NX_COMPANY_MSG_COUNT', 'Number', cMsgCnt <= 1 ? 0 : cMsgCnt - 1);
            }

            this.messageQueue[index].Confirmed = true;
        }

        return index !== -1;
    }

    public messages(): AtsuMessage[] {
        return this.messageQueue;
    }

    public outputMessages(): AtsuMessage[] {
        return this.messageQueue.filter((entry) => entry.Direction === AtsuMessageDirection.Output);
    }

    public inputMessages(): AtsuMessage[] {
        return this.messageQueue.filter((entry) => entry.Direction === AtsuMessageDirection.Input);
    }

    public uidRegistered(uid: number): boolean {
        return this.messageQueue.findIndex((element) => uid === element.UniqueMessageID) !== -1;
    }

    public insertMessage(message: AtsuMessage): void {
        this.messageQueue.unshift(message);

        if (message.Direction === AtsuMessageDirection.Input) {
            // increase the company message counter
            const cMsgCnt = SimVar.GetSimVarValue('L:A32NX_COMPANY_MSG_COUNT', 'Number');
            SimVar.SetSimVarValue('L:A32NX_COMPANY_MSG_COUNT', 'Number', cMsgCnt + 1);
        }
    }
}
