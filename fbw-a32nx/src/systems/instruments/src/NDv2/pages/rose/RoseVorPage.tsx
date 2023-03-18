import { FSComponent, DisplayComponent, ComponentProps, MappedSubject, Subject, Subscribable, VNode } from 'msfssdk';
import { Arinc429WordData } from '@shared/arinc429';
import { RoseMode, RoseModeProps } from './RoseMode';
import { TrackBug } from '../../shared/TrackBug';
import { RoseModeUnderlay } from './RoseModeUnderlay';
import { VorSimVars } from '../../../MsfsAvionicsCommon/providers/VorBusPublisher';
import { AdirsSimVars } from '../../../MsfsAvionicsCommon/SimVarTypes';
import { Flag } from '../../shared/Flag';
import { Arinc429RegisterSubject } from '../../../MsfsAvionicsCommon/Arinc429RegisterSubject';
import { NDControlEvents } from '../../NDControlEvents';
import { VorInfoIndicator } from './VorInfoIndicator';

export interface RoseVorProps extends RoseModeProps {
    index: 1 | 2,
}

export class RoseVorPage extends RoseMode<RoseVorProps> {
    isVisible = Subject.create(false);

    private readonly headingWord = Arinc429RegisterSubject.createEmpty();

    private readonly courseSub = Subject.create(0);

    private readonly courseDeviationSub = Subject.create(0);

    private readonly vorAvailableSub = Subject.create(false);

    onShow() {
        super.onShow();

        const publisher = this.props.bus.getPublisher<NDControlEvents>();

        publisher.pub('set_show_map', false);
    }

    onAfterRender(node: VNode) {
        super.onAfterRender(node);

        const sub = this.props.bus.getSubscriber<AdirsSimVars & VorSimVars>();

        const index = this.props.index;

        // TODO use DMC data
        sub.on('magHeadingRaw').whenChanged().handle((v) => this.headingWord.setWord(v));

        sub.on(`nav${index}Obs`).whenChanged().handle((v) => this.courseSub.set(v));

        sub.on(`nav${index}RadialError`).whenChanged().handle((v) => this.courseDeviationSub.set(v));

        sub.on(`nav${index}Available`).whenChanged().handle((v) => this.vorAvailableSub.set(v));
    }

    private readonly hdgFlagShown = MappedSubject.create(([headingWord]) => !headingWord.isNormalOperation(), this.headingWord);

    render(): VNode | null {
        return (
            <g visibility={this.isVisible.map((v) => (v ? 'visible' : 'hidden'))}>
                <VorInfoIndicator bus={this.props.bus} index={this.props.index} />

                <RoseModeUnderlay
                    bus={this.props.bus}
                    heading={this.props.heading}
                    visible={this.isVisible}
                />

                <VorCaptureOverlay
                    index={this.props.index}
                    heading={this.headingWord}
                    course={this.courseSub}
                    courseDeviation={this.courseDeviationSub}
                    vorAvailable={this.vorAvailableSub}
                />

                <TrackBug
                    bus={this.props.bus}
                    isUsingTrackUpMode={this.props.isUsingTrackUpMode}
                />

                <Flag shown={this.hdgFlagShown} x={384} y={241} class="Red FontLarge">HDG</Flag>
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
}

class VorCaptureOverlay extends DisplayComponent<VorCaptureOverlayProps> {
    private readonly visible = MappedSubject.create(([heading, vorAvailable]) => {
        return heading.isNormalOperation() && vorAvailable;
    }, this.props.heading, this.props.vorAvailable);

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
    }, this.props.heading)

    private readonly cdiPx = Subject.create(12);

    private readonly toward = Subject.create(true);

    private readonly directionTransform = MappedSubject.create(([cdiPx, toward]) => {
        return `translate(${cdiPx}, ${toward ? 0 : 160}) rotate(${toward ? 0 : 180} 384 304)`;
    }, this.cdiPx, this.toward);

    private readonly deviationTransform = MappedSubject.create(([cdiPx]) => {
        return `translate(${cdiPx}, 0)`;
    }, this.cdiPx)

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
