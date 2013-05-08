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
// Type serialization.
//
//-----------------------------------------------------------------------------

#include "Type.hpp"

#include "Class.hpp"
#include "Enum.hpp"
#include "Function.hpp"

#include "ObjectArchive.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type::saveObject
//
ObjectSave &Type::saveObject(ObjectSave &save) const
{
   return save << quals;
}

//
// Type::Load
//
ObjectLoad &Type::Load(ObjectLoad &load)
{
   Clas::Load(load);
   Enum::Load(load);

   return load;
}

//
// Type::LoadFinish
//
void Type::LoadFinish()
{
   Clas::LoadFinish();
   Enum::LoadFinish();
}

//
// Type::LoadType
//
auto Type::LoadType(ObjectLoad &load) -> TypeCR
{
   bigsint bits, offs;
   ClasCP clas;
   ParmCP parm;
   CallCon conv;

   TypeCR type = Void;

   Keyword kwrd;
   load >> kwrd;
   switch(static_cast<KeywordIndex>(kwrd))
   {
      #define BasicTypeCase(K) case KWRD_##K: type = K; break

      BasicTypeCase(Accum);
      BasicTypeCase(BoolC);
      BasicTypeCase(BoolCXX);
      BasicTypeCase(Char);
      BasicTypeCase(Char16);
      BasicTypeCase(Char32);
      BasicTypeCase(FastBool);
      BasicTypeCase(Float);
      BasicTypeCase(FloatComplex);
      BasicTypeCase(FloatImaginary);
      BasicTypeCase(Fract);
      BasicTypeCase(Int);
      BasicTypeCase(Label);
      BasicTypeCase(LongAccum);
      BasicTypeCase(LongFloat);
      BasicTypeCase(LongFloatComplex);
      BasicTypeCase(LongFloatImaginary);
      BasicTypeCase(LongFract);
      BasicTypeCase(LongInt);
      BasicTypeCase(LongLongAccum);
      BasicTypeCase(LongLongFloat);
      BasicTypeCase(LongLongFloatComplex);
      BasicTypeCase(LongLongFloatImaginary);
      BasicTypeCase(LongLongFract);
      BasicTypeCase(LongLongInt);
      BasicTypeCase(Nullptr);
      BasicTypeCase(SatAccum);
      BasicTypeCase(SatFract);
      BasicTypeCase(SatLongAccum);
      BasicTypeCase(SatLongFract);
      BasicTypeCase(SatLongLongAccum);
      BasicTypeCase(SatLongLongFract);
      BasicTypeCase(SatShortAccum);
      BasicTypeCase(SatShortFract);
      BasicTypeCase(SatUnsignedAccum);
      BasicTypeCase(SatUnsignedFract);
      BasicTypeCase(SatUnsignedLongAccum);
      BasicTypeCase(SatUnsignedLongFract);
      BasicTypeCase(SatUnsignedLongLongAccum);
      BasicTypeCase(SatUnsignedLongLongFract);
      BasicTypeCase(SatUnsignedShortAccum);
      BasicTypeCase(SatUnsignedShortFract);
      BasicTypeCase(ShortAccum);
      BasicTypeCase(ShortFloat);
      BasicTypeCase(ShortFloatComplex);
      BasicTypeCase(ShortFloatImaginary);
      BasicTypeCase(ShortFract);
      BasicTypeCase(ShortInt);
      BasicTypeCase(SignedChar);
      BasicTypeCase(String);
      BasicTypeCase(UnsignedAccum);
      BasicTypeCase(UnsignedChar);
      BasicTypeCase(UnsignedFract);
      BasicTypeCase(UnsignedInt);
      BasicTypeCase(UnsignedLongAccum);
      BasicTypeCase(UnsignedLongFract);
      BasicTypeCase(UnsignedLongInt);
      BasicTypeCase(UnsignedLongLongAccum);
      BasicTypeCase(UnsignedLongLongFract);
      BasicTypeCase(UnsignedLongLongInt);
      BasicTypeCase(UnsignedShortAccum);
      BasicTypeCase(UnsignedShortFract);
      BasicTypeCase(UnsignedShortInt);
      BasicTypeCase(Void);
      BasicTypeCase(WChar);

      #undef BasicTypeCase

   case KWRD_array0:   type = LoadType(load)->getArrayType();           break;
   case KWRD_arrayVLA: type = LoadType(load)->getArrayType(nullptr);    break;
   case KWRD_class:    type = Clas::Get(load.loadBits<ContextKey>());   break;
   case KWRD_enum:     type = Enum::Get(load.loadBits<ContextKey>());   break;
   case KWRD_lvref:    type = LoadType(load)->getLValueReferenceType(); break;
   case KWRD_pointer:  type = LoadType(load)->getPointerType();         break;
   case KWRD_rvref:    type = LoadType(load)->getRValueReferenceType(); break;

   case KWRD_array:
      load >> bits;
      type = LoadType(load);
      type = type->getArrayType(bits);
      break;

   case KWRD_bitfield:
      load >> bits >> offs;
      type = LoadType(load);
      type = type->getBitfieldType(bits, offs);
      break;

   case KWRD_function:
      type = LoadType(load);
      load >> parm >> conv;
      type = type->getFunctionType(parm, conv);
      break;

   case KWRD_memfunc:
      clas = Clas::LoadType(load);
      type = LoadType(load);
      load >> parm >> conv;
      type = clas->getMemberFunctionType(type, parm, conv);
      break;

   case KWRD_memptr:
      clas = Clas::LoadType(load);
      type = LoadType(load);
      type = clas->getMemberPointerType(type);
      break;

   default:
      Error_p("internal error: not type keyword");
   }

   return type->getQualType(load.loadBits<Qual>());
}

