use systems::{
    fuel::FuelPayload,
    shared::SurfacesPositions,
    simulation::{
        InitContext, SimulationElement, SimulationElementVisitor, SimulatorWriter, UpdateContext,
        VariableIdentifier, Write,
    },
    structural_flex::elevator_flex::FlexibleElevators,
    structural_flex::engine_wobble::EnginesFlexiblePhysics,
    structural_flex::wing_flex::{
        FlexPhysicsNG, WingAnimationMapper, WingFuelNodeMapper, WingLift, WingRootAcceleration,
    },
    structural_flex::SurfaceVibrationGenerator,
};

use crate::fuel::A380FuelTankType;

use uom::si::{f64::*, force::newton, mass::kilogram, ratio::ratio};

use nalgebra::{Vector3, Vector5};

pub struct A380StructuralFlex {
    ground_weight_ratio_id: VariableIdentifier,

    engines_flex_physics: EnginesFlexiblePhysics<4>,
    elevators_flex_physics: FlexibleElevators,
    wing_flex: WingFlexA380,

    surface_vibrations: SurfaceVibrationGenerator,
}
impl A380StructuralFlex {
    pub fn new(context: &mut InitContext) -> Self {
        Self {
            ground_weight_ratio_id: context
                .get_identifier("GROUND_WEIGHT_ON_WHEELS_RATIO".to_owned()),

            engines_flex_physics: EnginesFlexiblePhysics::new(context),
            elevators_flex_physics: FlexibleElevators::new(context),
            wing_flex: WingFlexA380::new(context),

            surface_vibrations: SurfaceVibrationGenerator::default(),
        }
    }

    pub fn update(
        &mut self,
        context: &UpdateContext,
        outer_inner_elevator_aero_torques: [(Torque, Torque); 2],
        up_down_rudder_aero_torques: (Torque, Torque),
        surfaces_positions: &impl SurfacesPositions,
        fuel_mass: &impl FuelPayload,
    ) {
        self.elevators_flex_physics.update(
            context,
            outer_inner_elevator_aero_torques,
            up_down_rudder_aero_torques,
            self.surface_vibrations.surface_vibration_acceleration(),
        );

        self.wing_flex.update(
            context,
            self.surface_vibrations.surface_vibration_acceleration(),
            surfaces_positions,
            fuel_mass,
        );

        self.engines_flex_physics
            .update(context, self.wing_flex.accelerations_at_engines_pylons());

        self.surface_vibrations
            .update(context, self.wing_flex.ground_weight_ratio());
    }
}
impl SimulationElement for A380StructuralFlex {
    fn accept<T: SimulationElementVisitor>(&mut self, visitor: &mut T) {
        self.elevators_flex_physics.accept(visitor);
        self.engines_flex_physics.accept(visitor);
        self.wing_flex.accept(visitor);

        visitor.visit(self);
    }

    fn write(&self, writer: &mut SimulatorWriter) {
        writer.write(
            &self.ground_weight_ratio_id,
            self.wing_flex.ground_weight_ratio().get::<ratio>(),
        );
    }
}

struct A380WingLiftModifier {
    lateral_offset: Ratio,

    left_wing_lift: Force,
    right_wing_lift: Force,

    standard_lift_spread: Vector5<f64>,
    lift_left_table_newton: Vector5<f64>,
    lift_right_table_newton: Vector5<f64>,
}
impl A380WingLiftModifier {
    const LATERAL_OFFSET_GAIN: f64 = 0.25;

    // Ratio of the total lift on each wing section.
    // Sum shall be 1.
    const NOMINAL_WING_LIFT_SPREAD_RATIOS: [f64; 5] = [0., 0.42, 0.40, 0.15, 0.03];

    // GAIN to determine how much a surface spoils lift when deployed. 0.3 means a fully deployed surface reduce lift by 30%
    const SPOILER_SURFACES_SPOIL_GAIN: f64 = 0.4;
    const AILERON_SURFACES_SPOIL_GAIN: f64 = 0.2;
    const FLAPS_SURFACES_SPOIL_GAIN: f64 = 0.3;

    fn update(&mut self, total_lift: Force, surfaces_positions: &impl SurfacesPositions) {
        self.compute_lift_modifiers(total_lift, surfaces_positions);
    }

