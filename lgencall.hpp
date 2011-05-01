/******************************************************************************
* Copyright (C) 2011 Olivetti Engineering SA, CH 1400 Yverdon-les-Bains.  
* Original author: Patrick Rapin. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

// Version 1.0.9

#ifndef LUA_CLASSES_BASED_CALL_H
#define LUA_CLASSES_BASED_CALL_H

/* LCBC_USE_WIDESTRING defines the support for wide character (Unicode) strings:
   0 : wchar_t and wstring are not supported; to make sure they are #defined to dummy types
   1 : no conversion, wide character strings are pushed verbatim to the stack (raw bytes);
       script snippets cannot be wchar_t* (you would get syntax errors looking like Chineese)
   2 : conversions between wchar_t* and char* use wctomb and mbtowc standard functions;
       the resulting strings depend on your current locale
   3 : conversions between wchar_t* and char* are done by the library, using UTF-8 encoding */
#ifndef LCBC_USE_WIDESTRING
#define LCBC_USE_WIDESTRING 3
#endif

/* LCBC_USE_CSL defines if some features of the C++ Standard Library
   should be supported. The supported classes used are string, wstring, 
   vector<T>, map<K,T>, list<T> and set<T>.
   0: no support
   1: C++ Standard Library classes can be used in the calls. */
#ifndef LCBC_USE_CSL
#define LCBC_USE_CSL 0
#endif

/* LCBC_USE_MFC enables a few number of Microsoft Foundation Classes (MFC)
   objects to be directly supported. Currently, CString (both ANSI and Unicode) 
   and CArray<T> classes are handled. More to come.
*/
#ifndef LCBC_USE_MFC
#define LCBC_USE_MFC 0
#endif

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <cstring>
#include <cstdlib>

#if LCBC_USE_WIDESTRING
#include <cwchar>
#endif

#if LCBC_USE_CSL
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#endif

#if LCBC_USE_MFC
#define _WIN32_WINNT 0x0500
#include <afx.h>
#include <afxtempl.h>
#endif

namespace lua {
using namespace std;

enum eNil { nil };

#if LCBC_USE_WIDESTRING == 0
enum dummy_wchar_t {};
class dummy_wstring {};
#define wchar_t dummy_wchar_t
#define wstring dummy_wstring
#endif

class Input
{
public:
	Input(eNil) { pPush = &Input::PushNil; }
	Input(bool value) { pPush = &Input::PushBoolean; BooleanValue = value; }
	Input(lua_CFunction value) { pPush = &Input::PushFunction; FunctionValue = value; Size = 0;}
	Input(lua_CFunction value, size_t len) { pPush = &Input::PushFunction; FunctionValue = value; Size=len; }
	template<class T> Input(T value) { pPush = &Input::PushNumber; NumberValue = (lua_Number)value; }
	template<class T> Input(T* value) { pPush = &Input::PushValue<T>; PointerValue = value; }
	template<class T> Input(const T* value) { pPush = &Input::PushValue<T>; PointerValue = value; }
	template<class T> Input(const T* value, size_t size) { pPush = &Input::PushSizedValue<T>; PointerValue = value; Size = size; }
	template<class T> Input(size_t len, const T* value) { pPush = &Input::PushArray<T>; PointerValue = value; Size=len; }
	template<class T, size_t L2> Input(size_t len1, const T value[][L2]) { pPush = &Input::Push2DArray<T,L2>; PointerValue = value; Size=len1; }
#if LCBC_USE_CSL
	Input(const string& value);
	Input(const wstring& value);
	template<class T> Input(const vector<T>& value) { pPush = &Input::PushVector<T>; PointerValue = &value; }
	template<class Key, class T> Input(const map<Key,T>& value) { pPush = &Input::PushMap<Key,T>; PointerValue = &value; }
	template<class T> Input(const list<T>& value) { pPush = &Input::PushList<T>; PointerValue = &value; }
	template<class T> Input(const set<T>& value) { pPush = &Input::PushSet<T>; PointerValue = &value; }
#endif
#if LCBC_USE_MFC
	Input(const CStringA& value);
	Input(const CStringW& value);
	template<class T, class A> Input(const CArray<T,A>& value) { pPush = &Input::PushCArray<T,A>; PointerValue = &value; }
#endif

