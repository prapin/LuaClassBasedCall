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

// Version 2.2.3

#ifndef LUA_CLASSES_BASED_CALL_H
#define LUA_CLASSES_BASED_CALL_H

/* LCBC_USE_WIDESTRING defines the support for wide character (Unicode) strings:
   0 : wchar_t and wstring are not supported; to make sure they are #defined to dummy types;
   1 : wchar_t and wstring can be used as arguments to Input, Output and Script classes.
       You have to call WideString::SetMode<>() template function to set the desired conversion.
   The template parameter of SetMode can be one of these:
   RawMode : no conversion, wide character strings are pushed verbatim to the stack (raw bytes);
       script snippets cannot be wchar_t* (you would get syntax errors looking like Chineese)
   LocaleMode : conversions between wchar_t* and char* use wctomb and mbtowc standard functions;
       the resulting strings depend on your current locale;
   Utf8Mode : conversions between wchar_t* and char* are done by the library, using UTF-8 encoding */
#ifndef LCBC_USE_WIDESTRING
#define LCBC_USE_WIDESTRING 1
#endif

/* LCBC_USE_CSL defines if data containers and some other classes from the 
   C++ Standard Library should be handled. The supported classes are :
   - string, wstring
   - vector<T>, map<K,T>, list<T>, set<T>, deque<T>, multiset<T>, multimap<K,T>
   - queue<T,C>, priority_queue<T,C>, stack<T,C>
   - pair<T1,T2>, valarray<T>, bitset<N>
   0: no support;
   1: C++ Standard Library data can be exchanged with Lua. 
*/
#ifndef LCBC_USE_CSL
#define LCBC_USE_CSL 0
#endif

#ifndef LCBC_USE_QT
#define LCBC_USE_QT 0
#endif

/* LCBC_USE_MFC enables some Microsoft Foundation Classes (MFC)
   objects to be directly handled. The supported classes are :
   - CObject* : using (de-)serialization in a userdata
   - CString (both ANSI and Unicode)
   - CArray<T,A>, CTypedPtrArray<B,T>, CByteArray, CDWordArray, CObArray, 
     CPtrArray, CStringArray, CUIntArray, CWordArray
   - CList<T,A>, CTypedPtrList<B,T>, CPtrList, CObList, CStringList
   - CMap<K,AK,V,AV>, CTypedPtrMap<B,K,V>, CMapWordToPtr, CMapPtrToWord, 
     CMapPtrToPtr, CMapWordToOb, CMapStringToPtr, CMapStringToOb, CMapStringToString
   - CPoint, CRect, CSize, CTime, CTimeSpan
   0: no support
   1: Microsoft Foundation Classes data can be exchanged with Lua 
*/
#ifndef LCBC_USE_MFC
#define LCBC_USE_MFC 0
#endif

#ifndef LCBC_USE_TINYXML
#define LCBC_USE_TINYXML 0
#endif

/* LCBC_USE_EXCEPTIONS enables use of exceptions to signal Lua error.
   On some embedded systems, exceptions are switched off to save code.
*/
#ifndef LCBC_USE_EXCEPTIONS
#define LCBC_USE_EXCEPTIONS 1
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
#include <deque>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <valarray>
#include <bitset>
#endif

#if LCBC_USE_QT
#include <QString>
#endif

#if LCBC_USE_MFC
#define _WIN32_WINNT 0x0500
#include <afx.h>
#include <afxtempl.h>
#endif

#if LCBC_USE_TINYXML
#include <tinyxml.h>
#endif

#if (LUA_VERSION_NUM >= 502) && !defined(lua_objlen)
#define lua_objlen(L,i)		lua_rawlen(L, (i))
#endif

namespace lua {
using namespace std;

enum eNil { nil };

#if !LCBC_USE_WIDESTRING
enum dummy_wchar_t {};
class dummy_wstring {};
#define wchar_t dummy_wchar_t
#define wstring dummy_wstring
size_t wcslen(const dummy_wchar_t*);
#endif

#if !LCBC_USE_QT
enum dummy_QString {};
#define QString dummy_QString
#endif

enum WideStringMode { RawMode, LocaleMode, Utf8Mode };

class WideString
{
public:
	template<WideStringMode mode> static void SetMode(lua_State* L)
	{
		lua_pushcfunction(L, Push<mode>);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaClassBasedPushWideString"); 
		lua_pushcfunction(L, Get<mode>);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaClassBasedGetWideString"); 
	}
	template<WideStringMode input_mode, WideStringMode output_mode> static void SetMode(lua_State* L)
	{
		lua_pushcfunction(L, Push<input_mode>);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaClassBasedPushWideString"); 
		lua_pushcfunction(L, Get<output_mode>);
		lua_setfield(L, LUA_REGISTRYINDEX, "LuaClassBasedGetWideString"); 
	}
	static void Push(lua_State* L, const wchar_t* str) { Push(L, str, wcslen(str)); }
	static void Push(lua_State* L, const wchar_t* str, size_t len)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "LuaClassBasedPushWideString"); 
		lua_pushlstring(L, (const char*)str, len*sizeof(wchar_t));
		lua_call(L, 1, 1);
	}
	static const wchar_t* Get(lua_State* L, int idx) { size_t size; return Get(L, idx, size); }
	static const wchar_t* Get(lua_State* L, int idx, size_t& size)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "LuaClassBasedGetWideString");
		lua_pushvalue(L, idx);
		lua_call(L, 1, 1);
		lua_replace(L, idx);
		const wchar_t* res = (const wchar_t*)lua_tolstring(L, idx, &size);
		size /= sizeof(wchar_t);
		return res;
	}
private:
	template<WideStringMode mode> static int Push(lua_State* L);
	template<WideStringMode mode> static int Get(lua_State* L);
};

class QtString
{
public:
	static QString Get(lua_State* L, int idx) { size_t size; return Get(L, idx, size); }
	static QString Get(lua_State* L, int idx, size_t& size);
	static void Push(lua_State* L, const QString& str);
};

class Input;
class Registry
{
public:
	Registry(const Input& input_) : input(input_) {}
	const Input& get() const { return input; }
private:
	const Input& input;
};

