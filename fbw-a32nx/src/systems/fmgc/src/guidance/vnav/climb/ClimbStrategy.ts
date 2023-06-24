// Copyright (c) 2021-2023 FlyByWire Simulations
//
// SPDX-License-Identifier: GPL-3.0

import { VerticalProfileComputationParametersObserver } from '@fmgc/guidance/vnav/VerticalProfileComputationParameters';
import { DEFAULT_AIRCRAFT_CONFIG, DescentStrategy } from '@fmgc/guidance/vnav/descent/DescentStrategy';
import { WindComponent } from '@fmgc/guidance/vnav/wind';
import { VnavConfig } from '@fmgc/guidance/vnav/VnavConfig';
import { AircraftConfiguration } from '@fmgc/guidance/vnav/descent/ApproachPathBuilder';
import { MathUtils } from '@flybywiresim/fbw-sdk';
import { UnitType } from '@microsoft/msfs-sdk';
import { EngineModel } from '../EngineModel';
import { Predictions, StepResults } from '../Predictions';
import { AtmosphericConditions } from '../AtmosphericConditions';

// TODO: Consider encapsulating arguments in an object to avoid having to guess paramet.er order.
export interface ClimbStrategy {
    /**
     * Computes predictions for a single segment using the atmospheric conditions in the middle.
     * @param initialAltitude Altitude at the start of climb
     * @param finalAltitude Altitude to terminate the climb
     * @param speed
     * @param mach
     * @param fuelOnBoard Remainging fuel on board at the start of the climb
     * @returns `StepResults`
     */
    predictToAltitude(
        initialAltitude: number, finalAltitude: number, speed: Knots, mach: Mach, fuelOnBoard: number, headwindComponent: WindComponent, config?: AircraftConfiguration
    ): StepResults;

    predictToDistance(
        initialAltitude: number, distance: NauticalMiles, speed: Knots, mach: Mach, fuelOnBoard: number, headwindComponent: WindComponent, config?: AircraftConfiguration
    ): StepResults;

    predictToSpeed(
        initialAltitude: number, finalSpeed: Knots, initialSpeed: Knots, mach: Mach, fuelOnBoard: number, headwindComponent: WindComponent, config?: AircraftConfiguration
    ): StepResults;
}

export class VerticalSpeedStrategy implements ClimbStrategy, DescentStrategy {
    constructor(private observer: VerticalProfileComputationParametersObserver, private atmosphericConditions: AtmosphericConditions, private verticalSpeed: FeetPerMinute) { }

    predictToAltitude(
        initialAltitude: Feet, finalAltitude: Feet, speed: Knots, mach: Mach, fuelOnBoard: number, headwindComponent: WindComponent, config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause } = this.observer.get();

        return Predictions.verticalSpeedStep(
            initialAltitude,
            finalAltitude,
            this.verticalSpeed,
            speed,
            mach,
            zeroFuelWeight,
            fuelOnBoard,
            this.atmosphericConditions.isaDeviation,
            headwindComponent.value,
            tropoPause,
            config.gearExtended,
            config.flapConfig,
            config.gearExtended,
            perfFactor,
        );
    }

    predictToDistance(
        initialAltitude: Feet,
        distance: NauticalMiles,
        speed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause } = this.observer.get();

        return Predictions.verticalSpeedDistanceStep(
            initialAltitude,
            distance,
            this.verticalSpeed,
            speed,
            mach,
            zeroFuelWeight,
            fuelOnBoard,
            this.atmosphericConditions.isaDeviation,
            headwindComponent.value,
            tropoPause,
            config.speedbrakesExtended,
            config.flapConfig,
            config.gearExtended,
            perfFactor,
        );
    }

    predictToSpeed(
        initialAltitude: Feet,
        finalSpeed: Knots,
        initialSpeed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause, managedClimbSpeedMach } = this.observer.get();

        const computedMach = Math.min(this.atmosphericConditions.computeMachFromCas(initialAltitude, initialSpeed), mach);

        const n1 = this.verticalSpeed > 0
            ? getClimbThrustN1Limit(this.atmosphericConditions, initialAltitude, initialSpeed, managedClimbSpeedMach)
            : EngineModel.getIdleN1(initialAltitude, computedMach, tropoPause) + VnavConfig.IDLE_N1_MARGIN;

        return Predictions.verticalSpeedStepWithSpeedChange(
            initialAltitude,
            initialSpeed,
            finalSpeed,
            this.verticalSpeed,
            mach,
            n1,
            zeroFuelWeight,
            fuelOnBoard,
            headwindComponent.value,
            this.atmosphericConditions.isaDeviation,
            tropoPause,
            config.speedbrakesExtended,
            config.flapConfig,
            config.gearExtended,
            perfFactor,
        );
    }
}

export class FlightPathAngleStrategy implements ClimbStrategy, DescentStrategy {
    constructor(private observer: VerticalProfileComputationParametersObserver, private atmosphericConditions: AtmosphericConditions, public flightPathAngle: Radians) { }

    predictToAltitude(
        initialAltitude: Feet,
        finalAltitude: Feet,
        initialSpeed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause } = this.observer.get();
        const distance = UnitType.FOOT.convertTo(finalAltitude - initialAltitude, UnitType.NMILE) / Math.tan(this.flightPathAngle * MathUtils.DEGREES_TO_RADIANS);

