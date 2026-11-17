// Ty's demo (slice GE2): a 1998 text-mode tribute to Future Crew's Second Reality (1993), the easter egg a
// caller finds in Ty's Gallery after the ending. 80 x 25 character cells, sixteen colours, shade blocks and
// half blocks, the way an ANSI screen draws: every effect here is drawn in cells, nothing finer.
// The timeline and every word are in /BBS/DEMO/SR.INI (fetched as /demo/SR.INI). The music is the original
// MUSIC0.S3M of Second Reality (Purple Motion, Skaven), public domain since the 2013 source and data release;
// it plays through libopenmpt (BSD), already on the card for the music player (/player/libopenmpt.*).
// The effects are our own code, after the parts of the original. Original work, MIT.
//
//   TyDemo.run({ onDone: fn })
//
// Q or ESC ends it, M mutes (localStorage 'tb.mute', the board's one sound switch). At the end any key.
(function () {
  'use strict';
  const COLS = 80, ROWS = 25, CW = 8, CH = 16, W = COLS * CW, H = ROWS * CH, PW = COLS, PH = ROWS * 2, BASE = '/demo/';
  const rgb = (r, g, b) => (255 << 24) | (b << 16) | (g << 8) | r;
  // the CGA/VGA text palette, 0..15
  const PAL = [rgb(0, 0, 0), rgb(0, 0, 170), rgb(0, 170, 0), rgb(0, 170, 170), rgb(170, 0, 0), rgb(170, 0, 170), rgb(170, 85, 0), rgb(170, 170, 170),
    rgb(85, 85, 85), rgb(85, 85, 255), rgb(85, 255, 85), rgb(85, 255, 255), rgb(255, 85, 85), rgb(255, 85, 255), rgb(255, 255, 85), rgb(255, 255, 255)];
  const clamp = (v, a, b) => Math.max(a, Math.min(b, v));
  const ease = t => { t = clamp(t, 0, 1); return t * t * (3 - 2 * t); };
  // special cells: shade blocks and half blocks
  const G_LIGHT = 1, G_MED = 2, G_DARK = 3, G_FULL = 4, G_UP = 5, G_DOWN = 6;

  let st = null;

  // ---------------------------------------------------------------- the INI
  function parseIni(text) {
    const out = {}; let sec = '';
    for (let line of text.split(/\r?\n/)) {
      line = line.trim();
      if (!line || line[0] === ';' || line[0] === '#') continue;
      if (line[0] === '[') { sec = line.slice(1, line.indexOf(']')).trim().toUpperCase(); out[sec] = out[sec] || {}; continue; }
      const eq = line.indexOf('=');
      if (eq > 0 && sec) out[sec][line.slice(0, eq).trim().toLowerCase()] = line.slice(eq + 1).trim();
    }
    return out;
  }
  const lines = s => (s || '').split('|');

  // ---------------------------------------------------------------- glyphs: 8 x 16 masks
  const glyphs = {};
  function glyph(code) {
    if (glyphs[code]) return glyphs[code];
    const m = new Uint8Array(CW * CH);
    if (code === G_FULL) m.fill(1);
    else if (code === G_UP) m.fill(1, 0, CW * CH / 2);
    else if (code === G_DOWN) m.fill(1, CW * CH / 2);
    else if (code === G_LIGHT || code === G_MED || code === G_DARK) {
      for (let y = 0; y < CH; y++) for (let x = 0; x < CW; x++) {
        const a = (x & 1) === 0, b = (y & 1) === 0, c = ((x >> 1) + y) & 1;
        m[y * CW + x] = code === G_LIGHT ? (a && b && c ? 1 : 0) : code === G_MED ? ((x + y) & 1) : (a && b && c ? 0 : 1);
      }
    } else if (code > 32) {
      const c = document.createElement('canvas'); c.width = CW; c.height = CH;
      const x = c.getContext('2d', { willReadFrequently: true });
      x.font = 'bold 14px "Courier New", monospace'; x.textBaseline = 'middle'; x.textAlign = 'center'; x.fillStyle = '#fff';
      x.fillText(String.fromCharCode(code), CW / 2, CH / 2 + 1);
      const d = x.getImageData(0, 0, CW, CH).data;
      for (let i = 0; i < CW * CH; i++) m[i] = d[i * 4 + 3] > 120 ? 1 : 0;
    }
    return (glyphs[code] = m);
  }

  // ---------------------------------------------------------------- the screen: cells, and a half-block pixel layer
  function clear(bg) { st.ch.fill(32); st.fg.fill(7); st.bg.fill(bg || 0); }
  function cell(x, y, code, fg, bg) {
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
    const i = y * COLS + x; st.ch[i] = code; st.fg[i] = fg; if (bg !== undefined && bg >= 0) st.bg[i] = bg;
  }
  function print(x, y, str, fg, bg) { for (let i = 0; i < str.length; i++) cell(x + i, y, str.charCodeAt(i), fg, bg); }
  function center(y, str, fg, bg) { print(Math.floor((COLS - str.length) / 2), y, str, fg, bg); }
  // a text line typed out at cps characters a second
  const typed = (str, p, cps) => str.slice(0, clamp(Math.floor(p * cps), 0, str.length));
  // pixels: 80 x 50, each an index 0..15; flush() turns pairs into half-block cells
  function pclear(c) { st.px.fill(c || 0); }
  function pset(x, y, c) { x |= 0; y |= 0; if (x >= 0 && x < PW && y >= 0 && y < PH) st.px[y * PW + x] = c; }
  function flush(rowFrom, rowTo) {
    rowFrom = rowFrom || 0; rowTo = rowTo === undefined ? ROWS : rowTo;
    for (let y = rowFrom; y < rowTo; y++) for (let x = 0; x < COLS; x++) {
      const top = st.px[(y * 2) * PW + x], bot = st.px[(y * 2 + 1) * PW + x], i = y * COLS + x;
      if (top === bot) { st.ch[i] = 32; st.bg[i] = top; st.fg[i] = top; }
      else { st.ch[i] = G_UP; st.fg[i] = top; st.bg[i] = bot; }
    }
  }
  function draw() {
    const fb = st.fb;
    for (let cy = 0; cy < ROWS; cy++) for (let cx = 0; cx < COLS; cx++) {
      const i = cy * COLS + cx, m = glyph(st.ch[i]), f = PAL[st.fg[i] & 15], b = PAL[st.bg[i] & 15];
      let o = cy * CH * W + cx * CW;
      for (let y = 0, k = 0; y < CH; y++, o += W) for (let x = 0; x < CW; x++, k++) fb[o + x] = m[k] ? f : b;
    }
    st.ctx2d.putImageData(st.image, 0, 0);
  }
  // big letters in the pixel layer: the browser's own bold font, cut to pixels
  const bigCache = {};
  function bigMask(str, px) {
    const key = px + '|' + str; if (bigCache[key]) return bigCache[key];
    const c = document.createElement('canvas'), x = c.getContext('2d', { willReadFrequently: true });
    const font = 'bold ' + px + 'px "Arial Black", Arial, sans-serif';
    x.font = font; const w = Math.ceil(x.measureText(str).width) + 2, h = Math.ceil(px * 1.2);
    c.width = w; c.height = h; x.font = font; x.textBaseline = 'top'; x.fillStyle = '#fff'; x.fillText(str, 1, 0);
    const d = x.getImageData(0, 0, w, h).data, m = new Uint8Array(w * h);
    for (let i = 0; i < w * h; i++) m[i] = d[i * 4 + 3] > 100 ? 1 : 0;
    return (bigCache[key] = { w, h, m });
  }
  function big(str, y, colFn, px) {
    const mk = bigMask(str, px || 12), x0 = Math.floor((PW - mk.w) / 2);
    for (let j = 0; j < mk.h; j++) for (let i = 0; i < mk.w; i++) if (mk.m[j * mk.w + i]) pset(x0 + i, y + j, colFn(i, j));
  }

  // ---------------------------------------------------------------- the parts
  // a ramp of shade cells: [bg, fg, glyph], dark to light
  const RAMP_BLUE = [[0, 1, G_LIGHT], [0, 1, G_MED], [0, 1, G_DARK], [1, 9, G_LIGHT], [1, 9, G_MED], [1, 9, G_DARK], [9, 11, G_LIGHT], [9, 11, G_MED], [9, 11, G_DARK], [11, 15, G_LIGHT], [11, 15, G_MED], [11, 15, G_DARK]];
  const RAMP_FIRE = [[0, 4, G_LIGHT], [0, 4, G_MED], [4, 12, G_LIGHT], [4, 12, G_MED], [4, 12, G_DARK], [12, 6, G_MED], [6, 14, G_LIGHT], [6, 14, G_MED], [6, 14, G_DARK], [14, 15, G_MED], [14, 15, G_DARK], [15, 15, G_FULL]];
  const RAMP_VIOLET = [[0, 5, G_LIGHT], [0, 5, G_MED], [5, 13, G_LIGHT], [5, 13, G_MED], [5, 13, G_DARK], [13, 15, G_LIGHT], [13, 15, G_MED], [13, 11, G_MED], [11, 3, G_DARK], [3, 1, G_MED], [1, 0, G_MED], [0, 1, G_LIGHT]];
  function shade(x, y, v, ramp) {
    const r = ramp[clamp(Math.floor(v * ramp.length), 0, ramp.length - 1)];
    cell(x, y, r[2], r[1], r[0]);
  }
  // a fade of the whole screen to black at the start and end of a part
  function fadeEdges(p, d) {
    const k = Math.min(ease(p / 0.8), ease((d - p) / 0.8));
    if (k >= 1) return;
    const dim = [0, 0, 0, 1, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7];
    const steps = k < 0.33 ? 2 : k < 0.66 ? 1 : 0;
    for (let i = 0; i < st.fg.length; i++) for (let s = 0; s < steps; s++) { st.fg[i] = dim[st.fg[i]]; st.bg[i] = dim[st.bg[i]]; }
    if (k <= 0.05) { st.fg.fill(0); st.bg.fill(0); }
  }
  function caption(part, p) {   // Ty's line at the bottom, typed
    const t = part.text || ''; if (!t) return;
    for (let x = 0; x < COLS; x++) cell(x, ROWS - 1, 32, 7, 0);
    print(2, ROWS - 1, typed(t, p - 1, 24), 7, 0);
  }

  function intro(p, d, part) {   // typed lines on black, a cursor
    clear(0);
    const ls = lines(part.lines); let y = 9, at = 0.5;
    ls.forEach((l, n) => {
      const q = p - at; at += l.length / 22 + 1.2;
      if (q > 0) center(y + n * 2, typed(l, q, 22), n === 0 ? 14 : n === ls.length - 1 ? 8 : 7);
    });
    if (Math.floor(p * 2.5) % 2 === 0) cell(2, ROWS - 2, 95, 7);
    fadeEdges(p, d);
  }
  function title(p, d, part) {   // the name in big letters with a copper bar through it
    pclear(0);
    const t = p * 3;
    for (let y = 0; y < PH; y++) {   // two copper bars rolling
      const a = Math.sin(t + y * 0.12), b = Math.sin(t * 0.7 - y * 0.09);
      if (a > 0.92) for (let x = 0; x < PW; x++) pset(x, y, 9); else if (b > 0.95) for (let x = 0; x < PW; x++) pset(x, y, 1);
    }
    // the name in up to two lines (| splits it): 80 pixels is not wide
    const name = lines(part.name || 'SECOND|REALITY');
    name.forEach((w, n) => big(w, (name.length > 1 ? 3 : 12) + n * 15, (i, j) => [15, 11, 11, 3, 9, 1][clamp(Math.floor((j + Math.sin(t + i * 0.2 + n) * 1.5) / 3), 0, 5)], 13));
    flush();
    if (p > 2) center(18, typed(part.sub || '', p - 2, 20), 7, -1);
    if (p > 4) center(20, typed(part.sub2 || '', p - 4, 20), 8, -1);
    fadeEdges(p, d);
  }
  function dots(p, d, part) {   // a ball of dots bouncing on a floor, with its shadow
    pclear(0);
    for (let x = 0; x < PW; x++) for (let y = 38; y < PH; y++) if (((x + Math.floor(p * 8)) >> 2 ^ (y >> 1)) & 1) pset(x, y, y > 44 ? 8 : 0); else pset(x, y, 1);
    const bounce = Math.abs(Math.sin(p * 2.2)), cx = 40 + Math.sin(p * 0.6) * 18, cy = 30 - bounce * 18;
    const squash = bounce < 0.12 ? 1 + (0.12 - bounce) * 2.5 : 1;
    // shadow
    for (let a = 0; a < 6.28; a += 0.05) for (let r = 0; r < 1; r += 0.25) pset(cx + Math.cos(a) * 10 * r * (1.3 - bounce * 0.4), 41 + Math.sin(a) * 2 * r, 0);
    const pts = st.ball, ry = p * 1.3, rx = p * 0.8, cyr = Math.cos(ry), syr = Math.sin(ry), cxr = Math.cos(rx), sxr = Math.sin(rx);
    const proj = [];
    for (const q of pts) {
      let x = q[0] * cyr - q[2] * syr, z = q[0] * syr + q[2] * cyr, y = q[1];
      const y2 = y * cxr - z * sxr; z = y * sxr + z * cxr; y = y2;
      proj.push([cx + x * 10 * squash, cy + y * 10 / squash * 0.9, z]);
    }
    proj.sort((a, b) => b[2] - a[2]);
    for (const q of proj) pset(q[0], q[1], q[2] > 0.4 ? 8 : q[2] > -0.3 ? 7 : 15);
    flush();
    caption(part, p);
    fadeEdges(p, d);
  }
  function plasma(p, d, part) {   // three sines in shade blocks; the palette turns
    const t = p * 1.4, ramp = p < d / 2 ? RAMP_BLUE : RAMP_VIOLET;
    for (let y = 0; y < ROWS; y++) for (let x = 0; x < COLS; x++) {
      const v = Math.sin(x * 0.09 + t) + Math.sin((y * 0.22 + t * 0.7)) + Math.sin((x * 0.05 + y * 0.12 + t * 1.3)) + Math.sin(Math.hypot(x - 40 - Math.sin(t) * 20, (y - 12) * 2.2) * 0.12 - t);
      shade(x, y, ((v + 4) / 8 + t * 0.05) % 1, ramp);
    }
    caption(part, p);
    fadeEdges(p, d);
  }
  // glenz: a see-through solid. Front faces in two colours like a chessboard, back faces seen through them.
  const OCTA_V = [[1, 0, 0], [-1, 0, 0], [0, 1, 0], [0, -1, 0], [0, 0, 1], [0, 0, -1]];
  const OCTA_F = [[0, 2, 4], [2, 1, 4], [1, 3, 4], [3, 0, 4], [2, 0, 5], [1, 2, 5], [3, 1, 5], [0, 3, 5]];
  function tri(a, b, c, fn) {
    const minY = Math.max(0, Math.floor(Math.min(a[1], b[1], c[1]))), maxY = Math.min(PH - 1, Math.ceil(Math.max(a[1], b[1], c[1])));
    const minX = Math.max(0, Math.floor(Math.min(a[0], b[0], c[0]))), maxX = Math.min(PW - 1, Math.ceil(Math.max(a[0], b[0], c[0])));
    const area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]); if (Math.abs(area) < 0.01) return;
    for (let y = minY; y <= maxY; y++) for (let x = minX; x <= maxX; x++) {
      const px = x + 0.5, py = y + 0.5;
      const w0 = (b[0] - a[0]) * (py - a[1]) - (b[1] - a[1]) * (px - a[0]);
      const w1 = (c[0] - b[0]) * (py - b[1]) - (c[1] - b[1]) * (px - b[0]);
      const w2 = (a[0] - c[0]) * (py - c[1]) - (a[1] - c[1]) * (px - c[0]);
      if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) fn(x, y);
    }
  }
  function glenz(p, d, part) {
    const front = st.front, back = st.back; front.fill(0); back.fill(0);
    const s = 7 + Math.sin(p * 0.9) * 1.5 + Math.min(1, p / 3) * 3, cx = 40 + Math.sin(p * 0.7) * 12, cy = 26 - Math.abs(Math.sin(p * 1.6)) * 6;
    const ax = p * 0.9, ay = p * 1.3, az = p * 0.4;
    const rot = v => {
      let [x, y, z] = v;
      let t = y * Math.cos(ax) - z * Math.sin(ax); z = y * Math.sin(ax) + z * Math.cos(ax); y = t;
      t = x * Math.cos(ay) + z * Math.sin(ay); z = -x * Math.sin(ay) + z * Math.cos(ay); x = t;
      t = x * Math.cos(az) - y * Math.sin(az); y = x * Math.sin(az) + y * Math.cos(az); x = t;
      const k = 3.2 / (3.2 + z); return [cx + x * s * 2 * k, cy + y * s * k, z];
    };
    // the star: each face of an octahedron raised to a point, so 24 small faces
    const V = OCTA_V.map(rot), faces = [];
    OCTA_F.forEach((f, n) => {
      const m = [0, 1, 2].map(i => OCTA_V[f[0]][i] + OCTA_V[f[1]][i] + OCTA_V[f[2]][i]);
      const len = Math.hypot(m[0], m[1], m[2]), tip = rot(m.map(c => c / len * (1.25 + Math.sin(p * 2) * 0.2)));
      for (let e = 0; e < 3; e++) faces.push({ a: V[f[e]], b: V[f[(e + 1) % 3]], c: tip, n: n * 3 + e });
    });
    for (const f of faces) {
      const cross = (f.b[0] - f.a[0]) * (f.c[1] - f.a[1]) - (f.b[1] - f.a[1]) * (f.c[0] - f.a[0]);
      const buf = cross < 0 ? front : back, bit = (f.n & 1) ? 2 : 1;
      tri(f.a, f.b, f.c, (x, y) => { buf[y * PW + x] |= bit; });
    }
    // the floor grid behind
    for (let y = 0; y < PH; y++) for (let x = 0; x < PW; x++) {
      const i = y * PW + x, grid = ((x + Math.floor(p * 6)) % 8 === 0 || (y + Math.floor(p * 3)) % 6 === 0) ? 8 : 0;
      const f = front[i], b = back[i];
      st.px[i] = f ? (f & 1 ? (b ? 11 : 15) : (b ? 1 : 9)) : b ? (b & 1 ? 3 : 1) : grid;
    }
    flush();
    caption(part, p);
    fadeEdges(p, d);
  }
  function tunnel(p, d, part) {   // a tunnel in half blocks, checked, lit from the far end
    const t = p;
    for (let y = 0; y < PH; y++) for (let x = 0; x < PW; x++) {
      const dx = (x - 40 - Math.sin(t * 0.8) * 10) / 2, dy = (y - 25 - Math.cos(t * 0.6) * 6);
      const dist = Math.hypot(dx, dy) + 0.001, a = Math.atan2(dy, dx);
      const u = Math.floor(a / Math.PI * 8 + t * 2), v = Math.floor(48 / dist + t * 6);
      const light = clamp(dist / 22, 0, 1), on = (u ^ v) & 1;
      st.px[y * PW + x] = light < 0.15 ? 0 : on ? (light > 0.6 ? 14 : light > 0.35 ? 6 : 4) : (light > 0.6 ? 12 : light > 0.35 ? 4 : 0);
    }
    flush();
    caption(part, p);
    fadeEdges(p, d);
  }
  function note(p, d, part) {   // Ty's note over a slow, dim plasma
    const t = p * 0.5;
    for (let y = 0; y < ROWS; y++) for (let x = 0; x < COLS; x++) {
      const v = Math.sin(x * 0.07 + t) + Math.sin(y * 0.2 - t) + Math.sin((x + y) * 0.05 + t * 1.5);
      const r = [[0, 1, G_LIGHT], [0, 1, G_MED], [0, 8, G_LIGHT], [0, 0, G_FULL]][clamp(Math.floor((v + 3) / 6 * 4), 0, 3)];
      cell(x, y, r[2], r[1], r[0]);
    }
    const ls = lines(part.lines); let at = 0.8;
    const top = Math.floor((ROWS - ls.length * 2) / 2);
    ls.forEach((l, n) => {
      const q = p - at; at += l.length / 18 + 1.4;
      if (q <= 0 || !l) return;
      const s = typed(l, q, 18), x0 = Math.floor((COLS - l.length) / 2);
      for (let i = -1; i <= l.length; i++) cell(x0 + i, top + n * 2, 32, 7, 0);
      print(x0, top + n * 2, s, n === ls.length - 1 ? 14 : 7, 0);
    });
    fadeEdges(p, d);
  }
  function scroll(p, d, part) {   // the end scroller over stars; headings in a copper colour
    pclear(0);
    for (const s of st.stars) {
      s[0] -= s[2] * 0.6; if (s[0] < 0) s[0] += PW;
      pset(s[0], s[1], s[2] > 0.7 ? 15 : s[2] > 0.4 ? 7 : 8);
    }
    flush();
    const ls = lines(part.lines), speed = (ls.length + ROWS) / Math.max(1, d - 2), off = ROWS - p * speed;
    const copper = [9, 11, 15, 11, 9, 3];
    ls.forEach((l, n) => {
      const y = Math.floor(off + n); if (y < 0 || y >= ROWS) return;
      const head = l.startsWith('*'), text = head ? l.slice(1) : l;
      if (text) center(y, text, head ? copper[Math.floor(p * 6 + n) % copper.length] : l.startsWith('(') ? 8 : 7, 0);
    });
    if (p < 0.8) fadeEdges(p, d);
  }
  function theEnd(q) {   // after the tune: the last screen waits for a key
    clear(0);
    const e = st.ini.END || {};
    center(11, e.text || 'END', 15);
    if (e.sub) center(13, e.sub, 8);
    if (q > 1.2 && Math.floor(q * 2) % 2 === 0) center(ROWS - 3, e.key || 'press any key', 7);
  }
  const PARTS = { intro, title, dots, plasma, glenz, tunnel, note, scroll };

  // ---------------------------------------------------------------- audio: libopenmpt, as the end demo does
  function loadLib() {
    if (window.libopenmpt && window.libopenmpt._openmpt_module_create_from_memory2) return Promise.resolve(window.libopenmpt);
    return new Promise((ok, fail) => {
      window.libopenmpt = { locateFile: f => '/player/' + f, onRuntimeInitialized() { ok(window.libopenmpt); } };
      const s = document.createElement('script');
      s.src = '/player/libopenmpt.js'; s.onerror = () => fail(new Error('no player library'));
      document.head.appendChild(s);
    });
  }
  const muted = () => { try { return localStorage.getItem('tb.mute') === '1'; } catch { return false; } };
  function setMute(on) {
    try { localStorage.setItem('tb.mute', on ? '1' : '0'); } catch { /* private mode */ }
    if (st.gain) st.gain.gain.value = on ? 0 : 0.8;
    if (st.mute) st.mute.textContent = on ? 'SOUND OFF' : 'MUTE';
  }
  async function startAudio(buf, want) {
    const lib = await loadLib();
    const AC = window.AudioContext || window.webkitAudioContext; if (!AC) return;
    const ctx = new AC(), data = new Uint8Array(buf), p = lib._malloc(data.length);
    window.HEAPU8.set(data, p);
    const mod = lib._openmpt_module_create_from_memory2(p, data.length, 0, 0, 0, 0, 0, 0, 0);
    lib._free(p);
    if (!mod) { ctx.close(); return; }
    // Second Reality's tune is a row of sequences split by end marks (the demo jumped between them): the INI names
    // the ones to play in turn ("0,1": the start music, then the main tune); none plays the longest.
    const n = lib._openmpt_module_get_num_subsongs(mod);
    let subs = String(want || '').split(',').map(x => parseInt(x, 10)).filter(x => x >= 0 && x < n);
    if (!subs.length) {
      let best = -1, sub = 0;
      for (let i = 0; i < n; i++) { lib._openmpt_module_select_subsong(mod, i); const d = lib._openmpt_module_get_duration_seconds(mod); if (d > best) { best = d; sub = i; } }
      subs = [sub];
    }
    lib._openmpt_module_select_subsong(mod, subs.shift());
    lib._openmpt_module_set_repeat_count(mod, 0);
    const frames = 4096, bl = lib._malloc(frames * 4), br = lib._malloc(frames * 4);
    const node = ctx.createScriptProcessor(frames, 0, 2), gain = ctx.createGain();
    gain.gain.value = muted() ? 0 : 0.8;
    node.onaudioprocess = e => {
      const L = e.outputBuffer.getChannelData(0), R = e.outputBuffer.getChannelData(1);
      if (!st || st.stopMusic || !st.mod) { L.fill(0); R.fill(0); return; }
      let got = lib._openmpt_module_read_float_stereo(mod, ctx.sampleRate, frames, bl, br);
      L.set(new Float32Array(window.HEAPU8.buffer, bl, got)); R.set(new Float32Array(window.HEAPU8.buffer, br, got));
      if (got < frames && subs.length) {   // this sequence ended: the next one, without a gap
        lib._openmpt_module_select_subsong(mod, subs.shift());
        const more = lib._openmpt_module_read_float_stereo(mod, ctx.sampleRate, frames - got, bl, br);
        L.set(new Float32Array(window.HEAPU8.buffer, bl, more), got); R.set(new Float32Array(window.HEAPU8.buffer, br, more), got);
        got += more;
      }
      if (got < frames) { L.fill(0, got); R.fill(0, got); }
      st.audioPos += frames / ctx.sampleRate; st.audioAt = performance.now();
    };
    node.connect(gain); gain.connect(ctx.destination);
    st.ctx = ctx; st.lib = lib; st.mod = mod; st.node = node; st.gain = gain; st.audioPos = 0;
    if (ctx.state !== 'running') await ctx.resume().catch(() => {});
  }
  function stopAudio(s) {
    s.stopMusic = true;
    try { if (s.node) { s.node.disconnect(); s.node.onaudioprocess = null; } } catch { /* gone */ }
    try { if (s.lib && s.mod) s.lib._openmpt_module_destroy(s.mod); } catch { /* gone */ }
    s.mod = 0;
    try { if (s.ctx) s.ctx.close(); } catch { /* closed */ }
  }
  function clock() {
    if (st.ctx && st.ctx.state === 'running' && st.audioAt) return st.audioPos + Math.min(0.12, (performance.now() - st.audioAt) / 1000);
    return (performance.now() - st.t0) / 1000;
  }

  // ---------------------------------------------------------------- the loop
  function frame() {
    if (!st) return;
    st.raf = requestAnimationFrame(frame);
    const now = performance.now();
    if (now - st.lastFrame < 33) return;
    st.lastFrame = now;
    if (st.phase === 'play') {
      const t = clock(), part = st.parts.find(x => t >= x.start && t < x.end);
      if (st.gain && !muted()) st.gain.gain.value = 0.8 * clamp((st.endT - t) / st.fade, 0, 1);   // the music fades out with the scroller
      if (!part) { if (t >= st.endT) { st.phase = 'end'; st.endAt = now; stopAudio(st); } else clear(0); }
      else (PARTS[part.kind] || (() => clear(0)))(t - part.start, part.end - part.start, part);
    }
    if (st.phase === 'end') theEnd((now - st.endAt) / 1000);
    draw();
  }
  function finish() {
    if (!st) return;
    const s = st; st = null;
    cancelAnimationFrame(s.raf);
    stopAudio(s);
    document.removeEventListener('keydown', s.onKey, true);
    s.root.remove();
    s.onDone();
  }
  function onKey(e) {
    if (!st || e.isComposing) return;
    if (['Shift', 'Control', 'Alt', 'Meta', 'CapsLock'].includes(e.key)) return;
    e.preventDefault(); e.stopPropagation();
    const k = e.key.toLowerCase();
    if (st.phase === 'end') { if (performance.now() - st.endAt > 1200) finish(); return; }
    if (k === 'q' || k === 'escape') finish();
    else if (k === 'm') setMute(!muted());
  }

  async function run(opts) {
    if (st) return;
    const root = document.createElement('div');
    root.style.cssText = 'position:fixed;inset:0;z-index:40;background:#000;display:flex;align-items:center;justify-content:center;flex-direction:column;font:14px monospace;color:#aaa';
    const cv = document.createElement('canvas'); cv.width = W; cv.height = H;
    cv.style.cssText = 'width:min(100vw,148vh);height:auto;image-rendering:pixelated;background:#000';
    const bar = document.createElement('div'); bar.style.cssText = 'position:absolute;left:0;right:0;bottom:4px;display:flex;gap:8px;justify-content:center;opacity:.8';
    const mk = (label, fn) => { const b = document.createElement('button'); b.textContent = label; b.style.cssText = 'background:#000;color:#5ff;border:1px solid #066;font:inherit;min-height:32px;padding:0 10px'; b.onclick = e => { e.stopPropagation(); fn(); }; bar.appendChild(b); return b; };
    root.append(cv, bar);
    document.body.appendChild(root);
    const ctx2d = cv.getContext('2d', { alpha: false }), image = ctx2d.createImageData(W, H);
    st = { root, ctx2d, image, fb: new Uint32Array(image.data.buffer), ch: new Uint16Array(COLS * ROWS), fg: new Uint8Array(COLS * ROWS), bg: new Uint8Array(COLS * ROWS),
      px: new Uint8Array(PW * PH), front: new Uint8Array(PW * PH), back: new Uint8Array(PW * PH), phase: 'load', onDone: opts.onDone || (() => {}),
      lastFrame: 0, t0: performance.now(), stopMusic: false, ini: {}, ball: [], stars: [] };
    mk('QUIT (Q)', () => finish());
    st.mute = mk(muted() ? 'SOUND OFF' : 'MUTE', () => setMute(!muted()));
    root.addEventListener('click', () => { if (st && st.phase === 'end' && performance.now() - st.endAt > 1200) finish(); });
    st.onKey = onKey; document.addEventListener('keydown', onKey, true);
    clear(0); print(1, 1, 'LOADING', 8); draw();
    // the ball: points on a sphere, evenly spread
    const N = 140; for (let i = 0; i < N; i++) { const y = 1 - (i + 0.5) / N * 2, r = Math.sqrt(1 - y * y), a = i * 2.39996; st.ball.push([Math.cos(a) * r, y, Math.sin(a) * r]); }
    let seed = 7; const rnd = () => (seed = (seed * 1103515245 + 12345) & 0x7fffffff) / 0x7fffffff;
    for (let i = 0; i < 90; i++) st.stars.push([rnd() * PW, rnd() * PH, rnd()]);
    try {
      const ini = parseIni(await (await fetch(BASE + 'SR.INI', { cache: 'no-store' })).text());
      st.ini = ini;
      st.parts = Object.keys(ini).filter(k => /^P\d/.test(k)).sort().map(k => Object.assign({ kind: '' }, ini[k], { start: +ini[k].start, end: +ini[k].end }));
      st.endT = Math.max(...st.parts.map(p => p.end));
      st.fade = Math.max(0.5, +((ini.DEMO && ini.DEMO.fade) || 4));
      const tune = await (await fetch(BASE + ((ini.DEMO && ini.DEMO.tune) || 'MUSIC0.S3M'))).arrayBuffer();
      await startAudio(tune, ini.DEMO && ini.DEMO.subsong).catch(() => {});
    } catch (e) { finish(); return; }
    if (!st) return;
    st.t0 = performance.now(); st.audioAt = 0; st.audioPos = 0; st.phase = 'play';
    st.raf = requestAnimationFrame(frame);
  }
  window.TyDemo = { run };
})();
