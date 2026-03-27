/* ES5 JSON Test
 * Tests: JSON.stringify, JSON.parse,
 *        reviver function, replacer function/array,
 *        space parameter, nested objects/arrays,
 *        edge cases (null, undefined, numbers, booleans)
 */

var _p = 0, _f = 0, _s = "ES5 JSON";

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

print("=== " + _s + " ===");

/* JSON.stringify - primitives */
_t("stringify number", function() {
    _eq(JSON.stringify(42), "42");
    _eq(JSON.stringify(-3.14), "-3.14");
});

_t("stringify string", function() {
    _eq(JSON.stringify("hello"), '"hello"');
});

_t("stringify boolean", function() {
    _eq(JSON.stringify(true), "true");
    _eq(JSON.stringify(false), "false");
});

_t("stringify null", function() {
    _eq(JSON.stringify(null), "null");
});

_t("stringify undefined returns undefined (not string)", function() {
    _ok(JSON.stringify(undefined) === undefined);
});

/* JSON.stringify - objects */
_t("stringify simple object", function() {
    var s = JSON.stringify({ a: 1, b: 2 });
    _ok(s.indexOf('"a":1') !== -1);
    _ok(s.indexOf('"b":2') !== -1);
});

_t("stringify array", function() {
    _eq(JSON.stringify([1, 2, 3]), "[1,2,3]");
});

_t("stringify nested object", function() {
    var obj = { name: "test", data: { x: 1, y: 2 } };
    var s = JSON.stringify(obj);
    _ok(s.indexOf('"name":"test"') !== -1);
    _ok(s.indexOf('"data"') !== -1);
});

_t("stringify nested array", function() {
    var s = JSON.stringify([[1, 2], [3, 4]]);
    _eq(s, "[[1,2],[3,4]]");
});

_t("stringify mixed nested", function() {
    var obj = { arr: [1, { n: 2 }, 3], flag: true };
    var s = JSON.stringify(obj);
    _ok(s.indexOf('"arr"') !== -1);
    _ok(s.indexOf('"flag":true') !== -1);
});

_t("stringify omits undefined values in objects", function() {
    var obj = { a: 1, b: undefined, c: 3 };
    var s = JSON.stringify(obj);
    _ok(s.indexOf('"b"') === -1);
    _ok(s.indexOf('"a":1') !== -1);
});

_t("stringify replaces undefined in arrays with null", function() {
    var s = JSON.stringify([1, undefined, 3]);
    _eq(s, "[1,null,3]");
});

/* JSON.stringify - space parameter */
_t("stringify with numeric space", function() {
    var s = JSON.stringify({ a: 1 }, null, 2);
    _ok(s.indexOf("\n") !== -1, "should have newline");
    _ok(s.indexOf("  ") !== -1, "should have 2-space indent");
});

_t("stringify with string space", function() {
    var s = JSON.stringify({ x: 1 }, null, "\t");
    _ok(s.indexOf("\t") !== -1, "should have tab indent");
});

/* JSON.stringify - replacer array */
_t("stringify with replacer array filters keys", function() {
    var obj = { a: 1, b: 2, c: 3 };
    var s = JSON.stringify(obj, ["a", "c"]);
    _ok(s.indexOf('"a":1') !== -1);
    _ok(s.indexOf('"b"') === -1);
    _ok(s.indexOf('"c":3') !== -1);
});

/* JSON.stringify - replacer function */
_t("stringify with replacer function transforms values", function() {
    var obj = { a: 1, b: 2, c: 3 };
    var s = JSON.stringify(obj, function(key, val) {
        if (key === "b") return undefined;
        return val;
    });
    _ok(s.indexOf('"a":1') !== -1);
    _ok(s.indexOf('"b"') === -1);
});

_t("stringify replacer function receives key and value", function() {
    var keys = [];
    JSON.stringify({ x: 1, y: 2 }, function(k, v) {
        if (k !== "") keys.push(k);
        return v;
    });
    _ok(keys.indexOf("x") !== -1);
    _ok(keys.indexOf("y") !== -1);
});

/* JSON.parse - primitives */
_t("parse number", function() {
    _eq(JSON.parse("42"), 42);
    _eq(JSON.parse("-3.14"), -3.14);
});

_t("parse string", function() {
    _eq(JSON.parse('"hello"'), "hello");
});

_t("parse boolean", function() {
    _ok(JSON.parse("true") === true);
    _ok(JSON.parse("false") === false);
});

_t("parse null", function() {
    _ok(JSON.parse("null") === null);
});

/* JSON.parse - objects */
_t("parse simple object", function() {
    var obj = JSON.parse('{"a":1,"b":2}');
    _eq(obj.a, 1);
    _eq(obj.b, 2);
});

_t("parse array", function() {
    var arr = JSON.parse("[1,2,3]");
    _eq(arr.length, 3);
    _eq(arr[1], 2);
});

_t("parse nested object", function() {
    var obj = JSON.parse('{"name":"test","data":{"x":1}}');
    _eq(obj.name, "test");
    _eq(obj.data.x, 1);
});

_t("parse whitespace in JSON", function() {
    var obj = JSON.parse('{ "a" : 1 , "b" : 2 }');
    _eq(obj.a, 1);
    _eq(obj.b, 2);
});

/* JSON.parse - reviver function */
_t("parse with reviver transforms values", function() {
    var obj = JSON.parse('{"a":1,"b":2}', function(key, val) {
        if (typeof val === "number") return val * 10;
        return val;
    });
    _eq(obj.a, 10);
    _eq(obj.b, 20);
});

_t("parse reviver can exclude keys by returning undefined", function() {
    var obj = JSON.parse('{"a":1,"b":2,"c":3}', function(key, val) {
        if (key === "b") return undefined;
        return val;
    });
    _ok(obj.a === 1);
    _ok(obj.b === undefined);
    _ok(obj.c === 3);
});

/* Round-trip */
_t("stringify then parse round-trip", function() {
    var original = { name: "chip", id: 616, active: true, data: [1, 2, 3] };
    var parsed = JSON.parse(JSON.stringify(original));
    _eq(parsed.name, original.name);
    _eq(parsed.id, original.id);
    _ok(parsed.active === true);
    _eq(parsed.data.length, 3);
    _eq(parsed.data[2], 3);
});

/* JSON.parse - error handling */
_t("parse invalid JSON throws SyntaxError", function() {
    var threw = false;
    try {
        JSON.parse("{invalid}");
    } catch(e) {
        threw = true;
        _ok(e instanceof SyntaxError || typeof e === "object");
    }
    _ok(threw, "should throw on invalid JSON");
});

print("--- " + _s + ": " + _p + " passed, " + _f + " failed ---");
