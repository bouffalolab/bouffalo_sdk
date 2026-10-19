/* ES5 Function Methods & Strict Mode Test
 * Tests: Function.prototype.call, apply, bind,
 *        arguments object, strict mode this behavior,
 *        named function expressions, IIFE
 */

var _p = 0, _f = 0, _s = "ES5 Function Methods & Strict Mode";

function _t(name, fn) {
    try { fn(); print("[PASS] " + name); _p++; }
    catch(e) { print("[FAIL] " + name + " : " + e); _f++; }
}

function _eq(a, b) {
    if (a !== b) throw new Error("expected |" + b + "| got |" + a + "|");
}

function _ok(v, msg) {
    if (!v) throw new Error(msg || "assertion failed");
}

function _skip(name, reason) {
    print("[SKIP] " + name + " (" + reason + ")");
}

print("=== " + _s + " ===");

/* Function.prototype.call */
_t("call sets this context", function() {
    function greet() { return "Hello, " + this.name; }
    var result = greet.call({ name: "World" });
    _eq(result, "Hello, World");
});

_t("call passes arguments", function() {
    function add(a, b) { return a + b; }
    _eq(add.call(null, 3, 4), 7);
});

_t("call with method borrowing", function() {
    var obj1 = { val: 10, double: function() { return this.val * 2; } };
    var obj2 = { val: 5 };
    _eq(obj1.double.call(obj2), 10);
});

/* Function.prototype.apply */
_t("apply sets this context", function() {
    function sum() {
        var total = 0;
        for (var i = 0; i < arguments.length; i++) total += arguments[i];
        return total;
    }
    _eq(sum.apply(null, [1, 2, 3, 4]), 10);
});

_t("apply with Math.max", function() {
    var nums = [3, 1, 4, 1, 5, 9, 2, 6];
    _eq(Math.max.apply(null, nums), 9);
});

_t("apply spreads array as arguments", function() {
    function three(a, b, c) { return a + "-" + b + "-" + c; }
    _eq(three.apply(null, ["x", "y", "z"]), "x-y-z");
});

/* Function.prototype.bind */
_t("bind creates new function with fixed this", function() {
    function getVal() { return this.val; }
    var bound = getVal.bind({ val: 42 });
    _eq(bound(), 42);
});

_t("bind preserves fixed this regardless of call context", function() {
    var obj = { val: 100 };
    function getVal() { return this.val; }
    var bound = getVal.bind(obj);
    _eq(bound.call({ val: 999 }), 100);
});

_t("bind with partial application", function() {
    function multiply(a, b) { return a * b; }
    var double = multiply.bind(null, 2);
    _eq(double(5), 10);
    _eq(double(7), 14);
});

_t("bind partial application multiple args", function() {
    function add3(a, b, c) { return a + b + c; }
    var add10and20 = add3.bind(null, 10, 20);
    _eq(add10and20(5), 35);
});

/* arguments object */
_t("arguments contains all passed values", function() {
    function sum() {
        var total = 0;
        for (var i = 0; i < arguments.length; i++) total += arguments[i];
        return total;
    }
    _eq(sum(1, 2, 3), 6);
    _eq(sum(10, 20), 30);
});

_t("arguments.length reflects actual args", function() {
    function count() { return arguments.length; }
    _eq(count(), 0);
    _eq(count(1, 2, 3), 3);
});

_t("arguments accessible by index", function() {
    function first() { return arguments[0]; }
    _eq(first("hello", "world"), "hello");
});

/* Named function expressions */
_t("named function expression can recurse by name", function() {
    var factorial = function fact(n) {
        return n <= 1 ? 1 : n * fact(n - 1);
    };
    _eq(factorial(5), 120);
});

_t("named function expression name not in outer scope", function() {
    var fn = function inner() { return 1; };
    _ok(typeof inner === "undefined");
});

/* IIFE (Immediately Invoked Function Expression) */
_t("IIFE executes immediately", function() {
    var result = (function() { return 42; })();
    _eq(result, 42);
});

_t("IIFE creates private scope", function() {
    var x = 10;
    (function() {
        var x = 99;
        _eq(x, 99);
    })();
    _eq(x, 10);
});

/* Strict mode */
_t("strict mode: this is undefined in plain function", function() {
    var self;
    (function() {
        "use strict";
        self = this;
    })();
    _ok(self === undefined, "this should be undefined in strict function");
});

_skip("strict mode: duplicate param names are syntax error", "eval not supported in mquickjs");
_skip("strict mode: delete non-configurable throws", "eval not supported in mquickjs");
_skip("strict mode: with statement is syntax error", "eval not supported in mquickjs");

/* Function length property */
_t("function.length reflects parameter count", function() {
    function zero() {}
    function one(a) {}
    function two(a, b) {}
    _eq(zero.length, 0);
    _eq(one.length, 1);
    _eq(two.length, 2);
});

/* Function.prototype.toString */
_t("typeof function is 'function'", function() {
    _eq(typeof function(){}, "function");
    _eq(typeof Array.isArray, "function");
});

print("--- " + _s + ": " + _p + " passed, " + _f + " failed ---");
