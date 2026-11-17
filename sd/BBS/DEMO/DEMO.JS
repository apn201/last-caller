// HALCYON end demo (slice GE). A 1990s demo for the browser terminal: 320 x 200 chunky pixels, one module
// tune, parts cut on the beat. Served by the board from /BBS/DEMO (never from a CDN: the stick is offline).
// The timeline and every word are in /BBS/STORY/DEMO.INI, fetched as /demo/DEMO.INI. The tune is played by
// libopenmpt (BSD, already on the card for the music player, /player/libopenmpt.*). Original work, MIT.
//
//   HalcyonDemo.run({ variant: 'END1' | 'END2', onDone: fn })
//
// Q or ESC skips to NO CARRIER, M mutes (the setting is kept as localStorage 'tb.mute', one switch for every
// sound the board makes on its own), any key at NO CARRIER ends it and calls onDone.
(function () {
  'use strict';
  const W = 320, H = 200, BASE = '/demo/';
  // the board's palette: black ground, greys, the ship's cold cyan, ELIZA's amber, red only for an alarm
  const rgb = (r, g, b) => (255 << 24) | (b << 16) | (g << 8) | r;
  const C = { black: rgb(0, 0, 0), dgrey: rgb(85, 85, 85), grey: rgb(170, 170, 170), white: rgb(255, 255, 255),
    cyan: rgb(0, 170, 170), lcyan: rgb(85, 255, 255), blue: rgb(0, 0, 170), amber: rgb(170, 85, 0), yellow: rgb(255, 255, 85),
    lamber: rgb(255, 170, 40), red: rgb(255, 85, 85), green: rgb(85, 255, 85) };
  const scale = (col, k) => { k = Math.max(0, Math.min(1, k)); return rgb(((col & 255) * k) | 0, (((col >> 8) & 255) * k) | 0, (((col >> 16) & 255) * k) | 0); };
  const mix = (a, b, t) => rgb(((a & 255) * (1 - t) + (b & 255) * t) | 0, (((a >> 8) & 255) * (1 - t) + ((b >> 8) & 255) * t) | 0, (((a >> 16) & 255) * (1 - t) + ((b >> 16) & 255) * t) | 0);
  const clamp = (v, a, b) => Math.max(a, Math.min(b, v));
  const ease = t => { t = clamp(t, 0, 1); return t * t * (3 - 2 * t); };

  let st = null;   // the run in progress

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
  // a word of a part, in the caller's ending: key_END2 first when the caller stayed
  const word = (p, key, v) => (v === 'END2' && p[key + '_end2'] !== undefined ? p[key + '_end2'] : p[key]) || '';

  // ---------------------------------------------------------------- crisp pixel text
  const masks = {};
  function mask(str, px, bold) {
    const key = (bold ? 'b' : 'n') + px + '|' + str;
    if (masks[key]) return masks[key];
    const c = document.createElement('canvas'), x = c.getContext('2d', { willReadFrequently: true });
    const font = (bold ? 'bold ' : '') + px + 'px monospace';
    x.font = font;
    const w = Math.max(1, Math.ceil(x.measureText(str).width) + 2), h = Math.ceil(px * 1.35);
    c.width = w; c.height = h;
    x.font = font; x.textBaseline = 'top'; x.fillStyle = '#fff'; x.fillText(str, 1, 0);
    const d = x.getImageData(0, 0, w, h).data, m = new Uint8Array(w * h);
    for (let i = 0; i < w * h; i++) m[i] = d[i * 4 + 3] > 110 ? 1 : 0;
    return (masks[key] = { w, h, m });
  }
  function put(fb, fw, fh, mk, x, y, col, sc) {
    sc = sc || 1;
    for (let j = 0; j < mk.h; j++) for (let i = 0; i < mk.w; i++) {
      if (!mk.m[j * mk.w + i]) continue;
      const c = typeof col === 'function' ? col(i, j) : col;
      for (let b = 0; b < sc; b++) for (let a = 0; a < sc; a++) {
        const px = x + i * sc + a, py = y + j * sc + b;
        if (px >= 0 && px < fw && py >= 0 && py < fh) fb[py * fw + px] = c;
      }
    }
  }
  const text = (str, x, y, col, px, bold, sc) => put(st.fb, W, H, mask(str, px || 8, bold), x, y, col, sc);
  const textWidth = (str, px, bold, sc) => mask(str, px || 8, bold).w * (sc || 1);
  function wrap(str, width, px, bold) {
    const words = str.split(' '), lines = []; let cur = '';
    for (const w of words) {
      const t = cur ? cur + ' ' + w : w;
      if (cur && textWidth(t, px, bold) > width) { lines.push(cur); cur = w; } else cur = t;
    }
    if (cur) lines.push(cur);
    return lines;
  }

  // ---------------------------------------------------------------- drawing helpers
  const clear = col => st.fb.fill(col === undefined ? C.black : col);
  const px = (x, y, col) => { if (x >= 0 && x < W && y >= 0 && y < H) st.fb[y * W + x] = col; };
  function rect(x, y, w, h, col) { for (let j = 0; j < h; j++) for (let i = 0; i < w; i++) px(x + i, y + j, col); }
  function stars(t, speed, n, dim) {
    for (let i = 0; i < n; i++) {
      const s = st.stars[i];
      const z = ((s.z - t * speed) % 1 + 1) % 1, k = 0.05 + z * 0.95;
      const x = (W / 2 + s.x / (1.2 - z) * 0.6) | 0, y = (H / 2 + s.y / (1.2 - z) * 0.6) | 0;
      px(x, y, scale(C.white, (0.25 + z * 0.75) * (dim || 1)));
      if (z > 0.8) px(x + 1, y, scale(C.lcyan, 0.6 * (dim || 1)));
    }
  }
  function copper(t, y0, h, strength) {
    // horizontal bars that slide up and down, cold cyan to white, as a 1990s demo has them
    for (let b = 0; b < 4; b++) {
      const cy = y0 + h / 2 + Math.sin(t * 1.3 + b * 0.8) * (h / 2 - 8);
      for (let j = -6; j <= 6; j++) {
        const y = (cy + j) | 0; if (y < y0 || y >= y0 + h) continue;
        const k = (1 - Math.abs(j) / 6.5) * strength;
        const col = scale(mix(C.cyan, C.white, 1 - Math.abs(j) / 6.5), 0.2 + 0.8 * k);
        for (let x = 0; x < W; x++) st.fb[y * W + x] = col;
      }
    }
  }
  function earth(cy, r, t) {
    // the limb of the Earth, dithered in the blues, a little sunlight on the left
    const bayer = [0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5];
    for (let y = Math.max(0, cy - r); y < H; y++) for (let x = 0; x < W; x++) {
      const dx = x - W / 2, dy = y - (cy + r), d = Math.sqrt(dx * dx + dy * dy);
      if (d > r) continue;
      const edge = 1 - d / r, lit = clamp(0.45 - dx / (W * 1.4) + Math.sin(x * 0.07 + t * 0.1) * 0.05, 0, 1);
      const v = clamp(edge * 6 * lit + 0.05, 0, 1) * 3, i = Math.floor(v), f = v - i;
      const ramp = [C.black, C.blue, C.cyan, C.lcyan];
      const hi = f > (bayer[(y & 3) * 4 + (x & 3)] + 0.5) / 16 ? 1 : 0;
      px(x, y, ramp[Math.min(3, i + hi)]);
    }
  }
  function ring(cx, cy, r, t, n, col) {
    // ELIZA's amber ring: a row of status lights on a circle, one of them dark
    for (let i = 0; i < n; i++) {
      const a = i / n * Math.PI * 2 + t * 0.4, dark = i === 5;
      rect((cx + Math.cos(a) * r) | 0, (cy + Math.sin(a) * r) | 0, 3, 3, dark ? C.dgrey : col);
    }
  }
  function crt(src) { st.last.set(src); }

  // ---------------------------------------------------------------- the parts
  function typed(str, p, cps) { return str.slice(0, clamp(Math.floor(p * cps), 0, str.length)); }

  function release(p, d, part) {
    clear();
    const blink = ((p * 2) | 0) % 2 === 0;
    const lines = [];
    const sh = part.shell, ok = part.ok;
    const a = typed(sh, p - 0.4, 22);
    text(a + (p < 3 && blink ? '_' : ''), 8, 10, C.lcyan, 9);
    if (p > 2.0) text(ok, 8, 24, C.white, 9, true);
    void lines;
    if (p > 2.6) {
      const tp = clamp((p - 2.6) / 6.4, 0, 1), msgs = +part.messages || 41, files = +part.files || 3, names = (part.names || '').split('|');
      text(part.label || '', 8, 44, C.grey, 9);
      // 41 small squares and the three files go out as a packet bar
      const sent = Math.floor(tp * msgs);
      for (let i = 0; i < msgs; i++) rect(8 + i * 7, 58, 5, 8, i < sent ? (i % 7 ? C.lcyan : C.cyan) : C.dgrey);
      text(sent + ' / ' + msgs + ' MESSAGES', 8, 70, C.lcyan, 9);
      for (let f = 0; f < files; f++) {
        const fp = clamp(tp * (files + 0.5) - f, 0, 1), y = 88 + f * 18;
        text((names[f] || '').padEnd(12), 8, y, C.lamber, 9);
        rect(110, y + 1, 100, 8, C.dgrey); rect(110, y + 1, (100 * fp) | 0, 8, fp >= 1 ? C.lcyan : C.amber);
        text(((fp * 100) | 0) + '%', 216, y, C.grey, 9);
      }
      // a data stream: hex on the right, scrolling
      for (let j = 0; j < 14; j++) {
        const v = ((p * 30 + j * 977) | 0) * 2654435761 >>> 0;
        text(v.toString(16).slice(0, 6), 268, 8 + j * 12, scale(C.dgrey, 0.6 + 0.4 * ((j + (p * 8 | 0)) % 3 / 2)), 8);
      }
    }
    if (p > 9.8) {   // the modem, and the screen tears into the demo
      const k = ease((p - 9.8) / (d - 9.8));
      const src = st.fb.slice();
      for (let y = 0; y < H; y++) {
        const off = ((Math.sin(y * 0.21 + p * 40) * 40 + (Math.random() - 0.5) * 80) * k) | 0;
        const roll = k > 0.7 ? ((p * 400) | 0) % H : 0;
        const sy = (y + roll) % H;
        for (let x = 0; x < W; x++) {
          let v = src[sy * W + ((x - off + W * 2) % W)];
          if (Math.random() < 0.15 * k) v = Math.random() < 0.5 ? C.white : C.cyan;
          st.fb[y * W + x] = v;
        }
      }
      if (p > d - 0.12) st.fb.fill(C.white);
    }
    if (p > 8.8 && !part._sfx) { part._sfx = true; cue('/sfx/MODEM.WAV', 0.5); }
  }

  function title(p, d, part) {
    clear();
    copper(p, 0, H, 0.9 * ease(p / 1.5));
    // the bars sit behind a darkened band, the logo in front
    stars(p, 0.12, st.stars.length, ease(p / 1));
    earth(172 + (p < 1 ? (1 - ease(p)) * 30 : 0), 160, p);
    const name = part.name || 'HALCYON', mk = mask(name, 30, true), sc = 2, lw = mk.w * sc;
    const x0 = ((W - lw) / 2) | 0, y0 = 28;
    // sine wobble per scanline, the logo shades from white to cyan
    const buf = new Uint32Array(W * H); const save = st.fb; st.fb = buf;
    put(buf, W, H, mk, x0, y0, (i, j) => mix(C.white, C.cyan, clamp(j / mk.h, 0, 1)), sc);
    st.fb = save;
    for (let y = y0 - 4; y < y0 + mk.h * sc + 6; y++) {
      const off = Math.round(Math.sin(y * 0.16 + p * 3.1) * (1 + 3 * ease(p / 3)));
      for (let x = 0; x < W; x++) {
        const v = buf[y * W + clamp(x - off, 0, W - 1)];
        if (v) { st.fb[y * W + x] = v; if (y + 3 < H && !buf[(y + 3) * W + clamp(x - off - 2, 0, W - 1)]) { /* a shadow */ } }
      }
    }
    // the ship in orbit: a slow ellipse with a sine wobble, the solar panel dark
    const a = p * 0.5, sx = (W / 2 + Math.cos(a) * 110 - 44) | 0, sy = (118 + Math.sin(a) * 14 + Math.sin(p * 2.2) * 3) | 0;
    const img = st.img.ship;
    if (img) blitImg(img, sx, sy, 1, Math.sin(a) > 0 ? 1 : 0.55);
    text(part.text || '', ((W - textWidth(part.text || '', 9)) / 2) | 0, 108 + 0, C.grey, 9);
    text(part.text2 || '', ((W - textWidth(part.text2 || '', 9)) / 2) | 0, 122 + 0, C.dgrey, 9);
    if (p < 0.5) { const k = 1 - p / 0.5; for (let i = 0; i < W * H; i++) st.fb[i] = mix(st.fb[i], C.white, k * 0.9); }
  }

  function blitImg(img, x, y, sc, shade) {
    const c = img.canvas; if (!c.data) return;
    const d = c.data, w = c.width, h = c.height;
    for (let j = 0; j < h; j++) for (let i = 0; i < w; i++) {
      const o = (j * w + i) * 4; if (d[o + 3] < 128) continue;
      const col = scale(rgb(d[o], d[o + 1], d[o + 2]), shade === undefined ? 1 : shade);
      for (let b = 0; b < sc; b++) for (let a = 0; a < sc; a++) px(x + i * sc + a, y + j * sc + b, col);
    }
  }

  function plasma(t, k, tint) {
    for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
      const v = Math.sin(x * 0.045 + t) + Math.sin(y * 0.06 - t * 1.2) + Math.sin((x + y) * 0.03 + t * 0.7) + Math.sin(Math.hypot(x - 160, y - 100) * 0.05 - t);
      const u = (v + 4) / 8;
      st.fb[y * W + x] = scale(mix(C.blue, tint, u), 0.1 + 0.35 * u * k);
    }
  }
  function letter(p, d, part, v) {
    const fade = Math.min(ease(p / 0.7), ease((d - p) / 0.7));
    plasma(p * 0.8, fade, part.face === 'FACE_T.PNG' ? C.amber : C.cyan);
    const img = st.img[part.face];
    if (img) {
      // the face, twice as big, on a plasma that is dimmer behind it; a frame like a monitor bezel
      rect(14, 34, 104, 136, scale(C.black, 1)); rect(14, 34, 104, 1, C.dgrey); rect(14, 169, 104, 1, C.dgrey); rect(14, 34, 1, 136, C.dgrey); rect(117, 34, 1, 136, C.dgrey);
      blitImg(img, 20, 38, 2, fade);
    }
    const hd = 'FROM ' + part.from + '  TO ' + part.to;
    text(hd.slice(0, 40), 130, 40, scale(C.lamber, fade), 9, true);
    text(part.date || '', 130, 54, scale(C.dgrey, 0.5 + 0.5 * fade), 9);
    for (let i = 0; i < 190; i += 2) px(130 + i, 66, scale(C.dgrey, fade));
    const body = typed(part.text || '', p - 0.9, 26), lines = wrap(part.text || '', 184, 9, false);
    let used = 0;
    for (let i = 0; i < lines.length; i++) {
      const l = lines[i], shown = Math.max(0, Math.min(l.length, body.length - used - (used ? 0 : 0)));
      text(l.slice(0, shown), 130, 78 + i * 13, scale(C.white, fade), 9);
      used += l.length + 1;
    }
    if (p > 5.6) text(part.sign || '', 130, 78 + lines.length * 13 + 12, scale(C.lcyan, fade * ease((p - 5.6) / 0.6)), 9, true);
    if (((p * 2) | 0) % 2 === 0 && p < 5.5) rect(130 + textWidth(lines[Math.min(lines.length - 1, Math.max(0, lines.findIndex((l, i) => { let s = 0; for (let k = 0; k <= i; k++) s += lines[k].length + 1; return body.length < s; })))] || '', 9) + 2, 78, 4, 8, C.white);
    void v;
  }

  // the newspaper: a page drawn once into a texture, then spun and zoomed (a rotozoomer) onto the screen
  function paper(part, v) {
    const tex = new Uint32Array(W * H); const save = st.fb; st.fb = tex;
    tex.fill(mix(C.grey, C.white, 0.3));
    const ink = rgb(30, 30, 40), faint = rgb(120, 120, 128);
    const g = (str, x, y, col, pxs, bold) => text(str, x, y, col, pxs, bold);
    const mast = 'THE HARBOUR POST', mw = textWidth(mast, 18, true);
    g(mast, ((W - mw) / 2) | 0, 8, ink, 18, true);
    rect(10, 32, W - 20, 2, ink); rect(10, 36, W - 20, 1, ink);
    g(word(part, 'date', v), 12, 40, ink, 8);
    g('ONE PENNY', W - 12 - textWidth('ONE PENNY', 8), 40, ink, 8);
    const head = wrap(word(part, 'head', v), W - 28, 16, true);
    head.slice(0, 3).forEach((l, i) => g(l, 14, 56 + i * 20, ink, 16, true));
    const sub = wrap(word(part, 'sub', v), W - 28, 9, false);
    sub.slice(0, 4).forEach((l, i) => g(l, 14, 56 + head.length * 20 + 6 + i * 12, rgb(50, 50, 60), 9));
    let rnd = 7; const rand = () => (rnd = (rnd * 1103515245 + 12345) & 0x7fffffff) / 0x7fffffff;
    for (let col = 0; col < 3; col++) for (let y = 150; y < 190; y += 4) {
      const wlen = 70 + (rand() * 30 | 0); rect(14 + col * 100, y, wlen, 1, faint);
    }
    st.fb = save;
    return tex;
  }
  function news(p, d, part, v) {
    if (!part._tex) part._tex = paper(part, v);
    // spin in (fast, small), hold the page still and straight for a few seconds, spin away
    const tin = ease(p / 1.4), tout = ease((p - (d - 1.1)) / 1.1);
    const ang = (1 - tin) * 9 + tout * -7, zoom = 0.15 + 0.85 * tin + tout * 1.3;
    const cs = Math.cos(ang), sn = Math.sin(ang), tex = part._tex;
    const dark = scale(C.black, 1);
    for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
      const dx = (x - W / 2) / zoom, dy = (y - H / 2) / zoom;
      const u = Math.round(dx * cs - dy * sn + W / 2), w = Math.round(dx * sn + dy * cs + H / 2);
      st.fb[y * W + x] = u >= 0 && u < W && w >= 0 && w < H ? tex[w * W + u] : dark;
    }
    // a shadow and a little ink smudge on the edge of a settled page
    if (tin > 0.99 && tout < 0.01) for (let x = 0; x < W; x++) st.fb[(H - 1) * W + x] = scale(C.dgrey, 0.5);
  }

  function radio(p, d, part, v) {
    clear();
    const fade = Math.min(ease(p / 1.0), ease((d - p) / 1.0));
    stars(p, 0.03, st.stars.length, 0.8 * fade);
    earth(150, 170, p);
    // the ship goes over, once, from the left to the right
    const sx = (-90 + ease(p / (d - 1)) * (W + 90)) | 0, img = st.img.ship;
    if (img) blitImg(img, sx, (56 + Math.sin(p) * 4) | 0, 1, 0.9);
    // the beacon: a small scope trace that blinks the call
    const on = ((p * 3) | 0) % 5 < 2, y0 = 20;
    for (let x = 8; x < 120; x++) px(x, (y0 + Math.sin(x * 0.4 + p * 12) * (on ? 6 : 1)) | 0, scale(C.lcyan, 0.9 * fade));
    text('145.800  OH9ZQW', 8, 30, scale(C.dgrey, fade), 8);
    // a light on the night side, only if the queue left
    if (v === 'END1' && p > 6 && ((p * 1.5) | 0) % 2 === 0) { rect(246, 190, 3, 3, C.yellow); rect(245, 189, 5, 1, scale(C.yellow, 0.3)); }
    text(word(part, 'title', v), 8, 8, scale(C.lamber, fade), 9, true);
    const lines = word(part, 'text', v).split('|');
    let n = 0;
    lines.forEach((l, i) => {
      const t0 = 1.2 + i * 1.9, shown = typed(l, p - t0, 24);
      text(shown, 8, 112 + i * 12 - (lines.length > 4 ? 12 : 0), scale(i === lines.length - 1 && v === 'END1' ? C.white : C.grey, fade), 8, i === lines.length - 1 && v === 'END1');
      n++;
    });
    if (p > 1.2 + lines.length * 1.9) text(word(part, 'sign', v), 8, 112 + lines.length * 12 + 4 - (lines.length > 4 ? 12 : 0), scale(C.lcyan, fade), 8);
  }

  function credits(p, d, part) {
    clear();
    const fade = Math.min(ease(p / 1), ease((d - p) / 0.6));
    copper(p, 0, H, 0.35 * fade);
    // darken the middle so the list reads
    for (let y = 30; y < 150; y++) for (let x = 60; x < 260; x++) st.fb[y * W + x] = scale(st.fb[y * W + x], 0.25);
    ring(W / 2, 14, 9, p, 16, C.lamber);
    const lines = (part.lines || '').split('|'), speed = 14, y0 = 150 - p * speed;
    lines.forEach((l, i) => {
      const y = y0 + i * 13; if (y < 28 || y > 146) return;
      const head = lines[i - 1] === '-' || i === 0 || lines[i + 1] === '-';
      if (l === '-') return;
      const edge = ease(Math.min((y - 28) / 14, (146 - y) / 14));
      const col = head && lines[i + 1] !== undefined && /^[A-Z, ]+$/.test(l) && l.length < 30 && lines[i - 1] === '-' ? C.lamber : C.white;
      text(l, ((W - textWidth(l, 9, head)) / 2) | 0, y | 0, scale(col, edge * fade), 9, head);
    });
    // the sine scroller along the foot
    const sc = part.scroller || '', mk = mask(sc, 16, true), x0 = W - ((p * 80) % (mk.w + W));
    for (let x = 0; x < W; x++) {
      const sx = x - x0 | 0; if (sx < 0 || sx >= mk.w) continue;
      const off = Math.sin(x * 0.045 + p * 3) * 12;
      for (let j = 0; j < mk.h; j++) if (mk.m[j * mk.w + sx]) px(x, (168 + j + off) | 0, mix(C.cyan, C.white, (j / mk.h)));
    }
    text(part.tag || '', W - textWidth(part.tag || '', 8) - 6, H - 11, scale(C.dgrey, fade), 8);
    if (p > d - 1.6) for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) st.fb[y * W + x] = scale(st.fb[y * W + x], clamp((d - p) / 1.6, 0, 1));
  }

  // ---------------------------------------------------------------- the end: the picture goes out like a CRT
  function lost(q, ini) {
    clear();
    // q: seconds since the carrier dropped. 0 to 0.55 the picture collapses to a white line, then a dot, then black
    if (q < 0.55) {
      const k = q / 0.55, hgt = Math.max(1, (H * (1 - ease(Math.min(1, k * 1.15)))) | 0), y0 = (H / 2 - hgt / 2) | 0;
      for (let y = 0; y < hgt; y++) {
        const sy = Math.min(H - 1, ((y / hgt) * H) | 0);
        for (let x = 0; x < W; x++) st.fb[(y0 + y) * W + x] = hgt < 6 ? C.white : mix(st.last[sy * W + x], C.white, k * 0.6);
      }
    } else if (q < 0.85) {
      const k = (q - 0.55) / 0.3, w = Math.max(2, (W * (1 - ease(k))) | 0);
      rect(((W - w) / 2) | 0, H / 2 - 1, w, 2, scale(C.white, 1 - k * 0.5));
    } else if (q < 1.1) {
      const k = 1 - (q - 0.85) / 0.25; rect(W / 2 - 1, H / 2 - 1, 3, 3, scale(C.white, k));
    } else if (q > 1.9) {
      const a = (ini.DEMO.no_carrier || 'NO CARRIER'), b = (ini.DEMO.lost || 'CARRIER LOST. PRESS ANY KEY TO RECONNECT');
      text(a, ((W - textWidth(a, 16, true)) / 2) | 0, 80, C.white, 16, true);
      if (q > 2.8) text(b, ((W - textWidth(b, 9)) / 2) | 0, 112, C.grey, 9);
    }
  }

  // ---------------------------------------------------------------- audio: libopenmpt through a ScriptProcessor, as the music player does
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
    if (st.gain) st.gain.gain.value = on ? 0 : 0.9;
    if (st.mute) st.mute.textContent = on ? 'SOUND OFF' : 'MUTE';
  }
  function cue(url, vol) {
    try { const a = new Audio(url); a.volume = muted() ? 0 : vol; a.play().catch(() => {}); st.cues.push(a); } catch { /* no audio */ }
  }
  async function startAudio(buf) {
    const lib = await loadLib();
    const AC = window.AudioContext || window.webkitAudioContext; if (!AC) return;
    const ctx = new AC(), data = new Uint8Array(buf), p = lib._malloc(data.length);
    window.HEAPU8.set(data, p);
    const mod = lib._openmpt_module_create_from_memory2(p, data.length, 0, 0, 0, 0, 0, 0, 0);
    lib._free(p);
    if (!mod) { ctx.close(); return; }
    lib._openmpt_module_set_repeat_count(mod, 0);
    const frames = 4096, bl = lib._malloc(frames * 4), br = lib._malloc(frames * 4);
    const node = ctx.createScriptProcessor(frames, 0, 2), gain = ctx.createGain();
    gain.gain.value = muted() ? 0 : 0.9;
    node.onaudioprocess = e => {
      const L = e.outputBuffer.getChannelData(0), R = e.outputBuffer.getChannelData(1);
      if (st.stopMusic || !st.mod) { L.fill(0); R.fill(0); return; }
      const got = lib._openmpt_module_read_float_stereo(mod, ctx.sampleRate, frames, bl, br);
      const heap = window.HEAPU8.buffer;
      L.set(new Float32Array(heap, bl, frames)); R.set(new Float32Array(heap, br, frames));
      if (got < frames) { L.fill(0, got); R.fill(0, got); }
      st.audioPos = lib._openmpt_module_get_position_seconds(mod); st.audioAt = performance.now();
    };
    node.connect(gain); gain.connect(ctx.destination);
    st.ctx = ctx; st.lib = lib; st.mod = mod; st.node = node; st.gain = gain; st.bl = bl; st.br = br;
    if (ctx.state !== 'running') await ctx.resume().catch(() => {});
  }
  function stopAudio() {
    st.stopMusic = true;
    try { if (st.node) { st.node.disconnect(); st.node.onaudioprocess = null; } } catch { /* gone */ }
    try { if (st.lib && st.mod) st.lib._openmpt_module_destroy(st.mod); } catch { /* gone */ }
    st.mod = 0;
    try { if (st.ctx) st.ctx.close(); } catch { /* closed */ }
    st.cues.forEach(a => { try { a.pause(); } catch { /* gone */ } });
  }
  // the clock: the tune's own position when the audio runs, the wall clock when it does not
  function clock() {
    const wall = (performance.now() - st.t0) / 1000;
    if (st.ctx && st.ctx.state === 'running' && st.audioAt) return st.audioPos + Math.min(0.12, (performance.now() - st.audioAt) / 1000);
    return wall;
  }

  // ---------------------------------------------------------------- the loop
  function frame() {
    if (!st) return;
    st.raf = requestAnimationFrame(frame);
    const now = performance.now();
    if (now - st.lastFrame < 30) return;   // about 30 frames a second
    st.lastFrame = now;
    const ini = st.ini, v = st.variant;
    if (st.phase === 'play') {
      const t = clock();
      const part = st.parts.find(x => t >= x.start && t < x.end);
      if (!part) { if (t >= st.endT) drop(); }
      else {
        const p = t - part.start, d = part.end - part.start;
        ({ release, title, letter, news, radio, credits }[part.kind] || (() => clear()))(p, d, part, v);
        crt(st.fb);
      }
    } else if (st.phase === 'lost') {
      if (!st.lostAt) st.lostAt = now;
      lost((now - st.lostAt) / 1000, ini);
    }
    st.ctx2d.putImageData(st.image, 0, 0);
  }
  function drop() {   // the carrier drops: the music cuts, the picture goes out
    if (st.phase !== 'play') return;
    st.phase = 'lost'; st.lostAt = 0;
    stopAudio();
    if (st.skip) st.skip.hidden = true;
  }
  function finish() {
    if (!st) return;
    const s = st; st = null;
    cancelAnimationFrame(s.raf);
    s.stopMusic = true;
    try { if (s.node) { s.node.disconnect(); s.node.onaudioprocess = null; } } catch { /* gone */ }
    try { if (s.lib && s.mod) s.lib._openmpt_module_destroy(s.mod); } catch { /* gone */ }
    try { if (s.ctx) s.ctx.close(); } catch { /* closed */ }
    s.cues.forEach(a => { try { a.pause(); } catch { /* gone */ } });
    document.removeEventListener('keydown', s.onKey, true);
    s.root.remove();
    s.onDone();
  }
  function onKey(e) {
    if (!st || e.isComposing) return;
    if (['Shift', 'Control', 'Alt', 'Meta', 'CapsLock'].includes(e.key)) return;
    e.preventDefault(); e.stopPropagation();
    if (st.phase === 'lost') { if (st.lostAt && performance.now() - st.lostAt > 2800) finish(); return; }
    const k = e.key.toLowerCase();
    if (k === 'q' || k === 'escape') drop();
    else if (k === 'm') setMute(!muted());
  }

  function loadImage(name) {
    return new Promise(ok => {
      const im = new Image();
      im.onload = () => {
        const c = document.createElement('canvas'); c.width = im.width; c.height = im.height;
        const x = c.getContext('2d', { willReadFrequently: true }); x.drawImage(im, 0, 0);
        ok({ canvas: { width: im.width, height: im.height, data: x.getImageData(0, 0, im.width, im.height).data } });
      };
      im.onerror = () => ok(null);
      im.src = BASE + name;
    });
  }

  async function run(opts) {
    if (st) return;
    const variant = opts.variant === 'END2' ? 'END2' : 'END1';
    const root = document.createElement('div');
    root.style.cssText = 'position:fixed;inset:0;z-index:40;background:#000;display:flex;align-items:center;justify-content:center;flex-direction:column;font:14px monospace;color:#aaa';
    const cv = document.createElement('canvas'); cv.width = W; cv.height = H;
    cv.style.cssText = 'width:min(100vw,160vh);height:auto;image-rendering:pixelated;background:#000';
    const bar = document.createElement('div'); bar.style.cssText = 'position:absolute;left:0;right:0;bottom:4px;display:flex;gap:8px;justify-content:center;opacity:.8';
    const mk = (label, fn) => { const b = document.createElement('button'); b.textContent = label; b.style.cssText = 'background:#000;color:#5ff;border:1px solid #066;font:inherit;min-height:32px;padding:0 10px'; b.onclick = e => { e.stopPropagation(); fn(); }; bar.appendChild(b); return b; };
    root.append(cv, bar);
    document.body.appendChild(root);
    const ctx2d = cv.getContext('2d', { alpha: false }), image = ctx2d.createImageData(W, H);
    st = { root, ctx2d, image, fb: new Uint32Array(image.data.buffer), last: new Uint32Array(W * H), variant, phase: 'load', cues: [],
      onDone: opts.onDone || (() => {}), lastFrame: 0, t0: performance.now(), stopMusic: false, img: {}, stars: [] };
    st.skip = mk('SKIP (Q)', () => drop());
    st.mute = mk(muted() ? 'SOUND OFF' : 'MUTE', () => setMute(!muted()));
    root.addEventListener('click', () => { if (st && st.phase === 'lost' && st.lostAt && performance.now() - st.lostAt > 2800) finish(); });
    st.onKey = onKey; document.addEventListener('keydown', onKey, true);
    clear(); text('LOADING', 8, 8, C.dgrey, 9); ctx2d.putImageData(image, 0, 0);
    let seed = 3; const rnd = () => (seed = (seed * 1103515245 + 12345) & 0x7fffffff) / 0x7fffffff;
    for (let i = 0; i < 160; i++) st.stars.push({ x: (rnd() - 0.5) * 520, y: (rnd() - 0.5) * 330, z: rnd() });
    try {
      const ini = parseIni(await (await fetch(BASE + 'DEMO.INI', { cache: 'no-store' })).text());
      st.ini = ini;
      st.parts = Object.keys(ini).filter(k => /^P\d/.test(k)).sort().map(k => Object.assign({ kind: '', start: 0, end: 0 }, ini[k], { start: +ini[k].start, end: +ini[k].end }));
      st.endT = Math.max(...st.parts.map(p => p.end));
      const faces = [...new Set(st.parts.map(p => p.face).filter(Boolean))].concat('SHIP.PNG');
      await Promise.all(faces.map(async f => { const im = await loadImage(f); st.img[f === 'SHIP.PNG' ? 'ship' : f] = im; }));
      const tune = await (await fetch(BASE + (ini.DEMO.tune || 'TUNE.MOD'))).arrayBuffer();
      await startAudio(tune).catch(() => {});
    } catch (e) { finish(); return; }
    if (!st) return;
    st.t0 = performance.now(); st.phase = 'play';
    st.raf = requestAnimationFrame(frame);
  }
  window.HalcyonDemo = { run };
})();