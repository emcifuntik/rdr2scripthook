use crate::catalog::{Action, ModelPurpose, Screen};

use std::fmt::Write;

const SECTIONS: [Screen; 8] = [
    Screen::Player,
    Screen::Horses,
    Screen::PlayerModels,
    Screen::Peds,
    Screen::Vehicles,
    Screen::Teleports,
    Screen::Weather,
    Screen::Time,
];

pub enum Message {
    Ready,
    Close,
    Activate { screen: Screen, index: usize },
}

pub fn parse_message(json: &str) -> Option<Message> {
    match string_field(json, "type")? {
        "ready" => Some(Message::Ready),
        "close" => Some(Message::Close),
        "activate" => {
            let screen = Screen::from_index(number_field(json, "screen")?)?;
            let index = number_field(json, "index")?;
            Some(Message::Activate { screen, index })
        }
        _ => None,
    }
}

pub fn page_url(invincible: bool) -> String {
    let mut html = String::with_capacity(96 * 1024);
    html.push_str(PAGE_PREFIX);
    write_catalog(&mut html);
    html.push_str(";const bootstrap={invincible:");
    html.push_str(if invincible { "true" } else { "false" });
    html.push_str("};");
    html.push_str(PAGE_SUFFIX);
    data_url(&html)
}

pub fn status_json(text: &str, error: bool) -> String {
    let mut json = String::from("{\"type\":\"status\",\"text\":");
    push_json_string(&mut json, text);
    json.push_str(",\"error\":");
    json.push_str(if error { "true" } else { "false" });
    json.push('}');
    json
}

pub fn state_json(invincible: bool) -> String {
    format!(
        "{{\"type\":\"state\",\"invincible\":{}}}",
        if invincible { "true" } else { "false" }
    )
}

fn write_catalog(output: &mut String) {
    output.push('[');
    for (section_index, screen) in SECTIONS.into_iter().enumerate() {
        if section_index != 0 {
            output.push(',');
        }

        output.push_str("{\"id\":");
        let _ = write!(output, "{}", screen.index());
        output.push_str(",\"name\":");
        push_json_string(output, section_name(screen));
        output.push_str(",\"kicker\":");
        push_json_string(output, section_kicker(screen));
        output.push_str(",\"description\":");
        push_json_string(output, section_description(screen));
        output.push_str(",\"mark\":");
        push_json_string(output, section_mark(screen));
        output.push_str(",\"items\":[");

        for index in 0..screen.len() {
            let Some(entry) = screen.entry(index) else {
                continue;
            };
            if index != 0 {
                output.push(',');
            }
            output.push_str("{\"index\":");
            let _ = write!(output, "{index}");
            output.push_str(",\"label\":");
            push_json_string(output, entry.label);
            output.push_str(",\"tag\":");
            push_json_string(output, action_tag(entry.action));
            output.push_str(",\"detail\":");
            push_json_string(output, action_detail(entry.action));
            output.push_str(",\"toggle\":");
            output.push_str(if matches!(entry.action, Action::ToggleInvincibility) {
                "true"
            } else {
                "false"
            });
            output.push('}');
        }
        output.push_str("]}");
    }
    output.push(']');
}

const fn section_name(screen: Screen) -> &'static str {
    match screen {
        Screen::Player => "Player",
        Screen::Horses => "Horses",
        Screen::PlayerModels => "Characters",
        Screen::Peds => "Pedestrians",
        Screen::Vehicles => "Vehicles",
        Screen::Teleports => "Travel",
        Screen::Weather => "Weather",
        Screen::Time => "Time",
    }
}

const fn section_kicker(screen: Screen) -> &'static str {
    match screen {
        Screen::Player => "SURVIVAL",
        Screen::Horses => "STABLE",
        Screen::PlayerModels => "IDENTITY",
        Screen::Peds => "POPULATION",
        Screen::Vehicles => "TRANSPORT",
        Screen::Teleports => "DESTINATIONS",
        Screen::Weather => "ATMOSPHERE",
        Screen::Time => "CLOCK",
    }
}

const fn section_description(screen: Screen) -> &'static str {
    match screen {
        Screen::Player => "Restore vitals and control player protection.",
        Screen::Horses => "Summon a selected horse beside the player.",
        Screen::PlayerModels => "Change the active player character model.",
        Screen::Peds => "Spawn ambient characters into the world.",
        Screen::Vehicles => "Create wagons, boats, trains, and specialist vehicles.",
        Screen::Teleports => "Move instantly to a landmark or settlement.",
        Screen::Weather => "Apply a new weather state to the world.",
        Screen::Time => "Advance the in-game clock.",
    }
}

