//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Source-level type handling.
//
//-----------------------------------------------------------------------------

#include "VariableType.hpp"

#include "SourceException.hpp"

#include <algorithm>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string basic_names[] =
{
   "BT_BOOLHARD",
   "BT_BOOLSOFT",
   "BT_CHAR",
   "BT_INT",
   "BT_LABEL",
   "BT_REAL",
   "BT_STRING",
   "BT_UINT",

   "BT_VOID",

   "BT_ARRAY",
   "BT_POINTER",

   "BT_ENUM",
   "BT_STRUCT",
   "BT_UNION",

   "BT_ASMFUNC",
   "BT_BLOCK",
   "BT_FUNCTION",
   "BT_LINESPEC",
   "BT_NATIVE",
   "BT_SCRIPT"
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

template<typename T>
static bool operator != (std::vector<T> const &l, std::vector<T> const &r)
{
   if (l.size() != r.size()) return true;

   return std::mismatch(l.begin(), l.end(), r.begin()).first != l.end();
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//===================================================================
// Construction/Destruction.
//

//
// VariableType::VariableType
//
// Only used to create the basic BT_VOID type.
//
VariableType::VariableType()
 : next(this), prev(this), specnext(this), specprev(this),
   typeArr(NULL), typePtr(NULL), typeRet(this),
   basic(BT_VOID), quals(0), store(ST_ADDR), width(0),
   complete(true)
{
   --refCount;
}

//
// VariableType::VariableType
//
VariableType::VariableType(VariableType &type)
 : names(type.names), types(type.types),
   next(type.next), prev(&type), specnext(this), specprev(this),
   typeArr(NULL), typePtr(NULL), typeRet(type.typeRet),
   typeUnq(type.getUnqualified()),
   storeArea(type.storeArea),
   basic(type.basic), quals(type.quals), store(type.store), width(type.width),
   complete(type.complete)
{
   type.next->prev = this;
   type.next = this;
}

//
// VariableType::VariableType
//
VariableType::VariableType(BasicType _basic)
 : next(this), prev(this), specnext(this), specprev(this),
   typeArr(NULL), typePtr(NULL), typeRet(get_bt_void()),
   basic(_basic), quals(0), store(ST_ADDR), width(0),
   complete(true)
{
   switch (basic)
   {
   // Primitive types.
   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_INT:
   case BT_LABEL:
   case BT_REAL:
   case BT_UINT:
      break;

   case BT_STRING:
      typeRet = get_bt_char()->addQualifier(QUAL_CONST);
      break;

   case BT_VOID:
      break;

   // Reference types.
   case BT_ARRAY:
   case BT_POINTER:
      break;

   // Named types.
   case BT_ENUM:
   case BT_STRUCT:
   case BT_UNION:
      complete = false;
      break;

   // Anonymous types.
   case BT_ASMFUNC:
   case BT_BLOCK:
   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SCRIPT:
      break;
   }
}

//
// VariableType::~VariableType
//
VariableType::~VariableType()
{
   next->prev = prev;
   prev->next = next;

   specnext->specprev = specprev;
   specprev->specnext = specnext;

   switch (basic)
   {
   // Primitive types.
   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_INT:
   case BT_LABEL:
   case BT_REAL:
   case BT_STRING:
   case BT_UINT:
      break;

   case BT_VOID:
      break;

   // Reference types.
   case BT_ARRAY:
      if (typeRet->typeArr == this)
         typeRet->typeArr = specnext == this ? NULL : specnext;
      break;

   case BT_POINTER:
      typeRet->typePtr = NULL;
      break;

   // Named types.
   case BT_ENUM:
   case BT_STRUCT:
   case BT_UNION:
      break;

   // Anonymous types.
   case BT_ASMFUNC:
   case BT_BLOCK:
   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SCRIPT:
      break;
   }
}

//===================================================================
// Type creation.
//

//
// VariableType::getArray
//
VariableType::Reference VariableType::getArray(bigsint _width)
{
   if (!typeArr)
   {
      typeArr = new VariableType(BT_ARRAY);

      typeArr->typeRet = static_cast<Reference>(this);
      typeArr->width = _width;

      return static_cast<Reference>(typeArr);
   }

   for (VariableType *iter = typeArr->specnext; ; iter = iter->specnext)
   {
      if (iter->width == _width)
         return static_cast<Reference>(iter);

      if (iter == typeArr) break;
   }

   Reference type(new VariableType(BT_ARRAY));

   type->typeRet = Reference(this);
   type->width = _width;

   // Link into speclist.
   type->specprev = typeArr;
   type->specnext = typeArr->specnext;
   typeArr->specnext->specprev = type;
   typeArr->specnext = type;

   return type;
}

//
// VariableType::getPointer
//
VariableType::Reference VariableType::getPointer()
{
   if (typePtr)
      return static_cast<Reference>(typePtr);

   typePtr = new VariableType(BT_POINTER);

   typePtr->typeRet = Reference(this);

   return static_cast<Reference>(typePtr);
}

//
// VariableType::setQualifier
//
VariableType::Reference VariableType::setQualifier(unsigned _quals)
{
   if (quals == _quals)
      return static_cast<Reference>(this);

   for (VariableType *iter = next; iter != this; iter = iter->next)
   {
      if (iter->quals == _quals &&
          iter->store == store && iter->storeArea == iter->storeArea)
         return Reference(iter);
   }

   VariableType::Reference type(new VariableType(*this));

   type->quals = _quals;
   if (basic == BT_ARRAY)
      type->setReturn(type->typeRet->setQualifier(_quals));
   if (basic == BT_STRUCT || basic == BT_UNION || basic == BT_BLOCK)
   {
      for (Vector::iterator iter = type->types.begin();
           iter != type->types.end(); ++iter)
         *iter = (*iter)->setQualifier(_quals);
   }

   return type;
}

//
// VariableType::setReturn
//
void VariableType::setReturn(VariableType *type)
{
   if (basic == BT_ARRAY && typeRet->typeArr == this)
      typeRet->typeArr = specnext == this ? NULL : specnext;

   typeRet = static_cast<Reference>(type);
}

//
// VariableType::setStorage
//
VariableType::Reference VariableType::setStorage(StoreType _store)
{
   static std::string _storeArea;
   return setStorage(_store, _storeArea);
}

//
// VariableType::setStorage
//
VariableType::Reference VariableType::setStorage
(StoreType _store, std::string const &_storeArea)
{
   if (store == _store && storeArea == _storeArea)
      return static_cast<Reference>(this);

   for (VariableType *iter = next; iter != this; iter = iter->next)
   {
      if (iter->quals == quals &&
          iter->store == _store && iter->storeArea == _storeArea)
         return Reference(iter);
   }

   VariableType::Reference type(new VariableType(*this));

   type->store     = _store;
   type->storeArea = _storeArea;
   if (basic == BT_ARRAY)
      type->setReturn(type->typeRet->setStorage(_store, _storeArea));
   if (basic == BT_STRUCT || basic == BT_UNION || basic == BT_BLOCK)
   {
      for (Vector::iterator iter = type->types.begin();
           iter != type->types.end(); ++iter)
         *iter = (*iter)->setStorage(_store, _storeArea);
   }

   return type;
}

//===================================================================
// Type information.
//

//
// VariableType::getSize
//
bigsint VariableType::getSize(SourcePosition const &position) const
{
   if (!complete)
      throw SourceException("incomplete type", position, __func__);

   switch (basic)
   {
   case BT_ARRAY:
      return typeRet->getSize(position) * width;

   case BT_ASMFUNC:
   case BT_VOID:
      return 0;

   case BT_BLOCK:
   case BT_STRUCT:
   {
      bigsint size = 0;
      for (Vector::const_iterator iter = types.begin(); iter != types.end();
           ++iter)
         size += (*iter)->getSize(position);
      return size;
   }

   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_ENUM:
   case BT_FUNCTION:
   case BT_INT:
   case BT_LABEL:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_POINTER:
   case BT_REAL:
   case BT_SCRIPT:
   case BT_STRING:
   case BT_UINT:
      return 1;

   case BT_UNION:
   {
      bigsint size = 0;
      for (Vector::const_iterator iter = types.begin(); iter != types.end();
           ++iter)
	 {
         bigsint sizeNext = (*iter)->getSize(position);
         if (sizeNext > size) size = sizeNext;
	 }
      return size;
   }
   }

   throw SourceException("invalid type", position, __func__);
}

//===================================================================
// Type alteration.
//

//
// VariableType::makeComplete
//
void VariableType::makeComplete()
{
   if (complete) return;
   complete = true;
   next->makeComplete();
}

//
// VariableType::makeComplete
//
void VariableType::makeComplete(VecStr const &_names, Vector const &_types)
{
   if (complete) return;
   names = _names;
   types = _types;
   complete = true;
   next->makeComplete(_names, _types);
}

//===================================================================
// Member information.
//

//
// VariableType::getOffset
//
bigsint VariableType::getOffset
(std::string const &name, SourcePosition const &position)
{
   if (!complete)
      throw SourceException("incomplete type", position, __func__);

   bigsint offset = 0;
   for (size_t i = 0; i < names.size(); ++i)
   {
      if (names[i] == name) return offset;
      offset += types[i]->getSize(position);
   }

   throw SourceException("no such member '" + name + "'", position, __func__);
}

//
// VariableType::getType
//
VariableType::Reference VariableType::getType
(std::string const &name, SourcePosition const &position)
{
   if (!complete)
      throw SourceException("incomplete type", position, __func__);

   for (size_t i = 0; i < names.size(); ++i)
      if (names[i] == name) return Reference(types[i]);

   throw SourceException("no such member '" + name + "'", position, __func__);
}

//===================================================================
// Basic types
//

//
// VariableType::get_bt_boolhard
//
VariableType::Reference VariableType::get_bt_boolhard()
{
   static Reference type(new VariableType(BT_BOOLHARD)); return type;
}

//
// VariableType::get_bt_boolsoft
//
VariableType::Reference VariableType::get_bt_boolsoft()
{
   static Reference type(new VariableType(BT_BOOLSOFT)); return type;
}

//
// VariableType::get_bt_char
//
VariableType::Reference VariableType::get_bt_char()
{
   static Reference type(new VariableType(BT_CHAR)); return type;
}

//
// VariableType::get_bt_int
//
VariableType::Reference VariableType::get_bt_int()
{
   static Reference type(new VariableType(BT_INT)); return type;
}

//
// VariableType::get_bt_label
//
VariableType::Reference VariableType::get_bt_label()
{
   static Reference type(new VariableType(BT_LABEL)); return type;
}

//
// VariableType::get_bt_real
//
VariableType::Reference VariableType::get_bt_real()
{
   static Reference type(new VariableType(BT_REAL)); return type;
}

//
// VariableType::get_bt_string
//
VariableType::Reference VariableType::get_bt_string()
{
   static Reference type(new VariableType(BT_STRING)); return type;
}

//
// VariableType::get_bt_uint
//
VariableType::Reference VariableType::get_bt_uint()
{
   static Reference type(new VariableType(BT_UINT)); return type;
}

//
// VariableType::get_bt_void
//
VariableType::Reference VariableType::get_bt_void()
{
   static Reference type(new VariableType); return type;
}

//===================================================================
// Named types
//

//
// VariableType::get_bt_enum
//
VariableType::Reference VariableType::get_bt_enum()
{
   return Reference(new VariableType(BT_ENUM));
}

//
// VariableType::get_bt_struct
//
VariableType::Reference VariableType::get_bt_struct()
{
   return Reference(new VariableType(BT_STRUCT));
}

//
// VariableType::get_bt_union
//
VariableType::Reference VariableType::get_bt_union()
{
   return Reference(new VariableType(BT_UNION));
}

//===================================================================
// Anonymous types
//

//
// VariableType::get_bt_anonymous
//
VariableType::Reference VariableType::get_bt_anonymous
(Vector types, VariableType *typeRet, VariableType *head, BasicType basic)
{
   // Parameters and return are unqualified.
   if (basic != BT_BLOCK)
      for (Vector::iterator iter = types.begin(); iter != types.end(); ++iter)
         *iter = (*iter)->getUnqualified();

   typeRet = typeRet->getUnqualified();

   // Look for a matching type. (Equality to head is handled by caller.)
   for (VariableType *iter = head->specnext; iter != head; iter = iter->specnext)
   {
      if (iter->typeRet == typeRet && iter->types == types)
         return static_cast<Reference>(iter);
   }

   // No matching type exists, create a new one.
   Reference type(new VariableType(basic));

   type->types = types;
   type->typeRet = static_cast<Reference>(typeRet);
   type->specprev = head;
   type->specnext = head->specnext;
   head->specnext->specprev = type;
   head->specnext = type;

   return type;
}

//
// VariableType::get_bt_asmfunc
//
VariableType::Reference VariableType::get_bt_asmfunc
(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_ASMFUNC));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_ASMFUNC);
}

