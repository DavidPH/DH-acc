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

#include "ObjectExpression.hpp"
#include "SourceException.hpp"

#include <algorithm>
#include <sstream>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static std::ostream &operator << (std::ostream &out, VariableType const *type);

static std::ostream &operator << (std::ostream &out, VariableType::BasicType basic);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string const basic_names[] =
{
   // Void type.
   /* BT_VOID    */ "void",

   // Arithmetic types.
   /* BT_BIT_HRD */ "bool",
   /* BT_BIT_SFT */ "softbool",

   /* BT_CHR     */ "char",

   /* BT_CLX     */ "complex",
   /* BT_CLX_IM  */ "imaginary",

   /* BT_SAT     */ "sat",

   /* BT_ACC_HH  */ "unsigned short short accum",
   /* BT_ACC_H   */ "unsigned short accum",
   /* BT_ACC     */ "unsigned accum",
   /* BT_ACC_L   */ "unsigned long accum",
   /* BT_ACC_LL  */ "unsigned long long accum",

   /* BT_ANG_HH  */ "unsigned short short fract",
   /* BT_ANG_H   */ "unsigned short fract",
   /* BT_ANG     */ "unsigned fract",
   /* BT_ANG_L   */ "unsigned long fract",
   /* BT_ANG_LL  */ "unsigned long long fract",

   /* BT_FIX_HH  */ "short short accum",
   /* BT_FIX_H   */ "short accum",
   /* BT_FIX     */ "accum",
   /* BT_FIX_L   */ "long accum",
   /* BT_FIX_LL  */ "long long accum",

   /* BT_FLT_HH  */ "short short float",
   /* BT_FLT_H   */ "short float",
   /* BT_FLT     */ "float",
   /* BT_FLT_L   */ "double",
   /* BT_FLT_LL  */ "long double",

   /* BT_FRA_HH  */ "short short fract",
   /* BT_FRA_H   */ "short fract",
   /* BT_FRA     */ "fract",
   /* BT_FRA_L   */ "long fract",
   /* BT_FRA_LL  */ "long long fract",

   /* BT_INT_HH  */ "signed char",
   /* BT_INT_H   */ "short",
   /* BT_INT     */ "int",
   /* BT_INT_L   */ "long",
   /* BT_INT_LL  */ "long long",

   /* BT_UNS_HH  */ "unsigned char",
   /* BT_UNS_H   */ "unsigned short",
   /* BT_UNS     */ "unsigned",
   /* BT_UNS_L   */ "unsigned long",
   /* BT_UNS_LL  */ "unsigned long long",

   // Special types.
   /* BT_LABEL   */ "label",
   /* BT_STR     */ "str",

   // Reference types.
   /* BT_ARR     */ "array",
   /* BT_PTR     */ "pointer",
   /* BT_PTR_NUL */ "nullptr_t",

   // Named types.
   /* BT_ENUM    */ "enum",
   /* BT_STRUCT  */ "struct",
   /* BT_UNION   */ "union",

   // Anonymous types.
   /* BT_BLOCK   */ "block",
   /* BT_FUN     */ "func_t",
   /* BT_FUN_ASM */ "asmfunc_t",
   /* BT_FUN_LIN */ "lnspec_t",
   /* BT_FUN_NAT */ "native_t",
   /* BT_FUN_SNA */ "snam_t",
   /* BT_FUN_SNU */ "snum_t",
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_near_pointers;


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
   case VariableType::BT_ACC_HH:
   case VariableType::BT_ACC_H:
   case VariableType::BT_ACC:
   case VariableType::BT_ACC_L:
   case VariableType::BT_ACC_LL:
   case VariableType::BT_ANG_HH:
   case VariableType::BT_ANG_H:
   case VariableType::BT_ANG:
   case VariableType::BT_ANG_L:
   case VariableType::BT_ANG_LL:
   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FRA_HH:
   case VariableType::BT_FRA_H:
   case VariableType::BT_FRA:
   case VariableType::BT_FRA_L:
   case VariableType::BT_FRA_LL:
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
   case VariableType::BT_SAT:
      out << type->getBasicType() << ' ' << type->getTypes()[0]->getBasicType();
      break;

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
      out << type->getBasicType() << ' ' << type->getName();
      break;

      // Anonymous types.
   case VariableType::BT_BLOCK:
      out << make_string(type->getBasicType());
      out << '{';
   {
      VariableType::Vector const &types = type->getTypes();

      for(size_t i = 0; i < types.size(); ++i)
         out << types[i] << ';';
   }
      out << '}';
      break;

   case VariableType::BT_FUN:
   case VariableType::BT_FUN_ASM:
   case VariableType::BT_FUN_LIN:
   case VariableType::BT_FUN_NAT:
   case VariableType::BT_FUN_SNA:
   case VariableType::BT_FUN_SNU:
      out << type->getBasicType();
      out << ' ' << type->getReturn();
      out << '(';
   {
      VariableType::Vector const &types = type->getTypes();

      for(size_t i = 0; i < types.size(); ++i)
      {
         if(types[i])
            out << types[i];
         else
            out << "...";

         if(i+1 < types.size()) out << ',';
      }
   }
      out << ')';
      break;
   }

   if(type->getQualifier(VariableType::QUAL_CONST))
      out << " const";

   if(type->getQualifier(VariableType::QUAL_VOLATILE))
      out << " volatile";

   if(type->getQualifier(VariableType::QUAL_RESTRICT))
      out << " restrict";

   switch(type->getStoreType())
   {
   case STORE_NONE:
      break;

   case STORE_FAR:
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
static std::ostream &operator << (std::ostream &out, VariableType::BasicType basic)
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
   basic(BT_VOID), quals(0), store(STORE_NONE), width(0),
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
   basic(_basic), quals(0), store(STORE_NONE), width(0),
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
   case BT_ACC_HH:
   case BT_ACC_H:
   case BT_ACC:
   case BT_ACC_L:
   case BT_ACC_LL:
   case BT_ANG_HH:
   case BT_ANG_H:
   case BT_ANG:
   case BT_ANG_L:
   case BT_ANG_LL:
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
   case BT_FRA_HH:
   case BT_FRA_H:
   case BT_FRA:
   case BT_FRA_L:
   case BT_FRA_LL:
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

   case BT_SAT:
      types.resize(width = 1);
      names.push_back("");
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
      typeRet = get_bt_chr()->setQualifier(QUAL_CONST)->setStorage(STORE_STRING);
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
   case BT_BLOCK:
   case BT_FUN:
   case BT_FUN_ASM:
   case BT_FUN_LIN:
   case BT_FUN_NAT:
   case BT_FUN_SNA:
   case BT_FUN_SNU:
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
   case BT_SAT:
   case BT_ACC_HH:
   case BT_ACC_H:
   case BT_ACC:
   case BT_ACC_L:
   case BT_ACC_LL:
   case BT_ANG_HH:
   case BT_ANG_H:
   case BT_ANG:
   case BT_ANG_L:
   case BT_ANG_LL:
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
   case BT_FRA_HH:
   case BT_FRA_H:
   case BT_FRA:
   case BT_FRA_L:
   case BT_FRA_LL:
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
   case BT_BLOCK:
   case BT_FUN:
   case BT_FUN_ASM:
   case BT_FUN_LIN:
   case BT_FUN_NAT:
   case BT_FUN_SNA:
   case BT_FUN_SNU:
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

   // Pointers shall not have no storage.
   if(store == STORE_NONE)
   {
      if(option_near_pointers)
         return setStorage(STORE_STATIC)->getPointer();
      else
         return setStorage(STORE_FAR)->getPointer();
   }

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

   #define TYPES \
      for(size_t i = 0; i < types.size(); ++i) \
      { \
         if(types[i]) \
            types[i]->getNameMangled(out); \
         else \
            out += "..."; \
         \
         if(i+1 < types.size()) out += ':'; \
      }

   #define WIDTH {std::ostringstream oss; oss << width; out += oss.str();}

   switch (basic)
   {
   case BT_ANG_HH:   out += "AHH"; break;
   case BT_ANG_H:    out += "AH"; break;
   case BT_ANG:      out += 'A'; break;
   case BT_ANG_L:    out += "AL"; break;
   case BT_ANG_LL:   out += "ALL"; break;
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
   case BT_FUN:      out += "F{"; TYPES; out += '}'; break;
   case BT_FUN_ASM:  out += "FA{"; TYPES; out += '}'; break;
   case BT_FUN_LIN:  out += "FL{"; TYPES; out += '}'; break;
   case BT_FUN_NAT:  out += "FN{"; TYPES; out += '}'; break;
   case BT_FUN_SNA:  out += "FSA{"; TYPES; out += '}'; break;
   case BT_FUN_SNU:  out += "FSU{"; TYPES; out += '}'; break;
   case BT_INT_HH:   out += "IHH"; break;
   case BT_INT_H:    out += "IH"; break;
   case BT_INT:      out += 'I'; break;
   case BT_INT_L:    out += "IL"; break;
   case BT_INT_LL:   out += "ILL"; break;
   case BT_CLX_IM:   out += "I{"; TYPE0; out += '}'; break;
   case BT_ACC_HH:   out += "KHH"; break;
   case BT_ACC_H:    out += "KH"; break;
   case BT_ACC:      out += 'K'; break;
   case BT_ACC_L:    out += "KL"; break;
   case BT_ACC_LL:   out += "KLL"; break;
   case BT_LABEL:    out += "LABEL"; break;
   case BT_PTR:      out += "P{"; RETRN; out += '}'; break;
   case BT_PTR_NUL:  out += "PN"; break;
   case BT_FRA_HH:   out += "RHH"; break;
   case BT_FRA_H:    out += "RH"; break;
   case BT_FRA:      out += 'R'; break;
   case BT_FRA_L:    out += "RL"; break;
   case BT_FRA_LL:   out += "RLL"; break;
   case BT_STR:      out += 'S'; break;
   case BT_STRUCT:   out += "S("; out += name; out += ')'; break;
   case BT_SAT:      out += "S{"; TYPE0; out += '}'; break;
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
   case STORE_FAR:            out += "sF"; break;
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
      Error_NP("incomplete type");

   switch (basic)
   {
   case BT_VOID:
      return 0;

   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_ACC_HH:
   case BT_ACC_H:
   case BT_ACC:
   case BT_ANG_HH:
   case BT_ANG_H:
   case BT_ANG:
   case BT_ANG_L:
   case BT_ANG_LL:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FLT_HH:
   case BT_FLT_H:
   case BT_FLT:
   case BT_FRA_HH:
   case BT_FRA_H:
   case BT_FRA:
   case BT_FRA_L:
   case BT_FRA_LL:
   case BT_INT_HH:
   case BT_INT_H:
   case BT_INT:
   case BT_UNS_HH:
   case BT_UNS_H:
   case BT_UNS:

   case BT_LABEL:
   case BT_STR:

   case BT_PTR_NUL:

   case BT_ENUM:
      return 1;

   case BT_CLX:
   case BT_CLX_IM:
   case BT_SAT:
      return typeRet->getSize(pos) * width;

   case BT_ACC_L:
   case BT_ACC_LL:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FLT_L:
   case BT_FLT_LL:
   case BT_INT_L:
   case BT_INT_LL:
   case BT_UNS_L:
   case BT_UNS_LL:
      return 2;

   case BT_ARR:
      return width ? typeRet->getSize(pos) * width : 0;

   case BT_PTR:
      if(is_bt_function(typeRet->basic))
         return typeRet->getSize(pos);

      if(typeRet->store == STORE_FAR || typeRet->store == STORE_STRING)
         return 2;

      return 1;

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

   case BT_FUN_ASM:
      return 0;

   case BT_FUN:
   case BT_FUN_LIN:
   case BT_FUN_NAT:
   case BT_FUN_SNA:
   case BT_FUN_SNU:
      return 1;
   }

   Error_NP("invalid type");
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
// VariableType::getIndex
//
biguint VariableType::getIndex(std::string const &memName, SourcePosition const &pos)
{
   if(!complete)
      Error_NP("incomplete type");

   for(biguint i = 0; i < names.size(); ++i)
      if(names[i] == memName) return i;

   Error_NP("no such member: %s", memName.c_str());
}

//
// VariableType::getOffset
//
bigsint VariableType::getOffset
(std::string const &memName, SourcePosition const &pos)
{
   if (!complete)
      Error_NP("incomplete type");

   if (basic == BT_UNION) return 0;

   bigsint offset = 0;
   for (size_t i = 0; i < names.size(); ++i)
   {
      if (names[i] == memName) return offset;
      offset += types[i]->getSize(pos);
   }

   Error_NP("no such member: %s", memName.c_str());
}

//
// VariableType::getType
//
VariableType::Reference VariableType::getType
(std::string const &memName, SourcePosition const &pos)
{
   if (!complete)
      Error_NP("incomplete type");

   for (size_t i = 0; i < names.size(); ++i)
      if(names[i] == memName) return types[i]->setStorage(this);

   Error_NP("no such member: %s", memName.c_str());
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

   case BT_ACC_HH: return get_bt_acc_hh();
   case BT_ACC_H:  return get_bt_acc_h();
   case BT_ACC:    return get_bt_acc();
   case BT_ACC_L:  return get_bt_acc_l();
   case BT_ACC_LL: return get_bt_acc_ll();

   case BT_ANG_HH: return get_bt_ang_hh();
   case BT_ANG_H:  return get_bt_ang_h();
   case BT_ANG:    return get_bt_ang();
   case BT_ANG_L:  return get_bt_ang_l();
   case BT_ANG_LL: return get_bt_ang_ll();

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

   case BT_FRA_HH: return get_bt_fra_hh();
   case BT_FRA_H:  return get_bt_fra_h();
   case BT_FRA:    return get_bt_fra();
   case BT_FRA_L:  return get_bt_fra_l();
   case BT_FRA_LL: return get_bt_fra_ll();

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
   static Pointer types[25]; return get_bt_complex(types, bt, BT_CLX);
}

//
// VariableType::get_bt_clx_im
//
VariableType::Reference VariableType::get_bt_clx_im(BasicType bt)
{
   static Pointer types[25]; return get_bt_complex(types, bt, BT_CLX_IM);
}

//
// VariableType::get_bt_complex
//
VariableType::Reference VariableType::get_bt_complex(Pointer *type, BasicType bt,
                                                     BasicType clx)
{
   switch(bt)
   {
   case BT_ACC_HH: type +=  0; break;
   case BT_ACC_H:  type +=  1; break;
   case BT_ACC:    type +=  2; break;
   case BT_ACC_L:  type +=  3; break;
   case BT_ACC_LL: type +=  4; break;
   case BT_ANG_HH: type +=  5; break;
   case BT_ANG_H:  type +=  6; break;
   case BT_ANG:    type +=  7; break;
   case BT_ANG_L:  type +=  8; break;
   case BT_ANG_LL: type +=  9; break;
   case BT_FIX_HH: type += 10; break;
   case BT_FIX_H:  type += 11; break;
   case BT_FIX:    type += 12; break;
   case BT_FIX_L:  type += 13; break;
   case BT_FIX_LL: type += 14; break;
   case BT_FLT_HH: type += 15; break;
   case BT_FLT_H:  type += 16; break;
   case BT_FLT:    type += 17; break;
   case BT_FLT_L:  type += 18; break;
   case BT_FLT_LL: type += 19; break;
   case BT_FRA_HH: type += 20; break;
   case BT_FRA_H:  type += 21; break;
   case BT_FRA:    type += 22; break;
   case BT_FRA_L:  type += 23; break;
   case BT_FRA_LL: type += 24; break;
   default: return get_bt_void();
   }

   if(!*type)
   {
      (*type = new VariableType(clx))->types[0] = get_bt(bt);

      if(clx == BT_CLX) (*type)->types[1] = get_bt_clx_im(bt);
   }

   return static_cast<Reference>(*type);
}

//
// VariableType::get_bt_sat
//
VariableType::Reference VariableType::get_bt_sat(BasicType bt)
{
   static Pointer types[20];
   Pointer *type;

   switch(bt)
   {
   case BT_ACC_HH: type = types +  0; break;
   case BT_ACC_H:  type = types +  1; break;
   case BT_ACC:    type = types +  2; break;
   case BT_ACC_L:  type = types +  3; break;
   case BT_ACC_LL: type = types +  4; break;
   case BT_ANG_HH: type = types +  5; break;
   case BT_ANG_H:  type = types +  6; break;
   case BT_ANG:    type = types +  7; break;
   case BT_ANG_L:  type = types +  8; break;
   case BT_ANG_LL: type = types +  9; break;
   case BT_FIX_HH: type = types + 10; break;
   case BT_FIX_H:  type = types + 11; break;
   case BT_FIX:    type = types + 12; break;
   case BT_FIX_L:  type = types + 13; break;
   case BT_FIX_LL: type = types + 14; break;
   case BT_FRA_HH: type = types + 15; break;
   case BT_FRA_H:  type = types + 17; break;
   case BT_FRA:    type = types + 18; break;
   case BT_FRA_L:  type = types + 19; break;
   case BT_FRA_LL: type = types + 20; break;
   default: return get_bt_void();
   }

   if(!*type) (*type = new VariableType(BT_SAT))->types[0] = get_bt(bt);

   return static_cast<Reference>(*type);
}

//
// VariableType::get_bt_acc_hh
//
VariableType::Reference VariableType::get_bt_acc_hh()
{
   static Reference type(new VariableType(BT_ACC_HH)); return type;
}

//
// VariableType::get_bt_acc_h
//
VariableType::Reference VariableType::get_bt_acc_h()
{
   static Reference type(new VariableType(BT_ACC_H)); return type;
}

//
// VariableType::get_bt_acc
//
VariableType::Reference VariableType::get_bt_acc()
{
   static Reference type(new VariableType(BT_ACC)); return type;
}

//
// VariableType::get_bt_acc_l
//
VariableType::Reference VariableType::get_bt_acc_l()
{
   static Reference type(new VariableType(BT_ACC_L)); return type;
}

//
// VariableType::get_bt_acc_ll
//
VariableType::Reference VariableType::get_bt_acc_ll()
{
   static Reference type(new VariableType(BT_ACC_LL)); return type;
}

//
// VariableType::get_bt_ang_hh
//
VariableType::Reference VariableType::get_bt_ang_hh()
{
   static Reference type(new VariableType(BT_ANG_HH)); return type;
}

//
// VariableType::get_bt_ang_h
//
VariableType::Reference VariableType::get_bt_ang_h()
{
   static Reference type(new VariableType(BT_ANG_H)); return type;
}

//
// VariableType::get_bt_ang
//
VariableType::Reference VariableType::get_bt_ang()
{
   static Reference type(new VariableType(BT_ANG)); return type;
}

//
// VariableType::get_bt_ang_l
//
VariableType::Reference VariableType::get_bt_ang_l()
{
   static Reference type(new VariableType(BT_ANG_L)); return type;
}

//
// VariableType::get_bt_ang_ll
//
VariableType::Reference VariableType::get_bt_ang_ll()
{
   static Reference type(new VariableType(BT_ANG_LL)); return type;
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
// VariableType::get_bt_fra_hh
//
VariableType::Reference VariableType::get_bt_fra_hh()
{
   static Reference type(new VariableType(BT_FRA_HH)); return type;
}

//
// VariableType::get_bt_fra_h
//
VariableType::Reference VariableType::get_bt_fra_h()
{
   static Reference type(new VariableType(BT_FRA_H)); return type;
}

//
// VariableType::get_bt_fra
//
VariableType::Reference VariableType::get_bt_fra()
{
   static Reference type(new VariableType(BT_FRA)); return type;
}

//
// VariableType::get_bt_fra_l
//
VariableType::Reference VariableType::get_bt_fra_l()
{
   static Reference type(new VariableType(BT_FRA_L)); return type;
}

//
// VariableType::get_bt_fra_ll
//
VariableType::Reference VariableType::get_bt_fra_ll()
{
   static Reference type(new VariableType(BT_FRA_LL)); return type;
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
         if(*iter) *iter = (*iter)->getUnqualified();

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
// VariableType::get_bt_fun
//
VariableType::Reference VariableType::get_bt_fun(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUN));

   if(types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUN);
}

//
// VariableType::get_bt_fun_asm
//
VariableType::Reference VariableType::get_bt_fun_asm(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUN_ASM));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUN_ASM);
}

