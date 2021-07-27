import { Degrees, NauticalMiles, Feet, Knots } from '@typings/types';
import { Guidable } from '@fmgc/guidance/Geometry';
import { LatLongData } from '@typings/fs-base-ui/html_ui/JS/Types';
import { WayPoint } from '@fmgc/types/fstypes/FSTypes';

export enum AltitudeConstraintType {
    at,
    atOrAbove,
    atOrBelow,
    range,
}

export enum SpeedConstraintType {
    at,
    atOrAbove,
    atOrBelow,
}

export interface AltitudeConstraint {
    type: AltitudeConstraintType,
    altitude1: Feet,
    altitude2: Feet | undefined,
}

export interface SpeedConstraint {
    type: SpeedConstraintType,
    speed: Knots,
}

export interface Leg extends Guidable {
    get bearing(): Degrees;

    get distance(): NauticalMiles;

    get speedConstraint(): SpeedConstraint | undefined;

    get terminatorLocation(): LatLongData | undefined;

    getPseudoWaypointLocation(distanceBeforeTerminator: NauticalMiles): LatLongData | undefined;

    getGuidanceParameters(ppos: LatLongAlt, trueTrack: Degrees);

    /**
     * Calculates directed DTG parameter
     *
     * @param ppos {LatLong} the current position of the aircraft
     */
    getDistanceToGo(ppos: LatLongData): NauticalMiles;

    isAbeam(ppos);
}

export interface AltitudeConstrainedLeg extends Leg {
    get altitudeConstraint(): AltitudeConstraint | undefined;
}

export function getAltitudeConstraintFromWaypoint(wp: WayPoint): AltitudeConstraint | undefined {
    if (wp.altDesc && wp.altitude1) {
        let ac: AltitudeConstraint;
        ac.altitude1 = wp.altitude1;
        ac.altitude2 = undefined;
        switch (wp.altDesc) {
        case 1:
            ac.type = AltitudeConstraintType.at;
            break;
        case 2:
            ac.type = AltitudeConstraintType.atOrAbove;
            break;
        case 3:
            ac.type = AltitudeConstraintType.atOrBelow;
            break;
        case 4:
            ac.type = AltitudeConstraintType.range;
            ac.altitude2 = wp.altitude2;
            break;
        default:
            break;
        }
        return ac;
    }
    return undefined;
}

export function getSpeedConstraintFromWaypoint(wp: WayPoint): SpeedConstraint | undefined {
    if (wp.speedConstraint) {
        let sc: SpeedConstraint;
        sc.type = SpeedConstraintType.at;
        sc.speed = wp.speedConstraint;
        return sc;
    }
    return undefined;
}

export function waypointToTerminatorLocation(wp: WayPoint): LatLongData {
    const loc: LatLongData = {
        lat: wp.infos.coordinates.lat,
        long: wp.infos.coordinates.long,
    };
    return loc;
}
