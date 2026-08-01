use std::cell::{Cell, RefCell};
use std::rc::Rc;

pub type CallbackId = u32;
type TickCallback = Rc<RefCell<Box<dyn FnMut()>>>;
type KeyCallback = Rc<RefCell<Box<dyn FnMut(u32)>>>;

thread_local! {
    static NEXT_ID: Cell<CallbackId> = const { Cell::new(1) };
    static TICK_CALLBACKS: RefCell<Vec<(CallbackId, TickCallback)>> = const { RefCell::new(Vec::new()) };
    static KEY_DOWN_CALLBACKS: RefCell<Vec<(CallbackId, KeyCallback)>> = const { RefCell::new(Vec::new()) };
    static KEY_UP_CALLBACKS: RefCell<Vec<(CallbackId, KeyCallback)>> = const { RefCell::new(Vec::new()) };
}

fn next_id() -> CallbackId {
    NEXT_ID.with(|next| {
        let id = next.get();
        next.set(id.wrapping_add(1).max(1));
        id
    })
}

pub fn add_tick_callback(callback: impl FnMut() + 'static) -> CallbackId {
    let id = next_id();
    TICK_CALLBACKS.with(|callbacks| {
        callbacks
            .borrow_mut()
            .push((id, Rc::new(RefCell::new(Box::new(callback)))));
    });
    id
}

pub fn remove_tick_callback(id: CallbackId) -> bool {
    TICK_CALLBACKS.with(|callbacks| remove(callbacks, id))
}

pub fn add_key_down_callback(callback: impl FnMut(u32) + 'static) -> CallbackId {
    let id = next_id();
    KEY_DOWN_CALLBACKS.with(|callbacks| {
        callbacks
            .borrow_mut()
            .push((id, Rc::new(RefCell::new(Box::new(callback)))));
    });
    id
}

pub fn remove_key_down_callback(id: CallbackId) -> bool {
    KEY_DOWN_CALLBACKS.with(|callbacks| remove(callbacks, id))
}

pub fn add_key_up_callback(callback: impl FnMut(u32) + 'static) -> CallbackId {
    let id = next_id();
    KEY_UP_CALLBACKS.with(|callbacks| {
        callbacks
            .borrow_mut()
            .push((id, Rc::new(RefCell::new(Box::new(callback)))));
    });
    id
}

pub fn remove_key_up_callback(id: CallbackId) -> bool {
    KEY_UP_CALLBACKS.with(|callbacks| remove(callbacks, id))
}

fn remove<T>(callbacks: &RefCell<Vec<(CallbackId, T)>>, id: CallbackId) -> bool {
    let mut callbacks = callbacks.borrow_mut();
    let old_length = callbacks.len();
    callbacks.retain(|entry| entry.0 != id);
    callbacks.len() != old_length
}

#[doc(hidden)]
pub fn __dispatch_tick() {
    let callbacks = TICK_CALLBACKS.with(|callbacks| {
        callbacks
            .borrow()
            .iter()
            .map(|entry| Rc::clone(&entry.1))
            .collect::<Vec<_>>()
    });
    for callback in callbacks {
        (callback.borrow_mut())();
    }
}

#[doc(hidden)]
pub fn __dispatch_key_down(key: u32) {
    dispatch_key(&KEY_DOWN_CALLBACKS, key);
}

#[doc(hidden)]
pub fn __dispatch_key_up(key: u32) {
    dispatch_key(&KEY_UP_CALLBACKS, key);
}

fn dispatch_key(
    callbacks: &'static std::thread::LocalKey<RefCell<Vec<(CallbackId, KeyCallback)>>>,
    key: u32,
) {
    let callbacks = callbacks.with(|callbacks| {
        callbacks
            .borrow()
            .iter()
            .map(|entry| Rc::clone(&entry.1))
            .collect::<Vec<_>>()
    });
    for callback in callbacks {
        (callback.borrow_mut())(key);
    }
}

#[doc(hidden)]
pub fn __clear() {
    TICK_CALLBACKS.with(|callbacks| callbacks.borrow_mut().clear());
    KEY_DOWN_CALLBACKS.with(|callbacks| callbacks.borrow_mut().clear());
    KEY_UP_CALLBACKS.with(|callbacks| callbacks.borrow_mut().clear());
}
