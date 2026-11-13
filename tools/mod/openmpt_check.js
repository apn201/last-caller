// Loads modules in the card's own libopenmpt build (the one the browser player uses)
// and prints type, channels, orders, duration and how loud they render.
//   node tools/mod/openmpt_check.js file.mod [file.mod ...]
const fs = require('fs');
const path = require('path');
const dir = path.join(__dirname, '..', '..', 'sd', 'BBS', 'ASSETS', 'PLAYER');
const wasm = fs.readFileSync(path.join(dir, 'libopenmpt.wasm'));
global.libopenmpt = {
  wasmBinary: wasm,
  locateFile: f => path.join(dir, f),
  onRuntimeInitialized() { run(global.libopenmpt); },
};
// A plain script, as in the browser: its top-level names (HEAPU8) become globals.
global.require = require;
global.__dirname = dir;
require('vm').runInThisContext(fs.readFileSync(path.join(dir, 'libopenmpt.js'), 'utf8'), { filename: 'libopenmpt.js' });

function run(lib) {
  let bad = 0;
  for (const file of process.argv.slice(2)) {
    const data = fs.readFileSync(file);
    const p = lib._malloc(data.length);
    global.HEAPU8.set(data, p);
    const mod = lib._openmpt_module_create_from_memory2(p, data.length, 0, 0, 0, 0, 0, 0, 0);
    lib._free(p);
    if (!mod) { console.log(path.basename(file) + ': CANNOT LOAD'); bad++; continue; }
    lib._openmpt_module_set_repeat_count(mod, 0);
    const frames = 4096, l = lib._malloc(frames * 4), r = lib._malloc(frames * 4);
    let total = 0, peak = 0, sumsq = 0;
    for (;;) {
      const got = lib._openmpt_module_read_float_stereo(mod, 44100, frames, l, r);
      if (!got) break;
      const a = new Float32Array(global.HEAPU8.buffer, l, got);
      for (const v of a) { peak = Math.max(peak, Math.abs(v)); sumsq += v * v; }
      total += got;
      if (total > 44100 * 600) break;
    }
    const secs = (total / 44100).toFixed(1);
    const rms = Math.sqrt(sumsq / Math.max(1, total)).toFixed(3);
    console.log(path.basename(file) + ': ' + lib._openmpt_module_get_num_channels(mod) + ' channels, ' +
      lib._openmpt_module_get_num_orders(mod) + ' orders, ' + lib._openmpt_module_get_num_patterns(mod) + ' patterns, ' +
      secs + ' s, peak ' + peak.toFixed(2) + ', rms ' + rms);
    if (!(total > 44100 * 10) || peak > 1.0) bad++;
    lib._openmpt_module_destroy(mod);
  }
  process.exit(bad ? 1 : 0);
}
