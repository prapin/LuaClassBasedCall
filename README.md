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
but you now have to explicitly specify  what are the input and output arguments.

The license used is MIT, like Lua itself.

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
*  Error messages include the stack back trace 
*  Some compilation switches can exclude unportable code or huge headers

### C++ types currently handled
*  Special enumerated `nil` value
*  Boolean values
*  All types of numerical values
*  Regular `const char*` strings
*  Wide character strings, automatically converted to/from UTF-8
*  C functions having the signature `lua_CFunction`
*  Lua threads represented as `lua_State*`
*  Generic `const void*` pointers, mapped to light or full userdata
*  C arrays of any supported type: the number of elements must be
   passed to the explicit constructor as its first argument.
*  Multidimensional C arrays (like `const int array[2][5][3]`). Only the
   first dimension (here `2`) has to be passed as the first argument.
*  Some template classes from the C++ Standard Library. _Any_ supported
   type can be used as template argument T or K.
   * `string`
   * `wstring`
   * `vector<T>`
   * `map<K,T>`
   * `list<T>`
   * `set<T>`
*  Some MFC (Microsoft Foundation Classes) elements (more to come):
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

* `Input`: This class stores a C++ value and is responsible to push the
           corresponding value onto Lua stack. There are a lot of overloaded
           constructors, with some being template ones, to support a great number
           of C++ value types. With template specialization, you can even add the
           support for your custom types.
* `Output`: This class stores a reference to a C++ variable and is responsible to 
            extract a Lua value from the stack into the corresponding C++ variable.
            Like `Input`, there are also several constructor overloads, most of them
            are template ones. You can also use template specialization for your types.
* `Array`:  This template class implement a simple array of objects. Its main particularity
            is that is has constructors taking 0, 1, 2, 4, 8, 16 and 32 arguments, each of the
            underlying object type. Because the last `(N/2)-1` arguments for each constructor
            have a default value of `nil`, you can in fact pass any number of objects (up to 32)
			when calling the constructor.
* `Inputs`: Defined as `Array<Input>`, it represents all input arguments for a call.
* `Outputs`: Similarly defined as `Array<Output>`, it represents all result variables from a call.
* `ErrorT<C>`: A simple container for a string, used to throw Lua errors as C++ exceptions.
               The C parameter can either be `char` or `wchar_t`.
* `ErrorA`: Defined as `ErrorT<char>`
* `ErrorW`: Defined as `ErrorT<wchar_t>`
* `Error`: Defined to either `ErrorA` or `ErrorW`, depending on the definition of `UNICODE`.
* `Lua`: The main class. It is responsible to open and close the Lua state, and most importantly
         to call Lua using a script, passing inputs and retrieving outputs. There are several
         forms of calling interfaces (`UCall`, `PCall`, `ECall`, `TCall`) and a number of overloads 
         to handle all user cases.

### Calling syntax

A Lua call follow one of the following generic syntax:

	luaObject . {UCall|PCall|ECall} ( [L]"lua code to execute" [, Inputs(inargs...)] [, Outputs(outargs...)] );
	luaObject . {UCall|PCall|ECall} ( [L]"lua code to execute", inarg [, outarg] );
	T outval = luaObject . TCall<T> ( [L]"lua code to execute" [, inargs...] );

As introduced earlier, `Lua` class has 4 different methods for performing a call to Lua. 

*  `UCall`: performs an unprotected Lua call, like `lua_call` API function. Lua will panic
            in case of errors, except if run under a protected environment.
*  `PCall`: does a protected call, using `lua_pcall` function. That interface has a string
            return type, returning either `NULL` when no error happens, or the error
			message including the stack trace when a Lua compile-time or run-time occurs.
*  `ECall`: calls Lua and throws exceptions in case of errors. You will need to catch
            `ErrorT<C>` objects to handle error messages.
*  `TCall`: has an alternate syntax inspired from `luabind::call_function`. In this variant,
            inputs aruguments are simply placed after the code snippet, without the `Inputs`
			constructor. There are no `Outputs` arguments, instead one (or zero) output result
			is directly returned by the function. You have to specify the template return type.
            `TCall` also throws an exception in case of errors.			

### Code footprint

All functions being `inline`, the footprint highly depends on the usage. Just including `lgencall.hpp`
by itself does not generate any code. Then, the more features you use, the higher the footprint.
In the extreme case when a lot of custom types, arrays and maps are used, the generated code could
become high, but this is unlikely. Most of the time, you only need a small subset of the overloaded
functions.

A compiler is free to decide whether or not it actually inlines code noted `inline`. This depends
on the optimization options and on the size of the function. Single instructions functions will normally
be inlined, but it would be better if there is only one copy of big function like `Input::PushWideString`.

Usage
-----

### First steps

First include in your C++ file the header file. Before the `#include` line,
you might want to `#define` some compilation switches used in the library
(as explained on top of the file):

	#define LCBC_USE_MFC 1
	#include "lgencall.hpp"

If you don't have name clash in your project, the next step is to import
the `lua` namespace into you code with the `using` keyword. It is best to put this
clause not in a shared header file, but close to where it is needed.

Then instantiate a `Lua` class object. This will automatically call `luaL_newstate`
and `luaL_openlibs`. Similarly, the destructor calls `lua_close`. Since there is a 
conversion to `lua_State*`, that main object can be passed as the first argument to
any Lua API function when needed.

You can now easily call Lua snippets. You have to decide which calling interface (`?Call`)
best fits your needs. Also, especially on Windows, you might
prefer to use wide character string versions of the functions. Note that if the code
snipped is a wide character string, so will the returned error message or the exception object.