        return Predictions.geometricStep(
            initialAltitude,
            finalAltitude,
            distance,
            initialSpeed,
            mach,
            zeroFuelWeight,
            fuelOnBoard,
            this.atmosphericConditions.isaDeviation,
            headwindComponent.value,
            tropoPause,
            config.gearExtended,
            config.flapConfig,
            config.speedbrakesExtended,
            perfFactor,
        );
    }

    predictToDistance(
        initialAltitude: Feet,
        distance: NauticalMiles,
        speed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause } = this.observer.get();

        const finalAltitude = initialAltitude + UnitType.NMILE.convertTo(distance, UnitType.FOOT) * Math.tan(this.flightPathAngle * MathUtils.DEGREES_TO_RADIANS);

        return Predictions.geometricStep(
            initialAltitude,
            finalAltitude,
            distance,
            speed,
            mach,
            zeroFuelWeight,
            fuelOnBoard,
            this.atmosphericConditions.isaDeviation,
            headwindComponent.value,
            tropoPause,
            config.gearExtended,
            config.flapConfig,
            config.speedbrakesExtended,
            perfFactor,
        );
    }

    /**
     * If the path is being built backwards and we are trying to calculate a deceleration segment, `finalSpeed` should be greater than `speed`.
     * In this case, this predicts a segment where the aircraft decelerates to `finalSpeed` from `speed`.
     */
    predictToSpeed(
        initialAltitude: Feet,
        finalSpeed: Knots,
        initialSpeed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause, managedClimbSpeedMach } = this.observer.get();

        const computedMach = Math.min(this.atmosphericConditions.computeMachFromCas(initialAltitude, initialSpeed), mach);
        const predictedN1 = this.flightPathAngle > 0
            ? getClimbThrustN1Limit(this.atmosphericConditions, initialAltitude, initialSpeed, managedClimbSpeedMach)
            : EngineModel.getIdleN1(initialAltitude, computedMach, tropoPause) + VnavConfig.IDLE_N1_MARGIN;

        return Predictions.speedChangeStep(
            this.flightPathAngle,
            initialAltitude,
            initialSpeed,
            finalSpeed,
            mach,
            mach,
            predictedN1,
            zeroFuelWeight,
            fuelOnBoard,
            headwindComponent.value,
            this.atmosphericConditions.isaDeviation,
            tropoPause,
            config.gearExtended,
            config.flapConfig,
            config.speedbrakesExtended,
            0,
            perfFactor,
        );
    }
}

export class ClimbThrustClimbStrategy implements ClimbStrategy {
    constructor(private observer: VerticalProfileComputationParametersObserver, private atmosphericConditions: AtmosphericConditions) { }

    predictToAltitude(
        initialAltitude: Feet, finalAltitude: Feet, speed: Knots, mach: Mach, fuelOnBoard: number, headwindComponent: WindComponent, config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, tropoPause, perfFactor, managedClimbSpeedMach } = this.observer.get();

        return Predictions.altitudeStep(
            initialAltitude,
            finalAltitude - initialAltitude,
            speed,
            mach,
            getClimbThrustN1Limit(this.atmosphericConditions, (initialAltitude + finalAltitude) / 2, speed, managedClimbSpeedMach),
            zeroFuelWeight,
            fuelOnBoard,
            headwindComponent.value,
            this.atmosphericConditions.isaDeviation,
            tropoPause,
            config.speedbrakesExtended,
            config.flapConfig,
            config.speedbrakesExtended,
            perfFactor,
        );
    }

    predictToDistance(
        initialAltitude: Feet,
        distance: NauticalMiles,
        speed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, tropoPause, perfFactor, managedClimbSpeedMach } = this.observer.get();

        return Predictions.distanceStep(
            initialAltitude,
            distance,
            speed,
            mach,
            getClimbThrustN1Limit(this.atmosphericConditions, initialAltitude, speed, managedClimbSpeedMach),
            zeroFuelWeight,
            fuelOnBoard,
            headwindComponent.value,
            this.atmosphericConditions.isaDeviation,
            tropoPause,
            config.speedbrakesExtended,
            config.flapConfig,
            config.gearExtended,
            perfFactor,
        );
    }

    predictToSpeed(
        initialAltitude: Feet,
        finalSpeed: Knots,
        initialSpeed: Knots,
        mach: Mach,
        fuelOnBoard: number,
        headwindComponent: WindComponent,
        config: AircraftConfiguration = DEFAULT_AIRCRAFT_CONFIG,
    ): StepResults {
        const { zeroFuelWeight, perfFactor, tropoPause, managedClimbSpeedMach } = this.observer.get();

        return Predictions.altitudeStepWithSpeedChange(
            initialAltitude,
            initialSpeed,
            finalSpeed,
            mach,
            getClimbThrustN1Limit(this.atmosphericConditions, initialAltitude, initialSpeed, managedClimbSpeedMach),
            zeroFuelWeight,
            fuelOnBoard,
            headwindComponent.value,
            this.atmosphericConditions.isaDeviation,
            tropoPause,
            config.speedbrakesExtended,
            config.flapConfig,
            config.gearExtended,
            perfFactor,
        );
    }
}

function getClimbThrustN1Limit(atmosphericConditions: AtmosphericConditions, altitude: Feet, speed: Knots, maxMach: Mach) {
    const climbSpeedMach = Math.min(maxMach, atmosphericConditions.computeMachFromCas(altitude, speed));
    const estimatedTat = atmosphericConditions.totalAirTemperatureFromMach(altitude, climbSpeedMach);

    return EngineModel.tableInterpolation(EngineModel.maxClimbThrustTableLeap, estimatedTat, altitude);
}
