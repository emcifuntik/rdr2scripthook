local PAGE_URL = "data:text/html;base64,PCFkb2N0eXBlIGh0bWw+PGh0bWw+PGhlYWQ+PG1ldGEgY2hhcnNldD0idXRmLTgiPjxzdHlsZT4qe2JveC1zaXppbmc6Ym9yZGVyLWJveH1odG1sLGJvZHl7bWFyZ2luOjA7d2lkdGg6MTAwJTtoZWlnaHQ6MTAwJTtvdmVyZmxvdzpoaWRkZW47YmFja2dyb3VuZDp0cmFuc3BhcmVudDtmb250LWZhbWlseTpHZW9yZ2lhLHNlcmlmO2NvbG9yOiNmZmY3ZWR9Ym9keXtkaXNwbGF5OmdyaWQ7cGxhY2UtaXRlbXM6Y2VudGVyO2JhY2tncm91bmQ6cmFkaWFsLWdyYWRpZW50KGNpcmNsZSBhdCA1MCUgNDUlLHJnYmEoMTIwLDUzLDE1LC4yKSxyZ2JhKDgsNSwzLC43OCkpfS5wYW5lbHtwb3NpdGlvbjpyZWxhdGl2ZTt3aWR0aDptaW4oODIwcHgsODJ2dyk7cGFkZGluZzo1NHB4O2JvcmRlcjoxcHggc29saWQgI2Y1OWUwYjY2O2JvcmRlci1yYWRpdXM6MjhweDtiYWNrZ3JvdW5kOmxpbmVhci1ncmFkaWVudCgxNDVkZWcscmdiYSg0MSwyNCwxMiwuOTYpLHJnYmEoMTIsMTAsOSwuOTYpKTtib3gtc2hhZG93OjAgMzVweCAxMTBweCAjMDAwYyxpbnNldCAwIDFweCAjZmJiZjI0NTV9Lmdsb3d7cG9zaXRpb246YWJzb2x1dGU7aW5zZXQ6LTJweDtib3JkZXItcmFkaXVzOjMwcHg7cG9pbnRlci1ldmVudHM6bm9uZTtib3gtc2hhZG93OjAgMCA1NXB4ICNmNTllMGIyMjthbmltYXRpb246cHVsc2UgMi44cyBlYXNlLWluLW91dCBpbmZpbml0ZX0uYmFkZ2V7Zm9udDo3MDAgMTRweCBTZWdvZSBVSSxzYW5zLXNlcmlmO2xldHRlci1zcGFjaW5nOi4yZW07Y29sb3I6I2ZiYmYyNH1oMXttYXJnaW46MTRweCAwIDhweDtmb250LXNpemU6NTRweDtsaW5lLWhlaWdodDoxO2NvbG9yOiNmZGU2OGF9cHtmb250OjE4cHgvMS42IFNlZ29lIFVJLHNhbnMtc2VyaWY7Y29sb3I6I2Q2ZDNkMX0uZ3JpZHtkaXNwbGF5OmdyaWQ7Z3JpZC10ZW1wbGF0ZS1jb2x1bW5zOnJlcGVhdCgzLDFmcik7Z2FwOjE0cHg7bWFyZ2luOjMwcHggMH0uc3RhdHtwYWRkaW5nOjE4cHg7Ym9yZGVyLXJhZGl1czoxNHB4O2JhY2tncm91bmQ6I2ZmZmZmZjBhO2JvcmRlcjoxcHggc29saWQgI2ZmZmZmZjEyfS5zdGF0IGJ7ZGlzcGxheTpibG9jaztjb2xvcjojZmJiZjI0O2ZvbnQ6NzAwIDIwcHggU2Vnb2UgVUksc2Fucy1zZXJpZn0uc3RhdCBzcGFue2ZvbnQ6MTNweCBTZWdvZSBVSSxzYW5zLXNlcmlmO2NvbG9yOiNhOGEyOWV9a2Jke3BhZGRpbmc6NXB4IDEwcHg7Ym9yZGVyLXJhZGl1czo3cHg7YmFja2dyb3VuZDojZmJiZjI0O2NvbG9yOiMyNzE0MDc7Zm9udDo4MDAgMTVweCBTZWdvZSBVSSxzYW5zLXNlcmlmfUBrZXlmcmFtZXMgcHVsc2V7NTAle2JveC1zaGFkb3c6MCAwIDg1cHggI2Y1OWUwYjU1fX08L3N0eWxlPjwvaGVhZD48Ym9keT48bWFpbiBjbGFzcz0icGFuZWwiPjxpIGNsYXNzPSJnbG93Ij48L2k+PGRpdiBjbGFzcz0iYmFkZ2UiPkxVQSBTQ1JJUFQgwrcgRlJPTlRJRVIgTEVER0VSPC9kaXY+PGgxPlRoZSBmcm9udGllciBpcyBjYWxsaW5nLjwvaDE+PHA+WW91ciB0cmFpbCBjb21wYW5pb24gaXMgcmVhZHkuIFRyYWNrIHRoZSBkYXksIHBsYW4gdGhlIG5leHQgcmlkZSwgYW5kIGtlZXAgdGhlIGVzc2VudGlhbHMgY2xvc2Ugd2hpbGUgeW91IGV4cGxvcmUgdGhlIG9wZW4gY291bnRyeS48L3A+PGRpdiBjbGFzcz0iZ3JpZCI+PGRpdiBjbGFzcz0ic3RhdCI+PGI+Q2FtcCBSZWFkeTwvYj48c3Bhbj5zdXBwbGllcyBwYWNrZWQ8L3NwYW4+PC9kaXY+PGRpdiBjbGFzcz0ic3RhdCI+PGI+MTIgVHJhaWxzPC9iPjxzcGFuPndhaXRpbmcgdG8gYmUgZXhwbG9yZWQ8L3NwYW4+PC9kaXY+PGRpdiBjbGFzcz0ic3RhdCI+PGI+Q2xlYXIgU2tpZXM8L2I+PHNwYW4+cGVyZmVjdCByaWRpbmcgd2VhdGhlcjwvc3Bhbj48L2Rpdj48L2Rpdj48cD5QcmVzcyA8a2JkPkY2PC9rYmQ+IGFnYWluIHRvIHJldHVybiB0byB0aGUgZ2FtZS48L3A+PC9tYWluPjwvYm9keT48L2h0bWw+"

