# logging

small raku logger. colored console output, writes to a file, thread safe.

![showcase](image.png)

## features

- 6 severity levels: info, success, debug, warning, error, fatal
- colored output in the console (using ansi escape codes)
- also writes everything to `log.txt` (appends, doesnt overwrite)
- shows the file and line the log came from, can be turned off
- `sprintf` style args, so `info("x = %d", 5)` just works
- set a minimum severity to hide the spam

## usage

add `lib` to your include path and use it:

```raku
use lib 'lib';
use Logging;

sub MAIN() {
    info("this is info");
    success("it worked");
    warning("this is a warning");
    error("this is an error");

    info("my name is %s and im %d years old", "mike", 17);
}
```

output looks something like

```
[2026-10-07 07:26:12] [INFO] [main.raku:5]: this is info
```

## settings

everything goes through the logger singleton

```raku
use Logging::Logger;
use Logging::Severity;

my $log = Logging::Logger.get;

$log.set-file('other.txt'); # (default is log.txt)
$log.set-min-severity(Warning);
$log.set-show-location(False);
```

## building the test

run the test script using `raku`:

```bash
raku test/main.raku
```

## license

do whatever you want with it