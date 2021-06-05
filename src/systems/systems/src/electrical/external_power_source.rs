use crate::{
    shared::PowerConsumptionReport,
    simulation::{Read, SimulationElement, SimulatorReader, SimulatorWriter, UpdateContext},
};
use uom::si::{electric_potential::volt, f64::*, frequency::hertz};

use super::{
    ElectricalStateWriter, Potential, PotentialOrigin, PotentialSource, ProvideFrequency,
    ProvidePotential,
};

pub struct ExternalPowerSource {
    writer: ElectricalStateWriter,
    is_connected: bool,
    output_frequency: Frequency,
    output_potential: ElectricPotential,
}
impl ExternalPowerSource {
    pub fn new() -> ExternalPowerSource {
        ExternalPowerSource {
            writer: ElectricalStateWriter::new("EXT_PWR"),
            is_connected: false,
            output_frequency: Frequency::new::<hertz>(0.),
            output_potential: ElectricPotential::new::<volt>(0.),
        }
    }

    pub fn update(&mut self, _: &UpdateContext) {}

    /// Indicates if the provided electricity's potential and frequency
    /// are within normal parameters. Use this to decide if the
    /// external power contactor should close.
    pub fn output_within_normal_parameters(&self) -> bool {
        self.should_provide_output() && self.potential_normal() && self.frequency_normal()
    }

    fn should_provide_output(&self) -> bool {
        self.is_connected
    }
}
impl PotentialSource for ExternalPowerSource {
    fn output(&self) -> Potential {
        if self.should_provide_output() {
            Potential::single(PotentialOrigin::External, self.output_potential)
        } else {
            Potential::none()
        }
    }
}
provide_potential!(ExternalPowerSource, (110.0..=120.0));
provide_frequency!(ExternalPowerSource, (390.0..=410.0));
impl SimulationElement for ExternalPowerSource {
    fn read(&mut self, reader: &mut SimulatorReader) {
        self.is_connected = reader.read("EXTERNAL POWER AVAILABLE:1");
    }

    fn write(&self, writer: &mut SimulatorWriter) {
        self.writer.write_alternating(self, writer);
    }

    fn process_power_consumption_report<T: PowerConsumptionReport>(&mut self, _: &T) {
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
}
impl Default for ExternalPowerSource {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod external_power_source_tests {
    use super::*;
    use crate::simulation::{test::SimulationTestBed, Aircraft, SimulationElementVisitor};

    struct ExternalPowerTestBed {
        test_bed: SimulationTestBed,
    }
    impl ExternalPowerTestBed {
        fn new() -> Self {
            Self {
                test_bed: SimulationTestBed::new(),
            }
        }

        fn with_disconnected_external_power(mut self) -> Self {
            self.disconnect_external_power();
            self
        }

        fn with_connected_external_power(mut self) -> Self {
            self.test_bed.write_bool("EXTERNAL POWER AVAILABLE:1", true);
            self
        }

        fn disconnect_external_power(&mut self) {
            self.test_bed
                .write_bool("EXTERNAL POWER AVAILABLE:1", false);
        }

        fn run_aircraft(&mut self, aircraft: &mut impl Aircraft) {
            self.test_bed.run_aircraft(aircraft);
        }

        fn frequency_is_normal(&mut self) -> bool {
            self.test_bed.read_bool("ELEC_EXT_PWR_FREQUENCY_NORMAL")
        }

        fn potential_is_normal(&mut self) -> bool {
            self.test_bed.read_bool("ELEC_EXT_PWR_POTENTIAL_NORMAL")
        }
    }

    struct TestAircraft {
        ext_pwr: ExternalPowerSource,
        ext_pwr_output_within_normal_parameters_before_processing_power_consumption_report: bool,
    }
    impl TestAircraft {
        fn new() -> Self {
            Self {
                ext_pwr: ExternalPowerSource::new(),
                ext_pwr_output_within_normal_parameters_before_processing_power_consumption_report: false,
            }
        }

