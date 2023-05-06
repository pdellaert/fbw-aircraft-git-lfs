//  Copyright (c) 2022 FlyByWire Simulations
//  SPDX-License-Identifier: GPL-3.0

import { FlightPlanMessage } from '@datalink/common';
import { NXDataStore } from '@shared/persistence';

const SimbriefUrl = 'https://www.simbrief.com/api/xml.fetcher.php?json=1&userid=';

/* eslint-disable camelcase */
interface IGeneral {
    icao_airline: string,
    flight_number: string,
    is_etops: string,
    constinex: string,
    initial_altitude: string,
    stepclimb_string: string,
    route: string,
    route_ifps: string,
}

interface IAirportNotam {
    notam_id: string,
    location_icao: string,
    date_created: string,
    date_effective: string,
    date_expire: string,
    date_expire_is_estimated: string,
    date_modified: string,
    notam_text: string,
    notam_raw: string,
}

interface IAirport {
    icao_code: string,
    iata_code: string,
    name: string,
    plan_rwy: string,
    notam: IAirportNotam[],
}

interface IFix {
    ident: string,
    name: string,
    type: string,
    pos_lat: string,
    pos_long: string,
    via_airway: string,
    is_sid_star: string,
}

interface INavigationLog {
    fix: IFix[],
}

interface IEtopsPoint {
    icao_code: string,
    pos_lat_apt: string,
    pos_long_apt: string,
    pos_lat_fix: string,
    pos_long_fix: string,
    elapsed_time: string,
    min_fob: string,
    est_fob: string,
}

interface IEtopsDiversionAirport {
    icao_code: string,
    track_true: string,
    distance: string,
    est_fob: string,
}

interface IEtopsEqualTimePoint {
    pos_lat: string,
    pos_long: string,
    elapsed_time: string,
    min_fob: string,
    est_fob: string,
    etops_condition: string,
    div_time: string,
    div_burn: string,
    critical_fuel: string,
    div_altitude: string,
    div_airport: IEtopsDiversionAirport[],
}

interface IEtopsCriticalPoint {
    fix_type: string,
    pos_lat: string,
    pos_long: string,
    elapsed_time: string,
    est_fob: string,
    critical_fuel: string,
}

interface IEtops {
    rule: string,
    entry: IEtopsPoint,
    exit: IEtopsPoint,
    suitable_airport: IAirport[],
    equal_time_point: IEtopsEqualTimePoint,
    critical_point: IEtopsCriticalPoint,
}

interface IAtc {
    callsign: string,
    fir_orig: string,
    fir_dest: string,
    fir_altn: string,
    fir_etps: string[],
    fir_enroute: string[],
}

interface IFuel {
    taxi: string,
    enroute_burn: string,
    contingency: string,
    alternate_burn: string,
    reserve: string,
    etops: string,
    extra: string,
    min_takeoff: string,
    plan_takeoff: string,
    plan_ramp: string,
    plan_landing: string,
}

interface ITimes {
    est_time_enroute: string,
    sched_time_enroute: string,
    sched_out: string,
    sched_off: string,
    sched_on: string,
    sched_in: string,
    sched_block: string,
    est_out: string,
    est_off: string,
    est_on: string,
    est_in: string,
    est_block: string,
    taxi_out: string,
    taxi_in: string,
    reserve_time: string,
    endurance: string,
    contfuel_time: string,
    etopsfuel_time: string,
    extrafuel_time: string,
}

interface IWeights {
    pax_count: string,
    bag_count: string,
    pax_weight: string,
    bag_weight: string,
    freight_added: string,
    cargo: string,
    payload: string,
}

interface INotamRecord {
    notam_id: string,
    icao_id: string,
    notam_created_dtg: string,
    notam_effective_dtg: string,
    notam_expire_dtg: string,
    notam_lastmod_dtg: string,
    notam_inserted_dtg: string,
    notam_text: string,
    notam_report: string,
}