	void Push(lua_State* L) const { (this->*pPush)(L); }
	static void PushWideString(lua_State* L, const wchar_t* str, size_t len);
private:
	void PushNil(lua_State* L) const { lua_pushnil(L); }
	void PushBoolean(lua_State* L) const { lua_pushboolean(L, BooleanValue); }
	void PushNumber(lua_State* L) const { lua_pushnumber(L, NumberValue); }
	void PushFunction(lua_State* L) const { lua_pushcclosure(L, FunctionValue, (int)Size); }
	template<class T> void PushNumber(lua_State* L) const { lua_pushnumber(L, lua_Number(*(T*)PointerValue)); }
	template<class T> void PushValue(lua_State* L) const;
	template<class T> void PushSizedValue(lua_State* L) const;
	template<class T> void PushArray(lua_State* L) const;
	template<class T, size_t L2> void Push2DArray(lua_State* L) const;
	template<class T> void PushVector(lua_State* L) const;
	template<class T> void PushList(lua_State* L) const;
	template<class T> void PushSet(lua_State* L) const;
	template<class T, class A> void PushCArray(lua_State* L) const;
	template<class Key, class T> void PushMap(lua_State* L) const;

	void (Input::*pPush)(lua_State* L) const;
	union 
	{
		bool BooleanValue;
		lua_Number NumberValue;
		const void* PointerValue;
		lua_CFunction FunctionValue;
	};
	size_t Size;
};

class Output
{
public:
	Output(eNil) { pGet = &Output::GetNil; }
	template<class T> Output(T& value) { pGet = &Output::GetValue<T>; PointerValue = &value; }
	template<class T> Output(size_t& size, T* value) { memset(value, 0, size*sizeof(T)); pGet = &Output::GetArray<T>; pSize = &size; PointerValue = value; }
	template<class T> Output(const T*& value, size_t& size) { pGet = &Output::GetSizedValue<T>; pSize = &size; PointerValue = &value; }
	template<class T, size_t L2> Output(size_t& len1, T value[][L2]) {pGet = &Output::Get2DArray<T,L2>; pSize = &len1; PointerValue = value;  }
#if LCBC_USE_CSL
	template<class T> Output(vector<T>& value) { pGet = &Output::GetVector<T>; PointerValue = &value; }
	template<class K, class T> Output(map<K,T>& value) { pGet = &Output::GetMap<K,T>; PointerValue = &value; }
	template<class T> Output(list<T>& value) { pGet = &Output::GetList<T>; PointerValue = &value; }
	template<class T> Output(set<T>& value) { pGet = &Output::GetSet<T>; PointerValue = &value; }
#endif
#if LCBC_USE_MFC
	template<class T, class A> Output(CArray<T,A>& value) { pGet = &Output::GetCArray<T,A>; PointerValue = &value; }
#endif

	void Get(lua_State* L, int idx) const  { (this->*pGet)(L, idx); }
	static const wchar_t* ToWideString(lua_State* L, int idx, size_t* psize);
private:
	size_t GetSize(size_t s1) const { size_t s2=*pSize; *pSize=s1; return s1 < s2 ? s1 : s2; }
	void GetNil(lua_State* L, int idx) const {}
	template<class T> void GetValue(lua_State* L, int idx) const { *(T*)PointerValue = (T)luaL_checknumber(L, idx); }
	template<class T> void GetSizedValue(lua_State* L, int idx) const;
	template<class T> void GetArray(lua_State* L, int idx) const;
	template<class T, size_t L2> void Get2DArray(lua_State* L, int idx) const;
	template<class T> void GetVector(lua_State* L, int idx) const;
	template<class K, class T> void GetMap(lua_State* L, int idx) const;
	template<class T> void GetList(lua_State* L, int idx) const;
	template<class T> void GetSet(lua_State* L, int idx) const;
	template<class T, class A> void GetCArray(lua_State* L, int idx) const;

