pub mod severity;
pub mod time;
pub mod file;
pub mod logger;

pub use severity::Severity;
pub use logger::Logger;

#[macro_export]
macro_rules! info {
    ($($arg:tt)*) => {
        $crate::Logger::get().lock().unwrap().log($crate::Severity::Info, format_args!($($arg)*), file!(), line!());
    };
}

#[macro_export]
macro_rules! success {
    ($($arg:tt)*) => {
        $crate::Logger::get().lock().unwrap().log($crate::Severity::Success, format_args!($($arg)*), file!(), line!());
    };
}

#[macro_export]
macro_rules! debug {
    ($($arg:tt)*) => {
        $crate::Logger::get().lock().unwrap().log($crate::Severity::Debug, format_args!($($arg)*), file!(), line!());
    };
}

#[macro_export]
macro_rules! warning {
    ($($arg:tt)*) => {
        $crate::Logger::get().lock().unwrap().log($crate::Severity::Warning, format_args!($($arg)*), file!(), line!());
    };
}

#[macro_export]
macro_rules! error {
    ($($arg:tt)*) => {
        $crate::Logger::get().lock().unwrap().log($crate::Severity::Error, format_args!($($arg)*), file!(), line!());
    };
}

#[macro_export]
macro_rules! fatal {
    ($($arg:tt)*) => {
        $crate::Logger::get().lock().unwrap().log($crate::Severity::Fatal, format_args!($($arg)*), file!(), line!());
    };
}
