local json = require('frontier.json')
local state = require('frontier.state')
local ui = require('frontier.ui')

local binding = input_register_binding(
    'toggle_frontier_watch',
    'Open/close Frontier Watch',
    'keyboard',
    'F9')

local model = state.new(game_time())
local visible = false
local cursor_held = false
local last_publish = -1000
local page_url = ui.page_url() -- prewarm encoding so the first open is instant

local function binding_parameter()
    local parameter = input_get_binding_parameter(binding)
    if type(parameter) == 'string' and #parameter > 0 then
        return parameter
    end
    return 'F9'
end

local function publish(force)
    if not visible or not webview_is_open() then return end

    local now = game_time()
    if not force and now - last_publish < 250 then return end
    last_publish = now

    local snapshot = state.snapshot(
        model,
        now,
        binding_parameter(),
        webview_cursor_ref_count())
    local status = webview_post_json(json.state(snapshot))
    if status ~= 0 and force then
        log('Frontier Watch could not publish WebView state', 1)
    end
end

local function hide()
    if not visible and not cursor_held then return end

    webview_set_focus(false)
    if cursor_held then
        webview_hide_cursor()
        cursor_held = false
    end
    webview_set_visible(false)
    visible = false
    log('Frontier Watch closed')
end

local function show()
    local status
    if webview_is_open() then
        status = webview_set_visible(true)
    else
        status = webview_open(page_url, 0, 0)
    end
    if status ~= 0 then
        log('Frontier Watch could not open its WebView', 2)
        return
    end

    if webview_set_focus(true) ~= 0 then
        webview_set_visible(false)
        log('Frontier Watch could not focus its WebView', 2)
        return
    end
    if webview_show_cursor() ~= 0 then
        webview_set_focus(false)
        webview_set_visible(false)
        log('Frontier Watch could not acquire the game cursor', 2)
        return
    end

    cursor_held = true
    visible = true
    model.status = 'The ledger is live. Mark anything worth remembering.'
    publish(true)
    log('Frontier Watch opened')
end

local function toggle()
    if visible then hide() else show() end
end

local function handle_webview_message(message)
    local action = message:match('"action"%s*:%s*"([^"\\]+)"')
    if action == 'close' then
        hide()
    elseif action == 'mark' then
        state.mark_trail(model)
        publish(true)
    elseif action == 'reset' then
        state.reset(model, game_time())
        publish(true)
    elseif action == 'ready' then
        publish(true)
    end
end

register_event('tick', function()
    model.ticks = model.ticks + 1

    while true do
        local event = input_poll_binding_event(binding)
        if event == 0 then break end
        if event == 1 then toggle() end
    end

    if visible then
        for _ = 1, 8 do
            local message = webview_poll_json()
            if type(message) ~= 'string' or #message == 0 then break end
            handle_webview_message(message)
        end
        publish(false)
    end
end)

register_event('shutdown', function()
    hide()
    if webview_is_open() then webview_close() end
    log('Frontier Watch shutting down')
end)

log('Frontier Watch initialized; use the configured binding to open it')
