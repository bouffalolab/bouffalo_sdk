/* ES5 Array Methods Test
 * Tests: isArray, forEach, map, filter, reduce, reduceRight,
 *        indexOf, lastIndexOf, every, some
 */

var _p = 0, _f = 0, _s = "ES5 Array Methods";

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

function _arr_eq(a, b) {
    if (a.length !== b.length) throw new Error("length mismatch: " + a.length + " vs " + b.length);
    for (var i = 0; i < a.length; i++) {
        if (a[i] !== b[i]) throw new Error("index " + i + ": expected |" + b[i] + "| got |" + a[i] + "|");
    }
}

print("=== " + _s + " ===");

/* Array.isArray */
_t("Array.isArray with array", function() {
    _ok(Array.isArray([]), "[] should be array");
    _ok(Array.isArray([1, 2, 3]), "[1,2,3] should be array");
});

_t("Array.isArray with non-array", function() {
    _ok(!Array.isArray({}), "{} should not be array");
    _ok(!Array.isArray("str"), "string should not be array");
    _ok(!Array.isArray(null), "null should not be array");
    _ok(!Array.isArray(42), "number should not be array");
});

/* forEach */
_t("forEach iterates all elements", function() {
    var sum = 0;
    [1, 2, 3, 4].forEach(function(v) { sum += v; });
    _eq(sum, 10);
});

_t("forEach provides index and array", function() {
    var indices = [];
    ["a", "b", "c"].forEach(function(v, i) { indices.push(i); });
    _arr_eq(indices, [0, 1, 2]);
});

_t("forEach with thisArg", function() {
    var obj = { total: 0 };
    [10, 20].forEach(function(v) { this.total += v; }, obj);
    _eq(obj.total, 30);
});

/* map */
_t("map returns new array", function() {
    var result = [1, 2, 3].map(function(v) { return v * 2; });
    _arr_eq(result, [2, 4, 6]);
});

_t("map does not modify original", function() {
    var orig = [1, 2, 3];
    orig.map(function(v) { return v * 10; });
    _arr_eq(orig, [1, 2, 3]);
});

_t("map with index", function() {
    var result = ["a", "b", "c"].map(function(v, i) { return i + ":" + v; });
    _arr_eq(result, ["0:a", "1:b", "2:c"]);
});

/* filter */
_t("filter returns matching elements", function() {
    var evens = [1, 2, 3, 4, 5, 6].filter(function(v) { return v % 2 === 0; });
    _arr_eq(evens, [2, 4, 6]);
});

_t("filter with no matches returns empty", function() {
    var result = [1, 3, 5].filter(function(v) { return v % 2 === 0; });
    _eq(result.length, 0);
});

_t("filter does not modify original", function() {
    var orig = [1, 2, 3, 4];
    orig.filter(function(v) { return v > 2; });
    _eq(orig.length, 4);
});

/* reduce */
_t("reduce sums values", function() {
    var sum = [1, 2, 3, 4].reduce(function(acc, v) { return acc + v; }, 0);
    _eq(sum, 10);
});

_t("reduce with no initial value", function() {
    var product = [2, 3, 4].reduce(function(acc, v) { return acc * v; });
    _eq(product, 24);
});

_t("reduce builds object", function() {
    var counts = ["a", "b", "a", "c", "b", "a"].reduce(function(acc, v) {
        acc[v] = (acc[v] || 0) + 1;
        return acc;
    }, {});
    _eq(counts["a"], 3);
    _eq(counts["b"], 2);
    _eq(counts["c"], 1);
});

/* reduceRight */
_t("reduceRight processes right to left", function() {
    var result = [[1, 2], [3, 4], [5, 6]].reduceRight(function(acc, v) {
        return acc.concat(v);
    }, []);
    _arr_eq(result, [5, 6, 3, 4, 1, 2]);
});

_t("reduceRight string concatenation order", function() {
    var result = ["a", "b", "c"].reduceRight(function(acc, v) { return acc + v; }, "");
    _eq(result, "cba");
});

/* indexOf */
_t("indexOf finds element", function() {
    _eq([10, 20, 30, 20].indexOf(20), 1);
});

_t("indexOf returns -1 when not found", function() {
    _eq([1, 2, 3].indexOf(99), -1);
});

_t("indexOf with fromIndex", function() {
    _eq([10, 20, 30, 20].indexOf(20, 2), 3);
});

_t("indexOf uses strict equality", function() {
    _eq([1, 2, "3"].indexOf(3), -1);
    _eq([1, 2, "3"].indexOf("3"), 2);
});

/* lastIndexOf */
_t("lastIndexOf finds last occurrence", function() {
    _eq([1, 2, 3, 2, 1].lastIndexOf(2), 3);
});

_t("lastIndexOf returns -1 when not found", function() {
    _eq([1, 2, 3].lastIndexOf(99), -1);
});

_t("lastIndexOf with fromIndex", function() {
    _eq([1, 2, 3, 2, 1].lastIndexOf(2, 2), 1);
});

/* every */
_t("every returns true when all match", function() {
    _ok([2, 4, 6].every(function(v) { return v % 2 === 0; }));
});

_t("every returns false when one fails", function() {
    _ok(![1, 2, 3].every(function(v) { return v % 2 === 0; }));
});

_t("every on empty array returns true", function() {
    _ok([].every(function() { return false; }));
});

/* some */
_t("some returns true when one matches", function() {
    _ok([1, 3, 4].some(function(v) { return v % 2 === 0; }));
});

_t("some returns false when none match", function() {
    _ok(![1, 3, 5].some(function(v) { return v % 2 === 0; }));
});

_t("some on empty array returns false", function() {
    _ok(![].some(function() { return true; }));
});

print("--- " + _s + ": " + _p + " passed, " + _f + " failed ---");
