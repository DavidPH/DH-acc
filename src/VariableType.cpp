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
   // Void type.
   "BT_VOID",

   // Arithmetic types.
   "BT_BIT_HRD",
   "BT_BIT_SFT",

   "BT_CHR",

   "BT_CLX",
   "BT_CLX_IM",

   "BT_FIX_HH",
   "BT_FIX_H",
   "BT_FIX",
   "BT_FIX_L",
   "BT_FIX_LL",

   "BT_FLT_HH",
   "BT_FLT_H",
   "BT_FLT",
   "BT_FLT_L",
   "BT_FLT_LL",

   "BT_INT_HH",
   "BT_INT_H",
   "BT_INT",
   "BT_INT_L",
   "BT_INT_LL",

   "BT_UNS_HH",
   "BT_UNS_H",
   "BT_UNS",
   "BT_UNS_L",
   "BT_UNS_LL",

   // Special types.
   "BT_LABEL",
   "BT_STR",

   // Reference types.
   "BT_ARR",
   "BT_PTR",
   "BT_PTR_NUL",

   // Named types.
   "BT_ENUM",
   "BT_STRUCT",
   "BT_UNION",

   // Anonymous types.
   "BT_BLOCK",
   "BT_ASMFUNC",
   "BT_FUNCTION",
   "BT_LINESPEC",
   "BT_NATIVE",
   "BT_SNAM",
   "BT_SNUM",
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// operator << <VariableType>
//
static std::ostream &operator << (std::ostream &out, VariableType const *type)
{
   switch (type->getBasicType())
   {
      // Void type.
   case VariableType::BT_VOID:
      out << type->getBasicType();
      break;

      // Arithmetic types.
   case VariableType::BT_BIT_HRD:
   case VariableType::BT_BIT_SFT:
   case VariableType::BT_CHR:
   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_INT_L:
   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
   case VariableType::BT_UNS_L:
   case VariableType::BT_UNS_LL:
      out << type->getBasicType();
      break;

   case VariableType::BT_CLX:
   case VariableType::BT_CLX_IM:
      out << type->getBasicType() << ' ' << type->getTypes()[0]->getBasicType();

      // Special types.
   case VariableType::BT_LABEL:
   case VariableType::BT_STR:
      out << type->getBasicType();
      break;

      // Reference types.
   case VariableType::BT_ARR:
      out << type->getReturn();
      out << '[' << type->getWidth() << ']';
      return out; // Don't show qualifiers on array.

   case VariableType::BT_PTR:
      out << type->getReturn() << " *";
      break;

   case VariableType::BT_PTR_NUL:
      out << type->getBasicType();
      break;

      // Named types.
   case VariableType::BT_ENUM:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
      out << type->getBasicType() << '{' << type->getName() << '}';
      break;

      // Anonymous types.
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

   case VariableType::BT_ASMFUNC:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_SNAM:
   case VariableType::BT_SNUM:
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

   switch (type->getStoreType())
   {
   case STORE_NONE:
      break;

   case STORE_STATIC:
   case STORE_AUTO:
   case STORE_CONST:
   case STORE_REGISTER:
   case STORE_MAPREGISTER:
   case STORE_WORLDREGISTER:
   case STORE_GLOBALREGISTER:
   case STORE_STRING:
      out << ' ' << make_string(type->getStoreType());
      break;

   case STORE_MAPARRAY:
   case STORE_WORLDARRAY:
   case STORE_GLOBALARRAY:
      out << ' ' << make_string(type->getStoreType());
      out << '(' << type->getStoreArea() << ')';
      break;
   }

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
   name(type.name), storeArea(type.storeArea),
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
      // Void type.
   case BT_VOID:
      break;

      // Arithmetic types.
   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FLT_HH:
   case BT_FLT_H:
   case BT_FLT:
   case BT_FLT_L:
   case BT_FLT_LL:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_INT_L:
   case BT_INT_LL:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:
   case BT_UNS_LL:
      break;

   case BT_CLX:
      types.resize(width = 2);
      names.push_back("real");
      names.push_back("imag");
      break;

   case BT_CLX_IM:
      types.resize(width = 1);
      names.push_back("real");
      break;

      // Special types.
   case BT_LABEL:
      break;

   case BT_STR:
      typeRet = get_bt_chr()->addQualifier(QUAL_CONST);
      break;

      // Reference types.
   case BT_ARR:
   case BT_PTR:
   case BT_PTR_NUL:
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
   case BT_SNAM:
   case BT_SNUM:
      break;
   }
}