interface INotams {
    notamdrec: INotamRecord[],
}

interface ISimbriefData {
    general: IGeneral,
    origin: IAirport,
    destination: IAirport,
    alternate: IAirport,
    navlog: INavigationLog,
    etops?: IEtops,
    fuel: IFuel,
    times: ITimes,
    weights: IWeights,
    notams: INotams
}
/* eslint-enable camelcase */

export class SimbriefConnector {
    private static receiveData(): Promise<ISimbriefData> {
        const simBriefUserId = NXDataStore.get('CONFIG_SIMBRIEF_USERID', '');

        if (simBriefUserId) {
            return fetch(`${SimbriefUrl}&userid=${simBriefUserId}`)
                .then((response) => {
                    if (!response.ok) {
                        throw new Error(`Simbrief API error: ${response.status}`);
                    }

                    return response.json();
                });
        }

        throw new Error('No SimBrief pilot ID provided');
    }

    public static receiveFlightplan(): Promise<FlightPlanMessage> {
        return SimbriefConnector.receiveData().then((ofp) => {
            const message = new FlightPlanMessage();

            /* extract the airport data */
            message.Origin.icao = ofp.origin.icao_code;
            message.Origin.runway = ofp.origin.plan_rwy;
            message.Destination.icao = ofp.destination.icao_code;
            message.Destination.runway = ofp.destination.plan_rwy;
            message.Alternate.icao = ofp.alternate.icao_code;
            message.Alternate.runway = ofp.alternate.plan_rwy;

            /* create the route chunks */
            for (let i = 0; i < ofp.navlog.fix.length; ++i) {
                const lastFix = ofp.navlog.fix[i - 1];
                const fix = ofp.navlog.fix[i];

                if (fix.ident === 'TOC' || fix.ident === 'TOD') {
                    continue;
                }

                const lastChunk = message.RouteChunks[message.RouteChunks.length - 1];

                if (fix.is_sid_star === '1') {
                    if (!lastChunk) {
                        message.RouteChunks.push({ instruction: 'procedure', ident: fix.via_airway });
                    } else if (lastChunk.instruction !== 'procedure') {
                        message.RouteChunks.push({ instruction: 'procedure', ident: fix.via_airway });
                    }
                } else if (lastChunk?.instruction === 'procedure' && lastChunk.ident === fix.via_airway) {
                    message.RouteChunks.push({ instruction: 'sidEnrouteTransition', ident: fix.ident, locationHint: { lat: parseFloat(fix.pos_lat), long: parseFloat(fix.pos_long) } });
                } else if (fix.via_airway === 'DCT') {
                    if (fix.type === 'ltlg') {
                        message.RouteChunks.push({ instruction: 'latlong', lat: parseFloat(fix.pos_lat), long: parseFloat(fix.pos_long) });
                    } else {
                        message.RouteChunks.push({ instruction: 'waypoint', ident: fix.ident, locationHint: { lat: parseFloat(fix.pos_lat), long: parseFloat(fix.pos_long) } });
                    }
                } else if (!(lastChunk && lastChunk.instruction === 'airway' && lastChunk.ident === fix.via_airway)) {
                    if (lastFix && lastChunk && lastChunk.instruction === 'airway' && fix.via_airway !== lastFix.via_airway) {
                        message.RouteChunks.push({ instruction: 'airwayTermination', ident: lastFix.ident });
                    }
                    message.RouteChunks.push({ instruction: 'airway', ident: fix.via_airway, locationHint: { lat: parseFloat(fix.pos_lat), long: parseFloat(fix.pos_long) } });
                } else if (ofp.navlog.fix[i + 1] && ofp.navlog[i + 1].via_airway !== fix.via_airway) {
                    message.RouteChunks.push({ instruction: 'airwayTermination', ident: fix.ident });
                }
            }

            return message;
        });
    }
}