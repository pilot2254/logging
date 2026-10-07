# logging

small header-only c++ logger. colored console output, writes to a file, thread safe. i made it because i dont want to rewrite the same cout stuff in every project

![showcase](image.png)

## features

- 6 severity levels: info, success, debug, warning, error, fatal
- colored output in the console (using termcolor)
- also writes everything to `log.txt` (appends, doesnt overwrite)
- shows the file and line the log came from, can be turned off
- `std::format` style args, so `logging::info("x = {}", 5)` just works
- set a minimum severity to hide the spam

## usage

copy the `include` folder into your project and include it

```cpp
#include "include/logging.hpp"

int main()
{
    logging::info("this is info");
    logging::success("it worked");
    logging::warning("this is a warning");
    logging::error("this is an error");

    logging::info("my name is {} and im {} years old", "mike", 17);

    //...

    return 0;
}
```

output looks something like

```
[2026-10-07 07:26:12.345] [INFO] [main.cpp:5]: this is info
```

## settings

everything goes through the logger singleton

```cpp
auto& log = logging::logger::get();

log.set_file("other.txt");(default is log.txt)
log.set_min_severity(logging::severity::warning);
log.set_show_location(false);
```

## requirements

- c++20 (uses `<format>`, `<source_location>` and concepts)
- msvc, the project is set up for visual studio (v145 toolset)

## building the test

open `logging.slnx` in visual studio and run it. the test is in `tests/main.cpp`

## license

do whatever you want with it