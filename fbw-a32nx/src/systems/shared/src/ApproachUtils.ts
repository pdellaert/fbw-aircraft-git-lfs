// Copyright (c) 2023 FlyByWire Simulations
// SPDX-License-Identifier: GPL-3.0

import { RunwayUtils } from '@shared/RunwayUtils';

export type ApproachNameComponents = {
    // the approach type, e.g. ILS or RNAV
    type: string,

    // the runway
    runway: string,

    // alphanumeric designator when multiple approaches of the same type exist for the same runway
    designator: string | undefined,
};

export class ApproachUtils {
    public static parseApproachName(name: string): ApproachNameComponents | undefined {
        // L(eft), C(entre), R(ight), T(true North) are the possible runway designators (ARINC424)
        // If there are multiple procedures for the same type of approach, an alphanumeric suffix is added to their names (last subpattern)
        // We are a little more lenient than ARINC424 in an effort to match non-perfect navdata, so we allow dashes, spaces, or nothing before the suffix
        const match = name.trim().match(/^(ILS|LOC|RNAV|NDB|VOR|GPS)? ?(RW)?([0-9]{1,2}[LCRT]?)?([\s-]*([A-Z0-9]))?$/);
        if (!match) {
            return undefined;
        }
        return {
            type: match[1] ?? '',
            runway: match[3] ?? '',
            designator: match[5] ?? '',
        };
    }

    public static parseApproach(approach: RawApproach): ApproachNameComponents | undefined {
        const type = ApproachUtils.approachTypeString(approach.approachType);
        const runway = RunwayUtils.runwayString(approach.runwayNumber, approach.runwayDesignator);
        const designator = approach.approachSuffix;
        return {
            type,
            runway,
            designator,
        };
    }

    private static formatShortApproachName(arg0: ApproachNameComponents | string | RawApproach): string {
        let appr: ApproachNameComponents;
        if (typeof arg0 === 'string') {
            appr = ApproachUtils.parseApproachName(arg0);
        } else if ('finalLegs' in arg0) {
            appr = ApproachUtils.parseApproach(arg0);
        } else {
            appr = arg0;
        }

        if (!appr) {
            return typeof arg0 === 'string' ? arg0 : '';
        }

        const runway = Avionics.Utils.formatRunway(appr.runway);
        const suffix = appr.designator ? `${runway.length > 2 ? '' : '-'}${appr.designator}` : '';
        return `${appr.type.replace('RNAV', 'RNV')}${runway}${suffix}`;
    }

    public static shortApproachName: {
        /**
         * Format an approach name in short format (max 7 chars)
         * @param name approach name from the nav database
         * @returns An approach name in short format (e.g. RNV23LY)
         */
        (name: string): string;
        /**
         * Format an approach name in short format (max 7 chars)
         * @param components Components of the approach name from {@link ApproachUtils.parseApproachName}
         * @returns An approach name in short format (e.g. RNV23LY)
         */
        (components: ApproachNameComponents): string;
        /**
         * Format an approach name in short format (max 7 chars)
         * @param approach A JS_Approach approach object
         * @returns An approach name in short format (e.g. RNV23LY)
         */
        (approach: RawApproach): string;
    } = ApproachUtils.formatShortApproachName;

    private static formatLongApproachName(arg0: ApproachNameComponents | string | RawApproach): string {
        let appr: ApproachNameComponents;
        if (typeof arg0 === 'string') {
            appr = ApproachUtils.parseApproachName(arg0);
        } else if ('finalLegs' in arg0) {
            appr = ApproachUtils.parseApproach(arg0);
        } else {
            appr = arg0;
        }

        const runway = Avionics.Utils.formatRunway(appr.runway);
        const suffix = appr.designator ? `-${appr.designator}` : '';
        return `${appr.type}${runway}${suffix}`;
    }

    public static longApproachName: {
        /**
         * Format an approach name in long format (max 9 chars)
         * @param name approach name from the nav database
         * @returns An approach name in long format (e.g. RNAV23L-Y)
         */
        (name: string): string;
        /**
         * Format an approach name in long format (max 9 chars)
         * @param components Components of the approach name from {@link ApproachUtils.parseApproachName}
         * @returns An approach name in long format (e.g. RNAV23L-Y)
         */
        (components: ApproachNameComponents): string;
        /**
         * Format an approach name in long format (max 9 chars)
         * @param approach A JS_Approach approach object
         * @returns An approach name in long format (e.g. RNAV23L-Y)
         */
        (approach: RawApproach): string;
    } = ApproachUtils.formatLongApproachName;

    public static approachTypeString(type: ApproachType): string {
        switch (type) {
        case ApproachType.APPROACH_TYPE_GPS:
            return 'GPS';
        case ApproachType.APPROACH_TYPE_ILS:
            return 'ILS';
        case ApproachType.APPROACH_TYPE_LDA:
            return 'LDA';
        case ApproachType.APPROACH_TYPE_LOCALIZER:
        case ApproachType.APPROACH_TYPE_LOCALIZER_BACK_COURSE:
            return 'LOC';
        case ApproachType.APPROACH_TYPE_NDB:
        case ApproachType.APPROACH_TYPE_NDBDME:
            return 'NDB';
        case ApproachType.APPROACH_TYPE_RNAV:
            return 'RNAV';
        case ApproachType.APPROACH_TYPE_SDF:
            return 'SDF';
        case ApproachType.APPROACH_TYPE_VOR:
        case ApproachType.APPROACH_TYPE_VORDME:
            return 'VOR';
        default:
            return '';
        }
    }
}