class Input
{
public:
	Input(eNil) { pPush = &Input::PushNil; }
	Input(bool value) { pPush = &Input::PushBoolean; BooleanValue = value; }
	Input(char value) { pPush = &Input::PushCharacter; CharacterValue = value; }
	Input(wchar_t value) { pPush = &Input::PushWideChar; WideCharValue = value; }
	Input(lua_CFunction value) { pPush = &Input::PushFunction; FunctionValue = value; Size = 0;}
	Input(lua_CFunction value, size_t len) { pPush = &Input::PushFunction; FunctionValue = value; Size=len; }
	Input(const Registry& value) { pPush = &Input::PushRegistry; PointerValue = &value; }
	template<class T> Input(T value) { pPush = &Input::PushNumber; NumberValue = (lua_Number)value; }
	template<class T> Input(T* value) { pPush = &Input::PushValue<T>; PointerValue = value; }
	template<class T> Input(const T* value) { pPush = &Input::PushValue<T>; PointerValue = value; }
	template<class T> Input(const T* value, size_t size) { pPush = &Input::PushSizedValue<T>; PointerValue = value; Size = size; }
	template<class T> Input(size_t len, const T* value) { pPush = &Input::PushArray<T>; PointerValue = value; Size=len; }
	template<class T, size_t L2> Input(size_t len1, const T value[][L2]) { pPush = &Input::Push2DArray<T,L2>; PointerValue = value; Size=len1; }
#if LCBC_USE_CSL
	Input(const string& value);
	Input(const wstring& value);
	template<class T1, class T2> Input(const pair<T1,T2>& value)  { pPush = &Input::PushPair<pair<T1,T2> >; PointerValue = &value; }
	template<class T, class A> Input(const vector<T,A>& value) { pPush = &Input::PushContainer<vector<T,A> >; PointerValue = &value; }
	template<class T, class A> Input(const list<T,A>& value) { pPush = &Input::PushContainer<list<T,A> >; PointerValue = &value; }
	template<class T, class A> Input(const deque<T,A>& value) { pPush = &Input::PushContainer<deque<T,A> >; PointerValue = &value; }
	template<class K, class T, class C, class A> Input(const map<K,T,C,A>& value) { pPush = &Input::PushMap<map<K,T,C,A> >; PointerValue = &value; }
	template<class K, class T, class C, class A> Input(const multimap<K,T,C,A>& value) { pPush = &Input::PushContainer<multimap<K,T,C,A> >; PointerValue = &value; }
	template<class T, class C, class A> Input(const set<T,C,A>& value) { pPush = &Input::PushSet<set<T,C,A> >; PointerValue = &value; }
	template<class T, class C, class A> Input(const multiset<T,C,A>& value) { pPush = &Input::PushSet<multiset<T,C,A> >; PointerValue = &value; }
	template<class T, class C> Input(const queue<T,C>& value) { pPush = &Input::PushQueue<queue<T,C> >; PointerValue = &value; }
	template<class T, class C> Input(const stack<T,C>& value) { pPush = &Input::PushQueue<stack<T,C> >; PointerValue = &value; }
	template<class T, class C, class P> Input(const priority_queue<T,C,P>& value) { pPush = &Input::PushQueue<priority_queue<T,C,P> >; PointerValue = &value; }
	template<class T> Input(const valarray<T>& value) { pPush = &Input::PushValArray<valarray<T> >; PointerValue = &value; }
	template<size_t N> Input(const bitset<N>& value) { pPush = &Input::PushValArray<bitset<N> >; PointerValue = &value; }
#endif
#if LCBC_USE_MFC
	Input(const CStringA& value);
	Input(const CStringW& value);
	Input(const CPoint& value);
	Input(const CRect& value);
	Input(const CSize& value);
	Input(const CTime& value);
	Input(const CTimeSpan& value);
	template<class T, class A> Input(const CArray<T,A>& value) { pPush = &Input::PushCArray<CArray<T,A> >; PointerValue = &value; }
	template<class B, class T> Input(const CTypedPtrArray<B,T>& value) { pPush = &Input::PushCArray<CTypedPtrArray<B,T> >; PointerValue = &value; }
	Input(const CByteArray& value) { pPush = &Input::PushCArray<CByteArray>; PointerValue = &value; }
	Input(const CDWordArray& value) { pPush = &Input::PushCArray<CDWordArray>; PointerValue = &value; }
	Input(const CObArray& value) { pPush = &Input::PushCArray<CObArray>; PointerValue = &value; }
	Input(const CPtrArray& value) { pPush = &Input::PushCArray<CPtrArray>; PointerValue = &value; }
	Input(const CStringArray& value) { pPush = &Input::PushCArray<CStringArray>; PointerValue = &value; }
	Input(const CUIntArray& value) { pPush = &Input::PushCArray<CUIntArray>; PointerValue = &value; }
	Input(const CWordArray& value) { pPush = &Input::PushCArray<CWordArray>; PointerValue = &value; }
	template<class T, class A> Input(const CList<T,A>& value) { pPush = &Input::PushCList<CList<T,A> >; PointerValue = &value; }
	template<class B, class T> Input(const CTypedPtrList<B,T>& value) { pPush = &Input::PushCList<CTypedPtrList<B,T> >; PointerValue = &value; }
	Input(const CPtrList& value) { pPush = &Input::PushCList<CPtrList>; PointerValue = &value; }
	Input(const CObList& value) { pPush = &Input::PushCList<CObList>; PointerValue = &value; }
	Input(const CStringList& value) { pPush = &Input::PushCList<CStringList>; PointerValue = &value; }
	template<class K, class AK, class V, class AV> Input(const CMap<K,AK,V,AV>& value) { pPush = &Input::PushCMap<CMap<K,AK,V,AV>,K,V>; PointerValue = &value; }
	template<class B, class K, class V> Input(const CTypedPtrMap<B,K,V>& value) { pPush = &Input::PushCMap<CTypedPtrMap<B,K,V>,K,V>; PointerValue = &value; }
	Input(const CMapWordToPtr& value) { pPush = &Input::PushCMap<CMapWordToPtr, WORD, void*>; PointerValue = &value; }
	Input(const CMapPtrToWord& value) { pPush = &Input::PushCMap<CMapPtrToWord, void*, WORD>; PointerValue = &value; }
	Input(const CMapPtrToPtr& value) { pPush = &Input::PushCMap<CMapPtrToPtr, void*, void*>; PointerValue = &value; }
	Input(const CMapWordToOb& value) { pPush = &Input::PushCMap<CMapWordToOb, WORD, CObject*>; PointerValue = &value; }
	Input(const CMapStringToPtr& value) { pPush = &Input::PushCMap<CMapStringToPtr, CString, void*>; PointerValue = &value; }
	Input(const CMapStringToOb& value) { pPush = &Input::PushCMap<CMapStringToOb, CString, CObject*>; PointerValue = &value; }
	Input(const CMapStringToString& value) { pPush = &Input::PushCMap<CMapStringToString, CString, CString>; PointerValue = &value; }
#endif
#if LCBC_USE_TINYXML
	Input(const TiXmlDocument& value) { pPush = &Input::PushTiXmlDocument; PointerValue = &value; }
#endif
	void Push(lua_State* L) const { (this->*pPush)(L); }
private:
	void PushNil(lua_State* L) const { lua_pushnil(L); }
	void PushBoolean(lua_State* L) const { lua_pushboolean(L, BooleanValue); }
	void PushCharacter(lua_State* L) const { lua_pushlstring(L, &CharacterValue, 1); }
	void PushWideChar(lua_State* L) const { WideString::Push(L, &WideCharValue, 1); }
	void PushNumber(lua_State* L) const { lua_pushnumber(L, NumberValue); }
	void PushFunction(lua_State* L) const { lua_pushcclosure(L, FunctionValue, (int)Size); }
	void PushRegistry(lua_State* L) const { ((const Registry*)PointerValue)->get().Push(L); lua_rawget(L, LUA_REGISTRYINDEX); }
	template<class T> void PushNumber(lua_State* L) const { lua_pushnumber(L, lua_Number(*(T*)PointerValue)); }
	template<class T> void PushValue(lua_State* L) const;
	template<class T> void PushSizedValue(lua_State* L) const;
	template<class T> void PushArray(lua_State* L) const;
	template<class T, size_t L2> void Push2DArray(lua_State* L) const;
	template<class T> void PushContainer(lua_State* L, const T* val) const;
	template<class T> void PushContainer(lua_State* L) const { return PushContainer(L, (const T*)PointerValue); }
	template<class T> void PushSet(lua_State* L) const;
	template<class T> void PushMap(lua_State* L) const;
	template<class T> void PushQueue(lua_State* L) const;
	template<class T> void PushValArray(lua_State* L) const;
	template<class T> void PushPair(lua_State* L) const;
	template<class T> void PushCArray(lua_State* L) const;
	template<class T> void PushCList(lua_State* L) const;
	template<class T, class K, class V> void PushCMap(lua_State* L) const;
	void PushTiXmlDocument(lua_State* L) const;

	void (Input::*pPush)(lua_State* L) const;
	union 
	{
		bool BooleanValue;
		char CharacterValue;
		wchar_t WideCharValue;
		lua_Number NumberValue;
		const void* PointerValue;
		lua_CFunction FunctionValue;
	};
	size_t Size;
};

