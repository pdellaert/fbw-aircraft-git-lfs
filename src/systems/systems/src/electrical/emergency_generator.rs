use std::time::Duration;

use super::{
    ElectricalStateWriter, Potential, PotentialOrigin, PotentialSource, ProvideFrequency,
    ProvidePotential,
};
use crate::{
    shared::{PowerConsumptionReport, RamAirTurbineHydraulicLoopPressurised},
    simulation::{SimulationElement, SimulatorWriter, UpdateContext},
};
use uom::si::{electric_potential::volt, f64::*, frequency::hertz};

pub struct EmergencyGenerator {
    writer: ElectricalStateWriter,
    supplying: bool,
    output_frequency: Frequency,
    output_potential: ElectricPotential,
    time_since_start: Duration,
    starting_or_started: bool,
}
impl EmergencyGenerator {
    pub fn new() -> EmergencyGenerator {
        EmergencyGenerator {
            writer: ElectricalStateWriter::new("EMER_GEN"),
            supplying: false,
            output_frequency: Frequency::new::<hertz>(0.),
            output_potential: ElectricPotential::new::<volt>(0.),
            time_since_start: Duration::from_secs(0),
            starting_or_started: false,
        }
    }

    pub fn update(
        &mut self,
        context: &UpdateContext,
        hydraulic: &impl RamAirTurbineHydraulicLoopPressurised,
    ) {
        // TODO: All of this is a very simple implementation.
        // Once hydraulics is available we should improve it.
        if self.starting_or_started {
            self.time_since_start += context.delta();
        }

        self.supplying = hydraulic.is_rat_hydraulic_loop_pressurised()
            && self.starting_or_started
            && self.time_since_start > Duration::from_secs(8);
    }

    pub fn start(&mut self) {
        self.starting_or_started = true;
    }

    #[cfg(test)]
    pub fn stop(&mut self) {
        self.starting_or_started = false;
    }

    /// Indicates if the provided electricity's potential and frequency
    /// are within normal parameters. Use this to decide if the
    /// generator contactor should close.
    pub fn output_within_normal_parameters(&self) -> bool {
        self.should_provide_output() && self.frequency_normal() && self.potential_normal()
    }

    fn should_provide_output(&self) -> bool {
        self.supplying
    }
}
impl PotentialSource for EmergencyGenerator {
    fn output(&self) -> Potential {
        if self.should_provide_output() {
            Potential::single(PotentialOrigin::EmergencyGenerator, self.output_potential)
        } else {
            Potential::none()
        }
    }
}
provide_frequency!(EmergencyGenerator, (390.0..=410.0));
provide_potential!(EmergencyGenerator, (110.0..=120.0));
impl SimulationElement for EmergencyGenerator {
    fn process_power_consumption_report<T: PowerConsumptionReport>(&mut self, _report: &T) {
        self.output_frequency = if self.should_provide_output() {
            Frequency::new::<hertz>(400.)
        } else {
            Frequency::new::<hertz>(0.)
        };

        self.output_potential = if self.should_provide_output() {
            ElectricPotential::new::<volt>(115.)
        } else {
            ElectricPotential::new::<volt>(0.)
        };
    }

    fn write(&self, writer: &mut SimulatorWriter) {
        self.writer.write_alternating(self, writer);
    }
}
impl Default for EmergencyGenerator {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod emergency_generator_tests {
    use super::*;
    use crate::simulation::{
        test::SimulationTestBed, Aircraft, SimulationElementVisitor, UpdateContext,
    };

    struct EmergencyGeneratorTestBed {
        test_bed: SimulationTestBed,
    }
    impl EmergencyGeneratorTestBed {
        fn new() -> Self {
            Self {
                test_bed: SimulationTestBed::new(),
            }
        }

        fn run_aircraft(&mut self, aircraft: &mut impl Aircraft, delta: Duration) {
            self.test_bed.set_delta(delta);
            self.test_bed.run_aircraft(aircraft);
        }

        fn frequency_is_normal(&mut self) -> bool {
            self.test_bed.read_bool("ELEC_EMER_GEN_FREQUENCY_NORMAL")
        }

        fn potential_is_normal(&mut self) -> bool {
            self.test_bed.read_bool("ELEC_EMER_GEN_POTENTIAL_NORMAL")
        }
    }

    struct TestHydraulicSystem {
        is_rat_hydraulic_loop_pressurised: bool,
    }
    impl TestHydraulicSystem {
        fn new() -> Self {
            Self {
                is_rat_hydraulic_loop_pressurised: true,
            }
        }

        fn set_rat_hydraulic_loop_pressurised(&mut self, pressurised: bool) {
            self.is_rat_hydraulic_loop_pressurised = pressurised;
        }
    }
    impl RamAirTurbineHydraulicLoopPressurised for TestHydraulicSystem {
        fn is_rat_hydraulic_loop_pressurised(&self) -> bool {
            self.is_rat_hydraulic_loop_pressurised
        }
    }