        fn ext_pwr_is_powered(&self) -> bool {
            self.ext_pwr.is_powered()
        }

        fn ext_pwr_output_within_normal_parameters_after_processing_power_consumption_report(
            &self,
        ) -> bool {
            self.ext_pwr.output_within_normal_parameters()
        }

        fn ext_pwr_output_within_normal_parameters_before_processing_power_consumption_report(
            &self,
        ) -> bool {
            self.ext_pwr_output_within_normal_parameters_before_processing_power_consumption_report
        }
    }
    impl Aircraft for TestAircraft {
        fn update_before_power_distribution(&mut self, _context: &UpdateContext) {
            self.ext_pwr_output_within_normal_parameters_before_processing_power_consumption_report = self.ext_pwr.output_within_normal_parameters();
        }
    }
    impl SimulationElement for TestAircraft {
        fn accept<T: SimulationElementVisitor>(&mut self, visitor: &mut T) {
            self.ext_pwr.accept(visitor);
            visitor.visit(self);
        }
    }

    #[test]
    fn when_disconnected_provides_no_output() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_disconnected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(!aircraft.ext_pwr_is_powered());
    }

    #[test]
    fn when_connected_provides_output() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_connected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(aircraft.ext_pwr_is_powered());
    }

    #[test]
    fn when_disconnected_frequency_not_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_disconnected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(!test_bed.frequency_is_normal());
    }

    #[test]
    fn when_connected_frequency_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_connected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(test_bed.frequency_is_normal());
    }

    #[test]
    fn when_disconnected_potential_not_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_disconnected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(!test_bed.potential_is_normal());
    }

    #[test]
    fn when_connected_potential_normal() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_connected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(test_bed.potential_is_normal());
    }

    #[test]
    fn output_not_within_normal_parameters_when_disconnected() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_disconnected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(!aircraft
            .ext_pwr_output_within_normal_parameters_after_processing_power_consumption_report());
    }

    #[test]
    fn output_within_normal_parameters_when_connected() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_connected_external_power();

        test_bed.run_aircraft(&mut aircraft);

        assert!(aircraft
            .ext_pwr_output_within_normal_parameters_after_processing_power_consumption_report());
    }

    #[test]
    fn output_within_normal_parameters_adapts_to_no_longer_supplying_ext_pwr_instantaneously() {
        // The frequency and potential of the external power source are only known at the end of a tick,
        // due to them being directly related to the power consumption (large changes can cause
        // spikes and dips). However, the decision if EXT PWR source can supply power is made much
        // earlier in the tick. This is especially of great consequence when EXT PWR source no longer
        // supplies potential but the previous tick's frequency and potential are still normal.
        // With this test we ensure that an EXT PWR source which is no longer supplying power is
        // immediately noticed.
        let mut aircraft = TestAircraft::new();
        let mut test_bed = ExternalPowerTestBed::new().with_connected_external_power();
        test_bed.run_aircraft(&mut aircraft);

        test_bed.disconnect_external_power();
        test_bed.run_aircraft(&mut aircraft);

        assert!(!aircraft
            .ext_pwr_output_within_normal_parameters_before_processing_power_consumption_report());
    }

    #[test]
    fn writes_its_state() {
        let mut aircraft = TestAircraft::new();
        let mut test_bed = SimulationTestBed::new();

        test_bed.run_aircraft(&mut aircraft);

        assert!(test_bed.contains_key("ELEC_EXT_PWR_POTENTIAL"));
        assert!(test_bed.contains_key("ELEC_EXT_PWR_POTENTIAL_NORMAL"));
        assert!(test_bed.contains_key("ELEC_EXT_PWR_FREQUENCY"));
        assert!(test_bed.contains_key("ELEC_EXT_PWR_FREQUENCY_NORMAL"));
    }
}