const fn section_mark(screen: Screen) -> &'static str {
    match screen {
        Screen::Player => "PL",
        Screen::Horses => "HR",
        Screen::PlayerModels => "CH",
        Screen::Peds => "PD",
        Screen::Vehicles => "VH",
        Screen::Teleports => "TR",
        Screen::Weather => "WX",
        Screen::Time => "TM",
    }
}

const fn action_tag(action: Action) -> &'static str {
    match action {
        Action::LoadModel {
            purpose: ModelPurpose::ChangePlayer,
            ..
        } => "APPLY",
        Action::LoadModel { .. } => "SPAWN",
        Action::Teleport { .. } => "TRAVEL",
        Action::SetWeather(_) => "WORLD",
        Action::AddTime { .. } => "CLOCK",
        Action::Heal | Action::RefillStamina => "RESTORE",
        Action::ToggleInvincibility => "TOGGLE",
    }
}

const fn action_detail(action: Action) -> &'static str {
    match action {
        Action::LoadModel {
            purpose: ModelPurpose::SpawnHorse,
            ..
        } => "Spawn nearby",
        Action::LoadModel {
            purpose: ModelPurpose::ChangePlayer,
            ..
        } => "Use this appearance",
        Action::LoadModel {
            purpose: ModelPurpose::SpawnPed,
            ..
        } => "Spawn and wander",
        Action::LoadModel {
            purpose: ModelPurpose::SpawnVehicle,
            ..
        } => "Replace last vehicle",
        Action::Teleport { .. } => "Teleport now",
        Action::SetWeather(_) => "Set weather",
        Action::AddTime { .. } => "Advance time",
        Action::Heal => "Fill health core",
        Action::RefillStamina => "Fill stamina core",
        Action::ToggleInvincibility => "Persistent protection",
    }
}

fn value_start<'a>(json: &'a str, name: &str) -> Option<&'a str> {
    let needle = format!("\"{name}\"");
    let after_name = json.get(json.find(&needle)? + needle.len()..)?;
    let after_colon = after_name.get(after_name.find(':')? + 1..)?;
    Some(after_colon.trim_start())
}

fn string_field<'a>(json: &'a str, name: &str) -> Option<&'a str> {
    let value = value_start(json, name)?.strip_prefix('"')?;
    let end = value.find('"')?;
    value.get(..end)
}

fn number_field(json: &str, name: &str) -> Option<usize> {
    let value = value_start(json, name)?;
    let digits = value
        .as_bytes()
        .iter()
        .take_while(|byte| byte.is_ascii_digit())
        .count();
    if digits == 0 {
        return None;
    }
    value.get(..digits)?.parse().ok()
}

fn push_json_string(output: &mut String, value: &str) {
    output.push('"');
    for character in value.chars() {
        match character {
            '"' => output.push_str("\\\""),
            '\\' => output.push_str("\\\\"),
            '\n' => output.push_str("\\n"),
            '\r' => output.push_str("\\r"),
            '\t' => output.push_str("\\t"),
            character if character.is_control() => {
                let _ = write!(output, "\\u{:04X}", character as u32);
            }
            character => output.push(character),
        }
    }
    output.push('"');
}

fn data_url(html: &str) -> String {
    const HEX: &[u8; 16] = b"0123456789ABCDEF";
    let mut result = String::with_capacity(html.len() * 2);
    result.push_str("data:text/html;charset=utf-8,");
    for byte in html.bytes() {
        if byte.is_ascii_alphanumeric() || matches!(byte, b'-' | b'_' | b'.' | b'~') {
            result.push(byte as char);
        } else {
            result.push('%');
            result.push(HEX[(byte >> 4) as usize] as char);
            result.push(HEX[(byte & 0x0F) as usize] as char);
        }
    }
    result
}

