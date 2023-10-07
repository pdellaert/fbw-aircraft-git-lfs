// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

'use strict';

const esbuild = require('esbuild');
const path = require('path');
const { esbuildModuleBuild } = require('#build-utils');

const outFile = 'fbw-a32nx/out/flybywire-aircraft-a320-neo/html_ui/JS/fbw-a32nx/atsu/fmsclient.js';

esbuild.build(esbuildModuleBuild('fbw-a32nx', 'AtsuFmsClient', path.join(__dirname, 'src/index.ts'), outFile));