//
// VariableType::~VariableType
//
VariableType::~VariableType()
{
   // Unlink from type list.
   next->prev = prev;
   prev->next = next;

   // Unlink from special list.
   specnext->specprev = specprev;
   specprev->specnext = specnext;

   switch (basic)
   {
      // Void type.
   case BT_VOID:
      break;

   // Arithmetic types.
   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_CLX:
   case BT_CLX_IM:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FLT_HH:
   case BT_FLT_H:
   case BT_FLT:
   case BT_FLT_L:
   case BT_FLT_LL:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_INT_L:
   case BT_INT_LL:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:
   case BT_UNS_LL:
      break;

      // Special types.
   case BT_LABEL:
   case BT_STR:
      break;

      // Reference types.
   case BT_ARR:
      if (typeRet->typeArr == this)
         typeRet->typeArr = specnext == this ? NULL : specnext;
      break;

   case BT_PTR:
      if(typeRet->typePtr == this)
         typeRet->typePtr = NULL;
      break;

   case BT_PTR_NUL:
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
   case BT_SNAM:
   case BT_SNUM:
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
      typeArr = new VariableType(BT_ARR);

      typeArr->typeRet = static_cast<Reference>(this);
      typeArr->storeArea = storeArea;
      typeArr->quals = quals;
      typeArr->store = store;
      typeArr->width = _width;

      if (typeUnq)
         typeArr->typeUnq = typeUnq->getArray(_width);

      return static_cast<Reference>(typeArr);
   }

   for (VariableType *iter = typeArr->specnext; ; iter = iter->specnext)
   {
      if (iter->width == _width)
         return static_cast<Reference>(iter);

      if (iter == typeArr) break;
   }

   Reference type(new VariableType(BT_ARR));

   type->typeRet = static_cast<Reference>(this);
   type->storeArea = storeArea;
   type->quals = quals;
   type->store = store;
   type->width = _width;

   if (typeUnq)
      type->typeUnq = typeUnq->getArray(_width);

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
   if (typePtr) return static_cast<Reference>(typePtr);

   typePtr = new VariableType(BT_PTR);

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

   if (basic == BT_ARR)
      return typeRet->setQualifier(_quals)->getArray(width);

   for (VariableType *iter = next; iter != this; iter = iter->next)
   {
      if (iter->quals == _quals &&
          iter->store == store && iter->storeArea == storeArea)
         return Reference(iter);
   }

   VariableType::Reference type(new VariableType(*this));

   type->quals = _quals;
   if(basic == BT_CLX || basic == BT_CLX_IM || basic == BT_STRUCT ||
      basic == BT_UNION || basic == BT_BLOCK)
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
   static std::string const _storeArea;
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

   if (basic == BT_ARR)
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
   if(basic == BT_CLX || basic == BT_CLX_IM || basic == BT_STRUCT ||
      basic == BT_UNION || basic == BT_BLOCK)
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

   #define TYPE0 types[0]->getNameMangled(out)

   #define TYPES                             \
   for (size_t i = 0; i < types.size(); ++i) \
   {                                         \
      types[i]->getNameMangled(out);         \
      if (i+1 < types.size()) out += ':';    \
   }

   #define WIDTH {std::ostringstream oss; oss << width; out += oss.str();}

   switch (basic)
   {
   case BT_ARR:      out += "A{"; RETRN; out += "}:["; WIDTH; out += ']'; break;
   case BT_BLOCK:    out += "B{"; TYPES; out += '}'; break;
   case BT_BIT_HRD:  out += 'B'; break;
   case BT_BIT_SFT:  out += "BS"; break;
   case BT_CHR:      out += 'C'; break;
   case BT_CLX:      out += "C{"; TYPE0; out += '}'; break;
   case BT_ENUM:     out += "E("; out += name; out += ')'; break;
   case BT_FLT_HH:   out += "FHH"; break;
   case BT_FLT_H:    out += "FH"; break;
   case BT_FLT:      out += 'F'; break;
   case BT_FLT_L:    out += "FL"; break;
   case BT_FLT_LL:   out += "FLL"; break;
   case BT_FUNCTION: out += "F{"; TYPES; out += '}'; break;
   case BT_ASMFUNC:  out += "FA{"; TYPES; out += '}'; break;
   case BT_LINESPEC: out += "FL{"; TYPES; out += '}'; break;
   case BT_NATIVE:   out += "FN{"; TYPES; out += '}'; break;
   case BT_SNAM:     out += "FSA{"; TYPES; out += '}'; break;
   case BT_SNUM:     out += "FSU{"; TYPES; out += '}'; break;
   case BT_INT_HH:   out += "IHH"; break;
   case BT_INT_H:    out += "IH"; break;
   case BT_INT:      out += 'I'; break;
   case BT_INT_L:    out += "IL"; break;
   case BT_INT_LL:   out += "ILL"; break;
   case BT_CLX_IM:   out += "I{"; TYPE0; out += '}'; break;
   case BT_LABEL:    out += "LABEL"; break;
   case BT_PTR:      out += "P{"; RETRN; out += '}'; break;
   case BT_PTR_NUL:  out += "PN"; break;
   case BT_STR:      out += 'S'; break;
   case BT_STRUCT:   out += "S("; out += name; out += ')'; break;
   case BT_UNS_HH:   out += "UHH"; break;
   case BT_UNS_H:    out += "UH"; break;
   case BT_UNS:      out += 'U'; break;
   case BT_UNS_L:    out += "UL"; break;
   case BT_UNS_LL:   out += "ULL"; break;
   case BT_UNION:    out += "U("; out += name; out += ')'; break;
   case BT_VOID:     out += 'V'; break;
   case BT_FIX_HH:   out += "XHH"; break;
   case BT_FIX_H:    out += "XH"; break;
   case BT_FIX:      out += 'X'; break;
   case BT_FIX_L:    out += "XL"; break;
   case BT_FIX_LL:   out += "XLL"; break;
   }

   if (getQualifier(QUAL_CONST))    out += 'c';
   if (getQualifier(QUAL_VOLATILE)) out += 'v';
   if (getQualifier(QUAL_RESTRICT)) out += 'r';

   switch (store)
   {
   case STORE_NONE: break;
   case STORE_STATIC:         out += "sS"; break;
   case STORE_AUTO:           out += "sA"; break;
   case STORE_CONST:          out += "sC"; break;
   case STORE_REGISTER:       out += "sR"; break;
   case STORE_MAPREGISTER:    out += "sMR"; break;
   case STORE_WORLDREGISTER:  out += "sWR"; break;
   case STORE_GLOBALREGISTER: out += "sGR"; break;
   case STORE_MAPARRAY:       out += "sMA("; out += storeArea; out += ')'; break;
   case STORE_WORLDARRAY:     out += "sWA("; out += storeArea; out += ')'; break;
   case STORE_GLOBALARRAY:    out += "sGA("; out += storeArea; out += ')'; break;
   case STORE_STRING:         out += "sSA"; break;
   }

   #undef WIDTH
   #undef TYPES
   #undef TYPE0
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
   case BT_VOID:
   case BT_ASMFUNC:
      return 0;

   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FLT_HH:
   case BT_FLT_H:
   case BT_FLT:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_INT_L:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:

   case BT_LABEL:
   case BT_STR:

   case BT_PTR_NUL:

   case BT_ENUM:

   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SNAM:
   case BT_SNUM:
      return 1;

   case BT_CLX:
   case BT_CLX_IM:
      return typeRet->getSize(pos) * width;

   case BT_FLT_L:
   case BT_FLT_LL:
   case BT_INT_LL:
   case BT_UNS_LL:
      return 2;

   case BT_ARR:
      return width ? typeRet->getSize(pos) * width : 0;

   case BT_PTR:
      return (typeRet->store == STORE_NONE || typeRet->store == STORE_STRING) ? 2 : 1;

   case BT_STRUCT:
   case BT_BLOCK:
   {
      bigsint size = 0;
      for (Vector::const_iterator iter = types.begin(); iter != types.end();
           ++iter)
         size += (*iter)->getSize(pos);
      return size;
   }

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
// VariableType::get_bt
//
VariableType::Reference VariableType::get_bt(BasicType bt)
{
   switch(bt)
   {
   default:
   case BT_VOID: return get_bt_void();

   case BT_BIT_HRD: return get_bt_bit_hrd();
   case BT_BIT_SFT: return get_bt_bit_sft();

   case BT_CHR: return get_bt_chr();

   case BT_FIX_HH: return get_bt_fix_hh();
   case BT_FIX_H:  return get_bt_fix_h();
   case BT_FIX:    return get_bt_fix();
   case BT_FIX_L:  return get_bt_fix_l();
   case BT_FIX_LL: return get_bt_fix_ll();

   case BT_FLT_HH: return get_bt_flt_hh();
   case BT_FLT_H:  return get_bt_flt_h();
   case BT_FLT:    return get_bt_flt();
   case BT_FLT_L:  return get_bt_flt_l();
   case BT_FLT_LL: return get_bt_flt_ll();

   case BT_INT_HH: return get_bt_int_hh();
   case BT_INT_H:  return get_bt_int_h();
   case BT_INT:    return get_bt_int();
   case BT_INT_L:  return get_bt_int_l();
   case BT_INT_LL: return get_bt_int_ll();

   case BT_UNS_HH: return get_bt_uns_hh();
   case BT_UNS_H:  return get_bt_uns_h();
   case BT_UNS:    return get_bt_uns();
   case BT_UNS_L:  return get_bt_uns_l();
   case BT_UNS_LL: return get_bt_uns_ll();

   case BT_LABEL: return get_bt_label();
   case BT_STR:   return get_bt_str();

   case BT_PTR_NUL: return get_bt_ptr_nul();
   }
}

//===================================================================
// Void type
//

//
// VariableType::get_bt_void
//
VariableType::Reference VariableType::get_bt_void()
{
   static Reference type(new VariableType); return type;
}

//===================================================================
// Arithmetic types
//

//
// VariableType::get_bt_bit_hrd
//
VariableType::Reference VariableType::get_bt_bit_hrd()
{
   static Reference type(new VariableType(BT_BIT_HRD)); return type;
}

//
// VariableType::get_bt_bit_sft
//
VariableType::Reference VariableType::get_bt_bit_sft()
{
   static Reference type(new VariableType(BT_BIT_SFT)); return type;
}

//
// VariableType::get_bt_chr
//
VariableType::Reference VariableType::get_bt_chr()
{
   static Reference type(new VariableType(BT_CHR)); return type;
}

//
// VariableType::get_bt_clx
//
VariableType::Reference VariableType::get_bt_clx(BasicType bt)
{
   static Pointer types[10]; return get_bt_complex(types, bt, BT_CLX);
}

//
// VariableType::get_bt_clx_im
//
VariableType::Reference VariableType::get_bt_clx_im(BasicType bt)
{
   static Pointer types[10]; return get_bt_complex(types, bt, BT_CLX_IM);
}

//
// VariableType::get_bt_complex
//
VariableType::Reference VariableType::get_bt_complex(Pointer *types, BasicType bt,
                                                     BasicType clx)
{
   switch(bt)
   {
   case BT_FIX_HH: types = types+0; break;
   case BT_FIX_H:  types = types+1; break;
   case BT_FIX:    types = types+2; break;
   case BT_FIX_L:  types = types+3; break;
   case BT_FIX_LL: types = types+4; break;
   case BT_FLT_HH: types = types+5; break;
   case BT_FLT_H:  types = types+6; break;
   case BT_FLT:    types = types+7; break;
   case BT_FLT_L:  types = types+8; break;
   case BT_FLT_LL: types = types+9; break;
   default: return get_bt_void();
   }

   if(!*types)
   {
      (*types = new VariableType(clx))->types[0] = get_bt(bt);

      if(clx == BT_CLX) (*types)->types[1] = get_bt_clx_im(bt);
   }

   return static_cast<Reference>(*types);
}

//
// VariableType::get_bt_fix_hh
//
VariableType::Reference VariableType::get_bt_fix_hh()
{
   static Reference type(new VariableType(BT_FIX_HH)); return type;
}

//
// VariableType::get_bt_fix_h
//
VariableType::Reference VariableType::get_bt_fix_h()
{
   static Reference type(new VariableType(BT_FIX_H)); return type;
}

//
// VariableType::get_bt_fix
//
VariableType::Reference VariableType::get_bt_fix()
{
   static Reference type(new VariableType(BT_FIX)); return type;
}

//
// VariableType::get_bt_fix_l
//
VariableType::Reference VariableType::get_bt_fix_l()
{
   static Reference type(new VariableType(BT_FIX_L)); return type;
}

//
// VariableType::get_bt_fix_ll
//
VariableType::Reference VariableType::get_bt_fix_ll()
{
   static Reference type(new VariableType(BT_FIX_LL)); return type;
}

//
// VariableType::get_bt_flt_hh
//
VariableType::Reference VariableType::get_bt_flt_hh()
{
   static Reference type(new VariableType(BT_FLT_HH)); return type;
}

//
// VariableType::get_bt_flt_h
//
VariableType::Reference VariableType::get_bt_flt_h()
{
   static Reference type(new VariableType(BT_FLT_H)); return type;
}

//
// VariableType::get_bt_flt
//
VariableType::Reference VariableType::get_bt_flt()
{
   static Reference type(new VariableType(BT_FLT)); return type;
}

//
// VariableType::get_bt_flt_l
//
VariableType::Reference VariableType::get_bt_flt_l()
{
   static Reference type(new VariableType(BT_FLT_L)); return type;
}

//
// VariableType::get_bt_flt_ll
//
VariableType::Reference VariableType::get_bt_flt_ll()
{
   static Reference type(new VariableType(BT_FLT_LL)); return type;
}

//
// VariableType::get_bt_int_hh
//
VariableType::Reference VariableType::get_bt_int_hh()
{
   static Reference type(new VariableType(BT_INT_HH)); return type;
}

//
// VariableType::get_bt_int_h
//
VariableType::Reference VariableType::get_bt_int_h()
{
   static Reference type(new VariableType(BT_INT_H)); return type;
}

//
// VariableType::get_bt_int
//
VariableType::Reference VariableType::get_bt_int()
{
   static Reference type(new VariableType(BT_INT)); return type;
}

//
// VariableType::get_bt_int_l
//
VariableType::Reference VariableType::get_bt_int_l()
{
   static Reference type(new VariableType(BT_INT_L)); return type;
}

//
// VariableType::get_bt_int_ll
//
VariableType::Reference VariableType::get_bt_int_ll()
{
   static Reference type(new VariableType(BT_INT_LL)); return type;
}

//
// VariableType::get_bt_uns_hh
//
VariableType::Reference VariableType::get_bt_uns_hh()
{
   static Reference type(new VariableType(BT_UNS_HH)); return type;
}

//
// VariableType::get_bt_uns_h
//
VariableType::Reference VariableType::get_bt_uns_h()
{
   static Reference type(new VariableType(BT_UNS_H)); return type;
}

//
// VariableType::get_bt_uns
//
VariableType::Reference VariableType::get_bt_uns()
{
   static Reference type(new VariableType(BT_UNS)); return type;
}

//
// VariableType::get_bt_uns_l
//
VariableType::Reference VariableType::get_bt_uns_l()
{
   static Reference type(new VariableType(BT_UNS_L)); return type;
}

//
// VariableType::get_bt_uns_ll
//
VariableType::Reference VariableType::get_bt_uns_ll()
{
   static Reference type(new VariableType(BT_UNS_LL)); return type;
}

//===================================================================
// Special types
//

//
// VariableType::get_bt_label
//
VariableType::Reference VariableType::get_bt_label()
{
   static Reference type(new VariableType(BT_LABEL)); return type;
}

//
// VariableType::get_bt_str
//
VariableType::Reference VariableType::get_bt_str()
{
   static Reference type(new VariableType(BT_STR)); return type;
}

//===================================================================
// Reference types
//

//
// VariableType::get_bt_ptr_nul
//
VariableType::Reference VariableType::get_bt_ptr_nul()
{
   static Reference type(new VariableType(BT_PTR_NUL)); return type;
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
// VariableType::get_bt_block
//
VariableType::Reference VariableType::get_bt_block(Vector const &types)
{
   static Reference type(new VariableType(BT_BLOCK));

   if (types.empty()) return type;

   return get_bt_anonymous(types, get_bt_void(), type, BT_BLOCK);
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
// VariableType::get_bt_snam
//
VariableType::Reference VariableType::get_bt_snam(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_SNAM));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_SNAM);
}

