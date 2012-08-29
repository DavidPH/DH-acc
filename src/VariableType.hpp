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

#ifndef HPP_VariableType_
#define HPP_VariableType_

#include "bignum.hpp"
#include "Counter.hpp"
#include "StoreType.hpp"

#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourcePosition;

//
// VariableType
//
// Stores information about source-level types.
//
class VariableType : PlainCounter
{
   MAKE_NOVIRTUAL_COUNTER_CLASS_BASE(VariableType, PlainCounter);

public:
   typedef std::vector<std::string> VecStr;
   typedef std::vector<Pointer> Vector;

   //
   // ::CastType
   //
   enum CastType
   {
      CAST_EXACT,
      CAST_POINT,
      CAST_PROMO,
      CAST_CONVE,
      CAST_FORCE,
      CAST_NEVER
   };

   //
   // BasicType
   //
   enum BasicType
   {
      // Void type.
      BT_VOID,

      // Arithmetic types.
      BT_BIT_HRD,
      BT_BIT_SFT,

      BT_CHR,

      BT_CLX,    // complex
      BT_CLX_IM, // imaginary

      BT_SAT,

      BT_ACC_HH,
      BT_ACC_H,
      BT_ACC,
      BT_ACC_L,
      BT_ACC_LL,

      BT_ANG_HH,
      BT_ANG_H,
      BT_ANG,
      BT_ANG_L,
      BT_ANG_LL,

      BT_FIX_HH,
      BT_FIX_H,
      BT_FIX,
      BT_FIX_L,
      BT_FIX_LL,

      BT_FLT_HH,
      BT_FLT_H,
      BT_FLT,
      BT_FLT_L,
      BT_FLT_LL,

      BT_FRA_HH,
      BT_FRA_H,
      BT_FRA,
      BT_FRA_L,
      BT_FRA_LL,

      BT_INT_HH,
      BT_INT_H,
      BT_INT,
      BT_INT_L,
      BT_INT_LL,

      BT_UNS_HH,
      BT_UNS_H,
      BT_UNS,
      BT_UNS_L,
      BT_UNS_LL,

      // Special types.
      BT_LABEL,
      BT_STR,

      // Reference types.
      BT_ARR,
      BT_PTR,
      BT_PTR_NUL,

      // Named types.
      BT_ENUM,
      BT_STRUCT,
      BT_UNION,

      // Anonymous types.
      BT_BLOCK,
      BT_FUN,
      BT_FUN_ASM, // asmfunc
      BT_FUN_LIN, // linepec
      BT_FUN_NAT, // native
      BT_FUN_SNA, // script, named
      BT_FUN_SNU, // script, numbered
   };

   //
   // Qualifier
   //
   enum Qualifier
   {
      // General qualifiers.
      QUAL_CONST    = 0x0001,
      QUAL_VOLATILE = 0x0002,
      QUAL_RESTRICT = 0x0004,

      // BT_FUN qualifiers.
      QUAL_AUTOCALL = 0x00010000,
   };


   // Type creation.
   Reference addQualifier(unsigned _quals) {return setQualifier(quals|_quals);}
   Reference getArray(bigsint width);
   Reference getPointer();
   Reference getReturn() const {return typeRet;}
   Reference getUnqualified() {return typeUnq ? static_cast<Reference>(typeUnq)
                                              : static_cast<Reference>(this);}
   Reference setQualifier(unsigned quals);
   Reference setStorage(StoreType store);
   Reference setStorage(StoreType store, std::string const &storeArea);
   Reference setStorage(VariableType const *type)
   {
      return setStorage(type->store, type->storeArea);
   }

   // Type information.
   BasicType getBasicType() const {return basic;}
   bool getComplete() const {return complete;}
   std::string const &getName() const {return name;}
   void getNameMangled(std::string &out) const;
   VecStr const &getNames() const {return names;}
   bool getQualifier(unsigned _quals) const {return (quals&_quals) == _quals;}
   unsigned getQualifiers() const {return quals;}
   bigsint getSize(SourcePosition const &position) const;
   StoreType getStoreType() const {return store;}
   std::string const &getStoreArea() const {return storeArea;}
   Vector const &getTypes() const {return types;}
   bigsint getWidth() const {return width;}

   // Type alteration.
   // Completes a BT_ENUM.
   void makeComplete();
   // Completes a BT_STRUCT or BT_UNION.
   void makeComplete(VecStr const &names, Vector const &types);

   // Member information.
   bigsint getOffset(std::string const &name, SourcePosition const &position);
   Reference getType(std::string const &name, SourcePosition const &position);