class SizeRef
{
public:
	SizeRef(size_t size) : Size(size) {}
	operator size_t&() { return Size; }
private:
	size_t Size;
};

class Output
{
public:
	Output(eNil) { pGet = &Output::GetNil; }
	Output(const Registry& value) { pGet = &Output::GetRegistry; PointerValue = (void*)&value; }
	template<class T> Output(T& value) { pGet = &Output::GetValue<T>; PointerValue = &value; }
	template<class T> Output(size_t& size, T* value) { memset(value, 0, size*sizeof(T)); pGet = &Output::GetArray<T>; pSize = &size; PointerValue = value; }
	template<class T> Output(const T*& value, size_t& size) { pGet = &Output::GetSizedValue<T>; pSize = &size; PointerValue = &value; }
	template<class T, size_t L2> Output(size_t& len1, T value[][L2]) {pGet = &Output::Get2DArray<T,L2>; pSize = &len1; PointerValue = value;  }
#if LCBC_USE_CSL
	template<class T1, class T2> Output(pair<T1,T2>& value)  { pGet = &Output::GetPair<pair<T1,T2> >; PointerValue = &value; }
	template<class T, class A> Output(vector<T,A>& value) { pGet = &Output::GetContainer<vector<T,A> >; PointerValue = &value; }
	template<class T, class A> Output(list<T,A>& value) { pGet = &Output::GetContainer<list<T,A> >; PointerValue = &value; }
	template<class T, class A> Output(deque<T,A>& value) { pGet = &Output::GetContainer<deque<T,A> >; PointerValue = &value; }
	template<class K, class T, class C, class A> Output(map<K,T,C,A>& value) { pGet = &Output::GetMap<map<K,T,C,A> >; PointerValue = &value; }
	template<class K, class T, class C, class A> Output(multimap<K,T,C,A>& value) { pGet = &Output::GetMultiMap<multimap<K,T,C,A> >; PointerValue = &value; }
	template<class T, class C, class A> Output(set<T,C,A>& value) { pGet = &Output::GetSet<set<T,C,A> >; PointerValue = &value; }
	template<class T, class C, class A> Output(multiset<T,C,A>& value) { pGet = &Output::GetSet<multiset<T,C,A> >; PointerValue = &value; }
	template<class T, class C> Output(queue<T,C>& value) { pGet = &Output::GetQueue<queue<T,C> >; PointerValue = &value; }
	template<class T, class C> Output(stack<T,C>& value) { pGet = &Output::GetQueue<stack<T,C> >; PointerValue = &value; }
	template<class T, class C, class P> Output(priority_queue<T,C,P>& value) { pGet = &Output::GetQueue<priority_queue<T,C,P> >; PointerValue = &value; }
	template<class T> Output(valarray<T>& value) { pGet = &Output::GetValArray<valarray<T>,T>; PointerValue = &value; }
	template<size_t N> Output(bitset<N>& value) { pGet = &Output::GetBitSet<bitset<N> >; PointerValue = &value; }
#endif
#if LCBC_USE_MFC
	template<class T, class A> Output(CArray<T,A>& value) { pGet = &Output::GetCArray<CArray<T,A>,T>; PointerValue = &value; }
	template<class B, class T> Output(CTypedPtrArray<B,T>& value) { pGet = &Output::GetCArray<CTypedPtrArray<B,T>,T>; PointerValue = &value; }
	Output(CByteArray& value) { pGet = &Output::GetCArray<CByteArray, BYTE>; PointerValue = &value; }
	Output(CDWordArray& value) { pGet = &Output::GetCArray<CDWordArray, DWORD>; PointerValue = &value; }
	Output(CObArray& value) { pGet = &Output::GetCArray<CObArray, CObject*>; PointerValue = &value; }
	Output(CPtrArray& value) { pGet = &Output::GetCArray<CPtrArray, void*>; PointerValue = &value; }
	Output(CStringArray& value) { pGet = &Output::GetCArray<CStringArray, CString>; PointerValue = &value; }
	Output(CUIntArray& value) { pGet = &Output::GetCArray<CUIntArray, UINT>; PointerValue = &value; }
	Output(CWordArray& value) { pGet = &Output::GetCArray<CWordArray, WORD>; PointerValue = &value; }
	template<class T, class A> Output(CList<T,A>& value) { pGet = &Output::GetCList<CList<T,A>,T>; PointerValue = &value; }
	template<class B, class T> Output(CTypedPtrList<B,T>& value) { pGet = &Output::GetCList<CTypedPtrList<B,T>,T>; PointerValue = &value; }
	Output(CPtrList& value) { pGet = &Output::GetCList<CPtrList, void*>; PointerValue = &value; }
	Output(CObList& value) { pGet = &Output::GetCList<CObList, CObject*>; PointerValue = &value; }
	Output(CStringList& value) { pGet = &Output::GetCList<CStringList, CString>; PointerValue = &value; }
	template<class K, class AK, class V, class AV> Output(CMap<K,AK,V,AV>& value) { pGet = &Output::GetCMap<CMap<K,AK,V,AV>,K,V>; PointerValue = &value; }
	template<class B, class K, class V> Output(CTypedPtrMap<B,K,V>& value) { pGet = &Output::GetCMap<CTypedPtrMap<B,K,V>,K,V>; PointerValue = &value; }
	Output(CMapWordToPtr& value) { pGet = &Output::GetCMap<CMapWordToPtr, WORD, void*>; PointerValue = &value; }
	Output(CMapPtrToWord& value) { pGet = &Output::GetCMap<CMapPtrToWord, void*, WORD>; PointerValue = &value; }
	Output(CMapPtrToPtr& value) { pGet = &Output::GetCMap<CMapPtrToPtr, void*, void*>; PointerValue = &value; }
	Output(CMapWordToOb& value) { pGet = &Output::GetCMap<CMapWordToOb, WORD, CObject*>; PointerValue = &value; }
	Output(CMapStringToPtr& value) { pGet = &Output::GetCMap<CMapStringToPtr, CString, void*>; PointerValue = &value; }
	Output(CMapStringToOb& value) { pGet = &Output::GetCMap<CMapStringToOb, CString, CObject*>; PointerValue = &value; }
	Output(CMapStringToString& value) { pGet = &Output::GetCMap<CMapStringToString, CString, CString>; PointerValue = &value; }
#endif

