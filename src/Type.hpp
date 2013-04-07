//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Source- and Object-level type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type_H__
#define Type_H__

#include "bignum.hpp"
#include "Counter.hpp"
#include "Keyword.hpp"

#include <ostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ParameterSet;
class SourceExpression;
class Type_Array;
class Type_Array0;
class Type_ArrayVLA;
class Type_ArrayVLA0;
class Type_Bitfield;
class Type_Function;
class Type_MemberFunction;
class Type_StaticFunction;
class Value;

typedef ContextKey AddressSpaceName;

//
// AddressSpaceBase
//
enum AddressSpaceBase
{
   // Generic address space.
   ADDR_Generic,

   // Special address spaces.
   ADDR_Copy,    // __address_space_copy
   ADDR_Literal,
   ADDR_Stack,
   ADDR_Void,

   // Intrinsic address spaces.
   ADDR_Far,            // __far
   ADDR_GlobalArrays,   // __global_arrays
   ADDR_GlobalRegister, // __global_register
   ADDR_Local,          // __local
   ADDR_LocalRegister,  // __local_register
   ADDR_MapArrays,      // __map_arrays
   ADDR_MapRegister,    // __map_register
   ADDR_Near,           // __near
   ADDR_StringArrays,   // __string_arrays
   ADDR_Varg,           // __va_addr
   ADDR_WorldArrays,    // __world_arrays
   ADDR_WorldRegister,  // __world_register

   // Declarable address spaces.
   ADDR_GlobalArray, // __global_array
   ADDR_MapArray,    // __map_array
   ADDR_StringArray, // __string_array
   ADDR_WorldArray,  // __world_array
};

//
// CallingConvention
//
enum CallingConvention
{
   CALL_Action,  // __action
   CALL_AsmFunc, // __asmfunc
   CALL_LangACS, // extern "ACS"
   CALL_LangASM, // extern "asm"
   CALL_LangAXX, // extern "ACS++"
   CALL_LangC,   // extern "C"
   CALL_LangCXX, // extern "C++"
   CALL_LangDS,  // extern "DS"
   CALL_Native,  // __native
   CALL_Script,  // __script
   CALL_ScriptI, // __scripti
   CALL_ScriptS, // __scripts
   CALL_Special, // __special
};

//
// NameMangleStyle
//
enum NameMangleStyle
{
   MANGLE_Default,
};

//
// AddressSpace
//
struct AddressSpace
{
   constexpr AddressSpace() : base{ADDR_Generic} {}
   constexpr AddressSpace(AddressSpaceBase base_, AddressSpaceName name_) :
      base{base_}, name{name_} {}

   void getNameMangle(std::ostream &out, NameMangleStyle mangle) const;

   AddressSpaceBase base;
   AddressSpaceName name;
};

//
// TypeQual
//
struct TypeQual
{
   constexpr TypeQual() : addressSpace{}, accessAtomic{false}, accessConst{false},
      accessRestrict{false}, accessVolatile{false} {}
   constexpr TypeQual(AddressSpaceName name, AddressSpaceBase base) :
      addressSpace{base, name}, accessAtomic{false}, accessConst{false},
      accessRestrict{false}, accessVolatile{false} {}
   constexpr TypeQual(bool a, bool c, bool r, bool v) : addressSpace{},
      accessAtomic{a}, accessConst{c}, accessRestrict{r}, accessVolatile{v} {}

   void getNameMangle(std::ostream &out, NameMangleStyle mangle) const;

   AddressSpace addressSpace;

   bool accessAtomic   : 1;
   bool accessConst    : 1;
   bool accessRestrict : 1;
   bool accessVolatile : 1;
};

//
// Type
//
class Type : public Counter
{
   CounterPreambleAbstract(Type, Counter);

protected:
   typedef AddressSpace Addr;

   typedef CallingConvention CallCon;

   typedef Type_MemberFunction MFun;
   typedef CounterReference<MFun const> MFunCR;

