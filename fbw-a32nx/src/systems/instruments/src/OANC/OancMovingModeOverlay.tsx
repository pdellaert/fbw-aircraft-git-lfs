// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import {
    ConsumerSubject,
    DisplayComponent,
    EventBus,
    FSComponent,
    MappedSubject,
    Subject,
    Subscribable,
    VNode,
} from '@microsoft/msfs-sdk';

import { Arinc429SignStatusMatrix, Arinc429Word } from '@shared/arinc429';
import { EfisNdMode } from '@shared/NavigationDisplay';
import { RoseModeUnderlay } from './OancRoseCompass';
import { OANC_RENDER_HEIGHT, OANC_RENDER_WIDTH } from './Oanc';
import { FcuSimVars } from '../MsfsAvionicsCommon/providers/FcuBusSimVarPublisher';
import { ArcModeUnderlay } from './OancArcModeCompass';
import { OancPlanModeCompass } from './OancPlanModeCompass';

export interface OancMapOverlayProps {
    bus: EventBus,

    oansRange: Subscribable<number>,

    rotation: Subscribable<number>,

    ndMode: Subscribable<EfisNdMode>,

    isMapPanned: Subscribable<boolean>,
}

export class OancMovingModeOverlay extends DisplayComponent<OancMapOverlayProps> {
    private readonly arcModeVisible = MappedSubject.create(([ndMode, isPanning]) => ndMode === EfisNdMode.ARC && isPanning, this.props.ndMode, this.props.isMapPanned);

    private readonly roseModeVisible = MappedSubject.create(([ndMode, isPanning]) => ndMode === EfisNdMode.ROSE_NAV && isPanning, this.props.ndMode, this.props.isMapPanned);

    render(): VNode | null {
        return (
            <svg
                class="oanc-svg"
                viewBox={`0 0 ${OANC_RENDER_WIDTH * 2} ${OANC_RENDER_HEIGHT * 2}`}
                style="position: absolute; width: 1536px; height: 1536px; left: -384px; top: -384px; pointer-events: none; z-index: 99;"
            >
                <RoseModeUnderlay
                    bus={this.props.bus}
                    visible={this.roseModeVisible}
                    rotation={this.props.rotation.map((r) => {
                        const word = Arinc429Word.empty();

                        word.ssm = Arinc429SignStatusMatrix.NormalOperation;
                        word.value = r;

                        return word;
                    })}
                    oansRange={this.props.oansRange}
                    doClip={false}
                />

                <ArcModeUnderlay
                    bus={this.props.bus}
                    visible={this.arcModeVisible}
                    rotation={this.props.rotation.map((r) => {
                        const word = Arinc429Word.empty();

                        word.ssm = Arinc429SignStatusMatrix.NormalOperation;
                        word.value = r;

                        return word;
                    })}
                    oansRange={this.props.oansRange}
                    doClip={false}
                    yOffset={620 - 384}
                />
            </svg>
        );
    }
}

export class OancStaticModeOverlay extends DisplayComponent<OancMapOverlayProps> {
    private readonly arcModeVisible = MappedSubject.create(([ndMode, isPanning]) => ndMode === EfisNdMode.ARC && !isPanning, this.props.ndMode, this.props.isMapPanned);

    private readonly roseModeVisible = MappedSubject.create(([ndMode, isPanning]) => ndMode === EfisNdMode.ROSE_NAV && !isPanning, this.props.ndMode, this.props.isMapPanned);

    render(): VNode | null {
        return (
            <svg
                class="oanc-svg"
                viewBox={`0 0 ${OANC_RENDER_WIDTH * 2} ${OANC_RENDER_HEIGHT * 2}`}
                style="position: absolute; width: 1536px; height: 1536px; left: -384px; top: -384px; pointer-events: none;"
            >
                <RoseModeUnderlay
                    bus={this.props.bus}
                    visible={this.roseModeVisible}
                    rotation={this.props.rotation.map((r) => {
                        const word = Arinc429Word.empty();

                        word.ssm = Arinc429SignStatusMatrix.NormalOperation;
                        word.value = r;

                        return word;
                    })}
                    oansRange={this.props.oansRange}
                    doClip
                />

                <ArcModeUnderlay
                    bus={this.props.bus}
                    visible={this.arcModeVisible}
                    rotation={this.props.rotation.map((r) => {
                        const word = Arinc429Word.empty();

                        word.ssm = Arinc429SignStatusMatrix.NormalOperation;
                        word.value = r;

                        return word;
                    })}
                    oansRange={this.props.oansRange}
                    doClip
                    yOffset={0}
                />

                <OancPlanModeCompass
                    bus={this.props.bus}
                    visible={this.props.ndMode.map((it) => it === EfisNdMode.PLAN)}
                    oansRange={this.props.oansRange}
                />
            </svg>
        );
    }
}
