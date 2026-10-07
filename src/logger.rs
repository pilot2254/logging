use std::sync::{Mutex, OnceLock};
use colored::Colorize;
use std::path::Path;
use std::fmt::Arguments;

use crate::file::File;
use crate::severity::Severity;
use crate::time::get_time;

pub struct Logger {
    file: File,
    min: Severity,
    show_location: bool,
}

impl Logger {
    fn new() -> Self {
        let mut file = File::new();
        file.open("log.txt");
        Self {
            file,
            min: Severity::Info,
            show_location: true,
        }
    }

    pub fn get() -> &'static Mutex<Logger> {
        static INSTANCE: OnceLock<Mutex<Logger>> = OnceLock::new();
        INSTANCE.get_or_init(|| Mutex::new(Logger::new()))
    }

    pub fn set_file<P: AsRef<Path>>(&mut self, path: P) -> bool {
        self.file.open(path)
    }

    pub fn set_min_severity(&mut self, s: Severity) {
        self.min = s;
    }

    pub fn set_show_location(&mut self, show: bool) {
        self.show_location = show;
    }

    pub fn log(&mut self, s: Severity, message: Arguments, file_name: &str, line: u32) {
        if s < self.min {
            return;
        }

        let mut out = format!("[{}] [{}]", get_time(), s.as_str());

        if self.show_location {
            let name = Path::new(file_name)
                .file_name()
                .unwrap_or_default()
                .to_string_lossy();
            out = format!("{} [{}:{}]", out, name, line);
        }

        out = format!("{}: {}", out, message);

        let colored_out = match s {
            Severity::Success => out.green(),
            Severity::Debug => out.cyan(),
            Severity::Warning => out.yellow(),
            Severity::Error => out.red(),
            Severity::Fatal => out.on_red().white(),
            _ => out.white(),
        };

        println!("{}", colored_out);
        self.file.write(&out);
    }
}
