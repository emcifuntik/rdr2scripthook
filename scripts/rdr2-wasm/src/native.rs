use crate::ffi;
use crate::types::Vector3;

use std::fmt;
use std::mem;

const KIND_RAW: u32 = 0;
const KIND_FLOAT: u32 = 1;
const KIND_UTF8: u32 = 2;
const KIND_VECTOR3: u32 = 3;
const KIND_IN_OUT_BUFFER: u32 = 4;
const KIND_OUT_BUFFER: u32 = 5;

/// A typed argument passed through the generic native-call ABI.
#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub struct NativeArg {
    kind: u32,
    data: u32,
    value: u64,
}
const _: [(); 16] = [(); mem::size_of::<NativeArg>()];

impl NativeArg {
    pub const fn raw(value: u64) -> Self {
        Self {
            kind: KIND_RAW,
            data: 0,
            value,
        }
    }

    pub const fn int(value: i32) -> Self {
        Self::raw(value as i64 as u64)
    }

    pub const fn hash(value: u32) -> Self {
        Self::raw(value as u64)
    }

    pub const fn any(value: i64) -> Self {
        Self::raw(value as u64)
    }

    pub fn float(value: f32) -> Self {
        Self {
            kind: KIND_FLOAT,
            data: 0,
            value: value.to_bits() as u64,
        }
    }

    pub const fn boolean(value: bool) -> Self {
        Self::raw(value as u64)
    }

    pub fn string(value: &str) -> Self {
        Self {
            kind: KIND_UTF8,
            data: value.len() as u32,
            value: value.as_ptr() as usize as u64,
        }
    }

    pub fn vector(value: Vector3) -> Self {
        Self {
            kind: KIND_VECTOR3,
            data: value.x.to_bits(),
            value: value.y.to_bits() as u64 | ((value.z.to_bits() as u64) << 32),
        }
    }

    pub fn in_out<T>(value: &mut T) -> Self {
        Self {
            kind: KIND_IN_OUT_BUFFER,
            data: mem::size_of::<T>() as u32,
            value: value as *mut T as usize as u64,
        }
    }

    pub fn output<T>(value: &mut mem::MaybeUninit<T>) -> Self {
        Self {
            kind: KIND_OUT_BUFFER,
            data: mem::size_of::<T>() as u32,
            value: value.as_mut_ptr() as usize as u64,
        }
    }
}

/// Raw result written by the host. Generated wrappers convert it to the
/// declared native return type.
#[repr(C)]
#[derive(Clone, Copy, Debug, Default)]
pub struct NativeResult {
    value: u64,
    vector: [f32; 3],
    reserved: u32,
}
const _: [(); 24] = [(); mem::size_of::<NativeResult>()];

impl NativeResult {
    pub const fn as_i32(self) -> i32 {
        self.value as i32
    }
    pub const fn as_u32(self) -> u32 {
        self.value as u32
    }
    pub const fn as_i64(self) -> i64 {
        self.value as i64
    }
    pub const fn as_u64(self) -> u64 {
        self.value
    }
    pub const fn as_bool(self) -> bool {
        self.value as i32 != 0
    }
    pub fn as_f32(self) -> f32 {
        f32::from_bits(self.value as u32)
    }
    pub const fn as_vector3(self) -> Vector3 {
        Vector3::new(self.vector[0], self.vector[1], self.vector[2])
    }
    pub const fn as_string(self) -> NativeString {
        NativeString(self.value)
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum NativeError {
    InvalidMemory,
    InvalidArgument,
    BridgeUnavailable,
    NotFound,
    Crashed,
    TooManyArguments,
    InvalidString,
    Unknown(i32),
}

impl NativeError {
    fn from_status(status: i32) -> Option<Self> {
        match status {
            0 => None,
            1 => Some(Self::InvalidMemory),
            2 => Some(Self::InvalidArgument),
            3 => Some(Self::BridgeUnavailable),
            4 => Some(Self::NotFound),
            5 => Some(Self::Crashed),
            6 => Some(Self::TooManyArguments),
            value => Some(Self::Unknown(value)),
        }
    }
}

impl fmt::Display for NativeError {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(formatter, "native call failed: {self:?}")
    }
}

impl std::error::Error for NativeError {}

/// A game-owned string pointer returned by a native.
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct NativeString(pub u64);

impl NativeString {
    pub fn read(self) -> Result<String, NativeError> {
        let length = unsafe { ffi::native_string_read(self.0, std::ptr::null_mut(), 0) };
        if length < 0 {
            return Err(NativeError::InvalidString);
        }

        let mut bytes = vec![0u8; length as usize + 1];
        let copied =
            unsafe { ffi::native_string_read(self.0, bytes.as_mut_ptr(), bytes.len() as i32) };
        if copied < 0 {
            return Err(NativeError::InvalidString);
        }
        bytes.truncate(copied as usize);
        Ok(String::from_utf8_lossy(&bytes).into_owned())
    }
}

/// Invoke any game native by hash.
///
/// # Safety
///
/// The caller must obey the selected native's contract. Invalid handles,
/// pointer shapes, or argument counts can violate game invariants even though
/// guest memory remains sandboxed.
pub unsafe fn invoke(hash: u64, arguments: &[NativeArg]) -> Result<NativeResult, NativeError> {
    if arguments.len() > 24 {
        return Err(NativeError::TooManyArguments);
    }

    let mut result = NativeResult::default();
    let status = unsafe {
        ffi::native_invoke(
            hash,
            arguments.as_ptr(),
            arguments.len() as i32,
            &mut result,
        )
    };
    match NativeError::from_status(status) {
        Some(error) => Err(error),
        None => Ok(result),
    }
}