//
// VariableType::get_bt_fun_lin
//
VariableType::Reference VariableType::get_bt_fun_lin(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUN_LIN));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUN_LIN);
}

//
// VariableType::get_bt_fun_nat
//
VariableType::Reference VariableType::get_bt_fun_nat(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUN_NAT));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUN_NAT);
}

//
// VariableType::get_bt_fun_sna
//
VariableType::Reference VariableType::get_bt_fun_sna(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUN_SNA));

   if (types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUN_SNA);
}

//
// VariableType::get_bt_fun_snu
//
VariableType::Reference VariableType::get_bt_fun_snu(Vector const &types, VariableType *typeRet)
{
   static Reference type(new VariableType(BT_FUN_SNU));

   if(types.empty() && typeRet->basic == BT_VOID) return type;

   return get_bt_anonymous(types, typeRet, type, BT_FUN_SNU);
}

//===================================================================
// Miscellaneous.
//

//
// VariableType::get_cast
//
VariableType::CastType VariableType::get_cast(VariableType *dst, VariableType *src,
   ObjectExpression *srcObj, CastType exact)
{
   if(dst == src || dst->getUnqualified() == src->getUnqualified())
      return exact;

   if(srcObj && !srcObj->canResolve()) srcObj = NULL;

   BasicType dstBT = dst->getBasicType();
   BasicType srcBT = src->getBasicType();

   if(dstBT != srcBT)
   {
      // array->pointer
      if(srcBT == BT_ARR && dstBT == BT_PTR)
         return get_cast(dst, src->getReturn()->getPointer(), NULL, CAST_POINT);

      // integer->pointer
      if(is_bt_integer(srcBT) && dstBT == BT_PTR)
      {
         if(srcObj && !srcObj->resolveINT())
            return CAST_PROMO;

         return CAST_FORCE;
      }

      // integer->string
      if(is_bt_integer(srcBT) && dstBT == BT_STR)
      {
         if(srcObj && !srcObj->resolveINT())
            return CAST_PROMO;

         return CAST_FORCE;
      }

      // nullptr->pointer
      if(srcBT == BT_PTR_NUL && dstBT == BT_PTR)
         return CAST_PROMO;

      // nullptr->function
      if(srcBT == BT_PTR_NUL && is_bt_function(dstBT))
         return CAST_PROMO;

      // nullptr->string
      if(srcBT == BT_PTR_NUL && dstBT == BT_STR)
         return CAST_PROMO;

      // pointer->boolean
      if(srcBT == BT_PTR && (dstBT == BT_BIT_HRD || dstBT == BT_BIT_SFT))
         return CAST_CONVE;

      // function->boolean
      if(is_bt_function(srcBT) && (dstBT == BT_BIT_HRD || dstBT == BT_BIT_SFT))
         return CAST_CONVE;

      // function->pointer
      if(is_bt_function(srcBT) && dstBT == BT_PTR)
         return get_cast(dst, src->getPointer(), NULL, exact);

      // string->array
      if(srcBT == BT_STR && dstBT == BT_ARR && dst->getReturn()->getBasicType() == BT_CHR)
         return CAST_FORCE;

      // string->integer
      if(srcBT == BT_STR && is_bt_integer(dstBT))
         return CAST_FORCE;

      // string->pointer
      if(srcBT == BT_STR && dstBT == BT_PTR)
         return get_cast(dst, src->getReturn()->getPointer(), NULL, CAST_POINT);

      // arithmetic->arithmetic
      // FIXME: Some arithmetic->arithmetic is CAST_CONVE.
      if(is_bt_arithmetic(srcBT) && is_bt_arithmetic(dstBT))
         return CAST_PROMO;

      // integer->enum
      if(is_bt_integer(srcBT) && dstBT == BT_ENUM)
         return CAST_CONVE;

      // arithmetic->enum
      if(is_bt_arithmetic(srcBT) && dstBT == BT_ENUM)
         return CAST_FORCE;

      // block->
      if(srcBT == BT_BLOCK)
      {
         CastType cast = exact;

         VecObj srcObjVec;
         if(srcObj)
            srcObj->expandOnce(&srcObjVec);
         else
            srcObjVec.resize(src->types.size());

         VecObj::const_iterator srcObjItr = srcObjVec.begin();
         Vector::const_iterator srcItr = src->types.begin(), srcEnd = src->types.end();

         // ->array
         if(dstBT == BT_ARR)
         {
            Reference dstItr = dst->getReturn();

            while(srcItr != srcEnd)
               cast = std::max(get_cast(dstItr, *srcItr++, *srcObjItr++), cast);

            return cast;
         }

         // ->struct
         if(dstBT == BT_STRUCT)
         {
            Vector::const_iterator dstItr = dst->types.begin(), dstEnd = dst->types.end();

            while(dstItr != dstEnd && srcItr != srcEnd)
               cast = std::max(get_cast(*dstItr++, *srcItr++, *srcObjItr++), cast);

            return cast;
         }

         // ->*
         if(src->types.empty())
            return CAST_CONVE;
         else
            return get_cast(dst, src->types.back(), srcObjVec.back(), CAST_PROMO);
      }

      // enum->int
      if(srcBT == BT_ENUM && (is_bt_integer(dstBT) && dstBT != BT_ENUM))
         return CAST_PROMO;

      // enum->arithmetic
      if(srcBT == BT_ENUM && is_bt_arithmetic(dstBT))
         return CAST_FORCE;

      // function->function
      if(is_bt_function(srcBT) && is_bt_function(dstBT))
         return CAST_FORCE;

      // pointer->int
      if(srcBT == BT_PTR && (is_bt_integer(dstBT) && dstBT != BT_ENUM))
         return CAST_FORCE;


      // *->void
      if(dstBT == BT_VOID)
         return CAST_CONVE;


      // *->*
      return CAST_NEVER;
   }
   else switch (dstBT)
   {
   case BT_ARR:
   case BT_BLOCK:
   case BT_STRUCT:
   case BT_UNION:
      return CAST_NEVER;

   case BT_FUN:
   case BT_FUN_ASM:
   case BT_FUN_LIN:
   case BT_FUN_NAT:
   case BT_FUN_SNA:
   case BT_FUN_SNU:
      return CAST_NEVER;

   case BT_BIT_HRD:
   case BT_BIT_SFT:
   case BT_CHR:
   case BT_ACC_HH:
   case BT_ACC_H:
   case BT_ACC:
   case BT_ACC_L:
   case BT_ACC_LL:
   case BT_ANG_HH:
   case BT_ANG_H:
   case BT_ANG:
   case BT_ANG_L:
   case BT_ANG_LL:
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
   case BT_FRA_HH:
   case BT_FRA_H:
   case BT_FRA:
   case BT_FRA_L:
   case BT_FRA_LL:
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
      return exact;

   case BT_CLX:
   case BT_CLX_IM:
   case BT_SAT:
      return get_cast(dst->types[0], src->types[0], NULL, exact);

   case BT_ENUM:
      return CAST_FORCE;

   case BT_PTR:
   {
      Reference dstR = dst->getReturn();
      Reference srcR = src->getReturn();

      // Should have been handled by the previous equality test...
      if(srcR == dstR)
         return exact;

      // Altering storage qualifiers must be explicit. (Not even const_cast.)
      if(srcR->getStoreType() != dstR->getStoreType() ||
         srcR->getStoreArea() != dstR->getStoreArea())
      {
         StoreType dstST = dstR->getStoreType();
         StoreType srcST = srcR->getStoreType();

         // Implicit conversion to far pointer is OK.
         // As long as it's from a type supported for far pointers, of course.
         if(dstST == STORE_FAR && srcST != STORE_REGISTER &&
            srcST != STORE_MAPREGISTER && srcST != STORE_WORLDREGISTER &&
            srcST != STORE_GLOBALREGISTER && srcST != STORE_MAPARRAY)
            return get_cast(dst, srcR->setStorage(STORE_FAR)->getPointer(), NULL, CAST_POINT);

         // Also for auto pointer to static pointer.
         if(srcST == STORE_AUTO && dstST == STORE_STATIC)
            return get_cast(dst, srcR->setStorage(STORE_STATIC)->getPointer(), NULL, CAST_POINT);

         // And by explicit I mean reinterpret. Changing storage is NOT COOL.
         return CAST_NEVER;
      }

      // If they differ only in qualifiers, it's time for const_cast!
      if(srcR->getUnqualified() == dstR->getUnqualified())
      {
         // This means the qualifiers are being lost.
         if(!dstR->getQualifier(srcR->getQualifiers()))
            return CAST_FORCE;
         // Otherwise, they're being added, which means implicit is allowed.
         else
            return CAST_PROMO;
      }

      // Losing qualifiers with differing types, it's time for explicit!
      if(!dstR->getQualifier(srcR->getQualifiers()))
         return CAST_FORCE;

      // Can go to and from void* implicitly.
      if(dstR->getBasicType() == BT_VOID || srcR->getBasicType() == BT_VOID)
         return CAST_CONVE;

      // Not losing qualifiers, but still differing types.
      return CAST_FORCE;
   }
   }

   return CAST_NEVER;
}

