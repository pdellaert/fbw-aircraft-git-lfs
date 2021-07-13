import React, { ReactElement, useState } from 'react';
import { connect } from 'react-redux';

import { useInteractionEvent } from '../../../Common/hooks';

import { LINESELECT_KEYS } from '../Buttons';
import { lineColors } from './Line';
import { scratchpadState } from '../../redux/reducers/scratchpadRedcuer';

type InteractiveSplitLineProps = {
    leftSide: ReactElement,
    rightSide: ReactElement,
    lsk: LINESELECT_KEYS,
    slashColor: lineColors,
    scratchpad: scratchpadState
}

const InteractiveSplitLine: React.FC<InteractiveSplitLineProps> = (
    { leftSide, rightSide, lsk, slashColor, scratchpad },
) => {
    const [leftValue, setLeftValue] = useState(() => {
        const { value } = leftSide.props;
        return value;
    });
    const [rightValue, setRightValue] = useState(() => {
        const { value } = rightSide.props;
        return value;
    });

    function splitScratchpadValue() {
        let [leftValue, rightValue] = scratchpad.currentMessage.split('/');

        if (leftValue.length <= 0) {
            leftValue = '';
        }
        if (rightValue) {
            if (rightValue.length <= 0) {
                rightValue = '';
            }
        }

        return [leftValue, rightValue];
    }

    useInteractionEvent(lsk, () => {
        const [lVal, rVal] = splitScratchpadValue();
        setLeftValue(lVal);
        setRightValue(rVal);
    });

    return (
        <>
            <p className="line">
                {React.cloneElement(leftSide, { value: leftValue })}
                <span className={`${slashColor}`}>/</span>
                {React.cloneElement(rightSide, { value: rightValue })}
            </p>
        </>
    );
};
const mapStateToProps = ({ scratchpad }) => ({ scratchpad });
export default connect(mapStateToProps)(InteractiveSplitLine);
