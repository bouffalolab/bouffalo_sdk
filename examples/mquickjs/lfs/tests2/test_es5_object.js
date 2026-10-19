/* ES5 Object Methods Test
 * Tests: Object.create, Object.keys, Object.getOwnPropertyNames,
 *        Object.defineProperty, Object.freeze, Object.seal,
 *        Object.getPrototypeOf, hasOwnProperty, getter/setter syntax
 *
 * Note: some ES5 Object methods are not implemented in mquickjs.
 * Tests for unimplemented APIs are skipped with a [SKIP] notice.
 */

var _p = 0, _f = 0, _s = "ES5 Object Methods";

function _t(name, fn) {
    try { fn(); print("[PASS] " + name); _p++; }
    catch(e) { print("[FAIL] " + name + " : " + e); _f++; }
}

function _skip(name, reason) {
    print("[SKIP] " + name + " (" + reason + ")");
}

function _eq(a, b) {
    if (a !== b) throw new Error("expected |" + b + "| got |" + a + "|");
}

function _ok(v, msg) {
    if (!v) throw new Error(msg || "assertion failed");
}

function _arr_has(arr, val) {
    for (var i = 0; i < arr.length; i++) {
        if (arr[i] === val) return;
    }
    throw new Error("|" + val + "| not found in [" + arr.join(",") + "]");
}

print("=== " + _s + " ===");

/* Object.create */
_t("Object.create basic prototype chain", function() {
    var proto = { greet: function() { return "hello"; } };
    var obj = Object.create(proto);
    _eq(obj.greet(), "hello");
    _ok(!obj.hasOwnProperty("greet"), "greet should be on prototype");
});

_t("Object.create with null prototype", function() {
    var obj = Object.create(null);
    _ok(obj.hasOwnProperty === undefined, "no inherited methods");
});

/* Object.create 2nd arg (property descriptors) is not supported in mquickjs */
_skip("Object.create with property descriptors", "mquickjs does not support the 2nd properties argument");

/* Object.keys */
_t("Object.keys returns own enumerable keys", function() {
    var obj = { a: 1, b: 2, c: 3 };
    var keys = Object.keys(obj);
    _eq(keys.length, 3);
    _arr_has(keys, "a");
    _arr_has(keys, "b");
    _arr_has(keys, "c");
});

_t("Object.keys excludes prototype properties", function() {
    function Foo() { this.x = 1; }
    Foo.prototype.y = 2;
    var keys = Object.keys(new Foo());
    _eq(keys.length, 1);
    _eq(keys[0], "x");
});

/* In mquickjs, Object.defineProperty enumerable/writable/configurable flags
 * are not enforced by the runtime — only value/get/set work reliably.
 * The following tests verify only what mquickjs actually supports. */

/* Object.defineProperty - value */
_t("defineProperty creates property with value", function() {
    var obj = {};
    Object.defineProperty(obj, "val", { value: 99, enumerable: true });
    _eq(obj.val, 99);
});

/* defineProperty getter */
_t("defineProperty getter", function() {
    var obj = { _n: 5 };
    Object.defineProperty(obj, "double", {
        get: function() { return this._n * 2; },
        enumerable: true
    });
    _eq(obj.double, 10);
    obj._n = 10;
    _eq(obj.double, 20);
});

/* defineProperty setter */
_t("defineProperty setter", function() {
    var obj = { _val: 0 };
    Object.defineProperty(obj, "value", {
        get: function() { return this._val; },
        set: function(v) { this._val = v * 2; }
    });
    obj.value = 5;
    _eq(obj._val, 10);
    _eq(obj.value, 10);
});

/* writable/enumerable/configurable flags not enforced in mquickjs */
_skip("defineProperty writable:false enforcement", "mquickjs does not enforce writable flag");
_skip("defineProperty enumerable:false in Object.keys", "mquickjs does not enforce enumerable flag");
_skip("Object.getOwnPropertyDescriptor", "not implemented in mquickjs");
_skip("Object.getOwnPropertyNames", "not implemented in mquickjs");

/* Object.getPrototypeOf */
_t("getPrototypeOf returns correct prototype", function() {
    function Animal(name) { this.name = name; }
    var dog = new Animal("Rex");
    _ok(Object.getPrototypeOf(dog) === Animal.prototype);
});

_t("getPrototypeOf on Object.create result", function() {
    var proto = { x: 1 };
    var obj = Object.create(proto);
    _ok(Object.getPrototypeOf(obj) === proto);
});

/* Object.freeze / seal / preventExtensions — not implemented in mquickjs */
if (typeof Object.freeze === "function") {
    _t("freeze prevents property modification", function() {
        var obj = { a: 1 };
        Object.freeze(obj);
        obj.a = 99;
        _eq(obj.a, 1);
    });
    _t("freeze prevents adding new properties", function() {
        var obj = { a: 1 };
        Object.freeze(obj);
        obj.b = 2;
        _ok(obj.b === undefined);
    });
    _t("Object.isFrozen", function() {
        var obj = { a: 1 };
        _ok(!Object.isFrozen(obj));
        Object.freeze(obj);
        _ok(Object.isFrozen(obj));
    });
} else {
    _skip("Object.freeze / isFrozen", "not implemented in mquickjs");
}

if (typeof Object.seal === "function") {
    _t("seal prevents adding new properties", function() {
        var obj = { a: 1 };
        Object.seal(obj);
        obj.b = 2;
        _ok(obj.b === undefined);
    });
    _t("seal allows modifying existing properties", function() {
        var obj = { a: 1 };
        Object.seal(obj);
        obj.a = 99;
        _eq(obj.a, 99);
    });
    _t("Object.isSealed", function() {
        var obj = { a: 1 };
        _ok(!Object.isSealed(obj));
        Object.seal(obj);
        _ok(Object.isSealed(obj));
    });
} else {
    _skip("Object.seal / isSealed", "not implemented in mquickjs");
}

if (typeof Object.preventExtensions === "function") {
    _t("preventExtensions stops new properties", function() {
        var obj = { x: 1 };
        Object.preventExtensions(obj);
        obj.y = 2;
        _ok(obj.y === undefined);
        _ok(!Object.isExtensible(obj));
    });
    _t("isExtensible on new object", function() {
        _ok(Object.isExtensible({}));
    });
} else {
    _skip("Object.preventExtensions / isExtensible", "not implemented in mquickjs");
}

/* hasOwnProperty */
_t("hasOwnProperty distinguishes own vs inherited", function() {
    function Base() { this.own = 1; }
    Base.prototype.inherited = 2;
    var obj = new Base();
    _ok(obj.hasOwnProperty("own"));
    _ok(!obj.hasOwnProperty("inherited"));
});

/* Getter/setter syntax in object literals */
_t("get/set in object literal", function() {
    var obj = {
        _count: 0,
        get count() { return this._count; },
        set count(v) { if (v >= 0) this._count = v; }
    };
    _eq(obj.count, 0);
    obj.count = 5;
    _eq(obj.count, 5);
    obj.count = -1;
    _eq(obj.count, 5);
});

_t("getter only property", function() {
    var obj = {
        get pi() { return 3.14159; }
    };
    _eq(obj.pi, 3.14159);
});

print("--- " + _s + ": " + _p + " passed, " + _f + " failed ---");