//
// VariableType::get_cast
//
VariableType::CastType VariableType::get_cast(Vector const &dst, Vector const &src)
{
   VecObj srcObj(src.size());

   return get_cast(dst, src, srcObj);
}

//
// VariableType::get_cast
//
VariableType::CastType VariableType::get_cast(Vector const &dst, Vector const &src,
                                              VecObj const &srcObj)
{
   if(dst.size() != src.size()) return CAST_NEVER;

   CastType cast = CAST_EXACT;

   VecObj::const_iterator srcObjItr = srcObj.begin();
   Vector::const_iterator srcItr = src.begin(), srcEnd = src.end();
   Vector::const_iterator dstItr = dst.begin();

   while(srcItr != srcEnd && *dstItr)
      cast = std::max(get_cast(*dstItr++, *srcItr++, *srcObjItr++), cast);

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
   case BT_ACC_HH:
   case BT_ACC_H:
   case BT_ACC:
   case BT_ACC_L:
   case BT_ACC_LL:
   case BT_ANG_HH:
   case BT_ANG_H:
   case BT_ANG:
   case BT_ANG_L:
   case BT_ANG_LL:
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
   case BT_FRA_HH:
   case BT_FRA_H:
   case BT_FRA:
   case BT_FRA_L:
   case BT_FRA_LL:
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
   case BT_ACC_HH:
   case BT_ACC_H:
   case BT_ACC:
   case BT_ACC_L:
   case BT_ACC_LL:
   case BT_ANG_HH:
   case BT_ANG_H:
   case BT_ANG:
   case BT_ANG_L:
   case BT_ANG_LL:
   case BT_FIX_HH:
   case BT_FIX_H:
   case BT_FIX:
   case BT_FIX_L:
   case BT_FIX_LL:
   case BT_FRA_HH:
   case BT_FRA_H:
   case BT_FRA:
   case BT_FRA_L:
   case BT_FRA_LL:
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
   case BT_FUN:
   case BT_FUN_ASM:
   case BT_FUN_LIN:
   case BT_FUN_NAT:
   case BT_FUN_SNA:
   case BT_FUN_SNU:
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

   case BT_FUN:
   case BT_FUN_LIN:
   case BT_FUN_NAT:
   case BT_FUN_SNA:
   case BT_FUN_SNU:
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
   static std::string const nullname = "...";

   if(!type)
      return nullname;

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

