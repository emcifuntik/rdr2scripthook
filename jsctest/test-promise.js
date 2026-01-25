// Test Promise support with setTimeout

function wait(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function main() {
    console.log("Starting Promise test...");
    console.log("Time: " + Date.now());

    console.log("Waiting 100ms...");
    await wait(100);
    console.log("After 100ms wait, Time: " + Date.now());

    console.log("Waiting another 50ms...");
    await wait(50);
    console.log("After 50ms wait, Time: " + Date.now());

    console.log("Promise test complete!");
}

// Run the async function
main().then(() => {
    console.log("Main function finished!");
}).catch(err => {
    console.log("Error: " + err);
});

console.log("Script setup complete (async work pending)");