The Lua script gets its arguments using the `...` syntax introduced in Lua 5.1, and 
outputs its results using the `return` keyword. Think of it
as if the snippet was included inside a function definition like this

	function fct(...) 
		-- your script snippet goes here 
	end
	
### Input arguments

Inside `Inputs()` constructor, nearly all simple value can be passed, since there are implicit 
`Input` constructors for most types. When there are more than one argument to a constructor, an
explicit construction is needed. This is the case for not zero terminated strings, full userdata,
and C array types (note that is the latter case, the size must be _before_ the pointer). 
This example shows a single `char` array interpreted in various ways:

	const char data[5] = {65,0,1,0,100};
	L.Call("data = {...}; print(...)", Inputs( 
		data,                      // push a zero-terminated string, of length 1
		(const void*)data,         // push a light userdata pointing to the C variable
		Input(data, sizeof(data)), // push a string of length 5
		Input((const void*)data, sizeof(data)), // push a full userdata of 5 bytes
		Input(sizeof(data), data))); // push a table containing 5 numbers

### Output results

Similarly, inside `Outputs()` constructor, you can pass variables of most simple
types. There are implicit `Output` constructors taking a _reference_ to those types
as arguments. Some constructors require an additional _reference_ to a size argument.
These are to obtain the size of a string or a full userdata, in addition to a pointer
to the value. 

To fill out C arrays, the size reference must be placed _before_ the pointer reference,
and _initialized_ with the number of elements of the container. After the call it will
receive the size of the Lua table (which can be smaller or larger than the
initial value). For arrays of `char` or `wchar_t` elements, but not `signed char` for
instance, a Lua string is expected and will be copied into the buffer. 

	const char* str1, *str2;
	const void* ptr1, *ptr2;
	char str3[8];
	signed char arr[10];
	size_t str2len, ptr2len, str3len=sizeof(str3), arrlen=sizeof(arr); 
	L.Call("s='P\\0Q\\0R'; u=io.stdin; return s,u,s,u,{1,2,3},s", Outputs( 
		str1,                  // get a zero-terminated string
		ptr1,                  // get a userdata pointer
		Output(str2, str2len), // get a string and its length
		Output(ptr2, ptr2len), // get a full userdata and its length
		Output(arrlen, arr),   // fill out an array containing 3 numbers
		Output(str3len, str3))); // copy a string into a buffer
	
		
### Full example

	#include "lgencall.hpp"
	#include <stdio.h>
	
	using namespace lua;
	
	int main(int argc, char* argv[])
	{
		Lua L;  // The constructor will open a new Lua state and include libraries
		L.UCall("print('Hello world')"); // Lua will panic in case of runtime error
		const char* error = L.PCall("mytable = {...};", Inputs(true, 2, 3.1416, "Hello", L"world"));
		if(error)
			printf("Lua error : %s\n", error);
		try
		{
			L.ECall("for k,v in pairs(mytable) do print(k,v,type(v)) end");
			bool res1; double res2; float res3; const wchar_t* res4; const char* res5;
			L.ECall("return unpack(mytable)", Outputs(res1, res2, res3, res4, res5));
			printf("mytable = { %d, %g, %g, %S, %s }\n", res1, res2, res3, res4, res5);
			double sum = L.TCall<double>("local a,b,c=...; return a+b+c", 2.3, 4, 3.1416);
			printf("The sum is %g\n", sum);
		}
		catch(Error err)
		{
			printf("Lua exception: %s\n", err.str());
		}
		return 0;
	} // The Lua state is closed by the destructor

### Custom types support

It is normally possible to input and output custom types like structures or classes, by using
template specialization, without modifying `lgencall.hpp` source file.

For input, the custom type must be a _pointer_ (if it is not, just pass the address of your value),
and defines a function of the following form (you must be under the namespace `lua`):

	namespace lua {
	template<> inline void Input::PushValue<yourType>(lua_State* L) const
	{
		const yourType* ptr = (const yourType*)PointerValue;
		// Push *ptr to Lua stack the way you want
	}
	}

For output, any type that can be passed by reference can be specialized. The prototype is:
	
	namespace lua {
	template<> inline void Output::GetValue<yourType>(lua_State* L, int idx) const
	{
		yourType* ptr = (yourType*) PointerValue;
		// Retrieve your value from Lua stack at index idx and set it to *ptr
	}
	}

#### Custom type example

	#include "lgencall.hpp"
	struct tMyStruct
	{
		const char* text;
		int number;
	};
	namespace lua {
	template<> inline void Input::PushValue<tMyStruct>(lua_State* L) const
	{
		const tMyStruct* ptr = (const tMyStruct*)PointerValue;
		lua_createtable(L, 0, 2);
		lua_pushstring(L, ptr->text);
		lua_setfield(L, -2, "text");
		lua_pushinteger(L, ptr->number);
		lua_setfield(L, -2, "number");
	}
	template<> inline void Output::GetValue<tMyStruct>(lua_State* L, int idx) const
	{
		tMyStruct* ptr = (tMyStruct*) PointerValue;
		luaL_checktype(L, idx, LUA_TTABLE);
		lua_getfield(L, idx, "text");
		ptr->text = luaL_checkstring(L, -1);
		lua_getfield(L, idx, "number");
		ptr->number = luaL_checkint(L, -1);
		lua_pop(L, 2);
	}
	}
	int main(int argc, char* argv[])
	{
		using namespace lua;
		Lua L;
		tMyStruct s1 = {"Hello world", 42}, s2;
		L.UCall("local s=...; print(s.text, s.number); s.text='new text'; return s", &s1, s2);
		printf("text = '%s', number = %d\n", s2.text, s2.number);
		return 0;
	}
