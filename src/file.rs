use std::fs::{File as StdFile, OpenOptions};
use std::io::Write;
use std::path::Path;

pub struct File {
    stream: Option<StdFile>,
}

impl File {
    pub fn new() -> Self {
        Self { stream: None }
    }

    pub fn open<P: AsRef<Path>>(&mut self, path: P) -> bool {
        self.stream = None;
        if let Ok(file) = OpenOptions::new().create(true).append(true).open(path) {
            self.stream = Some(file);
            true
        } else {
            false
        }
    }

    pub fn is_open(&self) -> bool {
        self.stream.is_some()
    }

    pub fn write(&mut self, line: &str) {
        if let Some(stream) = &mut self.stream {
            let _ = writeln!(stream, "{}", line);
            let _ = stream.flush();
        }
    }
}
