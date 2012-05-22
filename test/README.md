Tests for Lua Class Based Call
==============================

In this directory are a series of tests for the library.
Due to the nature of _LuaClassBasedCall_, the C++ files test compilation issues even
more than runtime errors. This is because if the test file compiles, it implies that
the C++ compiler could generate proper constructors for the data types, the most
challenging part of the library.

There are on purpose no `Makefile` in the directory. Such a `Makefile` would be very complex
because of the number of binaries to build, and the number of different platforms to support.
And it would miss one of the most important aspect: testing the compilation itself,
in different configurations.

But it is quite easy to build one of the tests, since they all consist in a single file.
Each of the source file `test_basic_types.cpp`, `test_csl.cpp`, `test_mfc.cpp`
and `test_unicode.cpp` contain a `main` function and have to be compiled alone
into a executable. This is because they use different compilation switches.

For example, using GCC, the following command line is sufficient to compile and run a test:

	g++ test_basic_types.cpp -I.. -llua -Wall -Wextra && ./a
	
Each test executable can accept the following command line options:

*  `-v`  Verbose: displays the data dump and CRCs in addition to the result
*  `-q`  Quiet: does not display anything

The return value is 0 if the test passes, and 1 if it fails.

Most tests work by dumping the relevant data into a string, computing the CRC32 of that string,
and comparing it to the expected value. For `Input` tests, the Lua module `dumper.lua` is used
to serialize Lua values into a string. This is a simplified version of my _DataDumper_ library
available at [Lua Wiki](http://lua-users.org/wiki/DataDumper). For `Output` tests, a `sprintf` 
on the C++ side is used for the dumping purpose. In the `test_csl.cpp` test,
overloads of the `<<` operator are used to ease the serialization of template containers.

If a test fails at runtime, no panic. First rerun it with the `-v` option. Then study the 
resulting string dump, comparing it with what you would expect by looking at the sources.
The CRC mismatch might come from a small difference, for example a different order in the
hash table dump. There is clearly a weakness here in the implementation design.

