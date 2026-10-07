# logging

small rust logger. colored console output, writes to a file, thread safe.

![showcase](image.png)

## features

- 6 severity levels: info, success, debug, warning, error, fatal
- colored output in the console (using colored)
- also writes everything to `log.txt` (appends, doesnt overwrite)
- shows the file and line the log came from, can be turned off
- `format!` style args, so `info!("x = {}", 5)` just works
- set a minimum severity to hide the spam

## usage

add to your `Cargo.toml` and use the macros:

```rust
use logging::{info, success, warning, error};

fn main() {
    info!("this is info");
    success!("it worked");
    warning!("this is a warning");
    error!("this is an error");

    info!("my name is {} and im {} years old", "mike", 17);
}
```

output looks something like

```
[2026-10-07 07:26:12] [INFO] [main.rs:5]: this is info
```

## settings

everything goes through the logger singleton

```rust
use logging::{Logger, Severity};

let mut log = Logger::get().lock().unwrap();
log.set_file("other.txt"); // (default is log.txt)
log.set_min_severity(Severity::Warning);
log.set_show_location(false);
```

## requirements

- rust edition 2021

## building the test

run `cargo run --bin main` to run the test in `tests/main.rs`.

## license

do whatever you want with it