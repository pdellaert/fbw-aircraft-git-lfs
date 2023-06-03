import {
    ConsumerSubject,
    DisplayComponent,
    EventBus,
    FSComponent,
    Subject,
    Subscribable,
    VNode,
} from '@microsoft/msfs-sdk';
import { DmcEvents } from 'instruments/src/MsfsAvionicsCommon/providers/DmcPublisher';
import { EfisNdMode } from '@shared/NavigationDisplay';
import { NDSimvars } from '../NDSimvarPublisher';
import { getSmallestAngle } from '../../PFD/PFDUtils';
import { Arinc429ConsumerSubject } from '../../MsfsAvionicsCommon/Arinc429ConsumerSubject';
import { FcuSimVars } from '../../MsfsAvionicsCommon/providers/FcuBusPublisher';

export interface TrackBugProps {
    bus: EventBus,
    isUsingTrackUpMode: Subscribable<boolean>,
}

export class TrackBug extends DisplayComponent<TrackBugProps> {
    private readonly ndMode = ConsumerSubject.create(null, EfisNdMode.ARC);

    private readonly headingWord = Arinc429ConsumerSubject.create(null);

    private readonly trackWord = Arinc429ConsumerSubject.create(null);

    private readonly diffSubject = Subject.create(0);

    private readonly bugShown = Subject.create(false);

    onAfterRender(node: VNode) {
        super.onAfterRender(node);

        const sub = this.props.bus.getSubscriber<DmcEvents & NDSimvars & FcuSimVars>();

        this.ndMode.setConsumer(sub.on('ndMode').whenChanged());
        this.headingWord.setConsumer(sub.on('heading'));
        this.trackWord.setConsumer(sub.on('track'));

        this.ndMode.sub(() => this.handleDisplay(), true);
        this.headingWord.sub(() => this.handleDisplay(), true);
        this.trackWord.sub(() => this.handleDisplay(), true);
    }

    private handleDisplay() {
        const wrongNdMode = this.ndMode.get() === EfisNdMode.PLAN;

        const headingInvalid = !this.headingWord.get().isNormalOperation();
        const trackInvalid = !this.trackWord.get().isNormalOperation();

        if (wrongNdMode || headingInvalid || trackInvalid) {
            this.bugShown.set(false);
        } else {
            let diff;
            if (this.props.isUsingTrackUpMode.get()) {
                diff = 0;
            } else {
                diff = getSmallestAngle(this.trackWord.get().value, this.headingWord.get().value);
            }

            this.bugShown.set(diff <= 40);
            this.diffSubject.set(diff);
        }
    }

    render(): VNode | null {
        return (
            <g
                visibility={this.bugShown.map((v) => (v ? 'inherit' : 'hidden'))}
                transform={this.diffSubject.map((diff) => `rotate(${diff} 384 620)`)}
            >
                <path
                    d="M384,128 L378,138 L384,148 L390,138 L384,128"
                    class="rounded shadow"
                    stroke-width={4.5}
                />
                <path
                    d="M384,128 L378,138 L384,148 L390,138 L384,128"
                    class="rounded Green"
                    stroke-width={3}
                />
            </g>
        );
    }
}