	void Get(lua_State* L, int idx) const  { (this->*pGet)(L, idx); }
private:
	size_t GetSize(size_t s1) const { size_t s2=*pSize; *pSize=s1; return s1 < s2 ? s1 : s2; }
	void GetNil(lua_State* /*L*/, int /*idx*/) const {}
	void GetRegistry(lua_State* L, int idx) const 
	{ 
		const Registry* reg = (const Registry*)PointerValue; 
		reg->get().Push(L); 
		lua_pushvalue(L, idx);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}
	template<class T> void GetValue(lua_State* L, int idx) const { *(T*)PointerValue = (T)luaL_checknumber(L, idx); }
	template<class T> void GetSizedValue(lua_State* L, int idx) const;
	template<class T> void GetArray(lua_State* L, int idx) const;
	template<class T, size_t L2> void Get2DArray(lua_State* L, int idx) const;
	template<class T> void GetPair(lua_State* L, int idx) const;
	template<class T> void GetContainer(lua_State* L, int idx) const;
	template<class T> void GetMultiMap(lua_State* L, int idx) const;
	template<class T> void GetMap(lua_State* L, int idx) const;
	template<class T> void GetSet(lua_State* L, int idx) const;
	template<class T> void GetQueue(lua_State* L, int idx) const;
	template<class T, class V> void GetValArray(lua_State* L, int idx) const;
	template<class T> void GetBitSet(lua_State* L, int idx) const;
	template<class C, class T> void GetCArray(lua_State* L, int idx) const;
	template<class C, class T> void GetCList(lua_State* L, int idx) const;
	template<class T, class K, class V> void GetCMap(lua_State* L, int idx) const;

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

template<> inline void Output::GetValue<char>(lua_State* L, int idx) const
{
	size_t len;
	const char* str = luaL_checklstring(L, idx, &len);
	if(len != 1)
		luaL_error(L, "String length must be 1 for char type but is %d bytes", len);
	*(char*)PointerValue = *str;
}

template<> inline void Output::GetValue<wchar_t>(lua_State* L, int idx) const
{
	size_t len;
	const wchar_t* str = WideString::Get(L, idx, len);
	if(len != 1)
		luaL_error(L, "String length must be 1 for wchar_t type but is %d characters", len);
	*(wchar_t*)PointerValue = *str;
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

template<> inline void Output::GetValue<void*>(lua_State* L, int idx) const
{
	*(void**)PointerValue = lua_touserdata(L, idx);
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
	WideString::Push(L, (const wchar_t*)PointerValue);
}

template<> inline void Input::PushSizedValue<wchar_t>(lua_State* L) const
{
	WideString::Push(L, (const wchar_t*)PointerValue, Size);
}

template<> inline void Output::GetArray<wchar_t>(lua_State* L, int idx) const
{
	size_t size;
	const wchar_t* str = WideString::Get(L, idx, size);
	memcpy(PointerValue, str, GetSize(size+1)*sizeof(wchar_t));
}

template<> inline void Output::GetValue<const wchar_t*>(lua_State* L, int idx) const
{
	*(const wchar_t**)PointerValue = WideString::Get(L, idx);
}

template<> inline void Output::GetSizedValue<wchar_t>(lua_State* L, int idx) const
{
	*(const wchar_t**)PointerValue = WideString::Get(L, idx, *pSize);
}

#if LCBC_USE_CSL
// This class is a hack to access the container member in adapters!
template<class T> class myqueue : public T
{
public:
	myqueue(const T& src) : T(src) {}
	typename T::container_type& get_container() { return this->c; }
};

template<class T> inline void Input::PushPair(lua_State* L) const
{
	const T* p = (const T*)PointerValue;
	lua_createtable(L, 2, 0);
	Input first(p->first);
	first.Push(L);
	lua_rawseti(L, -2, 1);
	Input second(p->second);
	second.Push(L);
	lua_rawseti(L, -2, 2);
}

template<class T> inline void Input::PushMap(lua_State* L) const
{
	const T* m = (const T*)PointerValue;
	typename T::const_iterator it;
	lua_createtable(L, 0, (int)m->size());
	for (it=m->begin() ; it != m->end(); it++)
	{
		Input key(it->first);
		key.Push(L);
		Input value(it->second);
		value.Push(L);
		lua_settable(L, -3);
	}
}

template<class T> inline void Input::PushContainer(lua_State* L, const T* v) const
{
	lua_createtable(L, (int)v->size(), 0);
	typename T::const_iterator it;
	int i=0;
	for (it=v->begin(); it != v->end(); it++,i++)
	{
		lua_pushinteger(L, i+1);
		Input input(*it);
		input.Push(L);
		lua_settable(L, -3);
	}
}

template<class T> inline void Input::PushSet(lua_State* L) const
{
	const T* s = (const T*)PointerValue;
	typename T::const_iterator it;
	lua_createtable(L, 0, (int)s->size());
	for (it=s->begin() ; it != s->end(); it++)
	{
		Input key(*it);
		key.Push(L);
		lua_pushinteger(L, s->count(*it));
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

template<class T> inline void Input::PushQueue(lua_State* L) const
{
	const T* q = (const T*)PointerValue;
	myqueue<T> copy(*q);
	typename T::container_type& c = copy.get_container();
	PushContainer(L, &c);
}

template<class T> inline void Input::PushValArray(lua_State* L) const
{
	const T* v = (const T*)PointerValue;
	size_t size = v->size();
	lua_createtable(L, (int)size, 0);
	for(size_t i=0;i<size;i++)
	{
		Input input((*v)[i]);
		input.Push(L);
		lua_rawseti(L, -2, (int)i+1);
	}
}

template<class T> inline void Output::GetPair(lua_State* L, int idx) const
{
	T* p = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	Output o1(p->first);
	o1.Get(L, top+1);
	Output o2(p->second);
	o2.Get(L, top+2);
	lua_settop(L, top);
}

template<> inline void Output::GetValue<string>(lua_State* L, int idx) const
{
	size_t size; 
	const char* str = luaL_checklstring(L, idx, &size); 
	((string*)PointerValue)->assign(str, size);
}

template<class T> inline void Output::GetContainer(lua_State* L, int idx) const
{
	T* v = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		typename T::value_type value;
		Output output(value);
		output.Get(L, top+1);
		v->push_back(value);
		lua_settop(L, top);
	}
}

template<class T> inline void Output::GetMultiMap(lua_State* L, int idx) const
{
	T* v = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		pair<typename T::key_type, typename T::mapped_type> value;
		Output output(value);
		output.Get(L, top+1);
		v->insert(value);
		lua_settop(L, top);
	}
}
template<class T> inline void Output::GetSet(lua_State* L, int idx) const
{
	T* s = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		lua_pushvalue(L, top+1);
		typename T::value_type key;
		Output outputKey(key);
		outputKey.Get(L, top+3);
		int val = luaL_checkint(L, top+2);
		lua_settop(L, top+1);
		for(int i=0;i<val;i++)
			s->insert(key);
	}
	lua_settop(L, top);
}

template<class T> inline void Output::GetMap(lua_State* L, int idx) const
{
	T* m = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		lua_pushvalue(L, top+1);
		typename T::key_type key;
		Output outputKey(key);
		outputKey.Get(L, top+3);
		typename T::mapped_type value;
		Output output(value);
		output.Get(L, top+2);
		lua_settop(L, top+1);
		(*m)[key] = value;
	}
	lua_settop(L, top);
}

template<class T> inline void Output::GetQueue(lua_State* L, int idx) const
{
	T* q = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		typename T::value_type value;
		Output output(value);
		output.Get(L, top+1);
		q->push(value);
		lua_settop(L, top);
	}
}

template<class T, class V> inline void Output::GetValArray(lua_State* L, int idx) const
{
	T* v = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	v->resize(len);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		V value;
		Output output(value);
		output.Get(L, top+1);
		(*v)[i] = value;
		lua_settop(L, top);
	}
}

template<class T> inline void Output::GetBitSet(lua_State* L, int idx) const
{
	T* v = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	size_t len = lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(size_t i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, (int)i+1);
		bool value;
		Output output(value);
		output.Get(L, top+1);
		(*v)[i] = value;
		lua_settop(L, top);
	}
}

#if LCBC_USE_WIDESTRING
template<> inline void Input::PushValue<wstring>(lua_State* L) const
{
	wstring* str = (wstring*)PointerValue;
	WideString::Push(L, str->data(), str->size());
}

inline Input::Input(const wstring& value) 
{ 
	pPush = &Input::PushValue<wstring>; 
	PointerValue = &value; 
}

template<> inline void Output::GetValue<wstring>(lua_State* L, int idx) const
{
	size_t size; 
	const wchar_t* str = WideString::Get(L, idx, size); 
	((wstring*)PointerValue)->assign(str, size);
}
#endif
#endif

#if LCBC_USE_MFC

