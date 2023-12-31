// Copyright (c) 2021-2022 FlyByWire Simulations
// Copyright (c) 2021-2022 Synaptic Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { LegType, Runway, TurnDirection } from 'msfs-navdata';
import { FlightPlanLegDefinition } from '@fmgc/flightplanning/new/legs/FlightPlanLegDefinition';

export function runwayIdent(runway: Runway) {
    return runway.ident.substring(2);
}

export function procedureLegIdentAndAnnotation(procedureLeg: FlightPlanLegDefinition, procedureIdent?: string): [ident: string, annotation: string] {
    const legType = procedureLeg.type;

    switch (legType) {
    case LegType.AF:
        return [
            procedureLeg.waypoint.ident,
            `${Math.round(procedureLeg.rho).toString().padStart(2, ' ')} ${procedureLeg.recommendedNavaid.ident.substring(0, 3)}`,
        ];
    case LegType.CF:
        return [
            procedureLeg.waypoint.ident,
            `C${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ];
    case LegType.IF:
    case LegType.DF:
    case LegType.TF:
        return [procedureLeg.waypoint.ident, procedureIdent ?? null];
    case LegType.RF:
        return [
            procedureLeg.waypoint.ident,
            `${Math.round(procedureLeg.length).toString().padStart(2, ' ')} ARC`,
        ];
    case LegType.CA:
    case LegType.FA:
    case LegType.VA:
        return [
            Math.round(procedureLeg.altitude1).toString().substring(0, 9),
            `${legType === LegType.VA ? 'H' : 'C'}${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ]; // TODO fix for VA
    case LegType.CD:
    case LegType.FC:
    case LegType.FD:
        const targetFix = procedureLeg.waypoint ?? procedureLeg.recommendedNavaid;

        return [
            `${targetFix.ident.substring(0, 3)}/${Math.round(procedureLeg.length).toString().padStart(2, '0')}`,
            `C${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ];
    case LegType.CI:
    case LegType.VI:
        return [
            'INTCPT',
            `${legType === LegType.CI ? 'C' : 'H'}${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ]; // TODO fix for VI
    case LegType.CR:
    case LegType.VR:
        return [
            `${procedureLeg.recommendedNavaid.ident.substring(0, 3)}${Math.round(procedureLeg.theta).toString().padStart(3, '0')}`,
            `${legType === LegType.VR ? 'H' : 'C'}${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ]; // TODO fix for VR
    case LegType.HA:
        return [
            Math.round(procedureLeg.altitude1).toString(),
            `HOLD ${procedureLeg.turnDirection === TurnDirection.Left ? 'L' : 'R'}`,
        ];
    case LegType.HF:
        return [
            procedureLeg.waypoint.ident,
            `HOLD ${procedureLeg.turnDirection === TurnDirection.Left ? 'L' : 'R'}`,
        ];
    case LegType.HM:
        return [ // TODO leg before
            procedureLeg.waypoint.ident,
            `C${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ];
    case LegType.PI:
        break;
    case LegType.VD:
        break;
    case LegType.FM:
    case LegType.VM:
        return [
            'MANUAL',
            `${legType === LegType.FM ? 'C' : 'H'}${Math.round(procedureLeg.magneticCourse).toString().padStart(3, '0')}°`,
        ]; // TODO fix for VM
    default:
        break;
    }

    return [`(UNKN ${LegType[legType]})`, 'UNKNOWN'];
}

export const pposPointIDent = 'PPOS';

export const turningPointIdent = 'T-P';

export const inboundPointIdent = 'IN-BND';

export const outboundPointIdent = 'OUT-BND';
