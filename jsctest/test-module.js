import * as natives from 'natives';

// Test ES6 module for jsctest
console.log("Module loading...");

export function init() {
    console.log("init() called from module!");
}

export function tick() {
    console.log("tick() called from module!");
}

function wait(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}


// Module top-level code
console.log("Module top-level code executed!");
await wait(10000);
console.log("Module waited 10 seconds!");

// Test export
export const VERSION = "1.0.0";
export default { init, tick, VERSION };
