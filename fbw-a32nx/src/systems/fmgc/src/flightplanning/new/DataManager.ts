// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { NXDataStore, Waypoint } from '@flybywiresim/fbw-sdk';
import { FmsError, FmsErrorType } from '@fmgc/FmsError';
import { DisplayInterface } from '@fmgc/flightplanning/new/interface/DisplayInterface';
import { WaypointFactory } from '@fmgc/flightplanning/new/waypoints/WaypointFactory';
import { Coordinates } from 'msfs-geo';

enum PilotWaypointType {
    Pbd = 1,
    Pbx = 2,
    LatLon = 3,
}

type SerializedWaypoint = {
    type: PilotWaypointType,
    ident: string,
    coordinates: Coordinates,
    pbxPlace1?: string,
    pbxBearing1?: DegreesTrue,
    pbxPlace2?: string,
    pbxBearing2?: DegreesTrue,
    pbdPlace?: string,
    pbdBearing?: DegreesTrue,
    pbdDistance?: NauticalMiles,
}

type LatLonWaypoint = {
    type: PilotWaypointType.LatLon,
    storedIndex: number,
    waypoint: Waypoint,
}

type PbxWaypoint = {
    type: PilotWaypointType.Pbx,
    storedIndex: number,
    waypoint: Waypoint,
    pbxPlace1: string,
    pbxBearing1: DegreesTrue,
    pbxPlace2: string,
    pbxBearing2: DegreesTrue,
}

type PbdWaypoint = {
    type: PilotWaypointType.Pbd,
    storedIndex: number,
    waypoint: Waypoint,
    pbdPlace: string,
    pbdBearing: DegreesTrue,
    pbdDistance: NauticalMiles,
}

export type PilotWaypoint = LatLonWaypoint | PbxWaypoint | PbdWaypoint;

export class DataManager {
    private static readonly STORED_WP_KEY: string = 'A32NX.StoredWaypoints';

    private storedWaypoints: PilotWaypoint[] = [];

    private latLonExtendedFormat = false;

    constructor(private fmc: DisplayInterface) {
        // we keep these in localStorage so they live for the same length of time as the flightplan (that they could appear in)
        // if the f-pln is not stored there anymore we can delete this
        const stored = localStorage.getItem(DataManager.STORED_WP_KEY);
        if (stored !== null) {
            this.storedWaypoints = JSON.parse(stored).map((wp: SerializedWaypoint, index: number) => (wp ? this.deserializeWaypoint(wp, index) : undefined));
        }

        NXDataStore.getAndSubscribe('LATLON_EXT_FMT', (_, value) => this.latLonExtendedFormat = value === '1', '0');
    }

    private serializeWaypoint(wp: PilotWaypoint): SerializedWaypoint {
        const { type, waypoint, ...other } = wp;

        return {
            type,
            ident: waypoint.ident,
            coordinates: waypoint.location,
            ...other,
        };
    }

    private deserializeWaypoint(wp: SerializedWaypoint, storedIndex: number): PilotWaypoint {
        const { type, ident, coordinates } = wp;

        switch (type) {
        case PilotWaypointType.LatLon:
            return {
                type: PilotWaypointType.LatLon,
                storedIndex,
                waypoint: WaypointFactory.fromLocation(ident, coordinates),
            };
        case PilotWaypointType.Pbx:
            return {
                type: PilotWaypointType.Pbx,
                storedIndex,
                waypoint: WaypointFactory.fromLocation(ident, coordinates),
                pbxPlace1: wp.pbxPlace1,
                pbxBearing1: wp.pbxBearing1,
                pbxPlace2: wp.pbxPlace2,
                pbxBearing2: wp.pbxBearing2,
            };
        case PilotWaypointType.Pbd:
            return {
                type: PilotWaypointType.Pbd,
                storedIndex,
                waypoint: WaypointFactory.fromLocation(ident, coordinates),
                pbdPlace: wp.pbdPlace,
                pbdBearing: wp.pbdBearing,
                pbdDistance: wp.pbdDistance,
            };
        default:
            return undefined;
        }
    }

    private generateStoredWaypointIndex() {
        for (let i = 0; i < 99; i++) {
            if (!this.storedWaypoints[i]) {
                return i;
            }
        }

        // TODO, delete oldest unused waypoint, only error if 99 in use
        throw new FmsError(FmsErrorType.ListOf99InUse);
    }

    private updateLocalStorage() {
        localStorage.setItem(
            DataManager.STORED_WP_KEY,
            JSON.stringify(this.storedWaypoints.map((wp) => (wp ? this.serializeWaypoint(wp) : undefined))),
        );
    }

    storeWaypoint(wp: PilotWaypoint, index: number) {
        this.storedWaypoints[index] = wp;
        this.updateLocalStorage();
    }

    async deleteStoredWaypoint(index: number, updateStorage = true) {
        if (!this.storedWaypoints[index]) {
            return true;
        }

        if (await this.fmc.isWaypointInUse(this.storedWaypoints[index].waypoint)) {
            return false;
        }

        delete this.storedWaypoints[index];

        if (updateStorage) {
            this.updateLocalStorage();
        }

        return true;
    }

    /**
     *
     * @returns true if all waypoints were deleted
     */
    async deleteAllStoredWaypoints() {
        const allDeleted = (await Promise.all(this.storedWaypoints.map((_, i) => this.deleteStoredWaypoint(i, false)))).reduce((allDel, deleted) => allDel && deleted, true);
        this.updateLocalStorage();

        return allDeleted;
    }

