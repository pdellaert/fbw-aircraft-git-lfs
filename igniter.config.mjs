import { ExecTask, TaskOfTasks } from '@flybywiresim/igniter';
import { getA32NXInstrumentsIgniterTasks } from './fbw-a32nx/src/systems/instruments/buildSrc/igniter/tasks.mjs';

export default new TaskOfTasks('all', [
    // A32NX Task
    new TaskOfTasks('a32nx', [
        // Prepare the out folder and any other pre tasks.
        // Currently, these can be run in parallel but in the future, we may need to run them in sequence if there are any dependencies.
        new TaskOfTasks('preparation', [
            new ExecTask('copy-base-files', 'npm run build-a32nx:copy-base-files'),
            new ExecTask('efb-translation', 'npm run build-a32nx:efb-translation'),
        ], true),

        // Group all WASM build tasks together but separate from the rest of the tasks as build run more stable like this.
        new TaskOfTasks('wasm', [
            new ExecTask('systems', [
                'npm run build-a32nx:systems',
                'wasm-opt -O1 -o fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/systems.wasm target/wasm32-wasi/release/a320_systems_wasm.wasm',
            ], [
                'fbw-a32nx/src/wasm/systems',
                'fbw-common/src/wasm/systems',
                'Cargo.lock',
                'Cargo.toml',
                'fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/systems.wasm'
            ]),
            new ExecTask('systems-fbw', [
                'npm run build-a32nx:fbw',
                'wasm-opt -O1 -o fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/fbw.wasm fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/fbw.wasm'
            ], [
                'fbw-a32nx/src/wasm/fbw_a320',
                'fbw-common/src/wasm/fbw_common',
                'fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/fbw.wasm'
            ]),
            new ExecTask('systems-fadec', [
                'npm run build-a32nx:fadec',
                'wasm-opt -O1 -o fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/fadec.wasm fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/fadec.wasm'
            ], [
                'fbw-a32nx/src/wasm/fadec_a320',
                'fbw-common/src/wasm/fbw_common',
                'fbw-common/src/wasm/fadec_common',
                'fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/fadec.wasm']),
            new ExecTask('flypad-backend', [
                'npm run build-a32nx:flypad-backend',
                'wasm-opt -O1 -o fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/flypad-backend.wasm fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/flypad-backend.wasm'
            ], [
                'fbw-a32nx/src/wasm/flypad-backend',
                'fbw-common/src/wasm/fbw_common',
                'fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/panel/flypad-backend.wasm']),
        ], true),

        // Group all typescript and react build tasks together.
        new TaskOfTasks('build', [
            new ExecTask('model', 'npm run build-a32nx:model',['fbw-a32nx/src/model', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/SimObjects/AirPlanes/FlyByWire_A320_NEO/model']),
            new ExecTask('behavior', 'npm run build-a32nx:behavior', ['fbw-a32nx/src/behavior', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/ModelBehaviorDefs/A32NX/generated']),

            new ExecTask('atsu','npm run build-a32nx:atsu', ['fbw-a32nx/src/atsu', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/atsu']),
            // TODO: Change out folder name - double check
            new ExecTask('failures','npm run build-a32nx:failures', ['fbw-a32nx/src/failures', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/failures/failures.js']),
            new ExecTask('fmgc','npm run build-a32nx:fmgc', ['fbw-a32nx/src/fmgc', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/fmgc']),
            new ExecTask('sentry-client','npm run build-a32nx:sentry-client', ['fbw-a32nx/src/sentry-client', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/sentry-client']),
            new ExecTask('simbridge-client', ['npm run build-a32nx:simbridge-client'], ['fbw-a32nx/src/simbridge-client', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/simbridge-client']),
            // TODO: this was missing before - added it but might have had a reason it was missong
            new ExecTask('tcas','npm run build-a32nx:tcas', ['fbw-a32nx/src/tcas', 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/tcas']),

            new TaskOfTasks('instruments',
                [
                    ...getA32NXInstrumentsIgniterTasks(),
                    new ExecTask('PFD', 'npm run build-a32nx:pfd', ['fbw-a32nx/src/systems/instruments/src/PFD','fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/Pages/VCockpit/Instruments/A32NX/PFD'])
                ],
                true),
        ], true),

        // Create final package meta files.
        new TaskOfTasks('dist', [
            new ExecTask('metadata', 'node scripts/metadata.js fbw-a32nx/out/flybywire-aircraft-a320-neo a32nx'),
            new ExecTask('manifests', 'node scripts/build_a32nx.js'),
        ]),
    ]),

    // A380X Tasks
    new TaskOfTasks('a380x', [

        new TaskOfTasks('preparation', [
            new ExecTask('copy-base-files', [
                'npm run build-a380x:copy-base-files',
                // temporary until folder exists
                'mkdir -p fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/',
            ]),
        ], true),

        new TaskOfTasks('wasm', [
            new ExecTask('systems', [
                'npm run build-a380x:systems',
                'wasm-opt -O1 -o fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/systems.wasm target/wasm32-wasi/release/a380_systems_wasm.wasm',
            ], [
                'fbw-a380x/src/wasm/systems',
                'fbw-common/src/wasm/systems',
                'Cargo.lock',
                'Cargo.toml',
                'fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/systems.wasm'
            ]),
            new ExecTask('systems-fadec', [
                'npm run build-a380x:fadec',
                'wasm-opt -O1 -o fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/fadec.wasm fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/fadec.wasm'
            ], [
                'fbw-a380x/src/wasm/fadec_a320',
                'fbw-common/src/wasm/fbw_common',
                'fbw-common/src/wasm/fadec_common',
                'fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/fadec.wasm']),
            new ExecTask('systems-fbw', [
                'npm run build-a380x:fbw',
                'wasm-opt -O1 -o fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/fbw.wasm fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/fbw.wasm'
            ], [
                'fbw-a380x/src/wasm/fbw_a320',
                'fbw-common/src/wasm/fbw_common',
                'fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/fbw.wasm'
            ]),
            new ExecTask('flypad-backend', [
                'npm run build-a380x:flypad-backend',
                'wasm-opt -O1 -o fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/flypad-backend.wasm fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/flypad-backend.wasm'
            ], [
                'fbw-a380x/src/wasm/flypad-backend',
                'fbw-common/src/wasm/fbw_common',
                'fbw-a380x/out/flybywire-aircraft-a380-841/SimObjects/AirPlanes/FlyByWire_A380_841/panel/flypad-backend.wasm']),
        ], true),
    ]),

]);
