// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { FSComponent, DisplayComponent, ComponentProps, MappedSubject, Subject, Subscribable, VNode } from '@microsoft/msfs-sdk';
import { Arinc429WordData } from '@flybywiresim/fbw-sdk';
import { DmcEvents } from 'instruments/src/MsfsAvionicsCommon/providers/DmcPublisher';
import { RoseMode, RoseModeProps } from './RoseMode';
import { RoseModeUnderlay } from './RoseModeUnderlay';
import { VorSimVars } from '../../../MsfsAvionicsCommon/providers/VorBusPublisher';
import { AdirsSimVars } from '../../../MsfsAvionicsCommon/SimVarTypes';
import { Flag } from '../../shared/Flag';
import { NDControlEvents } from '../../NDControlEvents';
import { VorInfoIndicator } from './VorInfoIndicator';
import { Arinc429ConsumerSubject } from '../../../MsfsAvionicsCommon/Arinc429ConsumerSubject';

export interface RoseVorProps extends RoseModeProps {
    index: 1 | 2,
}

export class RoseVorPage extends RoseMode<RoseVorProps> {
    isVisible = Subject.create(false);

    private readonly headingWord = Arinc429ConsumerSubject.create(null);

    private readonly courseSub = Subject.create(0);

    private readonly courseDeviationSub = Subject.create(0);

    private readonly vorAvailableSub = Subject.create(false);

    private readonly vorFrequencySub = Subject.create(0);

    onShow() {
        super.onShow();

        const publisher = this.props.bus.getPublisher<NDControlEvents>();

        publisher.pub('set_show_map', false);
    }

    onAfterRender(node: VNode) {
        super.onAfterRender(node);

        const sub = this.props.bus.getSubscriber<AdirsSimVars & DmcEvents & VorSimVars>();

        const index = this.props.index;

        this.headingWord.setConsumer(sub.on('heading'));

        sub.on(`nav${index}Obs`).whenChanged().handle((v) => this.courseSub.set(v));

        sub.on(`nav${index}RadialError`).whenChanged().handle((v) => this.courseDeviationSub.set(v));

        sub.on(`nav${index}Available`).whenChanged().handle((v) => this.vorAvailableSub.set(v));

        sub.on(`nav${index}Frequency`).whenChanged().handle((v) => this.vorFrequencySub.set(v));
    }

    private readonly hdgFlagShown = MappedSubject.create(([headingWord]) => !headingWord.isNormalOperation(), this.headingWord);

    render(): VNode | null {
        return (
            <g visibility={this.isVisible.map((v) => (v ? 'visible' : 'hidden'))}>
                <VorInfoIndicator bus={this.props.bus} index={this.props.index} />

                <RoseModeUnderlay
                    bus={this.props.bus}
                    heading={this.props.headingWord}
                    visible={this.isVisible}
                />

                <VorCaptureOverlay
                    index={this.props.index}
                    heading={this.props.headingWord}
                    course={this.courseSub}
                    courseDeviation={this.courseDeviationSub}
                    vorAvailable={this.vorAvailableSub}
                    vorFrequency={this.vorFrequencySub}
                />

                <Flag visible={this.hdgFlagShown} x={384} y={241} class="Red FontLarge">HDG</Flag>
            </g>
        );
    }
}

interface VorCaptureOverlayProps extends ComponentProps {
    index: 1 | 2,
    heading: Subscribable<Arinc429WordData>,
    course: Subscribable<number>,
    courseDeviation: Subscribable<number>,
    vorAvailable: Subscribable<boolean>,
    vorFrequency: Subscribable<number>,
}

class VorCaptureOverlay extends DisplayComponent<VorCaptureOverlayProps> {
    private readonly visible = MappedSubject.create(([heading]) => {
        return heading.isNormalOperation();
    }, this.props.heading);

    private readonly rotation = MappedSubject.create(([heading, course]) => {
        if (heading.isNormalOperation()) {
            return course - heading.value;
        }
        return 0;
    }, this.props.heading, this.props.course);

