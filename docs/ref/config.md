---
title: sm::config
parent: Reference
nav_order: 22
layout: page
permalink: /ref/config
---
# sm::config
{: .no_toc}
## Parameter management via JSON
{: .no_toc}
```c++
import sm.config;
```
Module file: [sm/config.cppm](https://github.com/sebsjames/maths/blob/main/sm/config.cppm).

**Table of Contents**

- TOC
{:toc}

## Summary

`sm::config` is a class for reading and writing program parameters that are
stored in a JSON file. It's intended for the kind of use case where you have
a simulation or other program with a fair number of numerical (or string, or
Boolean) parameters that you want to be able to set from a file rather than
hard-coding, along with a record of exactly what parameters were used for a
given run. `sm::config` uses [nlohmann::json](https://github.com/nlohmann/json)
internally to parse and hold the JSON.

`sm::config` also has a scheme for overriding individual parameters from the
command line, which is useful when you want to run many instances of a
program, sweeping one or two parameters, without editing the JSON file or
maintaining many near-identical copies of it.

## Constructing a config object

You can default-construct a `config` and call `init` (or `parse`) later, or
construct directly from a path to a JSON file:

```c++
import sm.config;

// Default construct now, load the JSON later
sm::config conf;
conf.init ("./params.json");

// Or construct directly from a path (const char* or std::string)
sm::config conf2 ("./params.json");
```

If the given file exists, it is parsed immediately and `sm::config::ready` is set `true`. If the file doesn't exist, `config::ready` stays `false` but the object
is still usable; all the getters will just return the default values
you supply (see below). This makes it easy to write code that works whether
or not a config file is actually present.

You can also construct a `config` directly from a string of JSON text with
`parse`, or from a pre-existing `nlohmann::json` object:

```c++
sm::config conf;
conf.parse (R"({"testint": 27, "testfloat": 7.63})");

nlohmann::json j = { {"testint", 27} };
sm::config conf2 (j);
```

## Reading parameters

Use the templated `get` method, providing a default value to fall back on if
the named parameter isn't present in the JSON:

```c++
#include <cstdint>
#include <iostream>
import sm.config;

int main()
{
    sm::config conf ("./params.json");

    const bool testbool = conf.get<bool> ("testbool", false);
    const std::int32_t testint = conf.get<std::int32_t> ("testint", 3);
    const float testfloat = conf.get<float> ("testfloat", 9.8f);
    const std::string name = conf.get<std::string> ("name", "unnamed");

    std::cout << "testbool=" << testbool << " testint=" << testint
              << " testfloat=" << testfloat << " name=" << name << std::endl;
}
```

`get<T>` works for `bool`, `std::int32_t`, `std::uint32_t`, `float`,
`double` and `std::string` when there's a command line override in play (see
below); for ordinary reads from the JSON (no override), it simply forwards to
`nlohmann::json`'s own `.get<T>()`, so any type that nlohmann::json knows how
to deserialize will work.

There's also an untyped `get`, which returns the raw `nlohmann::json` for a
named field (useful for nested objects or arrays you want to handle
yourself):

```c++
nlohmann::json sub = conf.get ("some_object");
```

### Reading arrays into sm::vec/sm::vvec

If a parameter is a JSON array of numbers, you can read it straight into an
`sm::vvec` (dynamically sized) or an `sm::vec` (fixed size `N`):

```c++
// JSON: "myarray": [1, 2, 3, 4]
sm::vvec<float> vv = conf.get_vvec<float> ("myarray");
sm::vec<float, 4> v = conf.get_vec<float, 4> ("myarray");
```

If the named array isn't present, `get_vvec` returns an empty `vvec` and
`get_vec` returns a `vec` of zeros, so these are safe to call even against a
`config` that failed to load a file.

## Writing parameters

You can set (or add) values on the underlying JSON with `set` and
`set_array`:

```c++
conf.set ("testint", 42);
conf.set ("name", std::string("my_experiment"));
conf.set_array ("myarray", std::vector<float>{1.0f, 2.0f, 3.0f});
```

Write the whole config back out to disk with `write()` (writes back to the
file it was constructed/`init`-ed with) or `write (const std::string&
outfile)` to save to a different path:

```c++
conf.write();                    // overwrite the original file
conf.write ("./params_out.json"); // or write to a new file
```

This is handy for recording, alongside your simulation output, exactly which
parameters (including any command-line overrides, see below) were used to
generate it. You can also get the JSON as a formatted string without writing
to a file with `conf.str()`.

## Command line overrides

`sm::config` supports overriding individual parameters from the command line,
using a `-co:name=value` syntax, without editing the JSON file itself. Call
`process_args` with your program's `argc`/`argv`:

```c++
int main (int argc, char** argv)
{
    sm::config conf ("./params.json");
    conf.process_args (argc, argv);

    // If invoked as: myprogram -co:testint=99
    // then conf.get<std::int32_t> ("testint", 3) now returns 99
    std::int32_t testint = conf.get<std::int32_t> ("testint", 3);
}
```

Values containing spaces should be quoted on the command line, e.g.
`-co:"name=something with spaces"`. Overrides also work for array-valued
parameters read via `get_vvec`/`get_vec`, using a comma-separated list:

```
myprogram -co:myarray=1,2,3,4
```

with either

```c++
sm::vvec<float> myvvec = conf.get_vvec<float> ("myarray");
```

or

```c++
sm::vec<float, 4> myvec = conf.get_vec<float, 4> ("myarray");
```

picking up the overridden values. Any overrides that were applied are
recorded in the `config_overrides` map (`std::map<std::string,
std::string>`), and are written out as a `config_overrides` object alongside
the rest of the JSON when you call `write()` or `str()`, so a run's actual
parameters are always fully recorded.

## Other members

* `bool ready`.  Set `true` once a JSON file/string has been successfully
  parsed.
* `std::string emsg`. Holds an error message if, for example, `write()`
  failed to open its output file.
* `nlohmann::json root`. The underlying JSON object, in case you need
  to interact with it directly via its own API.
* `std::string thefile`. The path associated with this `sm::config` (set by
  the constructor/`init`, and used as the default target for `write()`).

*This page was authored with AI, based on human written code in config.cppm, and reviewed by Seb James*
