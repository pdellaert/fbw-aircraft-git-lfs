// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { clampAngle } from 'msfs-geo';
import { MathUtils } from '@shared/MathUtils';
import { Feature, LineString } from '@turf/turf';
import { ArraySubject } from '@microsoft/msfs-sdk';
import { filterLabel, OancLabelFilter } from './OancLabelFIlter';
import { Label, LabelStyle, LABEL_VISIBILITY_RULES, Oanc, OANC_RENDER_HEIGHT, OANC_RENDER_WIDTH } from './Oanc';
import { intersectLineWithRectangle, midPoint, pointAngle, pointDistance } from './OancMapUtils';

export class OancLabelManager {
    constructor(
        public oanc: Oanc,
    ) {
    }

    public showLabels = false;

    public currentFilter: OancLabelFilter = { type: 'null' };

    public labels: Label[] = [];

    public visibleLabels = ArraySubject.create<Label>([]);

    public visibleLabelElements = new Map<Label, HTMLSpanElement>();

    public reflowLabels() {
        // eslint-disable-next-line prefer-const
        let [offsetX, offsetY] = this.oanc.mapParams.coordinatesToXYy(this.oanc.ppos);

        // TODO figure out how to not need this
        offsetY *= -1;

        const mapCurrentHeading = this.oanc.interpolatedMapHeading.get();

        const rotate = 180 - mapCurrentHeading;

        if (this.oanc.doneDrawing && this.showLabels && LABEL_VISIBILITY_RULES[this.oanc.zoomLevelIndex.get()]) {
            this.oanc.labelContainerRef.instance.style.visibility = 'visible';

            for (const label of this.visibleLabels.getArray()) {
                const element = this.visibleLabelElements.get(label);

                if (!element) {
                    continue;
                }

                const labelVisible = filterLabel(label, this.currentFilter);

                if (!labelVisible) {
                    element.style.visibility = 'hidden';
                    continue;
                }

                const [labelY, labelX] = label.position;

                const hypotenuse = Math.sqrt(labelX ** 2 + labelY ** 2) * this.oanc.getZoomLevelInverseScale();
                const angle = clampAngle(Math.atan2(labelY, labelX) * MathUtils.RADIANS_TO_DEGREES);

                const rotationAdjustX = hypotenuse * Math.cos((angle - rotate) * MathUtils.DEGREES_TO_RADIANS);
                const rotationAdjustY = hypotenuse * Math.sin((angle - rotate) * MathUtils.DEGREES_TO_RADIANS);

                const scaledOffsetX = offsetX * this.oanc.getZoomLevelInverseScale();
                const scaledOffsetY = offsetY * this.oanc.getZoomLevelInverseScale();

                if (label.style !== LabelStyle.RunwayAxis) {
                    let labelScreenX = (OANC_RENDER_WIDTH / 2) + -rotationAdjustX + -scaledOffsetX + this.oanc.panOffsetX.get();
                    let labelScreenY = (OANC_RENDER_HEIGHT / 2) + rotationAdjustY + scaledOffsetY + this.oanc.panOffsetY.get();

                    labelScreenX += this.oanc.modeAnimationOffsetX.get();
                    labelScreenY += this.oanc.modeAnimationOffsetY.get();

                    if (labelScreenX < 0 || labelScreenX > OANC_RENDER_WIDTH || labelScreenY < 0 || labelScreenY > OANC_RENDER_HEIGHT) {
                        element.style.visibility = 'hidden';
                        continue;
                    } else {
                        element.style.visibility = 'inherit';
                    }

                    element.style.left = `${labelScreenX}px`;
                    element.style.top = `${labelScreenY}px`;

                    if (label.style === LabelStyle.RunwayEnd) {
                        element.style.transform = `translate(-50%, -50%) rotate(${label.rotation - mapCurrentHeading}deg)`;
                    } else {
                        element.style.transform = 'translate(-50%, -50%)';
                    }
                } else {
                    const runway = label.associatedFeature as Feature<LineString>;

                    const screenAngle = clampAngle(label.rotation - mapCurrentHeading - (label.rotation > 180 ? 90 : -90));

                    const x1 = runway.geometry.coordinates[0][1];
                    const y1 = runway.geometry.coordinates[0][0];
                    const hy1 = Math.sqrt(x1 ** 2 + y1 ** 2) * this.oanc.getZoomLevelInverseScale();
                    const theta1 = clampAngle(Math.atan2(y1, x1) * MathUtils.RADIANS_TO_DEGREES);
                    const cx1 = hy1 * Math.cos((theta1 - rotate) * MathUtils.DEGREES_TO_RADIANS);
                    const cy1 = hy1 * Math.sin((theta1 - rotate) * MathUtils.DEGREES_TO_RADIANS);
                    const sx1 = (OANC_RENDER_WIDTH / 2) + -cx1 + -scaledOffsetX + this.oanc.panOffsetX.get();
                    const sy1 = (OANC_RENDER_HEIGHT / 2) + cy1 + scaledOffsetY + this.oanc.panOffsetY.get();

                    const x2 = runway.geometry.coordinates[runway.geometry.coordinates.length - 1][1];
                    const y2 = runway.geometry.coordinates[runway.geometry.coordinates.length - 1][0];
                    const hy2 = Math.sqrt(x2 ** 2 + y2 ** 2) * this.oanc.getZoomLevelInverseScale();
                    const theta2 = clampAngle(Math.atan2(y2, x2) * MathUtils.RADIANS_TO_DEGREES);
                    const cx2 = hy2 * Math.cos((theta2 - rotate) * MathUtils.DEGREES_TO_RADIANS);
                    const cy2 = hy2 * Math.sin((theta2 - rotate) * MathUtils.DEGREES_TO_RADIANS);
                    const sx2 = (OANC_RENDER_WIDTH / 2) + -cx2 + -scaledOffsetX + this.oanc.panOffsetX.get();
                    const sy2 = (OANC_RENDER_HEIGHT / 2) + cy2 + scaledOffsetY + this.oanc.panOffsetY.get();

                    const [lcx, lcy] = midPoint(sx1, sy1, sx2, sy2);

                    const rx = 0;
                    const ry = 0;
                    const rw = OANC_RENDER_WIDTH;
                    const rh = OANC_RENDER_HEIGHT;

                    const rwyAngle = clampAngle(pointAngle(sx1, sy1, sx2, sy2) - 90);

                    const intersections = intersectLineWithRectangle(sx1, sy1, sx2, sy2, rx, ry, rw, rh);

                    let x: number;
                    let y: number;
                    if (intersections.length > 1) {
                        const s1dlc = pointDistance(intersections[0][0], intersections[0][1], lcx, lcy);
                        // const s2dlc = pointDistance(intersections[1][0], intersections[1][1], lcx, lcy);

                        // const offset = s2dlc - s1dlc;

                        // Line screen center

                        const lscx = intersections[0][0] + (intersections[1][0] - intersections[0][0]) / 2;
                        const lscy = intersections[0][1] + (intersections[1][1] - intersections[0][1]) / 2;

                        // const lscd = pointDistance(lscx, lscy, lcx, lcy);

                        const lsci = intersectLineWithRectangle(lscx, lscy, lcx, lcy, rx, ry, rw, rh);

                        if (lsci.length > 0) {
                            x = intersections[0][0];
                            y = intersections[0][1];
                        } else {
                            x = lcx;
                            y = lcy;
                        }

                        // console.log(`${label.text}: ${lscd.toFixed(1)}, ${lsci.map((it) => it.join(', ')).join('; ')}`);

                        // const ox = s1dlc * Math.cos(screenAngle * MathUtils.DEGREES_TO_RADIANS);
                        // const oy = s1dlc * Math.sin(screenAngle * MathUtils.DEGREES_TO_RADIANS);

                        // x = (intersections[0][0]) - ox;
                        // y = (intersections[0][1]) - oy;
                    } else if (intersections.length > 0) {
                        x = intersections[0][0];
                        y = intersections[0][1];
                    } else {
                        x = -1000;
                        y = -1000;
                    }

                    x += this.oanc.modeAnimationOffsetX.get();
                    y += this.oanc.modeAnimationOffsetY.get();

                    element.style.left = `${x}px`;
                    element.style.top = `${y}px`;

                    // console.log('runway line heading: ', label.text, label.rotation);

                    element.style.transform = `translate(-50%, -50%) rotate(${rwyAngle + (rwyAngle > 180 ? 90 : -90)}deg)`;

                    // console.log(`Runway intersections (${label.text}):`, intersections);
                }
            }
        } else {
            this.oanc.labelContainerRef.instance.style.visibility = 'hidden';
        }
    }
}
