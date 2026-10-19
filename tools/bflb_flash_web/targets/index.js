/* Target registry, mirroring bflb_flash/targets/__init__.py. */
(function (root, factory) {
  "use strict";
  const isNode = typeof module !== "undefined" && module.exports;
  const bl602 = isNode ? require("./bl602.js") : root.BflbFlashTargetBL602;
  const bl616 = isNode ? require("./bl616.js") : root.BflbFlashTargetBL616;
  const bl616cl = isNode ? require("./bl616cl.js") : root.BflbFlashTargetBL616CL;
  const api = factory(bl602, bl616, bl616cl);
  root.BflbFlashTargets = api;
  if (isNode) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window, function (bl602, bl616, bl616cl) {
  "use strict";
  const CHIP_DEFS = Object.freeze({ bl602, bl616, bl616cl });
  return Object.freeze({ BL602: bl602, BL616: bl616, BL616CL: bl616cl, CHIP_DEFS });
});