template<class T> inline void Input::PushCArray(lua_State* L) const
{
	const T* v = (const T*)PointerValue;
	lua_createtable(L, (int)v->GetSize(), 0);
	for(int i=0;i<v->GetSize();i++)
	{
		Input input(v->GetAt(i));
		input.Push(L);
		lua_rawseti(L, -2, i+1);
	}
}

template<class T> inline void Input::PushCList(lua_State* L) const
{
	const T* v = (const T*)PointerValue;
	lua_createtable(L, (int)v->GetCount(), 0);
	POSITION pos = v->GetHeadPosition();
	for(int i=0;pos;i++)
	{
		Input input(v->GetNext(pos));
		input.Push(L);
		lua_rawseti(L, -2, i+1);
	}
}

template<class T, class K, class V> inline void Input::PushCMap(lua_State* L) const
{
	const T* v = (const T*)PointerValue;
	lua_createtable(L, 0, (int)v->GetCount());
	POSITION pos = v->GetStartPosition();
	while(pos)
	{
		K key;
		V value;
		v->GetNextAssoc(pos, key, value);
		Input inkey(key);
		inkey.Push(L);
		Input invalue(value);
		invalue.Push(L);
		lua_rawset(L, -3);
	}
}

template<class C, class T> inline void Output::GetCArray(lua_State* L, int idx) const
{
	C* v = (C*)PointerValue;
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

template<class C, class T> inline void Output::GetCList(lua_State* L, int idx) const
{
	C* v = (C*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int len = (int)lua_objlen(L, idx);
	int top = lua_gettop(L);
	for(int i=0;i<len;i++)
	{
		lua_rawgeti(L, idx, i+1);
		T value;
		Output output(value);
		output.Get(L, top+1);
		v->AddTail(value);
		lua_settop(L, top);
	}
}

template<class T, class K, class V> inline void Output::GetCMap(lua_State* L, int idx) const
{
	T* v = (T*)PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		lua_pushvalue(L, top+1);
		K key;
		Output outputKey(key);
		outputKey.Get(L, top+3);
		V value;
		Output output(value);
		output.Get(L, top+2);
		lua_settop(L, top+1);
		v->SetAt(key, value);
	}
	lua_settop(L, top);
}

template<> inline void Input::PushValue<CStringA>(lua_State* L) const
{
	const CStringA* str = (const CStringA*)PointerValue;
	lua_pushlstring(L, *str, str->GetLength());
}

inline Input::Input(const CStringA& value) 
{ 
	pPush = &Input::PushValue<CStringA>; 
	PointerValue = &value; 
}

template<> inline void Input::PushValue<CPoint>(lua_State* L) const
{
	const CPoint* p = (const CPoint*)PointerValue;
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, p->x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, p->y);
	lua_setfield(L, -2, "y");
}

inline Input::Input(const CPoint& value) 
{ 
	pPush = &Input::PushValue<CPoint>; 
	PointerValue = &value; 
}

template<> inline void Input::PushValue<CRect>(lua_State* L) const
{
	const CRect* r = (const CRect*)PointerValue;
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, r->top);
	lua_setfield(L, -2, "top");
	lua_pushinteger(L, r->bottom);
	lua_setfield(L, -2, "bottom");
	lua_pushinteger(L, r->left);
	lua_setfield(L, -2, "left");
	lua_pushinteger(L, r->right);
	lua_setfield(L, -2, "right");
}

inline Input::Input(const CRect& value) 
{ 
	pPush = &Input::PushValue<CRect>; 
	PointerValue = &value; 
}

template<> inline void Input::PushValue<CSize>(lua_State* L) const
{
	const CSize* s = (const CSize*)PointerValue;
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, s->cx);
	lua_setfield(L, -2, "cx");
	lua_pushinteger(L, s->cy);
	lua_setfield(L, -2, "cy");
}

inline Input::Input(const CSize& value) 
{ 
	pPush = &Input::PushValue<CSize>; 
	PointerValue = &value; 
}

template<> inline void Input::PushValue<CTime>(lua_State* L) const
{
	const CTime* t = (const CTime*)PointerValue;
	lua_pushnumber(L, (lua_Number)t->GetTime());
}

inline Input::Input(const CTime& value) 
{ 
	pPush = &Input::PushValue<CTime>; 
	PointerValue = &value; 
}

template<> inline void Input::PushValue<CTimeSpan>(lua_State* L) const
{
	const CTimeSpan* t = (const CTimeSpan*)PointerValue;
	lua_pushnumber(L, (lua_Number)t->GetTimeSpan());
}

inline Input::Input(const CTimeSpan& value) 
{ 
	pPush = &Input::PushValue<CTimeSpan>; 
	PointerValue = &value; 
}

template<> inline void Input::PushValue<CObject>(lua_State* L) const
{
	try 
	{
		CMemFile file;
		CArchive ar(&file, CArchive::store);
		const CObject* obj = (const CObject*)PointerValue;
		ar << obj;
		ar.Flush();
		size_t len = (size_t)file.GetLength();
		BYTE* buffer = file.Detach();
		void* pdst = lua_newuserdata(L, len);
		memcpy(pdst, buffer, len);
		delete [] buffer;
	}
	catch(CException* )
	{
		luaL_error(L, "Cannot serialize MFC object");
	}
}

template<> inline void Output::GetValue<CStringA>(lua_State* L, int idx) const
{
	size_t size; 
	const char* str = luaL_checklstring(L, idx, &size); 
	*(CStringA*)PointerValue = CStringA(str, (int)size);
}

template<> inline void Output::GetValue<CObject*>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TUSERDATA);
	void* buffer = lua_touserdata(L, idx);
	size_t len = lua_objlen(L, idx);
	try 
	{
		CMemFile file((BYTE*)buffer, (UINT)len);
		CArchive ar(&file, CArchive::load);
		CObject* obj;
		ar >> obj;
		ar.Flush();
		*(CObject**)PointerValue = obj;
	}
	catch(CException&)
	{
		luaL_error(L, "Cannot deserialize MFC object");
	}
}

template<> inline void Output::GetValue<CPoint>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	CPoint* p = (CPoint*)PointerValue;
	lua_getfield(L, idx, "x");
	p->x = luaL_checkint(L, -1);
	lua_getfield(L, idx, "y");
	p->y = luaL_checkint(L, -1);
	lua_settop(L, top);
}

template<> inline void Output::GetValue<CRect>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	CRect* p = (CRect*)PointerValue;
	lua_getfield(L, idx, "top");
	p->top = luaL_checkint(L, -1);
	lua_getfield(L, idx, "bottom");
	p->bottom = luaL_checkint(L, -1);
	lua_getfield(L, idx, "left");
	p->left = luaL_checkint(L, -1);
	lua_getfield(L, idx, "right");
	p->right = luaL_checkint(L, -1);
	lua_settop(L, top);
}

template<> inline void Output::GetValue<CSize>(lua_State* L, int idx) const
{
	luaL_checktype(L, idx, LUA_TTABLE);
	int top = lua_gettop(L);
	CSize* p = (CSize*)PointerValue;
	lua_getfield(L, idx, "cx");
	p->cx = luaL_checkint(L, -1);
	lua_getfield(L, idx, "cy");
	p->cy = luaL_checkint(L, -1);
	lua_settop(L, top);
}

template<> inline void Output::GetValue<CTime>(lua_State* L, int idx) const
{
	CTime* t = (CTime*)PointerValue;
	*t = (time_t)luaL_checknumber(L, idx);
}

template<> inline void Output::GetValue<CTimeSpan>(lua_State* L, int idx) const
{
	CTimeSpan* t = (CTimeSpan*)PointerValue;
	*t = (time_t)luaL_checknumber(L, idx);
}

