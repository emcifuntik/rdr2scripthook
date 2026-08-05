local json = {}

local escapes = {
    ['"'] = '\\"',
    ['\\'] = '\\\\',
    ['\b'] = '\\b',
    ['\f'] = '\\f',
    ['\n'] = '\\n',
    ['\r'] = '\\r',
    ['\t'] = '\\t',
}

function json.string(value)
    return '"' .. tostring(value):gsub('[%z\1-\31\\"]', function(character)
        return escapes[character] or string.format('\\u%04x', string.byte(character))
    end) .. '"'
end

function json.state(model)
    return table.concat({
        '{"kind":"state"',
        ',"session":', json.string(model.session),
        ',"gameTime":', json.string(model.game_time),
        ',"ticks":', tostring(model.ticks),
        ',"trailMarks":', tostring(model.trail_marks),
        ',"binding":', json.string(model.binding),
        ',"cursorRefs":', tostring(model.cursor_refs),
        ',"status":', json.string(model.status),
        '}',
    })
end

return json
