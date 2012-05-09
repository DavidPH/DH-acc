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

#include "option.hpp"
#include "SourceException.hpp"

#include <algorithm>
#include <sstream>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static std::ostream &operator <<
(std::ostream &out, VariableType const *type);

static std::ostream &operator <<
(std::ostream &out, VariableType::BasicType basic);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string const basic_names[] =
{
   "BT_BOOLHARD",
   "BT_BOOLSOFT",
   "BT_CHAR",
   "BT_FIXED",
   "BT_FLOAT",
   "BT_INT",
   "BT_LABEL",
   "BT_LFLOAT",
   "BT_LLFLOAT",
   "BT_LLONG",
   "BT_LONG",
   "BT_REAL",
   "BT_SCHAR",
   "BT_SHORT",
   "BT_STRING",
   "BT_UCHAR",
   "BT_UINT",
   "BT_ULLONG",
   "BT_ULONG",
   "BT_USHORT",

   "BT_VOID",

   "BT_ARRAY",
   "BT_NULLPTR",
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

//
// operator << <VariableType>
//
static std::ostream &operator <<
(std::ostream &out, VariableType const *type)
{
   switch (type->getBasicType())
   {
   case VariableType::BT_ARRAY:
      out << type->getReturn();
      out << '[' << type->getWidth() << ']';
      return out; // Don't show qualifiers on array.

   case VariableType::BT_BLOCK:
      out << make_string(type->getBasicType());
      out << '{';
   {
      VariableType::Vector const &types = type->getTypes();

      for (size_t i = 0; i < types.size(); ++i)
         out << types[i] << ';';
   }
      out << '}';
      break;

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_BOOLSOFT:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FIXED:
   case VariableType::BT_FLOAT:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_LLONG:
   case VariableType::BT_LONG:
   case VariableType::BT_NULLPTR:
   case VariableType::BT_REAL:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SHORT:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULLONG:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
   case VariableType::BT_STRING:
   case VariableType::BT_VOID:
      out << type->getBasicType();
      break;

   case VariableType::BT_POINTER:
      out << type->getReturn() << " *";
      break;

   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
      out << type->getBasicType() << '{' << type->getName() << '}';
      break;

   case VariableType::BT_ASMFUNC:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_SCRIPT:
      out << type->getBasicType();
      out << '(';
   {
      VariableType::Vector const &types = type->getTypes();

      for (size_t i = 0; i < types.size(); ++i)
      {
         out << types[i];
         if (i+1 < types.size()) out << ',';
      }
   }
      out << ')';
      out << "->(" << type->getReturn() << ')';
      break;
   }

   if (type->getQualifier(VariableType::QUAL_CONST))
      out << " const";

   if (type->getQualifier(VariableType::QUAL_VOLATILE))
      out << " volatile";

   if (type->getQualifier(VariableType::QUAL_RESTRICT))
      out << " restrict";

   if (type->getStoreType() != STORE_STATIC)
      out << ' ' << make_string(type->getStoreType());

   return out;
}

//
// operator << <VariableType::BasicType>
//
static std::ostream &operator <<
(std::ostream &out, VariableType::BasicType basic)
{
   return out << basic_names[basic];
}

//
// operator != <std::vector>
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
   basic(BT_VOID), quals(0), store(STORE_STATIC), width(0),
   complete(true)
{
   --refCount;
}

//
// VariableType::VariableType
//
VariableType::VariableType(VariableType &type)
 : Super(type),
   names(type.names), types(type.types),
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
   basic(_basic), quals(0), store(STORE_STATIC), width(0),
   complete(true)
{
   switch (basic)
   {
   // Primitive types.
   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_FIXED:
   case BT_FLOAT:
   case BT_INT:
   case BT_LABEL:
   case BT_LFLOAT:
   case BT_LLFLOAT:
   case BT_LLONG:
   case BT_LONG:
   case BT_REAL:
   case BT_SCHAR:
   case BT_SHORT:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULLONG:
   case BT_ULONG:
   case BT_USHORT:
      break;

   case BT_STRING:
      typeRet = get_bt_char()->addQualifier(QUAL_CONST);
      break;

   case BT_VOID:
      break;

   // Reference types.
   case BT_ARRAY:
   case BT_NULLPTR:
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
   case BT_FIXED:
   case BT_FLOAT:
   case BT_INT:
   case BT_LABEL:
   case BT_LFLOAT:
   case BT_LLFLOAT:
   case BT_LLONG:
   case BT_LONG:
   case BT_REAL:
   case BT_SCHAR:
   case BT_SHORT:
   case BT_STRING:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULLONG:
   case BT_ULONG:
   case BT_USHORT:
      break;

   case BT_VOID:
      break;

   // Reference types.
   case BT_ARRAY:
      if (typeRet->typeArr == this)
         typeRet->typeArr = specnext == this ? NULL : specnext;
      break;

   case BT_NULLPTR:
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
      typeArr->storeArea = storeArea;
      typeArr->quals = quals;
      typeArr->store = store;
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
   type->storeArea = storeArea;
   type->quals = quals;
   type->store = store;
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

   if (basic == BT_ARRAY)
      return typeRet->setQualifier(_quals)->getArray(width);

   for (VariableType *iter = next; iter != this; iter = iter->next)
   {
      if (iter->quals == _quals &&
          iter->store == store && iter->storeArea == storeArea)
         return Reference(iter);
   }

   VariableType::Reference type(new VariableType(*this));

   type->quals = _quals;
   if (basic == BT_STRUCT || basic == BT_UNION || basic == BT_BLOCK)
   {
      for (Vector::iterator iter = type->types.begin();
           iter != type->types.end(); ++iter)
         *iter = (*iter)->setQualifier(_quals);
   }

   return type;
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

   if (basic == BT_ARRAY)
      return typeRet->setStorage(_store, _storeArea)->getArray(width);

   for (VariableType *iter = next; iter != this; iter = iter->next)
   {
      if (iter->quals == quals &&
          iter->store == _store && iter->storeArea == _storeArea)
         return Reference(iter);
   }

   VariableType::Reference type(new VariableType(*this));

   type->store     = _store;
   type->storeArea = _storeArea;
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
// VariableType::getNameMangled
//
void VariableType::getNameMangled(std::string &out) const
{
   #define RETRN typeRet->getNameMangled(out)

   #define TYPES                             \
   for (size_t i = 0; i < types.size(); ++i) \
   {                                         \
      types[i]->getNameMangled(out);         \
      if (i+1 < types.size()) out += ':';    \
   }

   #define WIDTH {std::ostringstream oss; oss << width; out += oss.str();}

   switch (basic)
   {
   case BT_ARRAY:    out += "A{"; RETRN; out += "}:["; WIDTH; out += ']'; break;
   case BT_BLOCK:    out += "B{"; TYPES; out += '}'; break;
   case BT_BOOLHARD: out += 'B'; break;
   case BT_BOOLSOFT: out += "BS"; break;
   case BT_CHAR:     out += 'C'; break;
   case BT_ENUM:     out += "E("; out += name; out += ')'; break;
   case BT_FLOAT:    out += 'F'; break;
   case BT_LFLOAT:   out += "FL"; break;
   case BT_LLFLOAT:  out += "FLL"; break;
   case BT_FUNCTION: out += "F{"; TYPES; out += '}'; break;
   case BT_ASMFUNC:  out += "FA{"; TYPES; out += '}'; break;
   case BT_LINESPEC: out += "FL{"; TYPES; out += '}'; break;
   case BT_NATIVE:   out += "FN{"; TYPES; out += '}'; break;
   case BT_SCRIPT:   out += "FS{"; TYPES; out += '}'; break;
   case BT_SCHAR:    out += "IHH"; break;
   case BT_SHORT:    out += "IH"; break;
   case BT_INT:      out += 'I'; break;
   case BT_LONG:     out += "IL"; break;
   case BT_LLONG:    out += "ILL"; break;
   case BT_LABEL:    out += "LABEL"; break;
   case BT_POINTER:  out += "P{"; RETRN; out += '}'; break;
   case BT_NULLPTR:  out += "PN"; break;
   case BT_REAL:     out += 'R'; break;
   case BT_STRING:   out += 'S'; break;
   case BT_STRUCT:   out += "S("; out += name; out += ')'; break;
   case BT_UCHAR:    out += "UHH"; break;
   case BT_USHORT:   out += "UH"; break;
   case BT_UINT:     out += 'U'; break;
   case BT_ULONG:    out += "UL"; break;
   case BT_ULLONG:   out += "ULL"; break;
   case BT_UNION:    out += "U("; out += name; out += ')'; break;
   case BT_VOID:     out += 'V'; break;
   case BT_FIXED:    out += 'X'; break;
   }

   if (getQualifier(QUAL_CONST))    out += 'c';
   if (getQualifier(QUAL_VOLATILE)) out += 'v';
   if (getQualifier(QUAL_RESTRICT)) out += 'r';

   switch (store)
   {
   case STORE_STATIC: break;
   case STORE_AUTO:           out += "sA"; break;
   case STORE_CONST:          out += "sC"; break;
   case STORE_REGISTER:       out += "sR"; break;
   case STORE_MAPREGISTER:    out += "sMR"; break;
   case STORE_WORLDREGISTER:  out += "sWR"; break;
   case STORE_GLOBALREGISTER: out += "sGR"; break;
   case STORE_MAPARRAY:       out += "sMA("; out += storeArea; out += ')'; break;
   case STORE_WORLDARRAY:     out += "sWA("; out += storeArea; out += ')'; break;
   case STORE_GLOBALARRAY:    out += "sGA("; out += storeArea; out += ')'; break;
   }

   #undef WIDTH
   #undef TYPES
   #undef RETRN
}

//
// VariableType::getSize
//
bigsint VariableType::getSize(SourcePosition const &pos) const
{
   if (!complete)
      ERROR_NP("incomplete type");

   switch (basic)
   {
   case BT_ARRAY:
      return width ? typeRet->getSize(pos) * width : 0;

   case BT_ASMFUNC:
   case BT_VOID:
      return 0;

   case BT_BLOCK:
   case BT_STRUCT:
   {
      bigsint size = 0;
      for (Vector::const_iterator iter = types.begin(); iter != types.end();
           ++iter)
         size += (*iter)->getSize(pos);
      return size;
   }

   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_ENUM:
   case BT_FIXED:
   case BT_FLOAT:
   case BT_FUNCTION:
   case BT_INT:
   case BT_LABEL:
   case BT_LINESPEC:
   case BT_LONG:
   case BT_NATIVE:
   case BT_NULLPTR:
   case BT_POINTER:
   case BT_REAL:
   case BT_SCHAR:
   case BT_SCRIPT:
   case BT_SHORT:
   case BT_STRING:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULONG:
   case BT_USHORT:
      return 1;

   case BT_LFLOAT:
   case BT_LLFLOAT:
   case BT_LLONG:
   case BT_ULLONG:
      return 2;

   case BT_UNION:
   {
      bigsint size = 0;
      for (Vector::const_iterator iter = types.begin(); iter != types.end();
           ++iter)
	 {
         bigsint sizeNext = (*iter)->getSize(pos);
         if (sizeNext > size) size = sizeNext;
	 }
      return size;
   }
   }

   ERROR_NP("invalid type");
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
(std::string const &memName, SourcePosition const &pos)
{
   if (!complete)
      ERROR_NP("incomplete type");

   if (basic == BT_UNION) return 0;

   bigsint offset = 0;
   for (size_t i = 0; i < names.size(); ++i)
   {
      if (names[i] == memName) return offset;
      offset += types[i]->getSize(pos);
   }

   ERROR_NP("no such member: %s", memName.c_str());
}

//
// VariableType::getType
//
VariableType::Reference VariableType::getType
(std::string const &memName, SourcePosition const &pos)
{
   if (!complete)
      ERROR_NP("incomplete type");

   for (size_t i = 0; i < names.size(); ++i)
      if (names[i] == memName) return Reference(types[i]);

   ERROR_NP("no such member: %s", memName.c_str());
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
// VariableType::get_bt_fixed
//
VariableType::Reference VariableType::get_bt_fixed()
{
   static Reference type(new VariableType(BT_FIXED)); return type;
}

//
// VariableType::get_bt_float
//
VariableType::Reference VariableType::get_bt_float()
{
   static Reference type(new VariableType(BT_FLOAT)); return type;
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
// VariableType::get_bt_lfloat
//
VariableType::Reference VariableType::get_bt_lfloat()
{
   static Reference type(new VariableType(BT_LFLOAT)); return type;
}

//
// VariableType::get_bt_llfloat
//
VariableType::Reference VariableType::get_bt_llfloat()
{
   static Reference type(new VariableType(BT_LLFLOAT)); return type;
}

//
// VariableType::get_bt_llong
//
VariableType::Reference VariableType::get_bt_llong()
{
   static Reference type(new VariableType(BT_LLONG)); return type;
}

//
// VariableType::get_bt_long
//
VariableType::Reference VariableType::get_bt_long()
{
   static Reference type(new VariableType(BT_LONG)); return type;
}

//
// VariableType::get_bt_real
//
VariableType::Reference VariableType::get_bt_real()
{
   static Reference type(new VariableType(BT_REAL)); return type;
}

//
// VariableType::get_bt_schar
//
VariableType::Reference VariableType::get_bt_schar()
{
   static Reference type(new VariableType(BT_SCHAR)); return type;
}

//
// VariableType::get_bt_short
//
VariableType::Reference VariableType::get_bt_short()
{
   static Reference type(new VariableType(BT_SHORT)); return type;
}

//
// VariableType::get_bt_string
//
VariableType::Reference VariableType::get_bt_string()
{
   static Reference type(new VariableType(BT_STRING)); return type;
}

//
// VariableType::get_bt_uchar
//
VariableType::Reference VariableType::get_bt_uchar()
{
   static Reference type(new VariableType(BT_UCHAR)); return type;
}

//
// VariableType::get_bt_uint
//
VariableType::Reference VariableType::get_bt_uint()
{
   static Reference type(new VariableType(BT_UINT)); return type;
}

//
// VariableType::get_bt_ullong
//
VariableType::Reference VariableType::get_bt_ullong()
{
   static Reference type(new VariableType(BT_ULLONG)); return type;
}

//
// VariableType::get_bt_ulong
//
VariableType::Reference VariableType::get_bt_ulong()
{
   static Reference type(new VariableType(BT_ULONG)); return type;
}

//
// VariableType::get_bt_ushort
//
VariableType::Reference VariableType::get_bt_ushort()
{
   static Reference type(new VariableType(BT_USHORT)); return type;
}

//
// VariableType::get_bt_void
//
VariableType::Reference VariableType::get_bt_void()
{
   static Reference type(new VariableType); return type;
}

//===================================================================
// Reference types
//

//
// VariableType::get_bt_nullptr
//
VariableType::Reference VariableType::get_bt_nullptr()
{
   static Reference type(new VariableType(BT_NULLPTR)); return type;
}

//===================================================================
// Named types
//

//
// VariableType::get_bt_enum
//
VariableType::Reference VariableType::get_bt_enum(std::string const &name)
{
   VariableType *type = new VariableType(BT_ENUM);
   type->name = name;
   return static_cast<Reference>(type);
}

//
// VariableType::get_bt_struct
//
VariableType::Reference VariableType::get_bt_struct(std::string const &name)
{
   VariableType *type = new VariableType(BT_STRUCT);
   type->name = name;
   return static_cast<Reference>(type);
}

//
// VariableType::get_bt_union
//
VariableType::Reference VariableType::get_bt_union(std::string const &name)
{
   VariableType *type = new VariableType(BT_UNION);
   type->name = name;
   return static_cast<Reference>(type);
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
// VariableType::get_cast
//
unsigned VariableType::get_cast(VariableType *dst, VariableType *src)
{
   if (dst == src || dst->getUnqualified() == src->getUnqualified())
      return CAST_ANY;

   BasicType dstBT = dst->getBasicType();
   BasicType srcBT = src->getBasicType();

   // Just pretend it's a pointer.
   if (dstBT == BT_POINTER && srcBT == BT_ARRAY)
      srcBT = BT_POINTER;

   if (dstBT != srcBT)
   {
      // nullptr->pointer
      if (srcBT == BT_NULLPTR && dstBT == BT_POINTER)
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // nullptr->function
      if (srcBT == BT_NULLPTR && is_bt_function(dstBT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // pointer->boolean
      if (srcBT == BT_POINTER && (dstBT == BT_BOOLHARD || dstBT == BT_BOOLSOFT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // function->boolean
      if (is_bt_function(srcBT) && (dstBT == BT_BOOLHARD || dstBT == BT_BOOLSOFT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // arithmetic<->arithmetic
      if (is_bt_arithmetic(srcBT) && is_bt_arithmetic(dstBT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // arithmetic->enum
      if (is_bt_arithmetic(srcBT) && dstBT == BT_ENUM)
         return CAST_EXPLICIT|CAST_REINTERPRET;

      // block->array,struct
      if (srcBT == BT_BLOCK && (dstBT == BT_ARRAY || dstBT == BT_STRUCT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // enum->int
      if (srcBT == BT_ENUM && (is_bt_integer(dstBT) && dstBT != BT_ENUM))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // enum->arithmetic
      if (srcBT == BT_ENUM && is_bt_arithmetic(dstBT))
         return CAST_EXPLICIT|CAST_REINTERPRET;

      // function<->function
      if (is_bt_function(srcBT) && is_bt_function(dstBT))
         return CAST_EXPLICIT;

      // int->pointer
      if (is_bt_integer(srcBT) && dstBT == BT_POINTER)
         return CAST_EXPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // pointer->int
      if (srcBT == BT_POINTER && (is_bt_integer(dstBT) && dstBT != BT_ENUM))
         return CAST_EXPLICIT|CAST_STATIC|CAST_REINTERPRET;


      // *->void
      if (dstBT == BT_VOID)
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;


      // *->*
      return CAST_EXPLICIT;
   }
   else switch (dstBT)
   {
   case BT_ARRAY:
   case BT_BLOCK:
   case BT_STRUCT:
   case BT_UNION:
      return 0;

   case BT_ASMFUNC:
   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SCRIPT:
      return CAST_EXPLICIT|CAST_REINTERPRET;

   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_CHAR:
   case BT_FIXED:
   case BT_FLOAT:
   case BT_INT:
   case BT_LABEL:
   case BT_LFLOAT:
   case BT_LLFLOAT:
   case BT_LLONG:
   case BT_LONG:
   case BT_NULLPTR:
   case BT_REAL:
   case BT_SCHAR:
   case BT_STRING:
   case BT_SHORT:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULLONG:
   case BT_ULONG:
   case BT_USHORT:
   case BT_VOID:
      // Should have been handled by the previous equality test...
      return CAST_ANY;

   case BT_ENUM:
      return CAST_EXPLICIT|CAST_REINTERPRET;

   case BT_POINTER:
   {
      Reference dstR = dst->getReturn();
      Reference srcR = src->getReturn();

      // Should have been handled by the previous equality test...
      if (srcR == dstR)
         return CAST_ANY;

      // Altering storage qualifiers must be explicit. (Not even const_cast.)
      if (srcR->getStoreType() != dstR->getStoreType() ||
          srcR->getStoreArea() != dstR->getStoreArea())
      {
         // ... Unless going from auto* to static* which is safe!
         if (srcR->getStoreType() != STORE_AUTO ||
             dstR->getStoreType() != STORE_STATIC)
            return CAST_EXPLICIT;

         // If only going from auto* to static*, consider it no change.
         // (It requires translation, but this make overloading sane.)
         if (srcR->setStorage(STORE_STATIC) == dstR)
            return CAST_ANY;
      }

      // If they differ only in qualifiers, it's time for const_cast!
      if (srcR->getUnqualified() == dstR->getUnqualified())
      {
         // This means the qualifiers are being lost.
         if (!dstR->getQualifier(srcR->getQualifiers()))
            return CAST_EXPLICIT|CAST_QUALIFIER;
         // Otherwise, they're being added, which means implicit is allowed.
         else
            return CAST_EXPLICIT|CAST_IMPLICIT|CAST_QUALIFIER;
      }

      // Losing qualifiers with differing types, it's time for explicit!
      if (!dstR->getQualifier(srcR->getQualifiers()))
         return CAST_EXPLICIT;

      // Can go to void* implicitly.
      if (dstR->getBasicType() == BT_VOID)
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // Can come from void* via static.
      if (dstR->getBasicType() == BT_VOID)
         return CAST_EXPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // Not losing qualifiers, but still differing types.
      return CAST_EXPLICIT|CAST_REINTERPRET;
   }
   }

   return 0;
}

//
// VariableType::get_cast
//
unsigned VariableType::get_cast(Vector const &dst, Vector const &src)
{
   if (dst.size() != src.size()) return 0;

   unsigned cast = CAST_ANY;

   Vector::const_iterator dstIt = dst.begin(), srcIt = src.begin();
   Vector::const_iterator dstEnd = dst.end();
   while (dstIt != dstEnd)
      cast &= get_cast(*dstIt++, *srcIt++);

   return cast;
}

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
   case BT_FIXED:
   case BT_FLOAT:
   case BT_INT:
   case BT_LFLOAT:
   case BT_LLFLOAT:
   case BT_LLONG:
   case BT_LONG:
   case BT_REAL:
   case BT_SCHAR:
   case BT_SHORT:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULLONG:
   case BT_ULONG:
   case BT_USHORT:
      return true;

   default:
      return false;
   }
}

//
// VariableType::is_bt_function
//
bool VariableType::is_bt_function(BasicType type)
{
   switch (type)
   {
   case BT_ASMFUNC:
   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SCRIPT:
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
   case BT_ENUM:
   case BT_INT:
   case BT_LLONG:
   case BT_LONG:
   case BT_SCHAR:
   case BT_SHORT:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULLONG:
   case BT_ULONG:
   case BT_USHORT:
      return true;

   default:
      return false;
   }
}

//
// VariableType::is_bt_unsigned
//
bool VariableType::is_bt_unsigned(BasicType type)
{
   switch (type)
   {
   case BT_BOOLHARD:
   case BT_BOOLSOFT:
   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_POINTER:
   case BT_SCRIPT:
   case BT_STRING:
   case BT_UCHAR:
   case BT_UINT:
   case BT_ULLONG:
   case BT_ULONG:
   case BT_USHORT:
      return true;

   default:
      return false;
   }
}

//
// make_string<VariableType>
//
std::string make_string(VariableType const *type)
{
   std::ostringstream name;

   name << type;

   return name.str();
}

//
// make_string<VariableType::BasicType>
//
std::string const &make_string(VariableType::BasicType basic)
{
   return basic_names[basic];
}

// EOF