#if LCBC_USE_WIDESTRING
template<> inline void Input::PushValue<CStringW>(lua_State* L) const
{
	CStringW* str = (CStringW*)PointerValue;
	WideString::Push(L, *str, str->GetLength());
}

inline Input::Input(const CStringW& value) 
{ 
	pPush = &Input::PushValue<CStringW>; 
	PointerValue = &value; 
}

template<> inline void Output::GetValue<CStringW>(lua_State* L, int idx) const
{
	size_t size; 
	const wchar_t* str = WideString::Get(L, idx, size); 
	*(CStringW*)PointerValue = CStringW(str, (int)size);
}
#endif
#endif

#if LCBC_USE_TINYXML
template<> inline void Input::PushValue<TiXmlNode>(lua_State* L) const
{
	const TiXmlNode* node = (const TiXmlNode*)PointerValue;
	int type = node->Type();
	switch(type)
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		break;
	case TiXmlNode::TINYXML_ELEMENT:
	{
		Input v(node->ToElement());
		v.Push(L);
		break;
	}
	case TiXmlNode::TINYXML_COMMENT:
		lua_pushfstring(L, "<!--%s-->", node->Value());
		break;
	case TiXmlNode::TINYXML_UNKNOWN:
		lua_pushfstring(L, "<%s>", node->Value());
		break;
	case TiXmlNode::TINYXML_TEXT:
		lua_pushfstring(L, "%s%s", *node->Value() == '<' ? " " : "", node->Value());
		break;
	case TiXmlNode::TINYXML_DECLARATION:
	{
		const TiXmlDeclaration* decl = node->ToDeclaration();
		int top = lua_gettop(L);
		lua_pushstring(L, "<?xml ");
		if(*decl->Version())
			lua_pushfstring(L,  "version=\"%s\" ", decl->Version());
		if(*decl->Encoding())
			lua_pushfstring(L,  "encoding=\"%s\" ", decl->Encoding());
		if(*decl->Standalone())
			lua_pushfstring(L,  "standalone=\"%s\" ", decl->Standalone());
		lua_pushstring(L, "?>");
		lua_concat(L, lua_gettop(L)-top);
		break;
	}
	}
}
template<> inline void Input::PushValue<TiXmlElement>(lua_State* L) const
{
	const TiXmlElement* elem = (const TiXmlElement*)PointerValue;
	lua_createtable(L, 0, 1);
	lua_pushstring(L, elem->Value());
	lua_rawseti(L, -2, 0);
	for(const TiXmlAttribute* attrib = elem->FirstAttribute();attrib;attrib=attrib->Next())
	{
		lua_pushstring(L, attrib->Value());
		if(lua_isnumber(L, -1))
		{
			lua_pushnumber(L, lua_tonumber(L, -1));
			lua_replace(L, -2);
		}
		lua_setfield(L, -2, attrib->Name());
	}
	int i=0;
	for(const TiXmlNode* child = elem->FirstChild();child;child = child->NextSibling(),i++)
	{
		Input v(child);
		v.Push(L);
		lua_rawseti(L, -2, i+1);
	}
}
inline void Input::PushTiXmlDocument(lua_State* L) const
{
	const TiXmlDocument* elem = (const TiXmlDocument*)PointerValue;
	lua_createtable(L, 0, 0);
	int i=0;
	for(const TiXmlNode* child = elem->FirstChild();child;child = child->NextSibling(),i++)
	{
		Input v(child);
		v.Push(L);
		lua_rawseti(L, -2, i+1);
	}
}
template<> inline void Output::GetValue<TiXmlNode*>(lua_State* L, int idx) const
{
	TiXmlNode** pnode = (TiXmlNode**)PointerValue;
	if(lua_istable(L, idx))
	{
		TiXmlElement* elem;
		Output v(elem);
		v.Get(L, idx);
		*pnode = elem;
	}
	else
	{
		const char* str = luaL_checkstring(L, idx);
		if(*str == '<')
		{
			TiXmlDocument doc;
			doc.Parse(str);
			*pnode = doc.FirstChild()->Clone();
		}
		else
		{
			*pnode = new TiXmlText(str);
		}
	}
}
template<> inline void Output::GetValue<TiXmlElement*>(lua_State* L, int idx) const
{
	TiXmlElement** pelem = (TiXmlElement**)PointerValue;
	int top = lua_gettop(L);
	lua_rawgeti(L, idx, 0);
	TiXmlElement* elem = new TiXmlElement(luaL_checkstring(L, top+1));
	lua_settop(L, top);
	*pelem = elem;
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		if(lua_type(L, top+1) == LUA_TSTRING)
		{
			elem->SetAttribute(lua_tostring(L, top+1), lua_tostring(L, top+2));
		}
		lua_settop(L, top+1);
	}
	lua_settop(L, top);
	int len=(int)lua_objlen(L, idx);
	for(int i=1;i<=len;i++)
	{
		lua_rawgeti(L, idx, i);
		TiXmlNode* node;
		Output v(node);
		v.Get(L, top+1);
		elem->LinkEndChild(node);
		lua_settop(L, top);
	}
}

template<> inline void Output::GetValue<TiXmlDocument>(lua_State* L, int idx) const
{
	TiXmlDocument* doc = (TiXmlDocument*)PointerValue;
	int top = lua_gettop(L);
	int len=(int)lua_objlen(L, idx);
	for(int i=1;i<=len;i++)
	{
		lua_rawgeti(L, idx, i);
		TiXmlNode* node;
		Output v(node);
		v.Get(L, top+1);
		doc->LinkEndChild(node);
		lua_settop(L, top);
	}
}

#endif


template<class T>
class Array
{
public:
	typedef const T& ref;
	Array() : Size(0) {}
	Array(const Array& src) { Init(src.Arguments, src.Size); }
	Array(const T* src[], size_t size) { Init(src, size); }
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
	void add(const T& arg) { Arguments[Size++] = &arg; }
	void empty() { Size = 0; }
private:
	void Init(const T* args[], size_t len)
	{
		Size = len;
		for(size_t i=0;i<len;i++)
			Arguments[i] = args[i];
	}
	size_t Size;
	const T* Arguments[32];
};

template<class C>
class ErrorT
{
public:
	ErrorT(C message) : Message(message)  {}
	operator C() const { return Message; }
	C str() const { return Message; }
private:
	C Message;
};

typedef ErrorT<const char*> ErrorA;
typedef ErrorT<const wchar_t*> ErrorW;
#if defined(_UNICODE) || defined(UNICODE)
typedef ErrorW Error;
#else
typedef ErrorA Error;
#endif

typedef Array<Input> Inputs;
typedef Array<Output> Outputs;

