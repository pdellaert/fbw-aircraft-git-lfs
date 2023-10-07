// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { ProcedureTransition } from 'msfs-navdata';
import { FlightPlanElement, FlightPlanLeg } from '@fmgc/flightplanning/new/legs/FlightPlanLeg';
import { SegmentClass } from '@fmgc/flightplanning/new/segments/SegmentClass';
import { BaseFlightPlan, FlightPlanQueuedOperation } from '@fmgc/flightplanning/new/plans/BaseFlightPlan';
import { ProcedureSegment } from '@fmgc/flightplanning/new/segments/ProcedureSegment';

export class DepartureEnrouteTransitionSegment extends ProcedureSegment<ProcedureTransition> {
    class = SegmentClass.Departure

    allLegs: FlightPlanElement[] = []

    get procedure(): ProcedureTransition | undefined {
        return this.departureEnrouteTransition;
    }

    private departureEnrouteTransition: ProcedureTransition | undefined = undefined

    setProcedure(ident: string | undefined, skipUpdateLegs?: boolean): Promise<void> {
        if (ident === undefined) {
            this.departureEnrouteTransition = undefined;

            if (!skipUpdateLegs) {
                this.allLegs.length = 0;

                this.flightPlan.syncSegmentLegsChange(this);
            }

            return;
        }

        const { originAirport, originRunway, originDeparture } = this.flightPlan;

        if (!originAirport || !originRunway || !originDeparture) {
            throw new Error('[FMS/FPM] Cannot set origin enroute transition without destination airport, runway and SID');
        }

        const originEnrouteTransitions = originDeparture.enrouteTransitions;

        const matchingOriginEnrouteTransition = originEnrouteTransitions.find((transition) => transition.ident === ident);

        if (!matchingOriginEnrouteTransition) {
            throw new Error(`[FMS/FPM] Can't find origin enroute transition '${ident}' for ${originAirport.ident} ${originDeparture.ident}`);
        }

        this.departureEnrouteTransition = matchingOriginEnrouteTransition;

        if (skipUpdateLegs) {
            return;
        }

        this.allLegs.length = 0;

        const mappedOriginEnrouteTransitionLegs = matchingOriginEnrouteTransition.legs.map((leg) => FlightPlanLeg.fromProcedureLeg(this, leg, matchingOriginEnrouteTransition.ident));
        this.allLegs.push(...mappedOriginEnrouteTransitionLegs);
        this.strung = false;

        this.flightPlan.enqueueOperation(FlightPlanQueuedOperation.Restring);
        this.flightPlan.enqueueOperation(FlightPlanQueuedOperation.SyncSegmentLegs, this);
    }

    clone(forPlan: BaseFlightPlan): DepartureEnrouteTransitionSegment {
        const newSegment = new DepartureEnrouteTransitionSegment(forPlan);

        newSegment.strung = this.strung;
        newSegment.allLegs = [...this.allLegs.map((it) => (it.isDiscontinuity === false ? it.clone(newSegment) : it))];
        newSegment.departureEnrouteTransition = this.departureEnrouteTransition;

        return newSegment;
    }
}
