// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { BasePublisher, EventBus } from '@microsoft/msfs-sdk';
import { EfisSide, NdSymbol, NdTraffic } from '@flybywiresim/fbw-sdk';
import { PathVector } from '@fmgc/guidance/lnav/PathVector';
import { GenericDataListenerSync } from '@flybywiresim/fbw-sdk';

export interface FmsSymbolsData {
    symbols: NdSymbol[],
    vectorsActive: PathVector[],
    vectorsDashed: PathVector[],
    vectorsTemporary: PathVector[],
    traffic: NdTraffic[],
}

export class FmsSymbolsPublisher extends BasePublisher<FmsSymbolsData> {
    private readonly events: GenericDataListenerSync[] = [];

    constructor(bus: EventBus, side: EfisSide) {
        super(bus);

        this.events.push(new GenericDataListenerSync((ev, data) => {
            this.publish('symbols', data);
        }, `A32NX_EFIS_${side}_SYMBOLS`));

        this.events.push(new GenericDataListenerSync((ev, data: PathVector[]) => {
            this.publish('vectorsActive', data);
        }, `A32NX_EFIS_VECTORS_${side}_ACTIVE`));

        this.events.push(new GenericDataListenerSync((ev, data: PathVector[]) => {
            this.publish('vectorsDashed', data);
        }, `A32NX_EFIS_VECTORS_${side}_DASHED`));

        this.events.push(new GenericDataListenerSync((ev, data: PathVector[]) => {
            this.publish('vectorsTemporary', data);
        }, `A32NX_EFIS_VECTORS_${side}_TEMPORARY`));

        this.events.push(new GenericDataListenerSync((ev, data: NdTraffic[]) => {
            this.publish('traffic', data);
        }, 'A32NX_TCAS_TRAFFIC'));
    }
}