	void (Output::*pGet)(lua_State* L, int idx) const;
	void* PointerValue;
	size_t* pSize;
};


template<> inline void Input::PushNumber<bool>(lua_State* L) const
{
	lua_pushboolean(L, *(bool*)PointerValue); 
}

template<> inline void Input::PushValue<char>(lua_State* L) const
{
	lua_pushstring(L, (const char*)PointerValue);
}

template<> inline void Input::PushSizedValue<char>(lua_State* L) const
{
	lua_pushlstring(L, (const char*)PointerValue, Size);
}

template<> inline void Input::PushValue<lua_State>(lua_State* L) const
{
	lua_pushthread((lua_State*)PointerValue); 
	lua_xmove((lua_State*)PointerValue, L, 1);
}

template<> inline void Input::PushValue<void>(lua_State* L) const
{
	lua_pushlightuserdata(L, (void*)PointerValue);
}

template<> inline void Input::PushSizedValue<void>(lua_State* L) const
{
	memcpy(lua_newuserdata(L, Size), PointerValue, Size);
}

template<class T> inline void Input::PushArray(lua_State* L) const
{
	const T* arr = (const T*)PointerValue;
	lua_createtable(L, (int)Size, 0);
	for(size_t i=0;i<Size;i++)
	{
		Input input(arr[i]);
		input.Push(L);
		lua_rawseti(L, -2, (int)i+1);
	}
}

template<class T, size_t L2> inline void Input::Push2DArray(lua_State* L) const
{
	const T (*arr)[L2] = (const T(*)[L2])PointerValue;
	lua_createtable(L, (int)Size, 0);
	for(size_t i=0;i<Size;i++)
	{
		Input input(L2, arr[i]);
		input.Push(L);
		lua_rawseti(L, -2, (int)i+1);
	}
}

template<> inline void Output::GetValue<bool>(lua_State* L, int idx) const
{
	*(bool*)PointerValue = lua_toboolean(L, idx) != 0;
}

template<> inline void Output::GetValue<const char*>(lua_State* L, int idx) const
{
	*(const char**)PointerValue = luaL_checklstring(L, idx, NULL);
}

template<> inline void Output::GetSizedValue<char>(lua_State* L, int idx) const
{
	*(const char**)PointerValue = luaL_checklstring(L, idx, pSize);
}

template<> inline void Output::GetValue<lua_CFunction>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TFUNCTION); 
	*(lua_CFunction*)PointerValue = lua_tocfunction(L, idx);
}

template<> inline void Output::GetValue<lua_State*>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TTHREAD); 
	*(lua_State**)PointerValue = lua_tothread(L, idx);
}

template<> inline void Output::GetValue<const void*>(lua_State* L, int idx) const
{
	*(const void**)PointerValue = lua_touserdata(L, idx);
}

template<> inline void Output::GetSizedValue<void>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TUSERDATA); 
	*(void**)PointerValue = (void*)lua_topointer(L, idx); 
	*pSize = lua_objlen(L, idx);
}


template<> inline void Output::GetArray<char>(lua_State* L, int idx) const
{
	size_t len = GetSize(lua_objlen(L, idx)+1);
	memcpy(PointerValue, luaL_checkstring(L, idx), len);
}

template<class T> inline void Output::GetArray(lua_State* L, int idx) const
{
	T* arr = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = GetSize(lua_objlen(L, idx));
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		Output output(arr[i]);
		output.Get(L,top+1);
		lua_settop(L, top);
	}
}

template<class T, size_t L2> inline void Output::Get2DArray(lua_State* L, int idx) const
{
	T (*arr)[L2] = (T(*)[L2])PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = GetSize(lua_objlen(L, idx));
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		size_t len2 = L2;
		Output output(len2, arr[i]);
		output.Get(L,top+1);
		lua_settop(L, top);
	}
}

template<> inline void Input::PushValue<wchar_t>(lua_State* L) const
{
	PushWideString(L, (const wchar_t*)PointerValue, 0);
}

template<> inline void Input::PushSizedValue<wchar_t>(lua_State* L) const
{
	PushWideString(L, (const wchar_t*)PointerValue, Size);
}

template<> inline void Output::GetArray<wchar_t>(lua_State* L, int idx) const
{
	size_t size;
	const wchar_t* str = ToWideString(L, idx, &size);
	memcpy(PointerValue, str, GetSize(size+1)*sizeof(wchar_t));
}

template<> inline void Output::GetValue<const wchar_t*>(lua_State* L, int idx) const
{
	*(const wchar_t**)PointerValue = ToWideString(L, idx, NULL);
}

template<> inline void Output::GetSizedValue<wchar_t>(lua_State* L, int idx) const
{
	*(const wchar_t**)PointerValue = ToWideString(L, idx, pSize);
}

#if LCBC_USE_CSL
template<class T> inline void Input::PushVector(lua_State* L) const
{
	const vector<T>* v = (const vector<T>*)PointerValue;
	lua_createtable(L, v->size(), 0);
	for(size_t i=0;i<v->size();i++)
	{
		Input input(v->at(i));
		input.Push(L);
		lua_rawseti(L, -2, (int)i+1);
	}
}