//
// Type::Save
//
ObjectSave &Type::Save(ObjectSave &save)
{
   Clas::Save(save);
   Enum::Save(save);

   return save;
}

//
// operator ObjectSave << AddressSpace
//
ObjectSave &operator << (ObjectSave &save, AddressSpace const &data)
{
   return save << data.base << data.name;
}

//
// operator ObjectSave << AddressSpaceBase
//
ObjectSave &operator << (ObjectSave &save, AddressSpaceBase const &data)
{
   return save.saveEnum(data);
}

//
// operator ObjectSave << CallingConvention
//
ObjectSave &operator << (ObjectSave &save, CallingConvention const &data)
{
   return save.saveEnum(data);
}

//
// operator ObjectSave << Type::TypeCP
//
ObjectSave &operator << (ObjectSave &save, Type::TypeCP const &data)
{
   if(data)
      return data->saveObject(save);
   else
      return save << KWRD;
}

//
// operator ObjectSave << Type::TypeCR
//
ObjectSave &operator << (ObjectSave &save, Type::TypeCR const &data)
{
   return data->saveObject(save);
}

//
// operator ObjectSave << TypeQual
//
ObjectSave &operator << (ObjectSave &save, TypeQual const &data)
{
   return save << data.addressSpace << data.accessAtomic << data.accessConst
      << data.accessRestrict << data.accessVolatile;
}

//
// operator ObjectLoad >> AddressSpace
//
ObjectLoad &operator >> (ObjectLoad &load, AddressSpace &data)
{
   return load >> data.base >> data.name;
}

//
// operator ObjectLoad >> AddressSpaceBase
//
ObjectLoad &operator >> (ObjectLoad &load, AddressSpaceBase &data)
{
   return load.loadEnum(data, ADDR_WorldArray, ADDR_Generic);
}

//
// operator ObjectLoad >> CallingConvention
//
ObjectLoad &operator >> (ObjectLoad &load, CallingConvention &data)
{
   return load.loadEnum(data, CALL_Special);
}

//
// operator ObjectLoad >> Type::TypeCP
//
ObjectLoad &operator >> (ObjectLoad &load, Type::TypeCP &data)
{
   try
   {
      data = Type::LoadType(load);
   }
   catch(...)
   {
      data = nullptr;
   }

   return load;
}

//
// operator ObjectLoad >> Type::TypeCR
//
ObjectLoad &operator >> (ObjectLoad &load, Type::TypeCR &data)
{
   data = Type::LoadType(load);
   return load;
}

//
// operator ObjectLoad >> TypeQual
//
ObjectLoad &operator >> (ObjectLoad &load, TypeQual &data)
{
   load >> data.addressSpace;
   data.accessAtomic   = load.loadBits(data.accessAtomic);
   data.accessConst    = load.loadBits(data.accessConst);
   data.accessRestrict = load.loadBits(data.accessRestrict);
   data.accessVolatile = load.loadBits(data.accessVolatile);
   return load;
}

// EOF

