#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum Severity {
    Info = 0,
    Success = 1,
    Debug = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5,
}

impl Severity {
    pub fn as_str(&self) -> &'static str {
        match self {
            Severity::Success => "SUCCESS",
            Severity::Debug => "DEBUG",
            Severity::Warning => "WARNING",
            Severity::Error => "ERROR",
            Severity::Fatal => "FATAL",
            _ => "INFO",
        }
    }
}