template<class Key, class T> inline void Input::PushMap(lua_State* L) const
{
	map<Key,T>* m = (map<Key,T>*)PointerValue;
	typename map<Key,T>::iterator it;
	lua_createtable(L, 0, m->size());
	for (it=m->begin() ; it != m->end(); it++)
	{
		Input key((*it).first);
		key.Push(L);
		Input value((*it).second);
		value.Push(L);
		lua_settable(L, -3);
	}
}

template<class T> inline void Input::PushList(lua_State* L) const
{
	list<T>* mylist = (list<T>*)PointerValue;
	lua_createtable(L, mylist->size(), 0);
	typename list<T>::iterator it;
	int i=0;
	for (it=mylist->begin(); it != mylist->end(); it++,i++)
	{
		lua_pushinteger(L, i+1);
		Input input(*it);
		input.Push(L);
		lua_settable(L, -3);
	}
}

template<class T> inline void Input::PushSet(lua_State* L) const
{
	set<T>* s = (set<T>*)PointerValue;
	typename set<T>::iterator it;
	lua_createtable(L, 0, s->size());
	for (it=s->begin() ; it != s->end(); it++)
	{
		Input key(*it);
		key.Push(L);
		lua_pushboolean(L, 1);
		lua_settable(L, -3);
	}
}

template<> inline void Input::PushValue<string>(lua_State* L) const
{
	string* str = (string*)PointerValue;
	lua_pushlstring(L, str->data(), str->size());
}

inline Input::Input(const string& value) 
{ 
	pPush = &Input::PushValue<string>; 
	PointerValue = &value; 
}

template<> void Output::GetValue<string>(lua_State* L, int idx) const
{
	size_t size; 
	const char* str = luaL_checklstring(L, idx, &size); 
	((string*)PointerValue)->assign(str, size);
}

template<class T> inline void Output::GetVector(lua_State* L, int idx) const
{
	vector<T>* v = (vector<T>*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		T value;
		Output output(value);
		output.Get(L, top+1);
		v->push_back(value);
		lua_settop(L, top);
	}
}

template<class T> inline void Output::GetList(lua_State* L, int idx) const
{
	list<T>* v = (list<T>*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		T value;
		Output output(value);
		output.Get(L, top+1);
		v->push_back(value);
		lua_settop(L, top);
	}
}

template<class T> inline void Output::GetSet(lua_State* L, int idx) const
{
	set<T>* s = (set<T>*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		lua_pushvalue(L, top+1);
		T key;
		Output outputKey(key);
		outputKey.Get(L, top+3);
		lua_settop(L, top+1);
		s->insert(key);
	}
	lua_settop(L, top);
}

template<class K, class T> inline void Output::GetMap(lua_State* L, int idx) const
{
	map<K,T>* m = (map<K,T>*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		lua_pushvalue(L, top+1);
		K key;
		Output outputKey(key);
		outputKey.Get(L, top+3);
		T value;
		Output output(value);
		output.Get(L, top+2);
		lua_settop(L, top+1);
		(*m)[key] = value;
	}
	lua_settop(L, top);
}

#if LCBC_USE_WIDESTRING
template<> inline void Input::PushValue<wstring>(lua_State* L) const
{
	wstring* str = (wstring*)PointerValue;
	PushWideString(L, str->data(), str->size());
}

inline Input::Input(const wstring& value) 
{ 
	pPush = &Input::PushValue<wstring>; 
	PointerValue = &value; 
}

template<> inline void Output::GetValue<wstring>(lua_State* L, int idx) const
{
	size_t size; 
	const wchar_t* str = ToWideString(L, idx, &size); 
	((wstring*)PointerValue)->assign(str, size);
}
#endif
#endif

#if LCBC_USE_MFC

template<class T, class A> inline void Input::PushCArray(lua_State* L) const
{
	const CArray<T,A>* v = (const CArray<T,A>*)PointerValue;
	lua_createtable(L, v->GetSize(), 0);
	for(int i=0;i<v->GetSize();i++)
	{
		Input input(v->GetAt(i));
		input.Push(L);
		lua_rawseti(L, -2, i+1);
	}
}

