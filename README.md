# poly

`poly` aims to add some things from STL that aren't available in `ETL` and extend it with
some functionality that can be useful in an embedded context.

`poly` only depends on ETL and on a very minimal subset of C++ headers
that [SEGGER embedded studio](https://www.segger.com/) provides.

While some things in this library overlaps with `ETL` this library is not meant to be as
portable as `ETL`, it makes more assumptions regarding which compilers are used.
This means that it uses compiler intrinsics for some parts that `ETL`
does not provide.

## Configuration

The following defines can be set at compile-time to enable certain behaviour.

To simplify things these can also be supplied in a `poly_config.hpp` file. If `poly_config.hpp` can be included
with `#include "poly_config.hpp"` it will be automatically picked up and used as well.

  * `POLY_CONFIG_ENABLE_DYNAMIC_ALLOC`: Enable dynamic allocations for allocators.
    * All allocators uses a `size_t` template to set the capacity of the allocator. When dynamic allocation is
      enabled the capacity `0` will use a dynamically allocating allocator.
  * `POLY_CONFIG_PANIC_STD_TERMINATE`: `poly::panic` will call `std::terminate` after the user-supplied panic handler.
  * `POLY_CHRONO_NO_LITERALS`: Do not make `chrono` literals available at global scope.
  * `POLY_CHRONO_ENABLE_DOUBLE`: Enable `long double` `chrono` literals.
  * `POLY_CONFIG_PANIC_STD_TERMINATE`: Call `std::terminate()` on panics.
  * A platform define
    * `POLY_PLATFORM_PC`
    * `POLY_PLATFORM_NRF52840`
    * `POLY_PLATFORM_TESTING`

The platform can also be configured with CMake by setting `POLY_PLATFORM`
to either `pc`, `nrf52840` or `testing`. If no platform is set then `pc` is
assumed.

## Building

### CMake configuration variables

  * `POLY_ETL_INCLUDE_DIR` - set to the ETL include directory to use. *Default external/etl*

## Examples

### IRQ Event runtime

`poly` contains an event runtime meant to bridge the gap between interrupts and normal "application"
code.

The runtime is driven completely by interrupts, interrupt events are pushed to the runtime which
queues a function to be run. This function is then run from main by calling `runtime.run_available()`

```cpp
// This function starts some IRQ-driven task that
// will eventually call irq_rt.post(poly::irq_baton{}, some_evt)
void start_irq_driven_task(poly::irq_event_runtime& irq_rt);

int main() {
    // A runtime with space for 100 events.
    // Each event must be statically allocated,
    // the runtime only stores pointers.
    poly::irq_event_runtime<100> irq_rt;
    
    start_irq_driven_task(irq_rt);

    while(true) {
        // Handle all IRQ events. IRQ events can
        // of course also trigger soft events,
        // to trigger other things to happen
        irq_rt.run_available();
    }
}
```

See examples for more examples how this can be utilized.

### chrono

A modified version of the [chrono](https://en.cppreference.com/w/cpp/header/chrono) header is available.

The main modifications are

* `nanosecond` isn't available
* No clocks are defined
    * `time_point` is available, so you can still define a clock of your own
* The standard `chrono` literals have been replaced with standards-conforming variants
    * `h` -> `_hour`
    * `min` -> `_min`
    * `s` -> `_sec`
    * `ms` -> `_ms`
    * `us` -> `_us`
    * `d` -> `_day`
    * `y` -> `_year`
* The literals taking `long double` arguments are disabled by defauled. Define `POLY_CHRONO_ENABLE_DOUBLE` to enable `long double`
  variants for the applicable types.
* The literals are available in global scope by default. Define `POLY_CHRONO_NO_LITERALS` to disable.

This allows you to use `poly::chrono` types instead of integers to communicate durations. It is heavily suggested
to use these types whenever you would normally use an integer to specify a duration.

```cpp
void start_timer(poly::chrono::milliseconds timeout); // Start a timer with a specified timeout

// Later on
start_timer(10_sec); // Start the timer with a 10 second timeout.
```

### string_literal

This is a `string_literal` type that is guaranteed to be a statically allocated string to this string and it will
always be available.

The only way to construct a `string_literal` type is by using the user-defined literal `_str`:

```cpp
#include "poly/string_literal.hpp"
void print_string(poly::string_literal str);

// Later
print_string("Hello world"_str);
```

Together with string_literal a utility macro is available: `POLY_ERROR_STR("error")`. This
can be used instead of `"error"_str` to add extra error information in debug mode. In release
mode only the `"error"` string will show. If `POLY_EMPTY_ERRORS` is defined an empty string will
be used as error instead.

### result

This is an open-source library developed by Andreas Wass and modified to be usable from within SEGGER embedded studio.

This library provides a `result<T, E>` template type that can be used to transport either an `ok` result or an
`error` value. A set of monadic functions are available to use or transform a value or error. It is heavily inspired by the
[Rust `Result`](https://doc.rust-lang.org/std/result/enum.Result.html) type.

```cpp
#include "poly/result.hpp"
#include "poly/string_literal.hpp"

void print_error(poly::string_literal error);
poly::result<void, poly::string_literal> try_do_something() {
    if(!do_A()) {
        // POLY_ERROR_STR is a utility macro to append file and line
        // in debug mode
        return poly::error(POLY_ERROR_STR("do_A failed"));
    }
    if(!do_B()) {
        return poly::error(POLY_ERROR_STR("do_B failed"));
    }

    return poly::ok();
}
poly::result<int, poly::string_literal> try_do_something_else() {
    if(!do_Hello()) {
        return poly::error(POLY_ERROR_STR("do_Hello failed"));
    }
    if(!do_World()) {
        return poly::error(POLY_ERROR_STR("do_World failed"));
    }
    return poly::ok(0);
}

int run_once() {
    return try_do_something() // Try to do something
            .and_then(try_do_something_else) // If that succeeded, do something more
            .or_else(print_error) // If any of the above fails then print the error
            .unwrap_or(-1); // And return -1, otherwise the stored "ok" integer is stored.
}
```

### panic handler

Attempting to `unwrap` a `result` that contains an error will result in a
panic. A `panic` can be detected by setting a custom panic handler via
`poly::set_panic_handler`. To signal a panic, call `poly::panic()`.

Note that `poly::panic()` will never return.

## License

This library is provided under [MIT license](LICENSE).

### Dependencies

  * `ETL` - [MIT license](LICENSE)
  * Parts of `libc++` - [Apache 2 with LLVM exception](LICENSE.APACHE2-LLVM)


