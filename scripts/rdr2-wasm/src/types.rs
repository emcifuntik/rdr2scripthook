/// A three-dimensional vector in game coordinates.
#[repr(C)]
#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub struct Vector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Vector3 {
    pub const fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }

    pub fn length(self) -> f32 {
        (self.x * self.x + self.y * self.y + self.z * self.z).sqrt()
    }

    pub fn normalized(self) -> Self {
        let length = self.length();
        if length == 0.0 {
            Self::default()
        } else {
            self * (1.0 / length)
        }
    }
}

impl std::ops::Add for Vector3 {
    type Output = Self;
    fn add(self, other: Self) -> Self {
        Self::new(self.x + other.x, self.y + other.y, self.z + other.z)
    }
}

impl std::ops::Sub for Vector3 {
    type Output = Self;
    fn sub(self, other: Self) -> Self {
        Self::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
}

impl std::ops::Mul<f32> for Vector3 {
    type Output = Self;
    fn mul(self, scalar: f32) -> Self {
        Self::new(self.x * scalar, self.y * scalar, self.z * scalar)
    }
}

pub type Any = i64;
pub type Hash = u32;
pub type Entity = i32;
pub type Ped = i32;
pub type Vehicle = i32;
pub type Object = i32;
pub type Cam = i32;
pub type Player = i32;
pub type Blip = i32;
pub type Pickup = i32;
pub type Interior = i32;
pub type FireId = i32;
pub type ScrHandle = i32;
pub type ItemSet = i32;
pub type Volume = i32;
pub type AnimScene = i32;
pub type PersChar = i32;
pub type PopZone = i32;
pub type Prompt = i32;
pub type PropSet = i32;