template<class T, class A> inline void Output::GetCArray(lua_State* L, int idx) const
{
	CArray<T,A>* v = (CArray<T,A>*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int len = (int)lua_objlen(L, idx);
	int top = lua_gettop(L);
	v->SetSize(len);
	for(int i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, i+1);
		T value;
		Output output(value);
		output.Get(L, top+1);
		v->SetAt(i, value);
		lua_settop(L, top);
	}
}

template<> inline void Input::PushValue<CStringA>(lua_State* L) const
{
	CStringA* str = (CStringA*)PointerValue;
	lua_pushlstring(L, *str, str->GetLength());
}

inline Input::Input(const CStringA& value) 
{ 
	pPush = &Input::PushValue<CStringA>; 
	PointerValue = &value; 
}

template<> inline void Output::GetValue<CStringA>(lua_State* L, int idx) const
{
	size_t size; 
	const char* str = luaL_checklstring(L, idx, &size); 
	*(CStringA*)PointerValue = CStringA(str, (int)size);
}

#if LCBC_USE_WIDESTRING
template<> inline void Input::PushValue<CStringW>(lua_State* L) const
{
	CStringW* str = (CStringW*)PointerValue;
	PushWideString(L, *str, str->GetLength());
}

inline Input::Input(const CStringW& value) 
{ 
	pPush = &Input::PushValue<CStringW>; 
	PointerValue = &value; 
}

template<> inline void Output::GetValue<CStringW>(lua_State* L, int idx) const
{
	size_t size; 
	const wchar_t* str = ToWideString(L, idx, &size); 
	*(CStringW*)PointerValue = CStringW(str, (int)size);
}
#endif
#endif

template<class T>
class Array
{
public:
	typedef const T& ref;
	Array() : Size(0), Arguments(NULL) {}
	Array(const Array& src) { Init(src.Arguments, src.Size); }
	~Array() { delete [] Arguments; }
	Array(ref arg1) { const T* args[] = { &arg1 }; Init(args, 1); }
	Array(ref arg1, ref arg2) { const T* args[] = { &arg1, &arg2 }; Init(args, 2); }
	Array(ref arg1, ref arg2, ref arg3, ref arg4=nil)
	{ 
		const T* args[] = { &arg1, &arg2, &arg3, &arg4 }; 
		Init(args, 4); 
	}
	Array(ref arg1, ref arg2, ref arg3, ref arg4,
		ref arg5, ref arg6=nil, ref arg7=nil, ref arg8=nil)
		{ const T* args[] = { &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8 }; Init(args, 8); }
	Array(ref arg1, ref arg2, ref arg3, ref arg4,
		ref arg5, ref arg6, ref arg7, ref arg8,
		ref arg9, ref arg10=nil, ref arg11=nil, ref arg12=nil,
		ref arg13=nil, ref arg14=nil, ref arg15=nil, ref arg16=nil)
		{ const T* args[] = { &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8,
		&arg9, &arg10, &arg11, &arg12, &arg13, &arg14, &arg15, &arg16};	Init(args, 16); }
	Array(ref arg1, ref arg2, ref arg3, ref arg4,
		ref arg5, ref arg6, ref arg7, ref arg8,
		ref arg9, ref arg10, ref arg11, ref arg12,
		ref arg13, ref arg14, ref arg15, ref arg16,
		ref arg17, ref arg18=nil, ref arg19=nil, ref arg20=nil,
		ref arg21=nil, ref arg22=nil, ref arg23=nil, ref arg24=nil,
		ref arg25=nil, ref arg26=nil, ref arg27=nil, ref arg28=nil,
		ref arg29=nil, ref arg30=nil, ref arg31=nil, ref arg32=nil) {
		const T* args[] = { &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8,
		&arg9, &arg10, &arg11, &arg12, &arg13, &arg14, &arg15, &arg16,
		&arg17, &arg18, &arg19, &arg20, &arg21, &arg22, &arg23, &arg24,
		&arg25, &arg26, &arg27, &arg28, &arg29, &arg30, &arg31, &arg32};	
		Init(args, 32); }
	Array& operator=(const Array& src) { delete[]Arguments; Init(src.Arguments, src.Size); return *this; }
	size_t size() const { return Size; }
	ref get(size_t idx) const { return *Arguments[idx]; }
private:
	void Init(const T* args[], size_t len)
	{
		Size = len;
		Arguments = new const T*[len];
		for(size_t i=0;i<len;i++)
			Arguments[i] = args[i];
	}
	size_t Size;
	const T** Arguments;
};

