Lua Class Based Call
====================

Introduction
------------

This library aims to perform generic calls to Lua through code snippets, passing any kind of arguments 
and retrieving all types of results.
It has the same goal as [`LuaGenericCall`](https://github.com/prapin/LuaGenericCall),
but with a completely different implementation. 

It mainly uses overloaded class constructors and templates to deal with
a lot of different data types. The binding takes place in a single C++ header file,
with all functions declared as `inline`.

The introduction example of `LuaGenericCall`, performing a simple multiplication and returning
the result or a possible error message, becomes:

	double result;
	const char* errmsg = L.PCall("local a,b = ...; return a*b", Inputs(3, 2.5), Outputs(result));

Compared to the original C version, there is no more the difficult-to-use `format` string,
but you now have to explicitely specifiy what are the input and output arguments.

Features
--------

*  Automatic Lua state creation and closing
*  Automatic C++ type handling for both input and output values
*  Lots of data types are already supported (see next paragraph)
*  Some types can specify an optional size to an explicit
   constructor, as a second argument
*  It is usually possible to add support for custom types externally
*  Code snippets can also be wide character strings
*  Various error handling possibilities:
   1. unprotected call
   2. protected call: the function returns the error message or `NULL`
   3. exception call: a C++ exception can be sent with the error message
*  Compiled code snippets are cached for performance
*  Error messages include the stack trace back
*  Some compilation switches can exclude unportable code or huge headers

### C++ types currently handled
*  Special enumerated `nil` value
*  Boolean values
*  All types of numerical values
*  Regular `const char*` strings
*  Wide character strings, automatically converted to/from UTF-8
*  C functions having the signature `lua_CFunction`
*  Lua threads represented as `lua_State*`
*  Generic `void*` pointers, mapped to light or full userdata
*  C arrays of any supported type: the number of elements must be
   passed to the explicit constructor as its first argument.
*  Some STL (Standard Template Library) classes are supported (more to come):
   * `string`
   * `wstring`
   * `vector<T>`
   * `map<K,T>`
*  Some MFC (Microsoft Foundation Classes) elements are also supported:
   * `CStringA`
   * `CStringW`
   * `CArray<T1,T2>`


Implementation
--------------

### Namespace

To limit the risk of name clashes, all the library header is placed under a C++
namespace called `lua`. It is easy to change the name of that namespace if your prefer.
A big advantage is that we can use very generic names for library classes, 
like `Input` or `Array`, without having to bother of naming problems with other libraries.

In general, you might want to include the following line either at the beginning of your C++
file or inside a function:

	using namespace lua;
	

### Classes

The header file defines the following classes:

* __`Input`__: This class stores a C++ value and is responsible to push the
               corresponding value onto Lua stack. There are a lot of overloaded
               constructors, with some being template ones, to support a great number
               of C++ value types. With template specialization, you can even add the
               support for your custom types.
* __`Output`__: This class stores a reference to a C++ variable and is responsible to 
                extract a Lua value from the stack into the corresponding C++ variable.
                Like `Input`, there are also several constructor overloads, most of them
                are template ones. You can also use template specialization for your types.
* __`Array`__:  This template class implement a simple array of objects. Its main particularity
                is that is has constructors taking 0, 1, 2, 4, 8, 16 and 32 arguments, each of the
                underlying object type. Because the last `(N/2)-1` arguments for each constructor
                have a default value of `nil`, you can in fact pass any number of objects (up to 32)
				when calling the constructor.
* __`Inputs`__: Defined as `Array<Input>`, it represents all input arguments for a call.
* __`Outputs`__: Similarly defined as `Array<Output>`, it represents all result variables from a call.
* __`ErrorA`__: A simple container for a string, used to throw Lua errors as C++ exceptions.
* __`ErrorW`__: The same as `ErrorA`, but with a wide character string.
* __`Error`__: Defined to either `ErrorA` or `ErrorW`, depending on the definition of `UNICODE`.
* __`Lua`__: The main class. It is responsible to open and close the Lua state, and most importantly
             to call Lua using a script, passing inputs and retrieving outputs. There are several
             forms of calling interfaces (`Call`, `PCall` and `ExceptCall`) and a number of overloads 
             to handle all user cases.
             
### Code footprint

All functions being `inline`, the footprint highly depends on the usage. Just including `lgencall.hpp`
by itself does not generate any code. Then, the more features you use, the higher the footprint.
In the extreme case when a lot of custom types, arrays and maps are used, the generated code could
become high, but this is unlikely. Most of the time, you only need a small subset of the overloaded
functions.

A compiler is free to decide whether or not it actually inlines code noted `inline`. This depends
on the optimization level, and on the size of the function. Single instructions functions will normally
be inlined, but it would be better if there is only one copy of big function like `Input::PushWideString`.

Usage
-----

### Example

	#define LCBC_USE_STL 0 // Exclude STL headers for faster compilation
	#include "lgencall.hpp"
	
	using namespace lua;
	
	int main(int argc, char* argv[])
	{
		Lua L;  // The constructor will open a new Lua state and include libraries
		L.Call("print('Hello world')"); // Lua will panic in case of runtime error
		 Inputs test(true, 2, 3.1416, "Hello", L"world");
		const char* error = L.PCall("mytable = {...};", Inputs(true, 2, 3.1416, "Hello", L"world"));
		if(error)
			printf("Lua error : %s\n", error);
		try
		{
			L.ExceptCall("for k,v in pairs(mytable) do print(k,v,type(v)) end");
			bool res1; double res2; float res3; const wchar_t* res4; const char* res5;
			L.ExceptCall("return unpack(mytable)", Outputs(res1, res2, res3, res4, res5));
			printf("myteable = { %d, %g, %g, %S, %s }\n", res1, res2, res3, res4, res5);
		}
		catch(Error err)
		{
			printf("Lua exception: %s\n", (const char*)err);
		}
	} // The Lua state is closed by the destructor