class Script
{
public:
	Script(const char* snippet) : string(snippet) { pKey=&Script::KeyString; pLoad=&Script::LoadString; }
	Script(const wchar_t* snippet) : wstring(snippet) { pKey=&Script::KeyWString; pLoad=&Script::LoadWString; }
	Script(const char* snippet, const char* name_) : string(snippet), name(name_) { pKey=&Script::KeyString; pLoad=&Script::LoadNamedString; }
	Script(const wchar_t* snippet, const wchar_t* name) : wstring(snippet), wname(name) { pKey=&Script::KeyWString; pLoad=&Script::LoadWNamedString; }
	Script(const QString& snippet) : qstring(&snippet) { pKey=&Script::KeyQString; pLoad=&Script::LoadQString; }
	Script(const QString& snippet, const QString& name) : qstring(&snippet), qname(&name) { pKey=&Script::KeyQString; pLoad=&Script::LoadQNamedString; }
	void pushkey(lua_State* L) const { (this->*pKey)(L); }
	int load(lua_State* L) const { return (this->*pLoad)(L); }
protected:
	Script() { pKey=&Script::KeyNil; }
	void KeyString(lua_State* L) const { lua_pushstring(L, string); }
	int LoadString(lua_State* L) const { return luaL_loadstring(L, string); }
	int LoadNamedString(lua_State* L) const { return luaL_loadbuffer(L, string, strlen(string), name); }
	void KeyWString(lua_State* L) const { lua_pushlstring(L, (const char*)wstring, wcslen(wstring)*sizeof(wchar_t)); }
	void KeyNil(lua_State* L) const { lua_pushnil(L); }
	int LoadWString(lua_State* L) const 
	{ 
		WideString::Push(L, wstring); 
		int res = luaL_loadstring(L, lua_tostring(L, -1)); 
		lua_remove(L, -2);
		return res;
	}
	int LoadWNamedString(lua_State* L) const 
	{ 
		WideString::Push(L, wstring); 
		WideString::Push(L, wname); 
		int res = luaL_loadbuffer(L, lua_tostring(L, -2), strlen(lua_tostring(L, -2)), lua_tostring(L, -1)); 
		lua_remove(L, -2);
		lua_remove(L, -2);
		return res;
	}
	void KeyQString(lua_State* L) const;
	int LoadQString(lua_State* L) const
	{ 
		QtString::Push(L, *qstring); 
		int res = luaL_loadstring(L, lua_tostring(L, -1)); 
		lua_remove(L, -2);
		return res;
	}
	int LoadQNamedString(lua_State* L) const
	{ 
		QtString::Push(L, *qstring); 
		QtString::Push(L, *qname); 
		int res = luaL_loadbuffer(L, lua_tostring(L, -2), strlen(lua_tostring(L, -2)), lua_tostring(L, -1)); 
		lua_remove(L, -2);
		lua_remove(L, -2);
		return res;
	}

	typedef void (Script::*pKey_t)(lua_State* L) const;
	typedef int (Script::*pLoad_t)(lua_State* L) const;
	pKey_t pKey;
	pLoad_t pLoad;
	union
	{
		const char* string;
		const wchar_t* wstring;
		const QString* qstring;
	};
	union
	{
		const char* name;
		const wchar_t* wname;
		const QString* qname;
	};
	
};

class File : public Script
{
public:
	File(const char* filename) { string=filename; pLoad=(pLoad_t)&File::LoadFile; }
	File(const wchar_t* filename) { wstring=filename; pLoad=(pLoad_t)&File::LoadWFile; }
	File(const QString& filename) { qstring=&filename; pLoad=(pLoad_t)&File::LoadQFile; }
private:
	int LoadFile(lua_State* L) const { return luaL_loadfile(L, string); }
	int LoadWFile(lua_State* L) const
	{ 
		WideString::Push(L, wstring); 
		int res = luaL_loadfile(L, lua_tostring(L, -1)); 
		lua_remove(L, -2);
		return res;
	}
	int LoadQFile(lua_State* L) const
	{ 
		QtString::Push(L, *qstring); 
		int res = luaL_loadfile(L, lua_tostring(L, -1)); 
		lua_remove(L, -2);
		return res;
	}
};

class Global : public Script
{
public:
	Global(const char* fctname) { string=fctname; pLoad=(pLoad_t)&Global::LoadGlobal; }
	Global(const wchar_t* fctname) { wstring=fctname; pLoad=(pLoad_t)&Global::LoadWGlobal; }
	Global(const QString& fctname) { qstring=&fctname; pLoad=(pLoad_t)&Global::LoadQGlobal; }
private:
	int LoadGlobal(lua_State* L) const { lua_getglobal(L, string); return 0; }
	int LoadWGlobal(lua_State* L) const
	{ 
		WideString::Push(L, wstring); 
		const char* str = lua_tostring(L, -1);
		lua_getglobal(L, str); 
		lua_remove(L, -2);
		return 0;
	}
	int LoadQGlobal(lua_State* L) const
	{ 
		QtString::Push(L, *qstring); 
		const char* str = lua_tostring(L, -1);
		lua_getglobal(L, str); 
		lua_remove(L, -2);
		return 0;
	}
};

template<class C>
class LuaT
{
public:
	LuaT(bool fOpenLibs=true)
	{ 
		L = luaL_newstate(); 
		if(fOpenLibs)
			luaL_openlibs(L); 
		FlushCache();
	}
	LuaT(lua_State* l) 
	{
		L = l; 
		Retain();
		FlushCache();
	}
	LuaT(const LuaT& src)
	{
		L = src.L; 
		Retain();
		FlushCache();
	}
	~LuaT() { Release(); }
	LuaT& operator=(const LuaT& src) 
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
	void UCall(const Script& script, const Input& input, const Output& output = nil) { UCall(script, Inputs(input), Outputs(output)); }
	void UCall(const Script& script, const Outputs& outputs) { UCall(script, Inputs(), outputs); }
	void UCall(const Script& script, const Output& output) { UCall(script, Inputs(), Outputs(output)); }
	void UCall(const Script& script, const Inputs& inputs = Inputs(), const Outputs& outputs = Outputs())
	{
		PrepareCall(script, inputs, outputs);
		DoCall();
	}
	C PCall(const Script& script, const Input& input, const Output& output = nil) {  return PCall(script, Inputs(input), Outputs(output)); }
	C PCall(const Script& script, const Outputs& outputs) { return PCall(script, Inputs(), outputs); }
	C PCall(const Script& script, const Output& output) { return PCall(script, Inputs(), Outputs(output)); }
	C PCall(const Script& script, const Inputs& inputs = Inputs(), const Outputs& outputs = Outputs())
	{
		PrepareCall(script, inputs, outputs);
#if LUA_VERSION_NUM >= 502
		lua_pushcfunction(L, DoCallS);
		lua_pushlightuserdata(L, this);
		if(lua_pcall(L, 1, 0, 0))
			return GetString(lua_gettop(L));
#else
		if(lua_cpcall(L, (lua_CFunction)DoCallS, this))
			return GetString(lua_gettop(L));
#endif
		return NullString();
	}
	void ECall(const Script& script, const Input& input, const Output& output = nil) { ECall(script, Inputs(input), Outputs(output)); }
	void ECall(const Script& script, const Outputs& outputs) { ECall(script, Inputs(), outputs); }
	void ECall(const Script& script, const Output& output) { ECall(script, Inputs(), Outputs(output)); }
	void ECall(const Script& script, const Inputs& inputs = Inputs(), const Outputs& outputs = Outputs())
#if LCBC_USE_EXCEPTIONS
	{
		C error = PCall(script, inputs, outputs);
		if(error != NullString())
			throw ErrorT<C>(error);
	}
#else
	;
#endif		
	typedef const Input& ref;
	template<class T> T TCall(const Script& script) { return DoTCall<T>(script, Inputs()); }
	template<class T> T TCall(const Script& script, ref arg1) { return DoTCall<T>(script, arg1); }
	template<class T> T TCall(const Script& script, ref arg1, ref arg2) { return DoTCall<T>(script, Inputs(arg1, arg2)); }
	template<class T> T TCall(const Script& script, ref arg1, ref arg2, ref arg3, ref arg4=nil) 
		{ return DoTCall<T>(script, Inputs(arg1, arg2, arg3, arg4)); }
	template<class T> T TCall(const Script& script, ref arg1, ref arg2, ref arg3, ref arg4, ref arg5, ref arg6=nil, ref arg7=nil, ref arg8=nil) 
		{ return DoTCall<T>(script, Inputs(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)); }
	template<class T> T TCall(const Script& script, ref arg1, ref arg2, ref arg3, ref arg4, ref arg5, ref arg6, ref arg7, ref arg8,
		ref arg9, ref arg10=nil, ref arg11=nil, ref arg12=nil, ref arg13=nil, ref arg14=nil, ref arg15=nil, ref arg16=nil)
		{ return DoTCall<T>(script, Inputs(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16)); }