template<class C>
class ErrorT
{
public:
	ErrorT(const C* message) : Message(message)  {}
	operator const C*() const { return Message; }
	const C* str() const { return Message; }
private:
	const C* Message;
};

typedef ErrorT<char> ErrorA;
typedef ErrorT<wchar_t> ErrorW;
#if defined(_UNICODE) || defined(UNICODE)
typedef ErrorW Error;
#else
typedef ErrorA Error;
#endif

typedef Array<Input> Inputs;
typedef Array<Output> Outputs;

class Lua
{
public:
	Lua()
	{ 
		L = luaL_newstate(); 
		luaL_openlibs(L); 
		FlushCache();
	}
	Lua(lua_State* l) 
	{
		L = l; 
		Retain();
		FlushCache();
	}
	Lua(const Lua& src)
	{
		L = src.L; 
		Retain();
		FlushCache();
	}
	~Lua() { Release(); }
	Lua& operator=(const Lua& src) 
	{ 
		Release(); 
		L = src.L; 
		Retain();
		FlushCache();
		return *this;
	}
	operator lua_State*() const { return L; }
	void FlushCache()
	{
		lua_createtable(L, 0, 0);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaClassBasedCaller");
	}
	template<class C> void UCall(const C* script, const Input& input, const Output& output = nil) { UCall(script, Inputs(input), Outputs(output)); }
	template<class C> void UCall(const C* script, const Outputs& outputs) { UCall<C>(script, Inputs(), outputs); }
	template<class C> void UCall(const C* script, const Inputs& inputs = Inputs(), const Outputs& outputs = Outputs())
	{
		PrepareCall(script, inputs, outputs);
		DoCall();
	}
	template<class C> const C* PCall(const C* script, const Input& input, const Output& output = nil) {  return PCall<C>(script, Inputs(input), Outputs(output)); }
	template<class C> const C* PCall(const C* script, const Outputs& outputs) { return PCall<C>(script, Inputs(), outputs); }
	template<class C> const C* PCall(const C* script, const Inputs& inputs = Inputs(), const Outputs& outputs = Outputs())
	{
		PrepareCall(script, inputs, outputs);
		if(lua_cpcall(L, (lua_CFunction)DoCallS, this))
			return GetString(lua_gettop(L), *script);
		return NULL;
	}
	template<class C> void ECall(const C* script, const Input& input, const Output& output = nil) { ECall<C>(script, Inputs(input), Outputs(output)); }
	template<class C> void ECall(const C* script, const Outputs& outputs) { ECall<C>(script, Inputs(), outputs); }
	template<class C> void ECall(const C* script, const Inputs& inputs = Inputs(), const Outputs& outputs = Outputs())
	{
		const C* error = PCall<C>(script, inputs, outputs);
		if(error)
			throw ErrorT<C>(error);
	}
	typedef const Input& ref;
	template<class T,class C> T TCall(const C* script) { return DoTCall<T,C>(script, Inputs()); }
	template<class T,class C> T TCall(const C* script, ref arg1) { return DoTCall<T,C>(script, arg1); }
	template<class T,class C> T TCall(const C* script, ref arg1, ref arg2) { return DoTCall<T,C>(script, Inputs(arg1, arg2)); }
	template<class T,class C> T TCall(const C* script, ref arg1, ref arg2, ref arg3, ref arg4=nil) 
		{ return DoTCall<T,C>(script, Inputs(arg1, arg2, arg3, arg4)); }
	template<class T,class C> T TCall(const C* script, ref arg1, ref arg2, ref arg3, ref arg4, ref arg5, ref arg6=nil, ref arg7=nil, ref arg8=nil) 
		{ return DoTCall<T,C>(script, Inputs(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)); }
	template<class T,class C> T TCall(const C* script, ref arg1, ref arg2, ref arg3, ref arg4, ref arg5, ref arg6, ref arg7, ref arg8,
		ref arg9, ref arg10=nil, ref arg11=nil, ref arg12=nil, ref arg13=nil, ref arg14=nil, ref arg15=nil, ref arg16=nil)
		{ return DoTCall<T,C>(script, Inputs(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16)); }
private:
	const char* GetString(int idx, char) { return lua_tostring(L, idx); }
	const wchar_t* GetString(int idx, wchar_t) { return Output::ToWideString(L, idx, NULL); }
	void PrepareCall(const wchar_t* script_, const Inputs& inputs, const Outputs& outputs)
	{
		PrepareCall("", inputs, outputs);
		Input::PushWideString(L, script_, 0);
		script = lua_tostring(L, -1);
	}
	void PrepareCall(const char* script_, const Inputs& inputs_, const Outputs& outputs_)
	{
		lua_settop(L, 0);
		script = script_;
		inputs = &inputs_;
		outputs = &outputs_;
	}
	void DoCall()
	{
		lua_pushcfunction(L, (lua_CFunction)traceback);
		int idxtrace = lua_gettop(L);
		lua_getfield(L, LUA_REGISTRYINDEX, "LuaClassBasedCaller");
		lua_getfield(L, -1, script);
		if(!lua_isfunction(L, -1))
		{
			if(luaL_loadstring(L, script))
				lua_error(L);
			lua_pushvalue(L, -1);
			lua_setfield(L, -4, script);
		}
		int base = lua_gettop(L);
		lua_checkstack(L, (int)inputs->size());
		for(size_t i=0;i<inputs->size(); i++)
			inputs->get(i).Push(L);
		if(lua_pcall(L, (int)inputs->size(), (int)outputs->size(), idxtrace))
			lua_error(L);
		for(size_t i=0;i<outputs->size(); i++)
			outputs->get(i).Get(L, (int)i+base);
	}
	static int DoCallS(lua_State* L)
	{
		Lua* This = (Lua*)lua_topointer(L, 1);
		This->DoCall();
		return 0;
	}
	template<class T,class C> T DoTCall(const C* script, const Inputs& inputs)
	{
		T value;
		ECall<C>(script, inputs, Outputs(value));
		return value;
	}
	/* Function copied from lua.c */
	static int traceback (lua_State *L) 
	{
		lua_getfield(L, LUA_GLOBALSINDEX, "debug");
		if (!lua_istable(L, -1)) {
			lua_pop(L, 1);
			return 1;
		}
		lua_getfield(L, -1, "traceback");
		if (!lua_isfunction(L, -1)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pushvalue(L, 1);  /* pass error message */
		lua_pushinteger(L, 2);  /* skip this function and traceback */
		lua_call(L, 2, 1);  /* call debug.traceback */
		return 1;
	}
	lua_Integer IncrRetainCount(int incr)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "LuaClassBasedRetainCount");
		lua_Integer count = lua_tointeger(L, -1);
		count = count + incr;
		lua_pushinteger(L, count);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaClassBasedRetainCount");
		return count;
	}
	void Retain() { IncrRetainCount(1); }
	void Release() { if(IncrRetainCount(-1) < 0) lua_close(L); }

	lua_State* L;
	const char* script;
	const Inputs* inputs; 
	const Outputs* outputs;
};