   typedef ParameterSet Parm;
   typedef CounterReference<Parm const> ParmCR;

   typedef TypeQual Qual;

   typedef Type_StaticFunction SFun;
   typedef CounterReference<SFun const> SFunCR;

   typedef SourceExpression         SrcExp;
   typedef CounterPointer<SrcExp>   SrcExpP;
   typedef CounterReference<SrcExp> SrcExpR;

   typedef ConstPointer   TypeCP;
   typedef ConstReference TypeCR;

   typedef CounterReference<Value> ValueR;

public:
   // Type creation.
   TypeCR getArrayType() const;             // Type/Array.cpp
   TypeCR getArrayType(bigsint size) const; // Type/Array.cpp
   TypeCR getArrayType(SrcExp *size) const; // Type/Array.cpp
   TypeCR getBitfieldType(bigsint bits,     // Type/Bitfield.cpp
      bigsint offs) const;
   TypeCR getFunctionType(Parm const *parm, // Type/Function.cpp
      CallCon conv) const;
   TypeCR getLValueReferenceType() const;   // Type/Pointer.cpp
   TypeCR getPointerType() const;           // Type/Pointer.cpp
   TypeCR getQualType() const {return qualNone;}
   TypeCR getQualType(TypeQual quals) const;
   TypeCR getRValueReferenceType() const;   // Type/Pointer.cpp

   // Type information.
   virtual bigsint getAlignment() const = 0;
   virtual TypeCR  getBase() const;
   virtual CallCon getCallConv() const;
   virtual Keyword getName() const;
           void    getNameMangle(std::ostream &out, NameMangleStyle mangle) const;
           Qual    getQual() const {return quals;}
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const = 0;
   virtual SrcExpR getSizeBytesVM() const;
   virtual bigsint getSizePtr() const = 0;
   virtual SrcExpR getSizePtrVM() const;
   virtual bigsint getSizeWords() const = 0;
   virtual SrcExpR getSizeWordsVM() const;

           bool isAtomic() const     {return quals.accessAtomic;}
   virtual bool isBoolean() const    {return false;}
   virtual bool isComplete() const   {return false;}
   virtual bool isComplex() const    {return false;}
           bool isConst() const      {return quals.accessConst;}
   virtual bool isImaginary() const  {return false;}
           bool isQual() const       {return this != qualNone;}
           bool isRestrict() const   {return quals.accessRestrict;}
   virtual bool isSaturating() const {return false;}
   virtual bool isSigned() const     {return false;}
   virtual bool isSizeVM() const     {return false;}
   virtual bool isVM() const         {return false;}
           bool isVolatile() const   {return quals.accessVolatile;}

