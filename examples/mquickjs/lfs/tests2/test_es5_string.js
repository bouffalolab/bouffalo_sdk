/* ES5 String Methods Test
 * Tests: trim, split (regex), replace (function), match, search,
 *        charAt, charCodeAt, String.fromCharCode, slice, substring,
 *        indexOf, lastIndexOf, toUpperCase, toLowerCase
 */

var _p = 0, _f = 0, _s = "ES5 String Methods";

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

/* trim */
_t("trim removes leading and trailing spaces", function() {
    _eq("  hello  ".trim(), "hello");
});

_t("trim removes tabs and newlines", function() {
    _eq("\t hello \n".trim(), "hello");
});

_t("trim on string with no whitespace", function() {
    _eq("hello".trim(), "hello");
});

_t("trim on empty string", function() {
    _eq("   ".trim(), "");
});

/* split with regex */
_t("split with string delimiter", function() {
    var parts = "a,b,c".split(",");
    _eq(parts.length, 3);
    _eq(parts[0], "a");
    _eq(parts[2], "c");
});

_t("split with regex delimiter", function() {
    var parts = "one1two2three".split(/\d/);
    _eq(parts.length, 3);
    _eq(parts[0], "one");
    _eq(parts[1], "two");
    _eq(parts[2], "three");
});

_t("split with limit", function() {
    var parts = "a,b,c,d".split(",", 2);
    _eq(parts.length, 2);
    _eq(parts[1], "b");
});

_t("split empty string", function() {
    var parts = "abc".split("");
    _eq(parts.length, 3);
    _eq(parts[0], "a");
});

/* replace with string */
_t("replace first occurrence", function() {
    _eq("aabbcc".replace("b", "X"), "aaXbcc");
});

_t("replace with regex global flag", function() {
    _eq("aabbcc".replace(/b/g, "X"), "aaXXcc");
});

_t("replace with function", function() {
    var result = "hello world".replace(/\w+/g, function(match) {
        return match.toUpperCase();
    });
    _eq(result, "HELLO WORLD");
});

_t("replace function receives match and index", function() {
    var indices = [];
    "ab cd".replace(/\w+/g, function(m, offset) {
        indices.push(offset);
        return m;
    });
    _eq(indices[0], 0);
    _eq(indices[1], 3);
});

/* match */
_t("match with global regex returns all", function() {
    var m = "test 1 and 2 and 3".match(/\d/g);
    _eq(m.length, 3);
    _eq(m[0], "1");
    _eq(m[2], "3");
});

_t("match without global returns first with groups", function() {
    var m = "2026-03-01".match(/(\d{4})-(\d{2})-(\d{2})/);
    _ok(m !== null);
    _eq(m[1], "2026");
    _eq(m[2], "03");
    _eq(m[3], "01");
});

_t("match returns null when no match", function() {
    var m = "hello".match(/\d+/);
    _ok(m === null);
});

/* search */
_t("search returns index of first match", function() {
    _eq("hello world".search(/world/), 6);
});

_t("search returns -1 when no match", function() {
    _eq("hello".search(/\d/), -1);
});

/* charAt / charCodeAt */
_t("charAt returns character at index", function() {
    _eq("hello".charAt(1), "e");
    _eq("hello".charAt(0), "h");
});

_t("charAt out of bounds returns empty string", function() {
    _eq("hi".charAt(99), "");
});

_t("charCodeAt returns numeric code", function() {
    _eq("A".charCodeAt(0), 65);
    _eq("a".charCodeAt(0), 97);
});

_t("charCodeAt out of bounds returns NaN", function() {
    _ok(isNaN("hi".charCodeAt(99)));
});

/* String.fromCharCode */
_t("fromCharCode creates string from codes", function() {
    _eq(String.fromCharCode(72, 101, 108, 108, 111), "Hello");
});

_t("fromCharCode single code", function() {
    _eq(String.fromCharCode(65), "A");
});

/* slice */
_t("slice extracts substring", function() {
    _eq("hello world".slice(6), "world");
    _eq("hello world".slice(0, 5), "hello");
});

_t("slice with negative index", function() {
    _eq("hello".slice(-3), "llo");
});

/* substring */
_t("substring extracts between indices", function() {
    _eq("hello world".substring(6, 11), "world");
});

_t("substring swaps arguments when start > end", function() {
    _eq("hello".substring(3, 1), "el");
});

/* indexOf / lastIndexOf on string */
_t("string indexOf finds substring", function() {
    _eq("hello world".indexOf("world"), 6);
    _eq("hello world".indexOf("xyz"), -1);
});

_t("string indexOf with fromIndex", function() {
    _eq("abcabc".indexOf("b", 3), 4);
});

_t("string lastIndexOf finds last occurrence", function() {
    _eq("abcabc".lastIndexOf("b"), 4);
});

/* case conversion */
_t("toUpperCase", function() {
    _eq("hello".toUpperCase(), "HELLO");
});

_t("toLowerCase", function() {
    _eq("WORLD".toLowerCase(), "world");
});

/* String as array-like */
_t("string length property", function() {
    _eq("hello".length, 5);
    _eq("".length, 0);
});

_t("bracket notation access", function() {
    _eq("abc"[0], "a");
    _eq("abc"[2], "c");
});

print("--- " + _s + ": " + _p + " passed, " + _f + " failed ---");
