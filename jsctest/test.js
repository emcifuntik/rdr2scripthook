// Simple test script for jsctest
console.log("Hello from JSC!");
console.log("Testing basic operations...");

// Test basic JS features
const arr = [1, 2, 3, 4, 5];
const sum = arr.reduce((a, b) => a + b, 0);
console.log("Sum of [1,2,3,4,5]:", sum);

// Test object
const obj = {
    name: "JSC Test",
    version: "1.0",
    features: ["modules", "async/await", "classes"]
};
console.log("Object:", JSON.stringify(obj, null, 2));

// Test class
class Greeter {
    constructor(name) {
        this.name = name;
    }

    greet() {
        return `Hello, ${this.name}!`;
    }
}

const greeter = new Greeter("World");
console.log(greeter.greet());

// Test async (will only show if microtasks are drained)
Promise.resolve("Async works!").then(msg => console.log(msg));

console.log("Script execution complete!");
