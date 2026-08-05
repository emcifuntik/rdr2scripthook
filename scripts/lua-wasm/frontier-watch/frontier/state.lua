local state = {}

local function format_duration(milliseconds)
    local total_seconds = math.max(0, math.floor(milliseconds / 1000))
    local hours = math.floor(total_seconds / 3600)
    local minutes = math.floor(total_seconds / 60) % 60
    local seconds = total_seconds % 60
    return string.format('%02d:%02d:%02d', hours, minutes, seconds)
end

local function format_game_time(milliseconds)
    local total_seconds = math.max(0, math.floor(milliseconds / 1000))
    local minutes = math.floor(total_seconds / 60) % 60
    local seconds = total_seconds % 60
    return string.format('%02d:%02d', minutes, seconds)
end

function state.new(started_at)
    return {
        started_at = started_at,
        ticks = 0,
        trail_marks = 0,
        status = 'The ledger is ready.',
    }
end

function state.reset(model, now)
    model.started_at = now
    model.trail_marks = 0
    model.status = 'A fresh page has been opened.'
end

function state.mark_trail(model)
    model.trail_marks = model.trail_marks + 1
    if model.trail_marks == 1 then
        model.status = 'First trail mark recorded.'
    else
        model.status = tostring(model.trail_marks) .. ' trail marks recorded.'
    end
end

function state.snapshot(model, now, binding, cursor_refs)
    return {
        session = format_duration(now - model.started_at),
        game_time = format_game_time(now),
        ticks = model.ticks,
        trail_marks = model.trail_marks,
        binding = binding,
        cursor_refs = cursor_refs,
        status = model.status,
    }
end

return state