    private readonly pointerColor = MappedSubject.create(([heading]) => {
        if (heading.isNormalOperation()) {
            return 'Cyan';
        }

        return 'White';
    }, this.props.heading);

    /*     useEffect(() => {
        let cdiDegrees: number;
        if (Math.abs(courseDeviation) <= 90) {
            cdiDegrees = courseDeviation;
            setToward(true);
        } else {
            cdiDegrees = Math.sign(courseDeviation) * -Avionics.Utils.diffAngle(180, Math.abs(courseDeviation));
            setToward(false);
        }
        setCdiPx(Math.min(12, Math.max(-12, cdiDegrees)) * 74 / 5);
    }, [courseDeviation.toFixed(2)]);
 */

    private readonly cdiPx = Subject.create(12);

    private readonly toward = Subject.create(true);

    private readonly directionTransform = MappedSubject.create(([cdiPx, toward]) => {
        return `translate(${cdiPx}, ${toward ? 0 : 160}) rotate(${toward ? 0 : 180} 384 304)`;
    }, this.cdiPx, this.toward);

    private readonly deviationTransform = MappedSubject.create(([cdiPx]) => {
        return `translate(${cdiPx}, 0)`;
    }, this.cdiPx)

    onAfterRender(node: VNode): void {
        super.onAfterRender(node);
        this.props.courseDeviation.sub((courseDeviation) => {
            let cdiDegrees: number;
            if (Math.abs(courseDeviation) <= 90) {
                cdiDegrees = courseDeviation;
                this.toward.set(true);
            } else {
                cdiDegrees = Math.sign(courseDeviation) * -Avionics.Utils.diffAngle(180, Math.abs(courseDeviation));
                this.toward.set(false);
            }
            this.cdiPx.set(Math.min(12, Math.max(-12, cdiDegrees)) * 74 / 5);
        });
    }

    render(): VNode | null {
        return (
            <g
                visibility={this.visible.map((visible) => (visible ? 'inherit' : 'hidden'))}
                transform={this.rotation.map((deg) => `rotate(${deg} 384 384)`)}
                stroke="white"
                stroke-width={3}
                fill="none"
            >
                <g id="vor-deviation-scale">
                    <circle cx={236} cy={384} r={5} />
                    <circle cx={310} cy={384} r={5} />
                    <circle cx={458} cy={384} r={5} />
                    <circle cx={532} cy={384} r={5} />
                </g>

                <g visibility={this.props.vorFrequency.map((v) => (v > 0 ? 'inherit' : 'hidden'))}>
                    <path
                        d="M352,256 L416,256 M384,134 L384,294 M384,474 L384,634"
                        class="rounded shadow"
                        id="vor-course-pointer-shadow"
                        stroke-width={4.5}
                    />
                    <path
                        d="M352,256 L416,256 M384,134 L384,294 M384,474 L384,634"
                        class={this.pointerColor.map((color) => `rounded ${color}`)}
                        id="vor-course-pointer"
                        stroke-width={4}
                    />
                </g>

                <g visibility={this.props.vorAvailable.map((available) => (available ? 'inherit' : 'hidden'))}>
                    <path
                        d="M372,322 L384,304 L396,322"
                        class="rounded shadow"
                        transform={this.directionTransform}
                        id="vor-deviation-direction-shadow"
                        stroke-width={4.5}
                    />
                    <path
                        d="M384,304 L384,464"
                        class="rounded shadow"
                        transform={this.deviationTransform}
                        id="vor-deviation-shadow"
                        stroke-width={4.5}
                    />
                    <path
                        d="M372,322 L384,304 L396,322"
                        class={this.pointerColor.map((color) => `rounded ${color}`)}
                        transform={this.directionTransform}
                        id="vor-deviation-direction"
                        stroke-width={4}
                    />
                    <path
                        d="M384,304 L384,464"
                        class={this.pointerColor.map((color) => `rounded ${color}`)}
                        transform={this.deviationTransform}
                        id="vor-deviation"
                        stroke-width={4}
                    />
                </g>
            </g>
        );
    }
}
