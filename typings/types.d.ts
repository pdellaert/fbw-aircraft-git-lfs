import { FlightPlanService as Service } from "../src/fmgc/src";
import { NavigationDatabase as Database, NavigationDatabaseBackend as DatabaseBackend } from '../src/fmgc/src/NavigationDatabase'
import { FlightPlanIndex as Index } from '../src/fmgc/src';

declare global {
    type NauticalMiles = number;
    type Heading = number;
    type Track = number;
    type Latitude = number;
    type Longitude = number;
    type Feet = number;
    type Knots = number;
    type FeetPerMinute = number;
    type Metres = number;
    type MetresPerSecond = number;
    type Mach = number;
    type Degrees = number;
    type DegreesMagnetic = number;
    type DegreesTrue = number;
    type Seconds = number;
    type Percent = number;
    type Radians = number;
    type RotationsPerMinute = number;
    type Angl16 = number;
    type RadiansPerSecond = number;
    type PercentOver100 = number;
    type Gallons = number;
    type Kilograms = number;
    type Celcius = number;
    type InchesOfMercury = number;
    type Millibar = number;
    type PressurePerSquareInch = number;

    namespace Fmgc {
        const FlightPlanService: typeof Service

        const NavigationDatabase: typeof Database

        const NavigationDatabaseBackend: typeof DatabaseBackend

        const FlightPlanIndex: typeof Index
    }
}

export {};