    numberOfStoredElements() {
        return {
            navaids: 0,
            routes: 0,
            runways: 0,
            waypoints: this.numberOfStoredWaypoints(),
            total: this.numberOfStoredWaypoints(),
        };
    }

    numberOfStoredWaypoints() {
        return this.storedWaypoints.reduce((count, wp) => (wp ? count + 1 : count), 0);
    }

    prevStoredWaypointIndex(currentIndex: number) {
        for (let i = currentIndex - 1; i >= 0; i--) {
            if (this.storedWaypoints[i]) {
                return i;
            }
        }
        for (let i = this.storedWaypoints.length - 1; i > currentIndex; i--) {
            if (this.storedWaypoints[i]) {
                return i;
            }
        }
        return currentIndex;
    }

    nextStoredWaypointIndex(currentIndex: number) {
        for (let i = currentIndex + 1; i < this.storedWaypoints.length; i++) {
            if (this.storedWaypoints[i]) {
                return i;
            }
        }

        for (let i = 0; i < currentIndex; i++) {
            if (this.storedWaypoints[i]) {
                return i;
            }
        }

        return currentIndex;
    }

    /**
     *
     * @param {number} index storage index of the waypoint
     * @returns the number of the stored waypoint, not counting empty storage indices
     */
    storedWaypointNumber(index: number) {
        let position = 0;
        for (let i = 0; i < this.storedWaypoints.length && i <= index; i++) {
            if (this.storedWaypoints[i]) {
                position++;
            }
        }

        return position;
    }

    /**
     * Creates a waypoint from lat/lon coordinates
     * @param coordinates Coordinates of the waypoint
     * @param stored Whether the waypoint is stored
     * @param ident The ident of the waypoint, if undefined it will be generated
     * @returns The created waypoint
     */
    createLatLonWaypoint(coordinates: Coordinates, stored: boolean, ident: string = undefined): LatLonWaypoint {
        const index = stored ? this.generateStoredWaypointIndex() : -1;

        if (ident === undefined) {
            if (this.latLonExtendedFormat) { // opc or ami option... common on A330...
                const latDeg = Math.abs(Math.trunc(coordinates.lat)).toFixed(0).padStart(2, '0');
                const lonDeg = Math.abs(Math.trunc(coordinates.long)).toFixed(0).padStart(3, '0');
                ident = `${coordinates.lat >= 0 ? 'N' : 'S'}${latDeg}${coordinates.long >= 0 ? 'E' : 'W'}${lonDeg}`;
            } else {
                ident = `LL${(index + 1).toFixed(0).padStart(2, '0')}`;
            }
        }

        const waypoint: LatLonWaypoint = {
            type: PilotWaypointType.LatLon,
            storedIndex: index,
            waypoint: Fmgc.WaypointFactory.fromLocation(ident, coordinates),
        };

        if (stored) {
            this.storeWaypoint(waypoint, index);
        }

        return waypoint;
    }

    /**
     *
     * @param place1
     * @param bearing1
     * @param place2
     * @param bearing2
     * @param stored
     * @param ident
     * @returns
     */
    createPlaceBearingPlaceBearingWaypoint(
        place1: Waypoint, bearing1: DegreesTrue, place2: Waypoint, bearing2: DegreesTrue, stored = false, ident: string = undefined,
    ): PbxWaypoint {
        const coordinates = A32NX_Util.greatCircleIntersection(place1.location, bearing1, place2.location, bearing2);
        const index = stored ? this.generateStoredWaypointIndex() : -1;

        if (ident === undefined) {
            ident = `PBX${(index + 1).toFixed(0).padStart(2, '0')}`;
        }

        const waypoint: PbxWaypoint = {
            type: PilotWaypointType.Pbx,
            storedIndex: index,
            waypoint: Fmgc.WaypointFactory.fromLocation(ident, coordinates),
            pbxPlace1: place1.ident.substring(0, 5),
            pbxBearing1: bearing1,
            pbxPlace2: place2.ident.substring(0, 5),
            pbxBearing2: bearing2,
        };

        if (stored) {
            this.storeWaypoint(waypoint, index);
        }

        return waypoint;
    }

    /**
     *
     * @param origin
     * @param bearing
     * @param distance
     * @param stored
     * @param ident
     * @returns
     */
    createPlaceBearingDistWaypoint(origin: Waypoint, bearing: DegreesTrue, distance: NauticalMiles, stored = false, ident: string = undefined): PbdWaypoint {
        const coordinates = Avionics.Utils.bearingDistanceToCoordinates(bearing, distance, origin.location.lat, origin.location.long);
        const index = stored ? this.generateStoredWaypointIndex() : -1;

        if (ident === undefined) {
            ident = `PBD${(index + 1).toFixed(0).padStart(2, '0')}`;
        }

        const waypoint: PbdWaypoint = {
            type: PilotWaypointType.Pbd,
            storedIndex: index,
            waypoint: Fmgc.WaypointFactory.fromLocation(ident, coordinates),
            pbdPlace: origin.ident,
            pbdBearing: bearing,
            pbdDistance: distance,
        };

        if (stored) {
            this.storeWaypoint(waypoint, index);
        }

        return waypoint;
    }

    getStoredWaypointsByIdent(ident: string): PilotWaypoint[] {
        return this.storedWaypoints.filter((wp) => wp && wp.waypoint.ident === ident);
    }
}