//
// VariableType::get_bt_snum
//
VariableType::Reference VariableType::get_bt_snum(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_SNUM));

   if(types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_SNUM);
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
   if (dstBT == BT_PTR && srcBT == BT_ARR)
      srcBT = BT_PTR;

   if (dstBT != srcBT)
   {
      // nullptr->pointer
      if (srcBT == BT_PTR_NUL && dstBT == BT_PTR)
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // nullptr->function
      if (srcBT == BT_PTR_NUL && is_bt_function(dstBT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // nullptr->string
      if(srcBT == BT_PTR_NUL && dstBT == BT_STR)
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // pointer->boolean
      if (srcBT == BT_PTR && (dstBT == BT_BIT_HRD || dstBT == BT_BIT_SFT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // function->boolean
      if (is_bt_function(srcBT) && (dstBT == BT_BIT_HRD || dstBT == BT_BIT_SFT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // arithmetic<->arithmetic
      if (is_bt_arithmetic(srcBT) && is_bt_arithmetic(dstBT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // arithmetic->enum
      if (is_bt_arithmetic(srcBT) && dstBT == BT_ENUM)
         return CAST_EXPLICIT|CAST_REINTERPRET;

      // block->array,struct
      if (srcBT == BT_BLOCK && (dstBT == BT_ARR || dstBT == BT_STRUCT))
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // block->*
      if(srcBT == BT_BLOCK && dstBT != BT_BLOCK)
      {
         if(src->types.empty())
            return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC;
         else
            return get_cast(dst, src->types.back());
      }

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
      if (is_bt_integer(srcBT) && dstBT == BT_PTR)
         return CAST_EXPLICIT|CAST_STATIC|CAST_REINTERPRET;

      // pointer->int
      if (srcBT == BT_PTR && (is_bt_integer(dstBT) && dstBT != BT_ENUM))
         return CAST_EXPLICIT|CAST_STATIC|CAST_REINTERPRET;


      // *->void
      if (dstBT == BT_VOID)
         return CAST_EXPLICIT|CAST_IMPLICIT|CAST_STATIC|CAST_REINTERPRET;


      // *->*
      return CAST_EXPLICIT;
   }
   else switch (dstBT)
   {
   case BT_ARR:
   case BT_BLOCK:
   case BT_STRUCT:
   case BT_UNION:
      return 0;

   case BT_ASMFUNC:
   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SNAM:
   case BT_SNUM:
      return CAST_EXPLICIT|CAST_REINTERPRET;

   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FLT_HH:
   case BT_FLT_H:
   case BT_FLT:
   case BT_FLT_L:
   case BT_FLT_LL:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_INT_L:
   case BT_INT_LL:
   case BT_LABEL:
   case BT_PTR_NUL:
   case BT_STR:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:
   case BT_UNS_LL:
   case BT_VOID:
      // Should have been handled by the previous equality test...
      return CAST_ANY;

   case BT_CLX:
   case BT_CLX_IM:
      return get_cast(dst->types[0], src->types[0]);

   case BT_ENUM:
      return CAST_EXPLICIT|CAST_REINTERPRET;

   case BT_PTR:
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
   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FLT_HH:
   case BT_FLT_H:
   case BT_FLT:
   case BT_FLT_L:
   case BT_FLT_LL:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_INT_L:
   case BT_INT_LL:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:
   case BT_UNS_LL:
      return true;

   default:
      return false;
   }
}

//
// VariableType::is_bt_fix
//
bool VariableType::is_bt_fix(BasicType type)
{
   switch (type)
   {
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
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
   case BT_SNAM:
   case BT_SNUM:
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
   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_INT_L:
   case BT_INT_LL:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:
   case BT_UNS_LL:

   case BT_ENUM:
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
   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:
   case BT_UNS_L:
   case BT_UNS_LL:

   case BT_STR:

   case BT_PTR:

   case BT_FUNCTION:
   case BT_LINESPEC:
   case BT_NATIVE:
   case BT_SNAM:
   case BT_SNUM:
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