   // Basic types.
   static Reference get_bt(BasicType bt);

   // Void type.
   static Reference get_bt_void();

   // Arithmetic types.
   static Reference get_bt_bit_hrd();
   static Reference get_bt_bit_sft();

   static Reference get_bt_chr();

   static Reference get_bt_clx(BasicType bt);
   static Reference get_bt_clx_im(BasicType bt);

   static Reference get_bt_sat(BasicType bt);

   static Reference get_bt_acc_hh();
   static Reference get_bt_acc_h();
   static Reference get_bt_acc();
   static Reference get_bt_acc_l();
   static Reference get_bt_acc_ll();

   static Reference get_bt_ang_hh();
   static Reference get_bt_ang_h();
   static Reference get_bt_ang();
   static Reference get_bt_ang_l();
   static Reference get_bt_ang_ll();

   static Reference get_bt_fix_hh();
   static Reference get_bt_fix_h();
   static Reference get_bt_fix();
   static Reference get_bt_fix_l();
   static Reference get_bt_fix_ll();

   static Reference get_bt_flt_hh();
   static Reference get_bt_flt_h();
   static Reference get_bt_flt();
   static Reference get_bt_flt_l();
   static Reference get_bt_flt_ll();

   static Reference get_bt_fra_hh();
   static Reference get_bt_fra_h();
   static Reference get_bt_fra();
   static Reference get_bt_fra_l();
   static Reference get_bt_fra_ll();

   static Reference get_bt_int_hh();
   static Reference get_bt_int_h();
   static Reference get_bt_int();
   static Reference get_bt_int_l();
   static Reference get_bt_int_ll();

   static Reference get_bt_uns_hh();
   static Reference get_bt_uns_h();
   static Reference get_bt_uns();
   static Reference get_bt_uns_l();
   static Reference get_bt_uns_ll();

   // Special types.
   static Reference get_bt_label();
   static Reference get_bt_str();

   // Reference types.
   static Reference get_bt_ptr_nul();

   // Named types.
   static Reference get_bt_enum(std::string const &name);
   static Reference get_bt_struct(std::string const &name);
   static Reference get_bt_union(std::string const &name);

   // Anonymous types.
   static Reference get_bt_block(Vector const &types);
   static Reference get_bt_fun(Vector const &types, VariableType *typeRet);
   static Reference get_bt_fun_asm(Vector const &types, VariableType *typeRet);
   static Reference get_bt_fun_lin(Vector const &types, VariableType *typeRet);
   static Reference get_bt_fun_nat(Vector const &types, VariableType *typeRet);
   static Reference get_bt_fun_sna(Vector const &types, VariableType *typeRet);
   static Reference get_bt_fun_snu(Vector const &types, VariableType *typeRet);

   static CastType get_cast(VariableType *dst, VariableType *src, CastType exact = CAST_EXACT);
   static CastType get_cast(Vector const &dst, Vector const &src);

   // Returns true if the given type is an arithmetic type.
   static bool is_bt_arithmetic(BasicType type);

   // Returns true if the given type is a BT_BIT* type.
   static bool is_bt_bit(BasicType type) {return type == BT_BIT_HRD || type == BT_BIT_SFT;}

   // Returns true if the given type is a BT_FIX* type.
   static bool is_bt_fix(BasicType type);

   static bool is_bt_function(BasicType type);

   // Returns true if the given type is an integer type.
   static bool is_bt_integer(BasicType type);

   // Returns true if the given type is a named or numbered script type.
   static bool is_bt_script(BasicType type) {return type == BT_FUN_SNA || type == BT_FUN_SNU;}

   // Returns true if the given type is an unsigned type.
   static bool is_bt_unsigned(BasicType type);

private:
   explicit VariableType();
   explicit VariableType(VariableType &type);
   explicit VariableType(BasicType basic);
   ~VariableType();

   VecStr names;
   Vector types;

   VariableType *next;
   VariableType *prev;

   VariableType *specnext;
   VariableType *specprev;

   VariableType *typeArr;
   VariableType *typePtr;
   Reference     typeRet;
   Pointer       typeUnq;

   std::string name;
   std::string storeArea;

   BasicType basic;
   unsigned  quals;
   StoreType store;
   bigsint   width;

   bool complete : 1;


   static Reference get_bt_anonymous
   (Vector types, VariableType *typeRet, VariableType *head, BasicType basic);

   static Reference get_bt_complex(Pointer *types, BasicType bt, BasicType clx);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

std::string make_string(VariableType const *type);
std::string const &make_string(VariableType::BasicType basic);

#endif//HPP_VariableType_

