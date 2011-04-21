Lua Class Based Call
====================

The code serves the same goal than `LuaGenericCall`, but with a completely
different implementation. 

The code mainly uses overloaded classes constructors and templates to deal with
a lot of different data types. The binding takes place in a single header file,
all code is in `inline` functions.

Classes
-------

The header file defines the following classes, all inside `lua` namespace.

* __`Input`__: This class stores a C++ value and is responsible to push the
               corresponding value onto Lua stack. There are a lot of overloaded
               constructors, with some being template ones, to support a great number
               of C++ value types. With template specialization, you can even add the
               support for your custom types.
* __`Output`__: This class stores a reference to a C++ variable and is responsible to 
                extract a Lua value from the stack into the corresponding C++ variable.
                Like Input, there is also a several constructor overloads, most of them
                are template ones. You can also use template specialization for your types.
* __`Array`__:  This template class implement a simple array of objects. Its main particularity
                is that is has constructors taken 1, 2, 4, 8, 16 and 32 arguments, each of the
                underlying object type. Because the last `(N/2)-1` arguments for each constructor
                has a default value of `nil`, you can in fact instance it with any number of
                objects up to 32.
* __`Inputs`__: Defined as `Array<Input>`, it represents all input arguments for a call.
* __`Outputs`__: Similarly defined as `Array<Output>`, it represents all result variables from a call.
* __`ErrorA`__: A simple container for a string, used to throw Lua errors as exceptions.
* __`ErrorW`__: The same as `ErrorA`, but with a wide char message.
* __`Error`__: Just a typedef to either `ErrorA` or `ErrorW`, depending on the definition of `UNICODE`
* __`Lua`__: The main class. It is responsible to open and close the Lua state, and most importantly
             to perform the call to Lua using a script, inputs and outputs. There are several
             form of interface (`Call`, `PCall` and `ExceptCall`) and a number of overloads 
             to handle all use cases.
             
             