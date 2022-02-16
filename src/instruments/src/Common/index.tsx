import React from 'react';
import ReactDOM from 'react-dom';
import * as Defaults from './defaults';
import { SimVarProvider } from './simVars';
import { FbwAircraftSentryClient } from '../../../sentry-client/src/FbwAircraftSentryClient';

declare const process: any;

/**
 * Use the given React element to render the instrument using React.
 */
export const render = (Slot: React.ReactElement, enableSentryTracing = false, sentryRootClient = false) => {
    new FbwAircraftSentryClient().onInstrumentLoaded({
        dsn: process.env.SENTRY_DSN,
        buildInfoFilePrefix: 'a32nx',
        enableTracing: enableSentryTracing,
        root: sentryRootClient,
    });

    ReactDOM.render(<SimVarProvider>{Slot}</SimVarProvider>, Defaults.getRenderTarget());
};

/**
 * Computes time delta out of absolute env time and previous
 * time debounced on time shift.
 */
export const debouncedTimeDelta = (
    absTimeSeconds: number,
    prevTimeSeconds: number,
): number => {
    const diff = Math.max(absTimeSeconds - prevTimeSeconds, 0);
    // 60s detects forward time-shift
    return diff < 60 ? diff : 0;
};
