import { Degrees, NauticalMiles, Track } from '@typings/types';
import { ControlLaw, GuidanceParameters } from '@fmgc/guidance/ControlLaws';
import { LatLongData } from '@typings/fs-base-ui/html_ui/JS/Types';
import { Leg } from '@fmgc/guidance/lnav/legs';
import { SegmentType } from '@fmgc/wtsdk';

// TODO needs updated with wind prediction, and maybe local magvar if following for longer distances
export class VMLeg implements Leg {
    public heading: Degrees;

    public initialCourse: Degrees;

    public segment: SegmentType;

    constructor(heading: Degrees, initialCourse: Degrees, segment: SegmentType) {
        this.heading = heading;
        this.initialCourse = initialCourse;
        this.segment = segment;
    }

    get bearing(): Degrees {
        return this.initialCourse;
    }

    get distance(): NauticalMiles {
        return 1;
    }

    // Manual legs don't have speed contraints
    get speedConstraint(): undefined {
        return undefined;
    }

    get altitudeConstraint(): undefined {
        return undefined;
    }

    get initialLocation(): undefined {
        return undefined;
    }

    // No terminator location since manual legs are infinite
    get terminatorLocation(): undefined {
        return undefined;
    }

    // Can't get pseudo-waypoint location without a finite terminator
    getPseudoWaypointLocation(_distanceBeforeTerminator: NauticalMiles): undefined {
        return undefined;
    }

    getGuidanceParameters(_ppos: LatLongData, _trueTrack: Track): GuidanceParameters | null {
        return {
            law: ControlLaw.HEADING,
            heading: this.heading,
        };
    }

    getDistanceToGo(_ppos: LatLongData): NauticalMiles {
        return 1;
    }

    isAbeam(_ppos: LatLongAlt): boolean {
        return true;
    }

    toString(): string {
        return `<VMLeg course=${this.heading}>`;
    }
}