   // Type classification. (See Type.cpp for chart.)
   virtual bool isAccumType() const                        {return false;}
   virtual bool isAggregateType() const                    {return false;}
   virtual bool isArithmeticType() const                   {return false;}
   virtual bool isArrayType() const                        {return false;}
   virtual bool isBitfieldType() const                     {return false;}
   virtual bool isCharacterType() const                    {return false;}
   virtual bool isComplexType() const                      {return false;}
   virtual bool isEnumeratedType() const                   {return false;}
   virtual bool isFixedPointType() const                   {return false;}
   virtual bool isFloatingType() const                     {return false;}
   virtual bool isFractType() const                        {return false;}
   virtual bool isFunctionType() const                     {return false;}
   virtual bool isImaginaryType() const                    {return false;}
   virtual bool isIntegerType() const                      {return false;}
   virtual bool isLabel() const                            {return false;}
   virtual bool isLValueReferenceType() const              {return false;}
   virtual bool isMemberFunctionType() const               {return false;}
   virtual bool isMemberPointerType() const                {return false;}
   virtual bool isNullptr() const                          {return false;}
   virtual bool isObjectType() const                       {return false;}
   virtual bool isPointerType() const                      {return false;}
   virtual bool isPrimaryFixedPointType() const            {return false;}
   virtual bool isPrimarySignedFixedPointType() const      {return false;}
   virtual bool isPrimaryUnsignedFixedPointType() const    {return false;}
   virtual bool isRealType() const                         {return false;}
   virtual bool isRealFloatingType() const                 {return false;}
   virtual bool isReferenceType() const                    {return false;}
   virtual bool isRValueReferenceType() const              {return false;}
   virtual bool isSaturatingFixedPointType() const         {return false;}
   virtual bool isSaturatingSignedFixedPointType() const   {return false;}
   virtual bool isSaturatingUnsignedFixedPointType() const {return false;}
   virtual bool isScalarType() const                       {return false;}
   virtual bool isScopedEnumerationType() const            {return false;}
   virtual bool isSignedAccumType() const                  {return false;}
   virtual bool isSignedFractType() const                  {return false;}
   virtual bool isSignedIntegerType() const                {return false;}
   virtual bool isStaticFunctionType() const               {return false;}
   virtual bool isString() const                           {return false;}
   virtual bool isStructureType() const                    {return false;}
   virtual bool isUnionType() const                        {return false;}
   virtual bool isUnscopedEnumerationType() const          {return false;}
   virtual bool isUnsignedAccumType() const                {return false;}
   virtual bool isUnsignedFractType() const                {return false;}
   virtual bool isUnsignedIntegerType() const              {return false;}
   virtual bool isVoid() const                             {return false;}


   friend class Type_Array;
   friend class Type_Array0;
   friend class Type_ArrayVLA;
   friend class Type_ArrayVLA0;
   friend class Type_Bitfield;
   friend class Type_LValueReference;
   friend class Type_Pointer;
   friend class Type_RValueReference;
   friend class Type_StaticFunction;

   static bigsint GetAlignmentHWord();
   static bigsint GetAlignmentWord();
   static void    GetNameMangle(CallCon conv, std::ostream &out, NameMangleStyle mangle);
   static void    GetNameMangle(ContextKey ctxk, std::ostream &out, NameMangleStyle mangle);
   static void    GetNameMangle(bigsint numb, std::ostream &out, NameMangleStyle mangle);
   static void    GetNameMangle(Type const *type, std::ostream &out, NameMangleStyle mangle);
   static bigsint GetSizeByte();
   static bigsint GetSizeHWord();
   static bigsint GetSizeWord();

   static TypeCR const Accum;
   static TypeCR const BoolC;
   static TypeCR const BoolCXX;
   static TypeCR const Char;
   static TypeCR const Char16;
   static TypeCR const Char32;
   static TypeCR const FastBool;
   static TypeCR const Float;
   static TypeCR const FloatComplex;
   static TypeCR const FloatImaginary;
   static TypeCR const Fract;
   static TypeCR const Int;
   static TypeCR const Label;
   static TypeCR const LongAccum;
   static TypeCR const LongFloat;
   static TypeCR const LongFloatComplex;
   static TypeCR const LongFloatImaginary;
   static TypeCR const LongFract;
   static TypeCR const LongInt;
   static TypeCR const LongLongAccum;
   static TypeCR const LongLongFloat;
   static TypeCR const LongLongFloatComplex;
   static TypeCR const LongLongFloatImaginary;
   static TypeCR const LongLongFract;
   static TypeCR const LongLongInt;
   static TypeCR const Nullptr;
   static TypeCR const SatAccum;
   static TypeCR const SatFract;
   static TypeCR const SatLongAccum;
   static TypeCR const SatLongFract;
   static TypeCR const SatLongLongAccum;
   static TypeCR const SatLongLongFract;
   static TypeCR const SatShortAccum;
   static TypeCR const SatShortFract;
   static TypeCR const SatUnsignedAccum;
   static TypeCR const SatUnsignedFract;
   static TypeCR const SatUnsignedLongAccum;
   static TypeCR const SatUnsignedLongFract;
   static TypeCR const SatUnsignedLongLongAccum;
   static TypeCR const SatUnsignedLongLongFract;
   static TypeCR const SatUnsignedShortAccum;
   static TypeCR const SatUnsignedShortFract;
   static TypeCR const ShortAccum;
   static TypeCR const ShortFloat;
   static TypeCR const ShortFloatComplex;
   static TypeCR const ShortFloatImaginary;
   static TypeCR const ShortFract;
   static TypeCR const ShortInt;
   static TypeCR const SignedChar;
   static TypeCR const String;
   static TypeCR const UnsignedAccum;
   static TypeCR const UnsignedChar;
   static TypeCR const UnsignedFract;
   static TypeCR const UnsignedInt;
   static TypeCR const UnsignedLongAccum;
   static TypeCR const UnsignedLongFract;
   static TypeCR const UnsignedLongInt;
   static TypeCR const UnsignedLongLongAccum;
   static TypeCR const UnsignedLongLongFract;
   static TypeCR const UnsignedLongLongInt;
   static TypeCR const UnsignedShortAccum;
   static TypeCR const UnsignedShortFract;
   static TypeCR const UnsignedShortInt;
   static TypeCR const Void;
   static TypeCR const WChar;

protected:
   Type();
   // Derived types must implement copy construction to be suitable for the
   // creation of qualified types.
   Type(Type const &type);
   virtual ~Type();

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const = 0;

