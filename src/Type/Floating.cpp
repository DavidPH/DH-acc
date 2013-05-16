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
// Floating type handling.
//
//-----------------------------------------------------------------------------

#include "Floating.hpp"

#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

//
// RealTable
//
// Used to determine the real type for [size].
//
static constexpr Type_Floating const *RealTable[4] =
{
   &Type_Floating::ShortFloatObj,
   &Type_Floating::FloatObj,
   &Type_Floating::LongFloatObj,
   &Type_Floating::LongLongFloatObj,
};

static constexpr int SizeBitsFTable[4] = { 5,  8, 11, 11};
static constexpr int SizeBitsITable[4] = {10, 23, 52, 52};

static constexpr int SizeBytesTable_ACS[2][4] = {{1, 1, 2, 2}, {1, 2, 4, 4}};
static constexpr int SizePtrTable_ACS[2][4] = {{1, 1, 2, 2}, {1, 2, 4, 4}};
static constexpr int SizeWordsTable_ACS[2][4] = {{1, 1, 2, 2}, {1, 2, 4, 4}};

static constexpr int SizeBytesTable_MageCraft[2][4] = {{2, 4, 8, 8}, {4, 8, 16, 16}};
static constexpr int SizePtrTable_MageCraft[2][4] = {{2, 1, 2, 2}, {1, 2, 4, 4}};
static constexpr int SizeWordsTable_MageCraft[2][4] = {{1, 1, 2, 2}, {1, 2, 4, 4}};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Floating const Type_Floating::ShortFloatComplexObj   {0, true, false};
Type_Floating const Type_Floating::FloatComplexObj        {1, true, false};
Type_Floating const Type_Floating::LongFloatComplexObj    {2, true, false};
Type_Floating const Type_Floating::LongLongFloatComplexObj{3, true, false};

Type_Floating const Type_Floating::ShortFloatImaginaryObj   {0, false, true};
Type_Floating const Type_Floating::FloatImaginaryObj        {1, false, true};
Type_Floating const Type_Floating::LongFloatImaginaryObj    {2, false, true};
Type_Floating const Type_Floating::LongLongFloatImaginaryObj{3, false, true};

Type_Floating const Type_Floating::ShortFloatObj   {0, false, false};
Type_Floating const Type_Floating::FloatObj        {1, false, false};
Type_Floating const Type_Floating::LongFloatObj    {2, false, false};
Type_Floating const Type_Floating::LongLongFloatObj{3, false, false};

Type::TypeCR const Type::ShortFloatComplex   {&Type_Floating::ShortFloatComplexObj};
Type::TypeCR const Type::FloatComplex        {&Type_Floating::FloatComplexObj};
Type::TypeCR const Type::LongFloatComplex    {&Type_Floating::LongFloatComplexObj};
Type::TypeCR const Type::LongLongFloatComplex{&Type_Floating::LongLongFloatComplexObj};

Type::TypeCR const Type::ShortFloatImaginary   {&Type_Floating::ShortFloatImaginaryObj};
Type::TypeCR const Type::FloatImaginary        {&Type_Floating::FloatImaginaryObj};
Type::TypeCR const Type::LongFloatImaginary    {&Type_Floating::LongFloatImaginaryObj};
Type::TypeCR const Type::LongLongFloatImaginary{&Type_Floating::LongLongFloatImaginaryObj};

Type::TypeCR const Type::ShortFloat   {&Type_Floating::ShortFloatObj};
Type::TypeCR const Type::Float        {&Type_Floating::FloatObj};
Type::TypeCR const Type::LongFloat    {&Type_Floating::LongFloatObj};
Type::TypeCR const Type::LongLongFloat{&Type_Floating::LongLongFloatObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Floating::Type_Floating
//
Type_Floating::Type_Floating(unsigned size_, bool comp_, bool imag_) :
   size{size_}, comp{comp_}, imag{imag_}, real{!comp_ && !imag_}
{
   ++refCount;
}

//
// Type_Floating::getAlignment
//
bigsint Type_Floating::getAlignment() const
{
   if(Target == TARGET_MageCraft) return size ? 4 : 2;

   return 1;
}

//
// Type_Floating::getBase
//
auto Type_Floating::getBase() const -> TypeCR
{
   if(real) Error_Np("internal error: real");

   return static_cast<TypeCR>(RealTable[size]);
}

//
// Type_Floating::getNameMangleBase
//
void Type_Floating::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BF";

   if(comp) out << 'C';
   if(imag) out << 'I';

   if(size != 1) out << "H DQ"[size];
}

//
// Type_Floating::getSizeBitsF
//
bigsint Type_Floating::getSizeBitsF() const
{
   return SizeBitsFTable[size];
}

//
// Type_Floating::getSizeBitsI
//
bigsint Type_Floating::getSizeBitsI() const
{
   return SizeBitsITable[size];
}

//
// Type_Floating::getSizeBytes
//
bigsint Type_Floating::getSizeBytes() const
{
   if(Target == TARGET_MageCraft) return SizeBytesTable_MageCraft[comp][size];

   return SizeBytesTable_ACS[comp][size];
}

//
// Type_Floating::getSizePtr
//
bigsint Type_Floating::getSizePtr() const
{
   if(Target == TARGET_MageCraft) return SizePtrTable_MageCraft[comp][size];

   return SizePtrTable_ACS[comp][size];
}

//
// Type_Floating::getSizeWords
//
bigsint Type_Floating::getSizeWords() const
{
   if(Target == TARGET_MageCraft) return SizeWordsTable_MageCraft[comp][size];

   return SizeWordsTable_ACS[comp][size];
}

//
// Type_Floating::saveObject
//
ObjectSave &Type_Floating::saveObject(ObjectSave &save) const
{
   if(comp) switch(size)
   {
   case 0: save << KWRD_ShortFloatComplex;    break;
   case 1: save << KWRD_FloatComplex;         break;
   case 2: save << KWRD_LongFloatComplex;     break;
   case 3: save << KWRD_LongLongFloatComplex; break;
   }
   else if(imag) switch(size)
   {
   case 0: save << KWRD_ShortFloatImaginary;    break;
   case 1: save << KWRD_FloatImaginary;         break;
   case 2: save << KWRD_LongFloatImaginary;     break;
   case 3: save << KWRD_LongLongFloatImaginary; break;
   }
   else switch(size)
   {
   case 0: save << KWRD_ShortFloat;    break;
   case 1: save << KWRD_Float;         break;
   case 2: save << KWRD_LongFloat;     break;
   case 3: save << KWRD_LongLongFloat; break;
   }

   return Super::saveObject(save);
}

// EOF