    struct TestAircraft {
        emer_gen: EmergencyGenerator,
        hydraulic: TestHydraulicSystem,
        generator_output_within_normal_parameters_before_processing_power_consumption_report: bool,
    }
    impl TestAircraft {
        fn new() -> Self {
            Self {
                emer_gen: EmergencyGenerator::new(),
                hydraulic: TestHydraulicSystem::new(),
                generator_output_within_normal_parameters_before_processing_power_consumption_report: false,
            }
        }

        fn emer_gen_is_powered(&self) -> bool {
            self.emer_gen.is_powered()
        }

        fn attempt_emer_gen_start(&mut self) {
            self.emer_gen.start();
        }

        fn stop_emer_gen(&mut self) {
            self.emer_gen.stop();
        }

        fn set_rat_hydraulic_loop_pressurised(&mut self, pressurised: bool) {
            self.hydraulic
                .set_rat_hydraulic_loop_pressurised(pressurised);
        }

        fn generator_output_within_normal_parameters_before_processing_power_consumption_report(
            &self,
        ) -> bool {
            self.generator_output_within_normal_parameters_before_processing_power_consumption_report
        }

        fn generator_output_within_normal_parameters_after_processing_power_consumption_report(
            &self,
        ) -> bool {
            self.emer_gen.output_within_normal_parameters()
        }
    }
    impl Aircraft for TestAircraft {
        fn update_before_power_distribution(&mut self, context: &UpdateContext) {
            self.emer_gen.update(context, &self.hydraulic);

            self.generator_output_within_normal_parameters_before_processing_power_consumption_report = self.emer_gen.output_within_normal_parameters();
        }
    }
    impl SimulationElement for TestAircraft {
        fn accept<T: SimulationElementVisitor>(&mut self, visitor: &mut T) {
            self.emer_gen.accept(visitor);

            visitor.visit(self);
        }
    }

    #[test]
    fn when_shutdown_has_no_output() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(!aircraft.emer_gen_is_powered());
    }

    #[test]
    fn when_started_provides_output() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        aircraft.attempt_emer_gen_start();
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(aircraft.emer_gen_is_powered());
    }

    #[test]
    fn when_started_without_hydraulic_pressure_is_unpowered() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        aircraft.attempt_emer_gen_start();
        aircraft.set_rat_hydraulic_loop_pressurised(false);
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(!aircraft.emer_gen_is_powered());
    }

    #[test]
    fn when_shutdown_frequency_not_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(!test_bed.frequency_is_normal());
    }

    #[test]
    fn when_started_frequency_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        aircraft.attempt_emer_gen_start();
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(test_bed.frequency_is_normal());
    }

    #[test]
    fn when_shutdown_potential_not_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(!test_bed.potential_is_normal());
    }

    #[test]
    fn when_started_potential_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        aircraft.attempt_emer_gen_start();
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(test_bed.potential_is_normal());
    }

    #[test]
    fn output_not_within_normal_parameters_when_shutdown() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(!aircraft
            .generator_output_within_normal_parameters_after_processing_power_consumption_report());
    }

    #[test]
    fn output_within_normal_parameters_when_started() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        aircraft.attempt_emer_gen_start();
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        assert!(aircraft
            .generator_output_within_normal_parameters_after_processing_power_consumption_report());
    }

    #[test]
    fn output_within_normal_parameters_adapts_to_no_longer_supplying_emer_gen_instantaneously() {
        // The frequency and potential of the generator are only known at the end of a tick,
        // due to them being directly related to the power consumption (large changes can cause
        // spikes and dips). However, the decision if a generator can supply power is made much
        // earlier in the tick. This is especially of great consequence when the generator no longer
        // supplies potential but the previous tick's frequency and potential are still normal.
        // With this test we ensure that a generator which is no longer supplying power is
        // immediately noticed.
        let mut aircraft = TestAircraft::new();
        let mut test_bed = EmergencyGeneratorTestBed::new();

        aircraft.attempt_emer_gen_start();
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(100));

        aircraft.stop_emer_gen();
        test_bed.run_aircraft(&mut aircraft, Duration::from_secs(0));

        assert!(!aircraft
            .generator_output_within_normal_parameters_before_processing_power_consumption_report(
            ));
    }

    #[test]
    fn writes_its_state() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = SimulationTestBed::new();

        test_bed.run_aircraft(&mut aircraft);

        assert!(test_bed.contains_key("ELEC_EMER_GEN_POTENTIAL"));
        assert!(test_bed.contains_key("ELEC_EMER_GEN_POTENTIAL_NORMAL"));
        assert!(test_bed.contains_key("ELEC_EMER_GEN_FREQUENCY"));
        assert!(test_bed.contains_key("ELEC_EMER_GEN_FREQUENCY_NORMAL"));
    }
}