   // Qualified types.
   Qual quals;
   TypeCR const qualNone;
   mutable Type *qualNext, *qualPrev;

   // Array types. (Type/Array.cpp)
   mutable Type_Array const    *arrType;
   mutable Type const          *arrType0;
   mutable Type_ArrayVLA const *arrTypeVLA;
   mutable Type const          *arrTypeVLA0;

   // Bitfield types. (Type/Bitfield.cpp)
   mutable Type_Bitfield const *bitType;

   // Function types. (Type/Function.cpp)
   mutable SFun const *sfnType;

   // Pointer types. (Type/Pointer.cpp)
   mutable Type const *lvrType; // lvalue reference
   mutable Type const *ptrType; // pointer
   mutable Type const *rvrType; // rvalue reference


   static void GetNameMangleCTXK(ContextKey ctxk, std::ostream &out, NameMangleStyle mangle);
};

//
// Value
//
// Stores an lvalue or rvalue. (Or xvalue or prvalue or glvalue, I suppose.)
//
class Value : public PlainCounter
{
   CounterPreambleNoVirtual(Value, PlainCounter);

protected:
   typedef SourceExpression         SrcExp;
   typedef CounterPointer<SrcExp>   SrcExpP;
   typedef CounterReference<SrcExp> SrcExpR;

   typedef Type::ConstReference TypeCR;

public:
   bool isLValue() const;
   bool isModifiableLValue() const;
   bool isRValue() const;

   TypeCR  const type;
   SrcExpR const addr;


   static Reference Create(Type const *type, SrcExp *addr);

private:
   Value(Type const *type, SrcExp *addr);
   ~Value();
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

constexpr TypeQual QUAL_None    {false, false, false, false};
constexpr TypeQual QUAL_Atomic  {true,  false, false, false};
constexpr TypeQual QUAL_Const   {false, true,  false, false};
constexpr TypeQual QUAL_Restrict{false, false, true,  false};
constexpr TypeQual QUAL_Volatile{false, false, false, true };


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

constexpr bool operator == (AddressSpace const &l, AddressSpace const &r);
constexpr bool operator == (TypeQual const &l, TypeQual const &r);

//
// operator AddressSpace == AddressSpace
//
constexpr bool operator == (AddressSpace const &l, AddressSpace const &r)
{
   return l.base == r.base && l.name == r.name;
}

//
// operator TypeQual == TypeQual
//
constexpr bool operator == (TypeQual const &l, TypeQual const &r)
{
   return l.addressSpace == r.addressSpace && l.accessConst == r.accessConst &&
      l.accessRestrict == r.accessRestrict && l.accessVolatile == r.accessVolatile;
}

#endif//Type_H__

