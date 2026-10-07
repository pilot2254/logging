# logging

small v logger. colored console output, writes to a file, thread safe.

![showcase](image.png)

## features

- 6 severity levels: info, success, debug, warning, error, fatal
- colored output in the console (using v's built-in term module)
- also writes everything to `log.txt` (appends, doesnt overwrite)
- shows the file and line the log came from, can be turned off
- V string interpolation allows `logging.info('x = $x')` seamlessly
- set a minimum severity to hide the spam

## usage

add this module and import it:

```v
import logging

fn main() {
    logging.info('this is info')
    logging.success('it worked')
    logging.warning('this is a warning')
    logging.error('this is an error')

    name := 'mike'
    age := 17
    logging.info('my name is $name and im $age years old')
}
```

output looks something like

```
[2026-10-07 07:26:12] [INFO] [main.v:5]: this is info
```

## settings

everything goes through the global logger singleton.

```v
import logging

mut log := logging.get()
log.set_file('other.txt') // (default is log.txt)
log.set_min_severity(.warning)
log.set_show_location(false)
```

## building the test

Since this logger utilizes V's `__global` for convenience to emulate the original singleton behavior, please run with the following flag:
`v run -enable-globals test/main.v`

## license

do whatever you want with it