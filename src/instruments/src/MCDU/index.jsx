import ReactDOM from 'react-dom';
import { useState } from 'react';
import {
    renderTarget,
    useUpdate,
    getSimVar,
} from '../util.mjs';
import './styles.scss';
import Titlebar from './Titlebar/Titlebar.jsx';
import PagesContainer from './Pages/PagesContainer.tsx';
import Scratchpad from './Scratchpad/Scratchpad.jsx';
import { RootContext } from './RootContext.jsx';

// TODO: Move anything dependent on ac power change to A32NX_Core
function powerAvailable() {
    // These are inlined so they're only evaluated if prior conditions return false.
    return (
        Simplane.getEngineActive(0) === 1 || Simplane.getEngineActive(1) === 1
    ) || (
        getSimVar('L:APU_GEN_ONLINE')
    ) || (
        getSimVar('EXTERNAL POWER AVAILABLE:1') && getSimVar('EXTERNAL POWER ON')
    );
}

function SelfTest() {
    return (
        <svg className="text-wrapper">
            <text x="246" y="170">SELF TEST IN PROGRESS</text>
            <text x="246" y="210">(MAX 10 SECONDS)</text>
        </svg>
    );
}

function Idle() {
    const [scratchpad, setScratchpad] = useState('SCRATCHPAD');
    const [title, setTitle] = useState('TITLE FIELD');

    return (
        <div className="mcdu-outer">
            <RootContext.Provider value={[scratchpad, setScratchpad, title, setTitle]}>
                <div className="mcdu-inner">
                    <Titlebar />
                    <div className="mcdu-content">
                        <PagesContainer />
                    </div>
                    <Scratchpad />
                </div>
            </RootContext.Provider>
        </div>
    );
}

const MCDU = () => {
    const [state, setState] = useState('DEFAULT');

    useUpdate((_deltaTime) => {
        if (state === 'OFF') {
            if (powerAvailable()) {
                setState('ON');
            }
        } else if (!powerAvailable()) {
            setState('OFF');
        }
    });

    switch (state) {
    case 'DEFAULT':
        if (getSimVar('L:A32NX_COLD_AND_DARK_SPAWN')) {
            setState('OFF');
        } else {
            setState('IDLE');
        }
        return <></>;
    case 'OFF':
        return <></>;
    case 'ON':
        setTimeout(() => {
            if (powerAvailable()) {
                setState('IDLE');
            }
        }, 5000);
        return <SelfTest />;
    case 'IDLE':
        return <Idle />;
    default:
        throw new RangeError();
    }
};

ReactDOM.render(<MCDU />, renderTarget);
