use crate::core;

use std::cell::{Cell, RefCell};
use std::rc::Rc;

pub type TimerId = u32;
type Callback = Rc<RefCell<Box<dyn FnMut()>>>;

struct Timer {
    id: TimerId,
    due: u32,
    interval: Option<u32>,
    callback: Callback,
}

thread_local! {
    static NEXT_ID: Cell<TimerId> = const { Cell::new(1) };
    static TIMERS: RefCell<Vec<Timer>> = const { RefCell::new(Vec::new()) };
}

fn schedule(callback: impl FnMut() + 'static, delay_ms: u32, interval: Option<u32>) -> TimerId {
    let id = NEXT_ID.with(|next| {
        let id = next.get();
        next.set(id.wrapping_add(1).max(1));
        id
    });
    let due = core::game_time().wrapping_add(delay_ms);
    TIMERS.with(|timers| {
        timers.borrow_mut().push(Timer {
            id,
            due,
            interval,
            callback: Rc::new(RefCell::new(Box::new(callback))),
        })
    });
    id
}

pub fn set_timeout(callback: impl FnMut() + 'static, delay_ms: u32) -> TimerId {
    schedule(callback, delay_ms, None)
}

pub fn set_interval(callback: impl FnMut() + 'static, interval_ms: u32) -> TimerId {
    let interval_ms = interval_ms.max(1);
    schedule(callback, interval_ms, Some(interval_ms))
}

pub fn clear_timeout(id: TimerId) -> bool {
    clear(id)
}
pub fn clear_interval(id: TimerId) -> bool {
    clear(id)
}

fn clear(id: TimerId) -> bool {
    TIMERS.with(|timers| {
        let mut timers = timers.borrow_mut();
        let old_length = timers.len();
        timers.retain(|timer| timer.id != id);
        timers.len() != old_length
    })
}

fn is_due(now: u32, due: u32) -> bool {
    now.wrapping_sub(due) < 0x8000_0000
}

#[doc(hidden)]
pub fn __dispatch() {
    let now = core::game_time();
    let callbacks = TIMERS.with(|timers| {
        let mut timers = timers.borrow_mut();
        let mut callbacks = Vec::new();
        let mut index = 0;
        while index < timers.len() {
            if !is_due(now, timers[index].due) {
                index += 1;
                continue;
            }

            callbacks.push(Rc::clone(&timers[index].callback));
            if let Some(interval) = timers[index].interval {
                timers[index].due = now.wrapping_add(interval);
                index += 1;
            } else {
                timers.remove(index);
            }
        }
        callbacks
    });

    for callback in callbacks {
        (callback.borrow_mut())();
    }
}

#[doc(hidden)]
pub fn __clear() {
    TIMERS.with(|timers| timers.borrow_mut().clear());
}