template<> inline void Lua::DoTCall<void, char>(const char* script, const Inputs& inputs) {	ECall<char>(script, inputs); }
template<> inline void Lua::DoTCall<void, wchar_t>(const wchar_t* script, const Inputs& inputs) { ECall<wchar_t>(script, inputs); }

#if LCBC_USE_WIDESTRING == 1
inline void Input::PushWideString(lua_State* L, const wchar_t* wstr, size_t len)
{
	if(len == 0)
		len = wcslen(wstr);
	lua_pushlstring(L, (const char*)wstr, 2*len);
}

inline const wchar_t* Output::ToWideString(lua_State* L, int idx, size_t* psize)
{
	lua_pushvalue(L, idx);
	lua_pushlstring(L, "\0\0\0\0", sizeof(wchar_t));
	lua_concat(L, 2);
	lua_replace(L, idx);
	size_t bsize;
	const wchar_t* res = (const wchar_t*)lua_tolstring(L, idx, &bsize);
	if(psize)
		*psize = (bsize-1)/sizeof(wchar_t);
	return res;
}
#elif LCBC_USE_WIDESTRING == 2
inline void Input::PushWideString(lua_State* L, const wchar_t* wstr, size_t len)
{
	size_t i;
	luaL_Buffer b;
	char buffer[10];
	if(len == 0)
		len = wcslen(wstr);
	luaL_buffinit(L, &b);
	for(i=0;i<len;i++)
	{
		int res = wctomb(buffer, wstr[i]);
		if(res == -1)
			luaL_error(L, "Error converting wide string to characters");
		luaL_addlstring(&b, buffer, res);
	}
	luaL_pushresult(&b);
}