RDR2_LUA_PAGE_URL = PAGE_URL

log("Lua example initialized")

local smoke_hash = joaat("lua-wasmtime-smoke")
log("Lua example ready; hash=" .. tostring(smoke_hash) ..
    ", game_time=" .. tostring(game_time()))

rdr2_lua_toggle_binding = input_register_binding(
    "toggle_lua_example",
    "Toggle Lua Frontier example",
    "keyboard",
    "F6")
log("Lua F6 binding handle=" .. tostring(rdr2_lua_toggle_binding))
rdr2_lua_visible = 0
rdr2_lua_cursor_held = 0
rdr2_lua_ticks = 0

function rdr2_lua_hide_webview()
    if rdr2_lua_visible == 0 and rdr2_lua_cursor_held == 0 then
        return
    end

    webview_set_focus(false)
    if rdr2_lua_cursor_held ~= 0 then
        webview_hide_cursor()
        rdr2_lua_cursor_held = 0
    end
    webview_set_visible(false)
    rdr2_lua_visible = 0
    log("Lua WebView hidden")
end

function rdr2_lua_show_webview()
    local status = webview_open(RDR2_LUA_PAGE_URL, 0, 0)
    if status ~= 0 then
        log("Could not open the Lua WebView", 2)
        return
    end

    status = webview_set_focus(true)
    if status ~= 0 then
        webview_set_visible(false)
        log("Could not focus the Lua WebView", 2)
        return
    end

    status = webview_show_cursor()
    if status ~= 0 then
        webview_set_focus(false)
        webview_set_visible(false)
        log("Could not show the Lua WebView cursor", 2)
        return
    end

    rdr2_lua_cursor_held = 1
    rdr2_lua_visible = 1
    log("Lua WebView opened; press F6 to close")
end

function rdr2_lua_toggle_webview()
    if rdr2_lua_visible ~= 0 then
        rdr2_lua_hide_webview()
    else
        rdr2_lua_show_webview()
    end
end

register_event("tick", function()
    rdr2_lua_ticks = rdr2_lua_ticks + 1
    if rdr2_lua_ticks == 1 then
        log("Lua example tick received")
    end

    local binding_event = input_poll_binding_event(rdr2_lua_toggle_binding)
    if binding_event == 1 then
        rdr2_lua_toggle_webview()
    end
end)

register_event("shutdown", function()
    -- Owned WebView, cursor, and binding resources are released by the host.
    log("Lua example shutting down")
end)
