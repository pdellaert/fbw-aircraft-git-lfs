'use strict';

const { JSDOM } = require('jsdom');
const rnp = require('@flybywiresim/rnp');
const fs = require('fs');
const path = require('path');

const SRC = path.resolve(__dirname, 'src');
const OUT = path.resolve(__dirname, '..', '..', 'A32NX/ModelBehaviorDefs/A32NX/generated');

fs.mkdirSync(OUT, { recursive: true });

function translate(source) {
    const dom = new JSDOM(source, {
        contentType: 'text/xml',
    });

    Array.from(dom.window.document.querySelectorAll('[type="rnp"]'))
        .forEach((e) => {
            const { messages, output } = rnp.translate(e.innerHTML, '(inline)', (r, s) => {
                throw new Error(`Could not resolve '${s}' from '${r}'`);
            });
            messages.forEach((m) => {
                process.stderr.write(`${m.level}: ${m.message}\n${m.detail}\n`);
            });

            const leading = /(^\s*)/.exec(e.innerHTML)[1];
            const trailing = /(\s*$)/.exec(e.innerHTML)[1];

            e.removeAttribute('type');
            e.innerHTML = `${leading}${output.replace(/\n/g, leading)}${trailing}`;
        });

    return dom.serialize();
}

fs.readdirSync(SRC)
    .forEach((f) => {
        const filename = path.join(SRC, f);
        const source = fs.readFileSync(filename, 'utf8');
        const translated = translate(source);
        fs.writeFileSync(path.join(OUT, f), translated);
    });