    fn compute_lift_modifiers(
        &mut self,
        total_lift: Force,
        surfaces_positions: &impl SurfacesPositions,
    ) {
        let wing_base_left_spoilers = surfaces_positions.left_spoilers_positions()[0..=1]
            .iter()
            .sum::<f64>()
            / 2.;
        let wing_mid_left_spoilers = surfaces_positions.left_spoilers_positions()[2..=7]
            .iter()
            .sum::<f64>()
            / 6.;

        let wing_base_right_spoilers = surfaces_positions.left_spoilers_positions()[0..=1]
            .iter()
            .sum::<f64>()
            / 2.;
        let wing_mid_right_spoilers = surfaces_positions.left_spoilers_positions()[2..=7]
            .iter()
            .sum::<f64>()
            / 6.;

        // Aileron position is converted from [0 1] to [-1 1] then we take the mean value
        // ((position1 - 0.5) * 2. + (position2 - 0.5) * 2.) / 2. <=> position1+position2 - 1
        let left_ailerons_mid = surfaces_positions.left_ailerons_positions()[0..=1]
            .iter()
            .sum::<f64>()
            - 1.;
        let right_ailerons_mid = surfaces_positions.right_ailerons_positions()[0..=1]
            .iter()
            .sum::<f64>()
            - 1.;

        let left_ailerons_tip = 2. * surfaces_positions.left_ailerons_positions()[2] - 1.;
        let right_ailerons_tip = 2. * surfaces_positions.right_ailerons_positions()[2] - 1.;

        self.lateral_offset = Ratio::new::<ratio>(
            ((wing_base_right_spoilers - wing_base_left_spoilers)
                + (wing_mid_right_spoilers - wing_mid_left_spoilers)
                + (right_ailerons_mid - left_ailerons_mid)
                + (right_ailerons_tip - left_ailerons_tip))
                / 4.,
        );

        self.lateral_offset *= Self::LATERAL_OFFSET_GAIN;

        self.left_wing_lift = 0.5 * (total_lift + self.lateral_offset() * total_lift);
        self.right_wing_lift = total_lift - self.left_wing_lift;

        let left_flap_lift_factor = surfaces_positions.left_flaps_position();
        let right_flap_lift_factor = surfaces_positions.right_flaps_position();

        // Lift factor is 1 - spoil factor. We consider positive position for a surface is spoiling lift
        // Spoiler panel deployed will be 1. Aileron Up will be 1. Aileron down will be -1 thus (1 - -1) = 2 adds lift
        let left_wing_lift_factor = Vector5::from([
            0.,
            (1. - wing_base_left_spoilers) * Self::SPOILER_SURFACES_SPOIL_GAIN
                + left_flap_lift_factor * Self::FLAPS_SURFACES_SPOIL_GAIN,
            (1. - wing_mid_left_spoilers) * Self::SPOILER_SURFACES_SPOIL_GAIN
                + left_flap_lift_factor * Self::FLAPS_SURFACES_SPOIL_GAIN,
            (1. - left_ailerons_mid) * Self::AILERON_SURFACES_SPOIL_GAIN,
            (1. - left_ailerons_tip) * Self::AILERON_SURFACES_SPOIL_GAIN,
        ]);
        let right_wing_lift_factor = Vector5::from([
            0.,
            (1. - wing_base_right_spoilers) * Self::SPOILER_SURFACES_SPOIL_GAIN
                + right_flap_lift_factor * Self::FLAPS_SURFACES_SPOIL_GAIN,
            (1. - wing_mid_right_spoilers) * Self::SPOILER_SURFACES_SPOIL_GAIN
                + right_flap_lift_factor * Self::FLAPS_SURFACES_SPOIL_GAIN,
            (1. - right_ailerons_mid) * Self::AILERON_SURFACES_SPOIL_GAIN,
            (1. - right_ailerons_tip) * Self::AILERON_SURFACES_SPOIL_GAIN,
        ]);

        let raw_left_total_factor = left_wing_lift_factor.component_mul(&self.standard_lift_spread);
        let raw_right_total_factor =
            right_wing_lift_factor.component_mul(&self.standard_lift_spread);

        let left_lift_factor_normalized = raw_left_total_factor / raw_left_total_factor.sum();
        let right_lift_factor_normalized = raw_right_total_factor / raw_right_total_factor.sum();

        self.lift_left_table_newton =
            left_lift_factor_normalized * self.left_wing_lift.get::<newton>();

        self.lift_right_table_newton =
            right_lift_factor_normalized * self.right_wing_lift.get::<newton>();
    }

    fn lateral_offset(&self) -> Ratio {
        self.lateral_offset
    }

    fn per_node_lift_left_wing_newton(&self) -> Vector5<f64> {
        self.lift_left_table_newton
    }

    fn per_node_lift_right_wing_newton(&self) -> Vector5<f64> {
        self.lift_right_table_newton
    }
}
impl Default for A380WingLiftModifier {
    fn default() -> Self {
        assert!(Vector5::from(Self::NOMINAL_WING_LIFT_SPREAD_RATIOS).sum() == 1.);

        Self {
            lateral_offset: Ratio::default(),

            left_wing_lift: Force::default(),
            right_wing_lift: Force::default(),

            standard_lift_spread: Vector5::from(Self::NOMINAL_WING_LIFT_SPREAD_RATIOS),
            lift_left_table_newton: Vector5::default(),
            lift_right_table_newton: Vector5::default(),
        }
    }
}

const WING_FLEX_NODE_NUMBER: usize = 5;
const WING_FLEX_LINK_NUMBER: usize = WING_FLEX_NODE_NUMBER - 1;
const FUEL_TANKS_NUMBER: usize = 5;

pub struct WingFlexA380 {
    left_flex_inboard_id: VariableIdentifier,
    left_flex_inboard_mid_id: VariableIdentifier,
    left_flex_outboard_mid_id: VariableIdentifier,
    left_flex_outboard_id: VariableIdentifier,

    right_flex_inboard_id: VariableIdentifier,
    right_flex_inboard_mid_id: VariableIdentifier,
    right_flex_outboard_mid_id: VariableIdentifier,
    right_flex_outboard_id: VariableIdentifier,

    wing_lift: WingLift,
    wing_lift_dynamic: A380WingLiftModifier,

    left_wing_fuel_mass: [Mass; FUEL_TANKS_NUMBER],
    right_wing_fuel_mass: [Mass; FUEL_TANKS_NUMBER],