//
// VariableType::get_bt_block
//
VariableType::Reference VariableType::get_bt_block(Vector const &types)
{
   static Reference type(new VariableType(BT_BLOCK));

   if (types.empty()) return type;

   return get_bt_anonymous(types, get_bt_void(), type, BT_BLOCK);
}

//
// VariableType::get_bt_function
//
VariableType::Reference VariableType::get_bt_function
(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUNCTION));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUNCTION);
}

//
// VariableType::get_bt_linespec
//
VariableType::Reference VariableType::get_bt_linespec
(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_LINESPEC));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_LINESPEC);
}

//
// VariableType::get_bt_native
//
VariableType::Reference VariableType::get_bt_native
(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_NATIVE));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_NATIVE);
}

//
// VariableType::get_bt_script
//
VariableType::Reference VariableType::get_bt_script
(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_SCRIPT));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_SCRIPT);
}

//===================================================================
// Miscellaneous.
//

//
// VariableType::is_bt_arithmetic
//
bool VariableType::is_bt_arithmetic(BasicType type)
{
   switch (type)
   {
   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_INT:
   case BT_REAL:
   case BT_UINT:
      return true;

   default:
      return false;
   }
}

//
// VariableType::is_bt_integer
//
bool VariableType::is_bt_integer(BasicType type)
{
   switch (type)
   {
   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_INT:
   case BT_UINT:
      return true;

   default:
      return false;
   }
}

//
// make_string<VariableType::BasicType>
//
std::string const &make_string(VariableType::BasicType basic)
{
   return basic_names[basic];
}

// EOF