	void VCall(const Script& script) { ECall(script); }
	void VCall(const Script& script, ref arg1) { ECall(script, arg1); }
	void VCall(const Script& script, ref arg1, ref arg2) { ECall(script, Inputs(arg1, arg2)); }
	void VCall(const Script& script, ref arg1, ref arg2, ref arg3, ref arg4=nil) 
		{ ECall(script, Inputs(arg1, arg2, arg3, arg4)); }
	void VCall(const Script& script, ref arg1, ref arg2, ref arg3, ref arg4, ref arg5, ref arg6=nil, ref arg7=nil, ref arg8=nil) 
		{ ECall(script, Inputs(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)); }
	void VCall(const Script& script, ref arg1, ref arg2, ref arg3, ref arg4, ref arg5, ref arg6, ref arg7, ref arg8,
		ref arg9, ref arg10=nil, ref arg11=nil, ref arg12=nil, ref arg13=nil, ref arg14=nil, ref arg15=nil, ref arg16=nil)
		{ ECall(script, Inputs(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16)); }

	LuaT& operator << (const Input& input) { shift_inputs.add(input); return *this; }
	LuaT& operator >> (const Output& output)  { shift_outputs.add(output); return *this; }
	C NullString() { return NULL; }
	C operator | (const Script& script) 
	{ 
		C error = PCall(script, shift_inputs, shift_outputs); 
		shift_inputs.empty();
		shift_outputs.empty();
		return error;
	}
#if LCBC_USE_EXCEPTIONS
	void operator & (const Script& script)
	{
		C error = operator | (script);
		if(error)
			throw ErrorT<C>(error);
	}
#endif		
private:
	C GetString(int idx);
	void PrepareCall(const Script& script_, const Inputs& inputs_, const Outputs& outputs_)
	{
		lua_settop(L, 0);
		script = &script_;
		inputs = &inputs_;
		outputs = &outputs_;
	}
	void DoCall()
	{
		lua_pushcfunction(L, (lua_CFunction)traceback);
		int idxtrace = lua_gettop(L);
		lua_getfield(L, LUA_REGISTRYINDEX, "LuaClassBasedCaller");
		script->pushkey(L);
		if(lua_toboolean(L, -1))
		{
			lua_rawget(L, -2);
			if(!lua_isfunction(L, -1))
			{
				if(script->load(L))
					lua_error(L);
				script->pushkey(L);
				lua_pushvalue(L, -2);
				lua_rawset(L, -5);
			}
		}
		else if(script->load(L))
			lua_error(L);
		lua_replace(L, 1);
		lua_settop(L, 1);
		lua_checkstack(L, (int)inputs->size());
		for(size_t i=0;i<inputs->size(); i++)
			inputs->get(i).Push(L);
		if(lua_pcall(L, (int)inputs->size(), (int)outputs->size(), idxtrace))
			lua_error(L);
		for(size_t i=0;i<outputs->size(); i++)
			outputs->get(i).Get(L, (int)i+1);
	}
	static int DoCallS(lua_State* L)
	{
		LuaT* This = (LuaT*)lua_topointer(L, 1);
		This->DoCall();
		return 0;
	}
	template<class T> T DoTCall(const Script& script, const Inputs& inputs)
	{
		T value;
		ECall(script, inputs, Outputs(value));
		return value;
	}
	/* Function copied from lua.c */
	static int traceback (lua_State *L) 
	{
		lua_getglobal(L, "debug");
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
	const Script* script;
	const Inputs* inputs; 
	const Outputs* outputs;
	Inputs shift_inputs;
	Outputs shift_outputs;
};

typedef LuaT<const char*> LuaA;
typedef LuaT<const wchar_t*> LuaW;
#if defined(_UNICODE) || defined(UNICODE)
typedef LuaW Lua;
#else
typedef LuaA Lua;
#endif

template<> inline const char* LuaT<const char*>::GetString(int idx) { return lua_tostring(L, idx); }
template<> inline const wchar_t* LuaT<const wchar_t*>::GetString(int idx) { return WideString::Get(L, idx); }
template<> inline QString LuaT<QString>::GetString(int idx) { return QtString::Get(L, idx); }

#if LCBC_USE_WIDESTRING
template<> inline int WideString::Push<RawMode>(lua_State* /*L*/) { return 1; }

template<> inline int WideString::Get<RawMode>(lua_State* L)
{
	lua_pushlstring(L, "\0\0\0\0", sizeof(wchar_t));
	lua_concat(L, 2);
	return 1;
}

template<> inline int WideString::Push<LocaleMode>(lua_State* L)
{
	size_t i, len;
	luaL_Buffer b;
	char buffer[10];
	const wchar_t* wstr = (const wchar_t*)lua_tolstring(L, 1, &len);
	len /= sizeof(wchar_t);
	luaL_buffinit(L, &b);
	for(i=0;i<len;i++)
	{
		int res = wctomb(buffer, wstr[i]);
		if(res == -1)
			luaL_error(L, "Error converting wide string to characters");
		luaL_addlstring(&b, buffer, res);
	}
	luaL_pushresult(&b);
	return 1;
}

template<> inline int WideString::Get<LocaleMode>(lua_State* L)
{
	size_t i;
	luaL_Buffer b;
	wchar_t wchar;
	size_t len, pos = 0;
	const char* psrc = luaL_checklstring(L, 1, &len);
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
	return 1;
}

template<> inline int WideString::Push<Utf8Mode>(lua_State* L)
{
	luaL_Buffer b;
	size_t i, len;
	wchar_t thres;
	char str[8], car;
	const wchar_t* wstr = (const wchar_t*)lua_tolstring(L, 1, &len);
	len /= sizeof(wchar_t);
	luaL_buffinit(L, &b);
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
	return 1;
}

template<> inline int WideString::Get<Utf8Mode>(lua_State* L)
{
	static const unsigned int min_value[] = {0xFFFFFFFF, 0x80, 0x800, 0x10000, 0x200000, 0xFFFFFFFF, 0xFFFFFFFF};
	luaL_Buffer b;
	int i,mask;
	unsigned int value;
	char car;
	size_t len;
	wchar_t wc;
	const char* str = (const char*)luaL_checklstring(L, 1, &len);
	const char* strend = str + len;
	luaL_buffinit(L, &b);
	while(str < strend)
	{
		car = *str++;
		if((car & 0x80) == 0)
			value = car;
		else
		{
			for(i=1,mask=0x40;car & mask;i++,mask>>=1) ;
			int len = i;
			value = car & (mask - 1);
			for(;i>1;i--)
			{
				car = *str++;
				if((car & 0xC0) != 0x80)
					luaL_error(L, "invalid UTF-8 string");
				value = (value << 6) | (car & 0x3F);
			}
			if(value < min_value[len-1])
				luaL_error(L, "overlong character in UTF-8");
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
	return 1;
}
#endif

#if LCBC_USE_QT
inline QString QtString::Get(lua_State* L, int idx, size_t& size)
{
	const char* str = luaL_checklstring(L, idx, &size);
	return QString::fromUtf8(str, (int)size);
}

inline void QtString::Push(lua_State* L, const QString& str) 
{
	QByteArray utf8 = str.toUtf8();
	lua_pushlstring(L, utf8, utf8.size());
}

inline void Script::KeyQString(lua_State* L) const 
{ 
	lua_pushlstring(L, (const char*)qstring->unicode(), qstring->size()*sizeof(QChar)); 
}

#else
#undef QString
#endif

#if !LCBC_USE_WIDESTRING
#undef wchar_t
#undef wstring
#endif

}
#endif