    fuel_mapper: WingFuelNodeMapper<FUEL_TANKS_NUMBER, WING_FLEX_NODE_NUMBER>,
    animation_mapper: WingAnimationMapper<WING_FLEX_NODE_NUMBER>,

    flex_physics: [FlexPhysicsNG<WING_FLEX_NODE_NUMBER, WING_FLEX_LINK_NUMBER>; 2],

    left_right_wing_root_position: [WingRootAcceleration; 2],
}
impl WingFlexA380 {
    const FLEX_COEFFICIENTS: [f64; WING_FLEX_LINK_NUMBER] =
        [20000000., 8000000., 5000000., 500000.];
    const DAMPING_COEFFICIENTS: [f64; WING_FLEX_LINK_NUMBER] = [800000., 400000., 150000., 6000.];

    const EMPTY_MASS_KG: [f64; WING_FLEX_NODE_NUMBER] = [0., 25000., 20000., 5000., 400.];

    const FUEL_MAPPING: [usize; FUEL_TANKS_NUMBER] = [1, 1, 2, 2, 3];

    const WING_NODES_X_COORDINATES: [f64; WING_FLEX_NODE_NUMBER] = [0., 11.5, 22.05, 29., 36.85];

    pub fn new(context: &mut InitContext) -> Self {
        let empty_mass = Self::EMPTY_MASS_KG.map(Mass::new::<kilogram>);

        Self {
            left_flex_inboard_id: context.get_identifier("WING_FLEX_LEFT_INBOARD".to_owned()),
            left_flex_inboard_mid_id: context
                .get_identifier("WING_FLEX_LEFT_INBOARD_MID".to_owned()),
            left_flex_outboard_mid_id: context
                .get_identifier("WING_FLEX_LEFT_OUTBOARD_MID".to_owned()),
            left_flex_outboard_id: context.get_identifier("WING_FLEX_LEFT_OUTBOARD".to_owned()),

            right_flex_inboard_id: context.get_identifier("WING_FLEX_RIGHT_INBOARD".to_owned()),
            right_flex_inboard_mid_id: context
                .get_identifier("WING_FLEX_RIGHT_INBOARD_MID".to_owned()),
            right_flex_outboard_mid_id: context
                .get_identifier("WING_FLEX_RIGHT_OUTBOARD_MID".to_owned()),
            right_flex_outboard_id: context.get_identifier("WING_FLEX_RIGHT_OUTBOARD".to_owned()),

            wing_lift: WingLift::new(context),
            wing_lift_dynamic: A380WingLiftModifier::default(),

            left_wing_fuel_mass: [Mass::default(); FUEL_TANKS_NUMBER],
            right_wing_fuel_mass: [Mass::default(); FUEL_TANKS_NUMBER],

            fuel_mapper: WingFuelNodeMapper::new(Self::FUEL_MAPPING),
            animation_mapper: WingAnimationMapper::new(Self::WING_NODES_X_COORDINATES),

            flex_physics: [1, 2].map(|_| {
                FlexPhysicsNG::new(
                    context,
                    empty_mass,
                    Self::FLEX_COEFFICIENTS,
                    Self::DAMPING_COEFFICIENTS,
                )
            }),

            left_right_wing_root_position: [-1., 1.]
                .map(|xneg| WingRootAcceleration::new(Vector3::new(xneg * 3.33668, -0.273, 6.903))),
        }
    }

    fn update_fuel_masses(&mut self, fuel_mass: &impl FuelPayload) {
        self.left_wing_fuel_mass = [
            fuel_mass.tank_mass(A380FuelTankType::LeftInner as usize),
            fuel_mass.tank_mass(A380FuelTankType::FeedTwo as usize),
            fuel_mass.tank_mass(A380FuelTankType::LeftMid as usize),
            fuel_mass.tank_mass(A380FuelTankType::FeedOne as usize),
            fuel_mass.tank_mass(A380FuelTankType::LeftOuter as usize),
        ];
        self.right_wing_fuel_mass = [
            fuel_mass.tank_mass(A380FuelTankType::RightInner as usize),
            fuel_mass.tank_mass(A380FuelTankType::FeedThree as usize),
            fuel_mass.tank_mass(A380FuelTankType::RightMid as usize),
            fuel_mass.tank_mass(A380FuelTankType::FeedFour as usize),
            fuel_mass.tank_mass(A380FuelTankType::RightOuter as usize),
        ];
    }

    pub fn update(
        &mut self,
        context: &UpdateContext,
        surface_vibration_acceleration: Acceleration,
        surfaces_positions: &impl SurfacesPositions,
        fuel_mass: &impl FuelPayload,
    ) {
        self.wing_lift.update(context);
        self.wing_lift_dynamic
            .update(self.wing_lift.total_plane_lift(), surfaces_positions);

        self.left_right_wing_root_position[0].update(context);
        self.left_right_wing_root_position[1].update(context);

        self.update_fuel_masses(fuel_mass);

        self.flex_physics[0].update(
            context,
            self.wing_lift_dynamic
                .per_node_lift_left_wing_newton()
                .as_slice(),
            self.fuel_mapper.fuel_masses(self.left_wing_fuel_mass),
            surface_vibration_acceleration + self.left_right_wing_root_position[0].acceleration(),
        );

        self.flex_physics[1].update(
            context,
            self.wing_lift_dynamic
                .per_node_lift_right_wing_newton()
                .as_slice(),
            self.fuel_mapper.fuel_masses(self.right_wing_fuel_mass),
            surface_vibration_acceleration + self.left_right_wing_root_position[1].acceleration(),
        );
    }