inline const wchar_t* Output::ToWideString(lua_State* L, int idx, size_t* psize)
{
	size_t i;
	luaL_Buffer b;
	wchar_t wchar;
	size_t len, pos = 0;
	const char* psrc = luaL_checklstring(L, idx, &len);
	luaL_buffinit(L, &b);
	for(i=0;i<len;i++)
	{
		int res = mbtowc(&wchar, psrc+pos, len-pos);
		if(res == -1)
			luaL_error(L, "Error converting character to wide string");
		if(res == 0)
		{
			res = 1;
			wchar = 0;
		}
		luaL_addlstring(&b, (const char*)&wchar, sizeof(wchar));
		pos += res;
	}
	wchar = 0;
	luaL_addlstring(&b, (const char*)&wchar, sizeof(wchar)-1);
	luaL_pushresult(&b);
	lua_replace(L, idx-(idx<0));
	size_t bsize;
	const wchar_t* res = (const wchar_t*)lua_tolstring(L, idx, &bsize);
	if(psize)
		*psize = (bsize-1)/sizeof(wchar_t);
	return res;
}

#elif LCBC_USE_WIDESTRING == 3
inline void Input::PushWideString(lua_State* L, const wchar_t* wstr, size_t len)
{
	luaL_Buffer b;
	size_t i;
	wchar_t thres;
	char str[8], car;
	luaL_buffinit(L, &b);
	if(len == 0)
		len = wcslen(wstr);
	for(i=0;i<len;i++)
	{
		char* pstr = str+sizeof(str);
		unsigned int value = wstr[i];
		*--pstr = 0;
		if(value < 0x80)
			*--pstr = (char)value;
		else
		{
			if((value & 0xFC00) == 0xD800) // UTF-16 surrogate pair
			{
				value = 0x10000 + ((value & 0x3FF) << 10);
				if(++i == len || ((wstr[i] & 0xFC00) != 0xDC00))
					break;
				value |= wstr[i] & 0x3FF;
			}
			for(thres=0x40;value>=thres;thres>>=1)
			{
				*--pstr = (char)((value & 0x3F) | 0x80);
				value >>= 6;
			}
			car = char((unsigned)-1 << (8-sizeof(str)+pstr-str) | value);
			*--pstr = car;
		}
		luaL_addlstring(&b, pstr, str+sizeof(str)-1-pstr);
	}
	luaL_pushresult(&b);
}

inline const wchar_t* Output::ToWideString(lua_State* L, int idx, size_t* psize)
{
	luaL_Buffer b;
	int i,mask;
	unsigned int value;
	char car;
	size_t len;
	wchar_t wc;
	const char* str = (const char*)luaL_checklstring(L, idx, &len);
	const char* strend = str + len;
	luaL_buffinit(L, &b);
	while(str < strend)
	{
		car = *str++;
		if((car & 0x80) == 0)
			value = car;
		else
		{
			for(i=1,mask=0x40;car & mask;i++,mask>>=1);
			value = car & (mask - 1);
			if(i == 1 || (value == 0 && (*str & 0x3F) < (0x100 >> i)))
				luaL_error(L, "overlong character in UTF-8");
			for(;i>1;i--)
			{
				car = *str++;
				if((car & 0xC0) != 0x80)
					luaL_error(L, "invalid UTF-8 string");
				value = (value << 6) | (car & 0x3F);
			}
		}
		// For UTF-16, generate surrogate pair outside BMP 
		if(sizeof(wchar_t) == 2 && value >= 0x10000)
		{
			value -= 0x10000;
			wc = (wchar_t)(0xD800 | (value >> 10));
			luaL_addlstring(&b, (const char*)&wc, sizeof(wc));
			wc = (wchar_t)(0xDC00 | (value & 0x3FF));
			luaL_addlstring(&b, (const char*)&wc, sizeof(wc));
		}
		else
		{
			wc = (wchar_t)value;
			luaL_addlstring(&b, (const char*)&wc, sizeof(wc));
		}
	}
	wc = 0;
	luaL_addlstring(&b, (const char*)&wc, sizeof(wc)-1);
	luaL_pushresult(&b);
	lua_replace(L, idx-(idx<0));
	size_t bsize;
	const wchar_t* res = (const wchar_t*)lua_tolstring(L, idx, &bsize);
	if(psize)
		*psize = (bsize-1)/sizeof(wchar_t);
	return res;
}
#endif

#if LCBC_USE_WIDESTRING == 0
#undef wchar_t
#undef wstring
#endif

}
#endif