const PAGE_PREFIX: &str = r#"<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
:root{color-scheme:dark;font-family:"Segoe UI",system-ui,sans-serif;font-size:clamp(15px,.55vw,21px);--ink:#f4ead7;--muted:#aaa293;--gold:#d2a85d;--gold2:#8d642d;--red:#9d3f35;--panel:rgba(16,17,18,.96);--line:rgba(226,200,154,.14)}
*{box-sizing:border-box}html,body{width:100%;height:100%;margin:0;overflow:hidden;background:transparent;color:var(--ink)}
body{display:grid;place-items:center;background:radial-gradient(circle at 72% 18%,rgba(139,88,35,.14),transparent 36%),rgba(4,5,6,.44);user-select:none}
button,input{font:inherit}.shell{width:min(86vw,2160px);height:min(84vh,1380px);display:grid;grid-template-columns:clamp(235px,20vw,460px) 1fr;overflow:hidden;border:1px solid rgba(230,202,153,.22);border-radius:18px;background:linear-gradient(145deg,rgba(25,24,22,.98),rgba(10,11,12,.98));box-shadow:0 34px 110px rgba(0,0,0,.72),inset 0 1px rgba(255,255,255,.035)}
.sidebar{position:relative;display:flex;min-width:0;flex-direction:column;padding:32px 22px 22px;border-right:1px solid var(--line);background:linear-gradient(180deg,rgba(58,42,25,.26),transparent 34%)}
.brand{display:flex;gap:15px;align-items:center;padding:0 10px 27px}.brand-mark{display:grid;width:48px;height:48px;place-items:center;border:1px solid rgba(210,168,93,.5);border-radius:50%;color:#17130d;background:var(--gold);font-family:Georgia,serif;font-weight:900;box-shadow:0 0 0 5px rgba(210,168,93,.08)}
.brand-copy small,.eyebrow{display:block;color:var(--gold);font-size:.68rem;font-weight:800;letter-spacing:.24em}.brand-copy strong{display:block;margin-top:4px;font-family:Georgia,serif;font-size:1.25rem;letter-spacing:.025em}
.nav{display:flex;flex:1;min-height:0;flex-direction:column;gap:5px;overflow:auto;padding:3px}.nav button{display:grid;grid-template-columns:39px 1fr auto;gap:12px;align-items:center;width:100%;padding:11px 12px;border:1px solid transparent;border-radius:10px;color:#bdb6aa;background:transparent;text-align:left;cursor:pointer;transition:.15s ease}
.nav button:hover{color:var(--ink);background:rgba(255,255,255,.035)}.nav button.active{color:var(--ink);border-color:rgba(210,168,93,.2);background:linear-gradient(90deg,rgba(210,168,93,.16),rgba(210,168,93,.035))}.nav-mark{display:grid;width:35px;height:35px;place-items:center;border:1px solid rgba(255,255,255,.09);border-radius:8px;color:#8e877b;font-size:.62rem;font-weight:900;letter-spacing:.08em}.active .nav-mark{border-color:rgba(210,168,93,.48);color:var(--gold)}.nav-count{color:#716c63;font-size:.72rem;font-variant-numeric:tabular-nums}
.hint{margin:18px 5px 0;padding:15px 14px;border-top:1px solid var(--line);color:#77736c;font-size:.68rem;line-height:1.7}.key{display:inline-grid;min-width:25px;height:22px;place-items:center;margin-right:4px;padding:0 6px;border:1px solid rgba(255,255,255,.12);border-radius:5px;color:#bcb5aa;background:#171819;box-shadow:inset 0 -2px rgba(0,0,0,.4)}
.main{display:flex;min-width:0;min-height:0;flex-direction:column}.topbar{display:flex;align-items:center;gap:18px;padding:22px 28px;border-bottom:1px solid var(--line)}.search{position:relative;flex:1}.search input{width:100%;padding:12px 44px 12px 16px;outline:0;border:1px solid rgba(255,255,255,.09);border-radius:10px;color:var(--ink);background:rgba(0,0,0,.2);transition:.15s}.search input:focus{border-color:rgba(210,168,93,.54);box-shadow:0 0 0 3px rgba(210,168,93,.08)}.search span{position:absolute;right:15px;top:50%;color:#716c63;transform:translateY(-50%)}
.close{width:42px;height:42px;border:1px solid rgba(255,255,255,.1);border-radius:10px;color:#aca59a;background:rgba(255,255,255,.025);cursor:pointer}.close:hover{border-color:rgba(157,63,53,.6);color:white;background:rgba(157,63,53,.28)}
.content{display:flex;min-height:0;flex:1;flex-direction:column;padding:30px 32px 24px}.heading{display:flex;align-items:flex-end;justify-content:space-between;gap:24px;margin-bottom:24px}.heading h1{margin:6px 0 7px;font-family:Georgia,"Times New Roman",serif;font-size:2.1rem;font-weight:500;letter-spacing:.012em}.heading p{max-width:690px;margin:0;color:var(--muted);font-size:.87rem}.result-count{padding-bottom:5px;color:#777169;font-size:.75rem;white-space:nowrap}
.actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px;overflow:auto;padding:2px 8px 26px 2px;scrollbar-color:#655237 transparent}.action{position:relative;display:grid;grid-template-columns:1fr auto;gap:6px 18px;min-height:76px;padding:15px 17px;outline:0;border:1px solid rgba(255,255,255,.075);border-radius:11px;color:var(--ink);background:linear-gradient(135deg,rgba(255,255,255,.045),rgba(255,255,255,.018));text-align:left;cursor:pointer;transition:transform .13s,border-color .13s,background .13s}.action:hover,.action.selected{z-index:1;border-color:rgba(210,168,93,.48);background:linear-gradient(135deg,rgba(210,168,93,.12),rgba(255,255,255,.025));transform:translateY(-1px)}.action.pending{pointer-events:none;opacity:.58}.action-title{overflow:hidden;font-weight:650;text-overflow:ellipsis;white-space:nowrap}.action-detail{grid-column:1;color:#888279;font-size:.72rem}.tag{grid-column:2;grid-row:1/3;align-self:center;padding:5px 7px;border:1px solid rgba(210,168,93,.18);border-radius:5px;color:#9f8b68;font-size:.56rem;font-weight:800;letter-spacing:.13em}.action.toggle-on{border-color:rgba(210,168,93,.65);background:linear-gradient(135deg,rgba(210,168,93,.21),rgba(104,73,32,.08))}.action.toggle-on .tag{color:#21180d;background:var(--gold)}
.empty{grid-column:1/-1;display:grid;min-height:260px;place-items:center;color:#777169;text-align:center}.toast{position:absolute;right:max(4vw,32px);bottom:max(4vh,28px);display:grid;grid-template-columns:8px 1fr;gap:13px;align-items:center;max-width:min(520px,80vw);padding:15px 19px;border:1px solid rgba(210,168,93,.28);border-radius:11px;background:rgba(15,16,17,.97);box-shadow:0 18px 54px rgba(0,0,0,.55);opacity:0;transform:translateY(18px);pointer-events:none;transition:.22s}.toast.show{opacity:1;transform:none}.toast-dot{width:8px;height:8px;border-radius:50%;background:var(--gold);box-shadow:0 0 14px var(--gold)}.toast.error{border-color:rgba(187,76,61,.5)}.toast.error .toast-dot{background:#c55245;box-shadow:0 0 14px #c55245}
@media(max-width:900px){.shell{width:94vw;height:92vh;grid-template-columns:210px 1fr}.actions{grid-template-columns:1fr}.content{padding:24px 22px}.sidebar{padding:24px 14px 16px}}
</style>
</head>
<body>
<section class="shell" aria-label="RDR2 trainer">
  <aside class="sidebar">
    <div class="brand"><div class="brand-mark">R</div><div class="brand-copy"><small>SCRIPT HOOK</small><strong>Frontier Trainer</strong></div></div>
    <nav class="nav" id="nav"></nav>
    <div class="hint"><span class="key">F3</span> close &nbsp; <span class="key">Esc</span> close<br><span class="key">Ctrl F</span> search &nbsp; <span class="key">Enter</span> select</div>
  </aside>
  <main class="main">
    <div class="topbar"><label class="search"><input id="search" type="search" autocomplete="off" spellcheck="false" placeholder="Search this category"><span>/</span></label><button class="close" id="close" title="Close trainer">&#10005;</button></div>
    <div class="content"><header class="heading"><div><span class="eyebrow" id="kicker"></span><h1 id="title"></h1><p id="description"></p></div><span class="result-count" id="count"></span></header><div class="actions" id="actions"></div></div>
  </main>
</section>
<div class="toast" id="toast"><i class="toast-dot"></i><span id="toastText"></span></div>
<script>
const sections="#;

const PAGE_SUFFIX: &str = r#"
let active=0,selected=0,query='',toastTimer=0,state={invincible:bootstrap.invincible};
const nav=document.querySelector('#nav'),actions=document.querySelector('#actions'),search=document.querySelector('#search');
const title=document.querySelector('#title'),kicker=document.querySelector('#kicker'),description=document.querySelector('#description'),count=document.querySelector('#count');
const toast=document.querySelector('#toast'),toastText=document.querySelector('#toastText');
const current=()=>sections[active];
const visibleItems=()=>current().items.filter(item=>item.label.toLowerCase().includes(query));
function send(message){window.chrome.webview.postMessage(message)}
function renderNav(){nav.replaceChildren(...sections.map((section,index)=>{const button=document.createElement('button');button.className=index===active?'active':'';button.innerHTML=`<span class="nav-mark">${section.mark}</span><span>${section.name}</span><span class="nav-count">${section.items.length}</span>`;button.onclick=()=>selectSection(index);return button}))}
function selectSection(index){active=(index+sections.length)%sections.length;selected=0;query='';search.value='';render()}
function render(){renderNav();const section=current(),items=visibleItems();kicker.textContent=section.kicker;title.textContent=section.name;description.textContent=section.description;count.textContent=`${items.length} ${items.length===1?'action':'actions'}`;actions.replaceChildren();if(!items.length){const empty=document.createElement('div');empty.className='empty';empty.innerHTML='<span>No matching actions.<br>Try another search.</span>';actions.append(empty);return}selected=Math.min(selected,items.length-1);items.forEach((item,index)=>{const button=document.createElement('button');const enabled=item.toggle&&state.invincible;button.className='action'+(index===selected?' selected':'')+(enabled?' toggle-on':'');button.dataset.index=String(index);const label=item.toggle?`Invincibility: ${state.invincible?'ON':'OFF'}`:item.label;button.innerHTML=`<span class="action-title"></span><span class="action-detail"></span><span class="tag">${item.toggle?(state.invincible?'ON':'OFF'):item.tag}</span>`;button.querySelector('.action-title').textContent=label;button.querySelector('.action-detail').textContent=item.detail;button.onmouseenter=()=>{selected=index;syncSelection()};button.onclick=()=>activate(item,button);actions.append(button)})}
function syncSelection(){const buttons=[...actions.querySelectorAll('.action')];buttons.forEach((button,index)=>button.classList.toggle('selected',index===selected));buttons[selected]?.scrollIntoView({block:'nearest'})}
function activate(item,button){button?.classList.add('pending');send({type:'activate',screen:current().id,index:item.index})}
function showToast(text,error=false){clearTimeout(toastTimer);toastText.textContent=text;toast.classList.toggle('error',!!error);toast.classList.add('show');toastTimer=setTimeout(()=>toast.classList.remove('show'),4200);actions.querySelectorAll('.pending').forEach(button=>button.classList.remove('pending'))}
function closeTrainer(){send({type:'close'})}
search.addEventListener('input',()=>{query=search.value.trim().toLowerCase();selected=0;render()});
document.querySelector('#close').onclick=closeTrainer;
document.addEventListener('contextmenu',event=>event.preventDefault());
document.addEventListener('keydown',event=>{if(event.key==='Escape'){event.preventDefault();closeTrainer();return}if(event.ctrlKey&&event.key.toLowerCase()==='f'){event.preventDefault();search.focus();search.select();return}if(document.activeElement===search&&event.key!=='ArrowDown'&&event.key!=='ArrowUp'&&event.key!=='Enter')return;const items=visibleItems();if(event.key==='ArrowDown'){event.preventDefault();selected=Math.min(selected+1,Math.max(0,items.length-1));syncSelection()}else if(event.key==='ArrowUp'){event.preventDefault();selected=Math.max(0,selected-1);syncSelection()}else if(event.key==='ArrowRight'){event.preventDefault();selectSection(active+1)}else if(event.key==='ArrowLeft'){event.preventDefault();selectSection(active-1)}else if(event.key==='Enter'&&items[selected]){event.preventDefault();activate(items[selected],actions.querySelectorAll('.action')[selected])}});
window.chrome.webview.addEventListener('message',event=>{const message=event.data||{};if(message.type==='status')showToast(message.text,Boolean(message.error));if(message.type==='state'){state.invincible=Boolean(message.invincible);render()}});
render();send({type:'ready'});
</script>
</body>
</html>"#;