    pub fn ground_weight_ratio(&self) -> Ratio {
        self.wing_lift.ground_weight_ratio()
    }

    // Accelerations (vertical) of engines pylons from eng1 to eng4
    pub fn accelerations_at_engines_pylons(&self) -> [Acceleration; 4] {
        [(0, 2), (0, 1), (1, 1), (1, 2)].map(|(phys_idx, node_idx)| {
            self.flex_physics[phys_idx].acceleration_at_node_idx(node_idx)
        })
    }

    #[cfg(test)]
    fn left_node_position(&self, node_id: usize) -> f64 {
        self.flex_physics[0].nodes_height_meters()[node_id]
    }
    #[cfg(test)]
    fn right_node_position(&self, node_id: usize) -> f64 {
        self.flex_physics[1].nodes_height_meters()[node_id]
    }
}
impl SimulationElement for WingFlexA380 {
    fn accept<T: SimulationElementVisitor>(&mut self, visitor: &mut T) {
        self.wing_lift.accept(visitor);

        // Calling only left wing as this is only used for dev purpose : live tuning of flex properties
        self.flex_physics[0].accept(visitor);

        visitor.visit(self);
    }

    fn write(&self, writer: &mut SimulatorWriter) {
        let bones_angles_left = self
            .animation_mapper
            .animation_angles(self.flex_physics[0].nodes_height_meters());

        writer.write(&self.left_flex_inboard_id, bones_angles_left[1]);
        writer.write(&self.left_flex_inboard_mid_id, bones_angles_left[2]);
        writer.write(&self.left_flex_outboard_mid_id, bones_angles_left[3]);
        writer.write(&self.left_flex_outboard_id, bones_angles_left[4]);

        let bones_angles_right = self
            .animation_mapper
            .animation_angles(self.flex_physics[1].nodes_height_meters());

        writer.write(&self.right_flex_inboard_id, bones_angles_right[1]);
        writer.write(&self.right_flex_inboard_mid_id, bones_angles_right[2]);
        writer.write(&self.right_flex_outboard_mid_id, bones_angles_right[3]);
        writer.write(&self.right_flex_outboard_id, bones_angles_right[4]);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    use crate::systems::simulation::test::{ElementCtorFn, WriteByName};
    use crate::systems::simulation::test::{SimulationTestBed, TestBed};
    use crate::systems::simulation::{Aircraft, SimulationElement, SimulationElementVisitor};
    use std::time::Duration;

    use uom::si::{angle::degree, velocity::knot};

    use ntest::assert_about_eq;

    struct TestSurfacesPositions {
        left_ailerons: [f64; 3],
        right_ailerons: [f64; 3],
        left_spoilers: [f64; 8],
        right_spoilers: [f64; 8],
        left_flaps: f64,
        right_flaps: f64,
    }
    impl TestSurfacesPositions {
        fn default() -> Self {
            Self {
                left_ailerons: [0.5; 3],
                right_ailerons: [0.5; 3],
                left_spoilers: [0.; 8],
                right_spoilers: [0.; 8],
                left_flaps: 0.,
                right_flaps: 0.,
            }
        }
    }
    impl SurfacesPositions for TestSurfacesPositions {
        fn left_ailerons_positions(&self) -> &[f64] {
            &self.left_ailerons
        }

        fn right_ailerons_positions(&self) -> &[f64] {
            &self.right_ailerons
        }

        fn left_spoilers_positions(&self) -> &[f64] {
            &self.left_spoilers
        }

        fn right_spoilers_positions(&self) -> &[f64] {
            &self.right_spoilers
        }

        fn left_flaps_position(&self) -> f64 {
            self.left_flaps
        }

        fn right_flaps_position(&self) -> f64 {
            self.right_flaps
        }
    }

    struct TestFuelPayload {
        fuel_mass: [Mass; 11],
    }
    impl TestFuelPayload {
        fn default() -> Self {
            Self {
                fuel_mass: [Mass::default(); 11],
            }
        }

        fn set_fuel(&mut self, tank_id: A380FuelTankType, mass: Mass) {
            self.fuel_mass[tank_id as usize] = mass;
        }
    }
    impl FuelPayload for TestFuelPayload {
        fn fore_aft_center_of_gravity(&self) -> f64 {
            0.
        }

        fn total_load(&self) -> Mass {
            Mass::default()
        }

        fn tank_mass(&self, t: usize) -> Mass {
            self.fuel_mass[t]
        }
    }

    struct WingFlexTestAircraft {
        wing_flex: WingFlexA380,
        surfaces_position: TestSurfacesPositions,
        fuel_payload: TestFuelPayload,
    }
    impl WingFlexTestAircraft {
        fn new(context: &mut InitContext) -> Self {
            Self {
                wing_flex: WingFlexA380::new(context),
                surfaces_position: TestSurfacesPositions::default(),
                fuel_payload: TestFuelPayload::default(),
            }
        }

        fn set_left_flaps(&mut self, position: f64) {
            self.surfaces_position.left_flaps = position;
        }

        fn set_right_flaps(&mut self, position: f64) {
            self.surfaces_position.right_flaps = position;
        }

        fn set_left_spoilers(&mut self, positions: [f64; 8]) {
            self.surfaces_position.left_spoilers = positions;
        }

        fn set_right_spoilers(&mut self, positions: [f64; 8]) {
            self.surfaces_position.right_spoilers = positions;
        }

        fn set_left_ailerons(&mut self, positions: [f64; 3]) {
            self.surfaces_position.left_ailerons = positions;
        }

        fn set_right_ailerons(&mut self, positions: [f64; 3]) {
            self.surfaces_position.right_ailerons = positions;
        }

        fn set_fuel(&mut self, tank_id: A380FuelTankType, mass: Mass) {
            self.fuel_payload.set_fuel(tank_id, mass);
        }
    }
    impl Aircraft for WingFlexTestAircraft {
        fn update_after_power_distribution(&mut self, context: &UpdateContext) {
            self.wing_flex.update(
                context,
                Acceleration::default(),
                &self.surfaces_position,
                &self.fuel_payload,
            );

            println!(
                "WING HEIGHTS L/O\\R => {:.2}_{:.2}_{:.2}_{:.2}_{:.2}/O\\{:.2}_{:.2}_{:.2}_{:.2}_{:.2}",
                self.wing_flex.left_node_position(4),
                self.wing_flex.left_node_position(3),
                self.wing_flex.left_node_position(2),
                self.wing_flex.left_node_position(1),
                self.wing_flex.left_node_position(0),
                self.wing_flex.right_node_position(0),
                self.wing_flex.right_node_position(1),
                self.wing_flex.right_node_position(2),
                self.wing_flex.right_node_position(3),
                self.wing_flex.right_node_position(4),
            );
        }
    }
    impl SimulationElement for WingFlexTestAircraft {
        fn accept<V: SimulationElementVisitor>(&mut self, visitor: &mut V) {
            self.wing_flex.accept(visitor);

            visitor.visit(self);
        }
    }

    struct WingFlexTestBed {
        test_bed: SimulationTestBed<WingFlexTestAircraft>,
    }
    impl WingFlexTestBed {
        const NOMINAL_WEIGHT_KG: f64 = 400000.;

        fn new() -> Self {
            Self {
                test_bed: SimulationTestBed::new(WingFlexTestAircraft::new),
            }
        }

        fn left_wing_lift_per_node(&self) -> Vector5<f64> {
            self.query(|a| {
                a.wing_flex
                    .wing_lift_dynamic
                    .per_node_lift_left_wing_newton()
            })
        }

        fn right_wing_lift_per_node(&self) -> Vector5<f64> {
            self.query(|a| {
                a.wing_flex
                    .wing_lift_dynamic
                    .per_node_lift_right_wing_newton()
            })
        }

        fn current_total_lift(&self) -> Force {
            self.query(|a| a.wing_flex.wing_lift.total_lift())
        }

        fn current_left_wing_lift(&self) -> Force {
            self.query(|a| a.wing_flex.wing_lift_dynamic.left_wing_lift)
        }

        fn current_right_wing_lift(&self) -> Force {
            self.query(|a| a.wing_flex.wing_lift_dynamic.right_wing_lift)
        }

        fn with_nominal_weight(mut self) -> Self {
            self.write_by_name(
                "TOTAL WEIGHT",
                Mass::new::<kilogram>(Self::NOMINAL_WEIGHT_KG),
            );
            self
        }

        fn with_max_fuel(mut self) -> Self {
            self.command(|a| {
                a.set_fuel(A380FuelTankType::LeftInner, Mass::new::<kilogram>(32000.))
            });
            self.command(|a| a.set_fuel(A380FuelTankType::FeedTwo, Mass::new::<kilogram>(20000.)));
            self.command(|a| a.set_fuel(A380FuelTankType::LeftMid, Mass::new::<kilogram>(28000.)));
            self.command(|a| a.set_fuel(A380FuelTankType::FeedOne, Mass::new::<kilogram>(20000.)));
            self.command(|a| a.set_fuel(A380FuelTankType::LeftOuter, Mass::new::<kilogram>(7200.)));

            self.command(|a| {
                a.set_fuel(A380FuelTankType::RightInner, Mass::new::<kilogram>(32000.))
            });
            self.command(|a| {
                a.set_fuel(A380FuelTankType::FeedThree, Mass::new::<kilogram>(20000.))
            });
            self.command(|a| a.set_fuel(A380FuelTankType::RightMid, Mass::new::<kilogram>(28000.)));
            self.command(|a| a.set_fuel(A380FuelTankType::FeedFour, Mass::new::<kilogram>(20000.)));
            self.command(|a| {
                a.set_fuel(A380FuelTankType::RightOuter, Mass::new::<kilogram>(7200.))
            });

            self
        }

        fn rotate_for_takeoff(mut self) -> Self {
            self.write_by_name("TOTAL WEIGHT", Mass::new::<kilogram>(400000.));
            self.write_by_name("AIRSPEED TRUE", Velocity::new::<knot>(150.));

            self.write_by_name("CONTACT POINT COMPRESSION:1", 30.);
            self.write_by_name("CONTACT POINT COMPRESSION:2", 30.);

            self.command(|a| a.set_left_flaps(0.5));
            self.command(|a| a.set_right_flaps(0.5));
            self
        }

        fn in_1g_flight(mut self) -> Self {
            self = self.neutral_ailerons();
            self = self.spoilers_retracted();
            self.write_by_name("TOTAL WEIGHT", Mass::new::<kilogram>(400000.));
            self.write_by_name("AIRSPEED TRUE", Velocity::new::<knot>(200.));
            self.write_by_name("CONTACT POINT COMPRESSION:1", 0.);
            self.write_by_name("CONTACT POINT COMPRESSION:2", 0.);
            self.write_by_name("CONTACT POINT COMPRESSION:3", 0.);
            self.write_by_name("CONTACT POINT COMPRESSION:4", 0.);
            self.write_by_name("CONTACT POINT COMPRESSION:5", 0.);

            self
        }

        fn steady_on_ground(mut self) -> Self {
            self.write_by_name("TOTAL WEIGHT", Mass::new::<kilogram>(400000.));
            self.write_by_name("CONTACT POINT COMPRESSION:1", 70.);
            self.write_by_name("CONTACT POINT COMPRESSION:2", 70.);
            self.write_by_name("CONTACT POINT COMPRESSION:3", 70.);
            self.write_by_name("CONTACT POINT COMPRESSION:4", 70.);
            self.write_by_name("CONTACT POINT COMPRESSION:5", 70.);

            self
        }

        fn left_turn_ailerons(mut self) -> Self {
            self.command(|a| a.set_right_ailerons([0.2, 0.2, 0.2]));
            self.command(|a| a.set_left_ailerons([0.8, 0.8, 0.8]));

            self
        }

        fn right_turn_ailerons(mut self) -> Self {
            self.command(|a| a.set_left_ailerons([0.2, 0.2, 0.2]));
            self.command(|a| a.set_right_ailerons([0.8, 0.8, 0.8]));

            self
        }

        fn neutral_ailerons(mut self) -> Self {
            self.command(|a| a.set_left_ailerons([0.5, 0.5, 0.5]));
            self.command(|a| a.set_right_ailerons([0.5, 0.5, 0.5]));

            self
        }

        fn spoilers_retracted(mut self) -> Self {
            self.command(|a| a.set_left_spoilers([0., 0., 0., 0., 0., 0., 0., 0.]));
            self.command(|a| a.set_right_spoilers([0., 0., 0., 0., 0., 0., 0., 0.]));

            self
        }

        fn spoilers_left_turn(mut self) -> Self {
            self.command(|a| a.set_left_spoilers([0., 0., 0., 0.5, 0.5, 0.5, 0.5, 0.5]));
            self.command(|a| a.set_right_spoilers([0., 0., 0., 0., 0., 0., 0., 0.]));

            self
        }

        fn spoilers_right_turn(mut self) -> Self {
            self.command(|a| a.set_right_spoilers([0., 0., 0., 0.5, 0.5, 0.5, 0.5, 0.5]));
            self.command(|a| a.set_left_spoilers([0., 0., 0., 0., 0., 0., 0., 0.]));

            self
        }

        fn run_waiting_for(mut self, delta: Duration) -> Self {
            self.test_bed.run_multiple_frames(delta);
            self
        }
    }
    impl TestBed for WingFlexTestBed {
        type Aircraft = WingFlexTestAircraft;

        fn test_bed(&self) -> &SimulationTestBed<WingFlexTestAircraft> {
            &self.test_bed
        }

        fn test_bed_mut(&mut self) -> &mut SimulationTestBed<WingFlexTestAircraft> {
            &mut self.test_bed
        }
    }

    #[test]
    fn fuel_mapping_tanks_1_2_left_wing() {
        let mut test_bed = SimulationTestBed::new(WingFlexTestAircraft::new);

        test_bed.run_with_delta(Duration::from_secs(1));

        let mut node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        let mut node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        let mut node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        let mut node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        let mut node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        println!(
            "FUELMASSES empty: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node1_mass.get::<kilogram>()
                + node2_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );

        test_bed.command(|a| a.set_fuel(A380FuelTankType::LeftInner, Mass::new::<kilogram>(3000.)));

        test_bed.run_with_delta(Duration::from_secs(1));

        node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        println!(
            "FUELMASSES after fueling inner: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node2_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );
        assert!(node1_mass.get::<kilogram>() >= 3000. && node1_mass.get::<kilogram>() <= 3100.);

        test_bed.command(|a| a.set_fuel(A380FuelTankType::FeedTwo, Mass::new::<kilogram>(3000.)));

        test_bed.run_with_delta(Duration::from_secs(1));

        node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));
        //Second tank should map to same node

        println!(
            "FUELMASSES after fueling feed 2: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node2_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );
        assert!(node1_mass.get::<kilogram>() >= 6000. && node1_mass.get::<kilogram>() <= 6200.);
    }

    #[test]
    fn fuel_mapping_tanks_3_4_left_wing() {
        let mut test_bed = SimulationTestBed::new(WingFlexTestAircraft::new);

        test_bed.run_with_delta(Duration::from_secs(1));

        let mut node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        let mut node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        let mut node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        let mut node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        let mut node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        println!(
            "FUELMASSES empty: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node1_mass.get::<kilogram>()
                + node2_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );

        test_bed.command(|a| a.set_fuel(A380FuelTankType::LeftMid, Mass::new::<kilogram>(3000.)));

        test_bed.run_with_delta(Duration::from_secs(1));

        node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        println!(
            "FUELMASSES after fueling left mid: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node1_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );
        assert!(node2_mass.get::<kilogram>() >= 3000. && node2_mass.get::<kilogram>() <= 3100.);

        test_bed.command(|a| a.set_fuel(A380FuelTankType::FeedOne, Mass::new::<kilogram>(3000.)));

        test_bed.run_with_delta(Duration::from_secs(1));

        node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        //Second tank should map to same node

        println!(
            "FUELMASSES after fueling left feed 1: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node1_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );
        assert!(node2_mass.get::<kilogram>() >= 6000. && node1_mass.get::<kilogram>() <= 6200.);
    }

    #[test]
    fn fuel_mapping_tanks_5_left_wing() {
        let mut test_bed = SimulationTestBed::new(WingFlexTestAircraft::new);

        test_bed.run_with_delta(Duration::from_secs(1));

        let mut node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        let mut node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        let mut node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        let mut node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        let mut node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        println!(
            "FUELMASSES empty: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node1_mass.get::<kilogram>()
                + node2_mass.get::<kilogram>()
                + node3_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );

        test_bed.command(|a| a.set_fuel(A380FuelTankType::LeftOuter, Mass::new::<kilogram>(3000.)));

        test_bed.run_with_delta(Duration::from_secs(1));

        node0_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(0));
        node1_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(1));
        node2_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(2));
        node3_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(3));
        node4_mass = test_bed.query(|a| a.wing_flex.flex_physics[0].node_fuel_mass(4));

