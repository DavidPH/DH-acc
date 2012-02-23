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
class VariableType : Counter
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(VariableType, Counter);

public:
   typedef std::vector<std::string> VecStr;
   typedef std::vector<Pointer> Vector;

   //
   // BasicType
   //
   enum BasicType
   {
      // Primitive types.
      BT_BOOLHARD,
      BT_BOOLSOFT,
      BT_CHAR,
      BT_INT,
      BT_LABEL,
      BT_REAL,
      BT_STRING,
      BT_UINT,

      BT_VOID,

      // Reference types.
      BT_ARRAY,
      BT_POINTER,

      // Named types.
      BT_ENUM,
      BT_STRUCT,
      BT_UNION,

      // Anonymous types.
      BT_ASMFUNC,
      BT_BLOCK,
      BT_FUNCTION,
      BT_LINESPEC,
      BT_NATIVE,
      BT_SCRIPT
   };

   //
   // Qualifier
   //
   enum Qualifier
   {
      QUAL_CONST    = 1,
      QUAL_VOLATILE = 2,
      QUAL_RESTRICT = 4
   };

   //
   // StoreType
   //
   // This is loosely related to SourceVariable::StorageClass.
   // The low bits are used to store the array number for ARRAY types.
   //
   enum StoreType
   {
      ST_ADDR,
      ST_REGISTER,
      ST_MAPREGISTER,
      ST_WORLDREGISTER,
      ST_GLOBALREGISTER,
      ST_MAPARRAY,
      ST_WORLDARRAY,
      ST_GLOBALARRAY
   };


   // Type creation.
   Reference addQualifier(unsigned _quals) {return setQualifier(quals|_quals);}
   Reference getArray(bigsint width);
   Reference getPointer();
   Reference getReturn() const {return typeRet;}
   Reference getUnqualified() {return Reference(typeUnq ? typeUnq.raw() : this);}
   Reference setQualifier(unsigned quals);
   Reference setStorage(StoreType store);
   Reference setStorage(StoreType store, std::string const &storeArea);

   // Type information.
   BasicType getBasicType() const {return basic;}
   bool getComplete() const {return complete;}
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
   static Reference get_bt_boolhard();
   static Reference get_bt_boolsoft();
   static Reference get_bt_char();
   static Reference get_bt_int();
   static Reference get_bt_label();
   static Reference get_bt_real();
   static Reference get_bt_string();
   static Reference get_bt_uint();
   static Reference get_bt_void();

   // Named types.
   static Reference get_bt_enum();
   static Reference get_bt_struct();
   static Reference get_bt_union();

   // Anonymous types.
   static Reference get_bt_asmfunc(Vector const &types, VariableType *typeRet);
   static Reference get_bt_block(Vector const &types);
   static Reference get_bt_function(Vector const &types, VariableType *typeRet);
   static Reference get_bt_linespec(Vector const &types, VariableType *typeRet);
   static Reference get_bt_native(Vector const &types, VariableType *typeRet);
   static Reference get_bt_script(Vector const &types, VariableType *typeRet);

   // Returns true if the given type is an arithmetic type.
   static bool is_bt_arithmetic(BasicType type);

   // Returns true if the given type is an integer type.
   static bool is_bt_integer(BasicType type);

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

   std::string storeArea;

   BasicType basic;
   unsigned  quals;
   StoreType store;
   bigsint   width;

   bool complete : 1;


   static Reference get_bt_anonymous
   (Vector types, VariableType *typeRet, VariableType *head, BasicType basic);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

std::string const &make_string(VariableType::BasicType basic);

#endif//HPP_VariableType_

