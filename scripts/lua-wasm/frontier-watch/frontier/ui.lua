local ui = {}

local alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
local cached_page_url

local html = [==[
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
  :root { color-scheme: dark; --paper:#e9d8b4; --ink:#1b120c; --rust:#a8492b; --gold:#d7ad65; }
  * { box-sizing:border-box; user-select:none; }
  html,body { width:100%; height:100%; margin:0; overflow:hidden; background:transparent; }
  body { display:flex; align-items:center; justify-content:flex-end; padding:5vh 4vw; font-family:Georgia,'Times New Roman',serif; }
  .shade { position:fixed; inset:0; background:linear-gradient(90deg,transparent 18%,rgba(6,4,3,.12) 48%,rgba(6,4,3,.64)); animation:shade-in .32s ease-out both; }
  .ledger { position:relative; width:min(540px,42vw); min-width:430px; padding:42px 42px 34px; color:var(--ink); border:1px solid rgba(255,236,191,.42); border-radius:5px 18px 18px 5px; background:
    linear-gradient(90deg,rgba(84,42,22,.15),transparent 7%),
    repeating-linear-gradient(0deg,rgba(90,50,25,.035) 0,rgba(90,50,25,.035) 1px,transparent 1px,transparent 5px),
    var(--paper);
    box-shadow:-18px 28px 75px rgba(0,0,0,.55),inset 0 0 80px rgba(91,43,17,.13); animation:open .42s cubic-bezier(.2,.85,.25,1) both; }
  .ledger:before { content:''; position:absolute; inset:11px; border:1px solid rgba(78,39,20,.22); pointer-events:none; }
  .topline { display:flex; justify-content:space-between; align-items:center; gap:20px; color:#68412c; font:700 11px/1.2 'Segoe UI',sans-serif; letter-spacing:.22em; text-transform:uppercase; }
  h1 { margin:16px 0 3px; font-size:48px; font-weight:500; letter-spacing:-.035em; }
  .rule { display:flex; align-items:center; gap:12px; margin:14px 0 28px; color:var(--rust); font:700 12px 'Segoe UI',sans-serif; letter-spacing:.14em; }
  .rule:before,.rule:after { content:''; height:1px; flex:1; background:linear-gradient(90deg,transparent,var(--rust)); }
  .rule:after { background:linear-gradient(90deg,var(--rust),transparent); }
  .clock { display:grid; grid-template-columns:1.4fr 1fr; gap:12px; }
  .card { padding:18px 19px; border:1px solid rgba(74,37,18,.18); background:rgba(255,250,235,.34); box-shadow:inset 0 1px rgba(255,255,255,.35); }
  .card strong { display:block; color:#2e1c12; font:600 27px/1.05 'Segoe UI',sans-serif; letter-spacing:.04em; }
  .card span { display:block; margin-top:7px; color:#79553e; font:700 10px 'Segoe UI',sans-serif; letter-spacing:.16em; text-transform:uppercase; }
  .stats { display:grid; grid-template-columns:repeat(3,1fr); margin:12px 0 22px; border:1px solid rgba(74,37,18,.18); }
  .stat { padding:15px 10px; text-align:center; border-right:1px solid rgba(74,37,18,.16); }
  .stat:last-child { border-right:0; }
  .stat b { display:block; font:700 20px 'Segoe UI',sans-serif; }
  .stat small { color:#79553e; font:700 9px 'Segoe UI',sans-serif; letter-spacing:.12em; text-transform:uppercase; }
  .status { min-height:44px; margin:0 0 20px; padding:12px 14px; border-left:3px solid var(--rust); color:#4f3020; background:rgba(168,73,43,.08); font-style:italic; line-height:1.35; }
  .actions { display:grid; grid-template-columns:1fr 1fr; gap:10px; }
  button { appearance:none; padding:13px 12px; border:1px solid #60351f; color:#f4e5c8; background:linear-gradient(#73412a,#492717); font:700 11px 'Segoe UI',sans-serif; letter-spacing:.13em; text-transform:uppercase; cursor:pointer; transition:.15s ease; }
  button:hover { transform:translateY(-1px); background:linear-gradient(#985237,#5e2e1b); box-shadow:0 6px 18px rgba(72,31,15,.22); }
  button.secondary { color:#50311e; background:transparent; }
  button.secondary:hover { background:rgba(96,53,31,.08); }
  .footer { display:flex; justify-content:space-between; margin-top:22px; color:#80614b; font:700 9px 'Segoe UI',sans-serif; letter-spacing:.12em; text-transform:uppercase; }
  kbd { padding:3px 7px; border:1px solid rgba(76,38,19,.35); border-bottom-width:2px; border-radius:3px; color:#492818; background:rgba(255,255,255,.3); font:700 10px 'Segoe UI',sans-serif; }
  @keyframes open { from { opacity:0; transform:translateX(50px) rotate(.6deg); } to { opacity:1; transform:none; } }
  @keyframes shade-in { from { opacity:0; } to { opacity:1; } }
</style>
</head>
<body>
<div class="shade"></div>
<main class="ledger">
  <div class="topline"><span>Arthur's field ledger</span><span id="binding">F9</span></div>
  <h1>Frontier Watch</h1>
  <div class="rule">Live trail record</div>
  <section class="clock">
    <div class="card"><strong id="session">00:00:00</strong><span>Time on the trail</span></div>
    <div class="card"><strong id="game-time">00:00</strong><span>Game clock</span></div>
  </section>
  <section class="stats">
    <div class="stat"><b id="marks">0</b><small>Trail marks</small></div>
    <div class="stat"><b id="ticks">0</b><small>Lua ticks</small></div>
    <div class="stat"><b id="cursor">0</b><small>Cursor refs</small></div>
  </section>
  <p class="status" id="status">Opening the ledger...</p>
  <div class="actions">
    <button id="mark">Mark this trail</button>
    <button class="secondary" id="reset">Begin new page</button>
  </div>
  <div class="footer"><span>Sandboxed Lua · Wasmtime</span><span><kbd>Esc</kbd> close</span></div>
</main>
<script>
  const $ = id => document.getElementById(id);
  const send = action => window.chrome.webview.postMessage({ action });
  $('mark').addEventListener('click', () => send('mark'));
  $('reset').addEventListener('click', () => send('reset'));
  addEventListener('keydown', event => { if (event.key === 'Escape') send('close'); });
  window.chrome.webview.addEventListener('message', event => {
    const data = event.data || {};
    if (data.kind !== 'state') return;
    $('session').textContent = data.session;
    $('game-time').textContent = data.gameTime;
    $('marks').textContent = data.trailMarks;
    $('ticks').textContent = Number(data.ticks).toLocaleString();
    $('cursor').textContent = data.cursorRefs;
    $('binding').textContent = data.binding || 'F9';
    $('status').textContent = data.status;
  });
  send('ready');
</script>
</body>
</html>
]==]

local function base64_encode(data)
    local output = {}
    local output_length = 0

    for index = 1, #data, 3 do
        local a, b, c = string.byte(data, index, index + 2)
        b = b or 0
        c = c or 0
        local packed = (a << 16) | (b << 8) | c
        local remaining = #data - index + 1

        output_length = output_length + 1
        output[output_length] = alphabet:sub(((packed >> 18) & 63) + 1, ((packed >> 18) & 63) + 1)
        output_length = output_length + 1
        output[output_length] = alphabet:sub(((packed >> 12) & 63) + 1, ((packed >> 12) & 63) + 1)
        output_length = output_length + 1
        output[output_length] = remaining >= 2
            and alphabet:sub(((packed >> 6) & 63) + 1, ((packed >> 6) & 63) + 1)
            or '='
        output_length = output_length + 1
        output[output_length] = remaining >= 3
            and alphabet:sub((packed & 63) + 1, (packed & 63) + 1)
            or '='
    end

    return table.concat(output)
end

function ui.page_url()
    if not cached_page_url then
        cached_page_url = 'data:text/html;base64,' .. base64_encode(html)
    end
    return cached_page_url
end

return ui
