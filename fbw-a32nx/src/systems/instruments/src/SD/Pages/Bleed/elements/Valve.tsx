import React, { FC } from 'react';

interface ValveProps {
    x: number,
    y: number,
    radius: number,
    position: 'V' |'H' | 'D',
    css: string,
    sdacDatum: boolean
}

const Valve: FC<ValveProps> = ({ x, y, radius, position, css, sdacDatum }) => (
    <g>
        <circle cx={x} cy={y} r={radius} className={css} />
        {!sdacDatum ? <text x={x + 1} y={y + 5} className="Small Amber Center">XX</text> : null}
        {sdacDatum && position === 'V' ? <path className={css} d={`M ${x},${y - radius} l 0,${2 * radius}`} /> : null}
        {sdacDatum && position === 'H' ? <path className={css} d={`M ${x - radius},${y} l ${2 * radius},0`} /> : null}
        {sdacDatum && position === 'D' ? <path className={css} d={`M ${x - 0.707 * radius},${y - 0.707 * radius} l ${1.414 * radius},${1.414 * radius}`} /> : null}
    </g>
);

export default Valve;
