import React, { memo } from 'react';
import { useSimVar } from '@instruments/common/simVars';
import { EngineNumber, Position } from '@instruments/common/types';
import { GaugeComponent, GaugeMarkerComponent } from '@instruments/common/gauges';

const OutflowValve: React.FC<Position & EngineNumber> = memo(({ x, y, engine }) => {
    const ofradius = 52;

    const [flightPhase] = useSimVar('L:A32NX_FWC_FLIGHT_PHASE', 'enum', 1000);
    const [outflowValueOpenPercentage] = useSimVar('L:A32NX_PRESS_OUTFLOW_VALVE_OPEN_PERCENTAGE', 'percent', 500);

    return (
        <>
            <text x={x - 32} y={y - 82} className="F26 Green">{engine}</text>
            <GaugeComponent
                x={x}
                y={y}
                radius={ofradius}
                startAngle={270 + ((outflowValueOpenPercentage / 100) * 90)}
                endAngle={360}
                visible
                className="Gauge"
            >
                <GaugeComponent x={x} y={y + 2} radius={ofradius - 3} startAngle={352} endAngle={360} visible className="Gauge Amber ThickAmberLine" />
                <GaugeMarkerComponent
                    value={outflowValueOpenPercentage}
                    x={x}
                    y={y}
                    min={0}
                    max={100}
                    radius={ofradius}
                    startAngle={270}
                    endAngle={360}
                    className={flightPhase >= 5 && flightPhase <= 7 && outflowValueOpenPercentage > 95 ? 'Amber Line' : 'Green Line'}
                    indicator
                    multiplierOuter={1}
                />
                <GaugeMarkerComponent
                    value={0}
                    x={x}
                    y={y}
                    min={0}
                    max={100}
                    radius={ofradius}
                    startAngle={270}
                    endAngle={360}
                    className="Gauge"
                />
                <GaugeMarkerComponent
                    value={25}
                    x={x}
                    y={y}
                    min={0}
                    max={100}
                    radius={ofradius}
                    startAngle={270}
                    endAngle={360}
                    className="Gauge"
                />
                <GaugeMarkerComponent
                    value={50}
                    x={x}
                    y={y}
                    min={0}
                    max={100}
                    radius={ofradius}
                    startAngle={270}
                    endAngle={360}
                    className="Gauge"
                />
                <GaugeMarkerComponent
                    value={75}
                    x={x}
                    y={y}
                    min={0}
                    max={100}
                    radius={ofradius}
                    startAngle={270}
                    endAngle={360}
                    className="Gauge"
                />
                <GaugeMarkerComponent
                    value={100}
                    x={x}
                    y={y}
                    min={0}
                    max={100}
                    radius={ofradius}
                    startAngle={270}
                    endAngle={360}
                    className="Gauge"
                />
            </GaugeComponent>
            <circle className="Green SW3 BackgroundFill" cx={x} cy={y} r={4} />
        </>
    );
});

export default OutflowValve;
