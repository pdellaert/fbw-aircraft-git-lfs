// Copyright (c) 2022 FlyByWire Simulations
// SPDX-License-Identifier: GPL-3.0

import fs from 'fs';
import {join} from 'path';
import {ExecTask} from '@flybywiresim/igniter';
import {Directories} from '../directories.mjs';

const ecamPages = [
    {
        name: 'eng-page',
        path: 'SD/Pages/Eng',
    },
    {
        name: 'door-page',
        path: 'SD/Pages/Door',
    },
    {
        name: 'cond-page',
        path: 'SD/Pages/Cond',
    },
    {
        name: 'fctl-page',
        path: 'SD/Pages/Fctl',
    },
    {
        name: 'elec-page',
        path: 'SD/Pages/Elec',
    },
    {
        name: 'hyd-page',
        path: 'SD/Pages/Hyd',
    },
    {
        name: 'wheel-page',
        path: 'SD/Pages/Wheel',
    },
    {
        name: 'crz-page',
        path: 'SD/Pages/Crz',
    },
    {
        name: 'fuel-page',
        path: 'SD/Pages/Fuel',
    },
    {
        name: 'apu-page',
        path: 'SD/Pages/Apu',
    },
    {
        name: 'press-page',
        path: 'SD/Pages/Press',
    },
    {
        name: 'bleed-page',
        path: 'SD/Pages/Bleed',
    },
    {
        name: 'status-page',
        path: 'SD/Pages/Status',
    },
];

export function getInputs() {
    const baseInstruments = fs.readdirSync(join(Directories.instruments, 'src'), { withFileTypes: true })
        .filter((d) => d.isDirectory() && fs.existsSync(join(Directories.instruments, 'src', d.name, 'config.json')));

    return [
        ...baseInstruments.map(({ name }) => ({ path: name, name, isInstrument: true })),
        ...ecamPages.map((def) => ({ ...def, isInstrument: false })),
    ];
}

export function getA32NXInstrumentsIgniterTasks() {
    const baseInstruments = fs.readdirSync(join(Directories.instruments, 'src'), { withFileTypes: true })
        .filter((d) => d.isDirectory() && fs.existsSync(join(Directories.instruments, 'src', d.name, 'config.json')));

    return [
        ...baseInstruments.map(({name}) => new ExecTask(
            name,
            `node fbw-a32nx/src/systems/instruments/buildSrc/igniter/worker.mjs ${name}`,
            [
                join('fbw-a32nx/src/systems/instruments/src', name),
                join('fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/Pages/VCockpit/Instruments/A32NX', name)],
        )),
        ...ecamPages.map(({name, path}) => new ExecTask(
            name,
            `node fbw-a32nx/src/systems/instruments/buildSrc/igniter/worker.mjs ${name}`,
            [
                join('fbw-a32nx/src/systems/instruments/src', path),
                join('fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/Pages/VCockpit/Instruments/A32NX/EcamPages', name)],
        )),
    ];
}
