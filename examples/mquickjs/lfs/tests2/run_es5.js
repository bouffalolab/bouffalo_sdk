/* ES5 Full Test Suite Runner
 * Loads all ES5 test modules in sequence.
 * Each module prints its own [PASS]/[FAIL] lines and a per-module summary.
 *
 * Usage on device:
 *   js_run /lfs/tests2/run_es5.js
 *
 * Or run a single module:
 *   js_run /lfs/tests2/test_es5_array.js
 */

print("========================================");
print("        ES5 Full Test Suite");
print("========================================");

load("/lfs/tests2/test_es5_array.js");
load("/lfs/tests2/test_es5_object.js");
load("/lfs/tests2/test_es5_string.js");
load("/lfs/tests2/test_es5_function.js");
load("/lfs/tests2/test_es5_json.js");

print("========================================");
print("        All modules completed");
print("========================================");
