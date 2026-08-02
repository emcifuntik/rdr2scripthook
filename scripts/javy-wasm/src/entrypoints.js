function trainerImplementation() {
    const implementation = globalThis.__rdr2JavyTrainer;
    if (!implementation) throw new Error("Javy trainer implementation is unavailable");
    return implementation;
}

export function init() {
    trainerImplementation().init();
}

export function tick() {
    trainerImplementation().tick();
}

export function shutdown() {
    trainerImplementation().shutdown();
}