        println!(
            "FUELMASSES after fueling left outer: {:.0}/{:.0}/{:.0}/{:.0}/{:.0}",
            node0_mass.get::<kilogram>(),
            node1_mass.get::<kilogram>(),
            node2_mass.get::<kilogram>(),
            node3_mass.get::<kilogram>(),
            node4_mass.get::<kilogram>(),
        );

        assert!(
            node0_mass.get::<kilogram>()
                + node1_mass.get::<kilogram>()
                + node2_mass.get::<kilogram>()
                + node4_mass.get::<kilogram>()
                <= 0.1
        );
        assert!(node3_mass.get::<kilogram>() >= 3000. && node2_mass.get::<kilogram>() <= 3100.);
    }

    #[test]
    fn animation_angles_with_0_heights_gives_zero_angles() {
        let test_bed = SimulationTestBed::from(ElementCtorFn(|_| {
            WingAnimationMapper::<5>::new([0., 1., 2., 3., 4.])
        }));

        let anim_angles = test_bed.query_element(|e| e.animation_angles([0.0, 0.0, 0.0, 0., 0.]));

        assert!(anim_angles[0] == Angle::default());
        assert!(anim_angles[1] == Angle::default());
        assert!(anim_angles[2] == Angle::default());
        assert!(anim_angles[3] == Angle::default());
        assert!(anim_angles[4] == Angle::default());
    }

    #[test]
    fn animation_angles_with_last_node_moved_1_up_gives_correct_angles() {
        let test_bed = SimulationTestBed::from(ElementCtorFn(|_| {
            WingAnimationMapper::<5>::new([0., 1., 2., 3., 4.])
        }));

        let anim_angles = test_bed.query_element(|e| e.animation_angles([0.0, 0.0, 0.0, 0., 1.]));

        assert!(anim_angles[0] == Angle::default());
        assert!(anim_angles[1] == Angle::default());
        assert!(anim_angles[2] == Angle::default());
        assert!(anim_angles[3] == Angle::default());
        assert!(anim_angles[4].get::<degree>() >= 44. && anim_angles[4].get::<degree>() <= 46.);
    }

    #[test]
    fn animation_angles_with_last_node_moved_1_down_gives_correct_angles() {
        let test_bed = SimulationTestBed::from(ElementCtorFn(|_| {
            WingAnimationMapper::<5>::new([0., 1., 2., 3., 4.])
        }));

        let anim_angles = test_bed.query_element(|e| e.animation_angles([0.0, 0.0, 0.0, 0., -1.]));

        assert!(anim_angles[0] == Angle::default());
        assert!(anim_angles[1] == Angle::default());
        assert!(anim_angles[2] == Angle::default());
        assert!(anim_angles[3] == Angle::default());
        assert!(anim_angles[4].get::<degree>() >= -46. && anim_angles[4].get::<degree>() <= -44.);
    }

    #[test]
    fn animation_angles_with_first_node_moved_1_up_gives_correct_angles() {
        let test_bed = SimulationTestBed::from(ElementCtorFn(|_| {
            WingAnimationMapper::<5>::new([0., 1., 2., 3., 4.])
        }));

        let anim_angles = test_bed.query_element(|e| e.animation_angles([0., 1., 1., 1., 1.]));

        assert_about_eq!(
            anim_angles[0].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
        assert!(anim_angles[1].get::<degree>() >= 44. && anim_angles[0].get::<degree>() <= 46.);
        assert_about_eq!(
            anim_angles[2].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
        assert_about_eq!(
            anim_angles[3].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
        assert_about_eq!(
            anim_angles[4].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
    }

    #[test]
    fn animation_angles_with_first_node_moved_1_down_gives_correct_angles() {
        let test_bed = SimulationTestBed::from(ElementCtorFn(|_| {
            WingAnimationMapper::<5>::new([0., 1., 2., 3., 4.])
        }));

        let anim_angles = test_bed.query_element(|e| e.animation_angles([0., -1., -1., -1., -1.]));

        assert!(anim_angles[0] == Angle::default());
        assert!(anim_angles[1].get::<degree>() <= -44. && anim_angles[1].get::<degree>() >= -46.);
        assert_about_eq!(
            anim_angles[2].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
        assert_about_eq!(
            anim_angles[3].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
        assert_about_eq!(
            anim_angles[4].get::<degree>(),
            Angle::default().get::<degree>(),
            1.0e-3
        );
    }

    #[test]
    fn steady_on_ground() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .steady_on_ground();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(3));

        assert!(test_bed.current_total_lift().get::<newton>() / 9.8 < 1000.);
        assert!(test_bed.current_total_lift().get::<newton>() / 9.8 > -1000.);
    }

    #[test]
    fn steady_on_ground_full_fuel() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .with_max_fuel()
            .steady_on_ground();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(3));

        assert!(test_bed.current_total_lift().get::<newton>() / 9.8 < 1000.);
        assert!(test_bed.current_total_lift().get::<newton>() / 9.8 > -1000.);
    }

    #[test]
    fn with_some_lift_on_ground_rotation() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .rotate_for_takeoff();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        assert!(
            test_bed.current_total_lift().get::<newton>() / 9.8
                < WingFlexTestBed::NOMINAL_WEIGHT_KG
        );
        assert!(
            test_bed.current_total_lift().get::<newton>() / 9.8
                > WingFlexTestBed::NOMINAL_WEIGHT_KG * 0.5
        );
    }

    #[test]
    fn in_straight_flight_has_plane_lift_equal_to_weight() {
        let mut test_bed = WingFlexTestBed::new().with_nominal_weight().in_1g_flight();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        assert!(
            test_bed.current_total_lift().get::<newton>() / 9.8
                < WingFlexTestBed::NOMINAL_WEIGHT_KG * 1.1
        );
        assert!(
            test_bed.current_total_lift().get::<newton>() / 9.8
                > WingFlexTestBed::NOMINAL_WEIGHT_KG * 0.9
        );
    }

    #[test]
    fn in_straight_flight_at_max_fuel_has_plane_lift_equal_to_weight() {
        let mut test_bed = WingFlexTestBed::new().with_max_fuel().in_1g_flight();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        assert!(
            test_bed.current_total_lift().get::<newton>() / 9.8
                < WingFlexTestBed::NOMINAL_WEIGHT_KG * 1.1
        );
        assert!(
            test_bed.current_total_lift().get::<newton>() / 9.8
                > WingFlexTestBed::NOMINAL_WEIGHT_KG * 0.9
        );
    }

    #[test]
    fn in_left_turn_flight_has_more_right_lift() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .in_1g_flight()
            .left_turn_ailerons()
            .spoilers_left_turn();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        assert!(
            test_bed.current_left_wing_lift().get::<newton>()
                < test_bed.current_right_wing_lift().get::<newton>()
        );
    }

    #[test]
    fn in_right_turn_flight_has_more_left_lift() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .in_1g_flight()
            .right_turn_ailerons()
            .spoilers_right_turn();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        assert!(
            test_bed.current_left_wing_lift().get::<newton>()
                > test_bed.current_right_wing_lift().get::<newton>()
        );
    }

    #[test]
    fn in_straight_flight_all_left_lifts_all_right_lifts_equals_total_lift() {
        let mut test_bed = WingFlexTestBed::new().with_nominal_weight().in_1g_flight();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        // One percent precision check on total lift value
        assert!(
            test_bed.left_wing_lift_per_node().sum() + test_bed.right_wing_lift_per_node().sum()
                <= test_bed.current_total_lift().get::<newton>() * 1.01
        );

        assert!(
            test_bed.left_wing_lift_per_node().sum() + test_bed.right_wing_lift_per_node().sum()
                >= test_bed.current_total_lift().get::<newton>() * 0.99
        );
    }

    #[test]
    fn in_right_turn_all_left_lifts_all_right_lifts_equals_total_lift() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .in_1g_flight()
            .right_turn_ailerons()
            .spoilers_right_turn();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        // One percent precision check on total lift value
        assert!(
            test_bed.left_wing_lift_per_node().sum() + test_bed.right_wing_lift_per_node().sum()
                <= test_bed.current_total_lift().get::<newton>() * 1.01
        );

        assert!(
            test_bed.left_wing_lift_per_node().sum() + test_bed.right_wing_lift_per_node().sum()
                >= test_bed.current_total_lift().get::<newton>() * 0.99
        );
    }

    #[test]
    fn in_left_turn_all_left_lifts_all_right_lifts_equals_total_lift() {
        let mut test_bed = WingFlexTestBed::new()
            .with_nominal_weight()
            .in_1g_flight()
            .left_turn_ailerons()
            .spoilers_left_turn();

        test_bed = test_bed.run_waiting_for(Duration::from_secs(1));

        // One percent precision check on total lift value
        assert!(
            test_bed.left_wing_lift_per_node().sum() + test_bed.right_wing_lift_per_node().sum()
                <= test_bed.current_total_lift().get::<newton>() * 1.01
        );

        assert!(
            test_bed.left_wing_lift_per_node().sum() + test_bed.right_wing_lift_per_node().sum()
                >= test_bed.current_total_lift().get::<newton>() * 0.99
        );
    }
}
