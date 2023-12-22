import React, { FC } from 'react';
import { RudderPosition } from './Rudder';

const SCALE_LENGTH = 116;
const MAX_DEFLECTION = 30;
const MIN_DEFLECTION = -30;

interface HorizontalDeflectionIndicationProps {
    x?: number,
    y?: number,
    powerAvail: boolean,
    deflectionInfoValid: boolean,
    deflection: number,
    position: RudderPosition,
}

export function deflectionToXOffset(deflection: number): number {
    const normalizedDeflection = deflection > 0 ? deflection / MAX_DEFLECTION : -deflection / MIN_DEFLECTION;

    return normalizedDeflection * SCALE_LENGTH / 2;
}

export const HorizontalDeflectionIndication: FC<HorizontalDeflectionIndicationProps> = ({ x = 0, y = 0, powerAvail, deflectionInfoValid, deflection, position }) => {
    const deflectionXValue = deflectionToXOffset(deflection);

    const onGround = true;

    const maxDeflectionVisible = onGround && deflectionInfoValid && powerAvail;

    const powerAvailableClass = powerAvail ? 'Green' : 'Amber';

    return (
        <g transform={`translate(${x} ${y})`}>
            <path className='Grey Fill' d='m0,0 h 116 v15 h-116 z' />

            <path className={`Green SW2 ${maxDeflectionVisible ? '' : 'Hide'}`} d='m-1,0 v 15 M117,0 v15' />

            <path className={`${powerAvailableClass} Fill ${deflectionInfoValid ? '' : 'Hide'}`} d={`m58,0 v15 h${deflectionXValue} v-15 z`} />
            {/* This is the small line in the middle of the scale, when the surface is neutral. */}
            <path className={`${powerAvailableClass} SW2 ${deflectionInfoValid ? '' : 'Hide'}`} d='m58,0 v15' />

            <text
                x={49}
                y={position === RudderPosition.Upper ? 15 : 20}
                className={`Amber F26 ${!deflectionInfoValid ? '' : 'Hide'}`}
            >
                X
            </text>
        </g>
    );
};
