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
// Fixed-point type handling.
//
//-----------------------------------------------------------------------------

#include "Fixed.hpp"

#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

//
// PrimaryTable
//
// Used to determine the primary type for [sign][frac][size].
//
static constexpr Type_Fixed const *PrimaryTable[2][2][4] =
{
   {
      {
         &Type_Fixed::UnsignedShortAccumObj,
         &Type_Fixed::UnsignedAccumObj,
         &Type_Fixed::UnsignedLongAccumObj,
         &Type_Fixed::UnsignedLongLongAccumObj,
      },
      {
         &Type_Fixed::UnsignedShortFractObj,
         &Type_Fixed::UnsignedFractObj,
         &Type_Fixed::UnsignedLongFractObj,
         &Type_Fixed::UnsignedLongLongFractObj,
      },
   },
   {
      {
         &Type_Fixed::ShortAccumObj,
         &Type_Fixed::AccumObj,
         &Type_Fixed::LongAccumObj,
         &Type_Fixed::LongLongAccumObj,
      },
      {
         &Type_Fixed::ShortFractObj,
         &Type_Fixed::FractObj,
         &Type_Fixed::LongFractObj,
         &Type_Fixed::LongLongFractObj,
      },
   },
};

//
// SaveObjectTable
//
// Converts a [satu][sign][frac][size] to a save KWRD.
//
static constexpr KeywordIndex SaveObjectTable[2][2][2][4] =
{
   {
      {
         {KWRD_ShortFract, KWRD_Fract, KWRD_LongFract, KWRD_LongLongFract},
         {KWRD_ShortAccum, KWRD_Accum, KWRD_LongAccum, KWRD_LongLongAccum},
      },
      {
         {KWRD_UnsignedShortFract, KWRD_UnsignedFract,
          KWRD_UnsignedLongFract,  KWRD_UnsignedLongLongFract},
         {KWRD_UnsignedShortAccum, KWRD_UnsignedAccum,
          KWRD_UnsignedLongAccum,  KWRD_UnsignedLongLongAccum},
      },
   },
   {
      {
         {KWRD_SatShortFract, KWRD_SatFract, KWRD_SatLongFract, KWRD_SatLongLongFract},
         {KWRD_SatShortAccum, KWRD_SatAccum, KWRD_SatLongAccum, KWRD_SatLongLongAccum},
      },
      {
         {KWRD_SatUnsignedShortFract, KWRD_SatUnsignedFract,
          KWRD_SatUnsignedLongFract,  KWRD_SatUnsignedLongLongFract},
         {KWRD_SatUnsignedShortAccum, KWRD_SatUnsignedAccum,
          KWRD_SatUnsignedLongAccum,  KWRD_SatUnsignedLongLongAccum},
      },
   },
};

static constexpr int SizeBitsTable_ACS[2][4] = {{16, 32, 64, 64}, {8, 16, 32, 32}};
static constexpr int SizeBytesTable_ACS[2][4] = {{1, 1, 2, 2}, {1, 1, 1, 1}};
static constexpr int SizePtrTable_ACS[2][4] = {{1, 1, 2, 2}, {1, 1, 1, 1}};
static constexpr int SizeWordsTable_ACS[2][4] = {{1, 1, 2, 2}, {1, 1, 1, 1}};

static constexpr int SizeBitsTable_MageCraft[2][4] = {{16, 32, 64, 64}, {8, 16, 32, 32}};
static constexpr int SizeBytesTable_MageCraft[2][4] = {{2, 4, 8, 8}, {1, 2, 4, 4}};
static constexpr int SizePtrTable_MageCraft[2][4] = {{2, 1, 2, 2}, {1, 2, 1, 1}};
static constexpr int SizeWordsTable_MageCraft[2][4] = {{1, 1, 2, 2}, {1, 1, 1, 1}};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Fixed const Type_Fixed::ShortFractObj   {0, false, true, true};
Type_Fixed const Type_Fixed::FractObj        {1, false, true, true};
Type_Fixed const Type_Fixed::LongFractObj    {2, false, true, true};
Type_Fixed const Type_Fixed::LongLongFractObj{3, false, true, true};
Type_Fixed const Type_Fixed::ShortAccumObj   {0, false, true, false};
Type_Fixed const Type_Fixed::AccumObj        {1, false, true, false};
Type_Fixed const Type_Fixed::LongAccumObj    {2, false, true, false};
Type_Fixed const Type_Fixed::LongLongAccumObj{3, false, true, false};

Type_Fixed const Type_Fixed::UnsignedShortFractObj   {0, false, false, true};
Type_Fixed const Type_Fixed::UnsignedFractObj        {1, false, false, true};
Type_Fixed const Type_Fixed::UnsignedLongFractObj    {2, false, false, true};
Type_Fixed const Type_Fixed::UnsignedLongLongFractObj{3, false, false, true};
Type_Fixed const Type_Fixed::UnsignedShortAccumObj   {0, false, false, false};
Type_Fixed const Type_Fixed::UnsignedAccumObj        {1, false, false, false};
Type_Fixed const Type_Fixed::UnsignedLongAccumObj    {2, false, false, false};
Type_Fixed const Type_Fixed::UnsignedLongLongAccumObj{3, false, false, false};

Type_Fixed const Type_Fixed::SatShortFractObj   {0, true, true, true};
Type_Fixed const Type_Fixed::SatFractObj        {1, true, true, true};
Type_Fixed const Type_Fixed::SatLongFractObj    {2, true, true, true};
Type_Fixed const Type_Fixed::SatLongLongFractObj{3, true, true, true};
Type_Fixed const Type_Fixed::SatShortAccumObj   {0, true, true, false};
Type_Fixed const Type_Fixed::SatAccumObj        {1, true, true, false};
Type_Fixed const Type_Fixed::SatLongAccumObj    {2, true, true, false};
Type_Fixed const Type_Fixed::SatLongLongAccumObj{3, true, true, false};

Type_Fixed const Type_Fixed::SatUnsignedShortFractObj   {0, true, false, true};
Type_Fixed const Type_Fixed::SatUnsignedFractObj        {1, true, false, true};
Type_Fixed const Type_Fixed::SatUnsignedLongFractObj    {2, true, false, true};
Type_Fixed const Type_Fixed::SatUnsignedLongLongFractObj{3, true, false, true};
Type_Fixed const Type_Fixed::SatUnsignedShortAccumObj   {0, true, false, false};
Type_Fixed const Type_Fixed::SatUnsignedAccumObj        {1, true, false, false};
Type_Fixed const Type_Fixed::SatUnsignedLongAccumObj    {2, true, false, false};
Type_Fixed const Type_Fixed::SatUnsignedLongLongAccumObj{3, true, false, false};

Type::TypeCR const Type::ShortFract   {&Type_Fixed::ShortFractObj};
Type::TypeCR const Type::Fract        {&Type_Fixed::FractObj};
Type::TypeCR const Type::LongFract    {&Type_Fixed::LongFractObj};
Type::TypeCR const Type::LongLongFract{&Type_Fixed::LongLongFractObj};
Type::TypeCR const Type::ShortAccum   {&Type_Fixed::ShortAccumObj};
Type::TypeCR const Type::Accum        {&Type_Fixed::AccumObj};
Type::TypeCR const Type::LongAccum    {&Type_Fixed::LongAccumObj};
Type::TypeCR const Type::LongLongAccum{&Type_Fixed::LongLongAccumObj};

Type::TypeCR const Type::UnsignedShortFract   {&Type_Fixed::UnsignedShortFractObj};
Type::TypeCR const Type::UnsignedFract        {&Type_Fixed::UnsignedFractObj};
Type::TypeCR const Type::UnsignedLongFract    {&Type_Fixed::UnsignedLongFractObj};
Type::TypeCR const Type::UnsignedLongLongFract{&Type_Fixed::UnsignedLongLongFractObj};
Type::TypeCR const Type::UnsignedShortAccum   {&Type_Fixed::UnsignedShortAccumObj};
Type::TypeCR const Type::UnsignedAccum        {&Type_Fixed::UnsignedAccumObj};
Type::TypeCR const Type::UnsignedLongAccum    {&Type_Fixed::UnsignedLongAccumObj};
Type::TypeCR const Type::UnsignedLongLongAccum{&Type_Fixed::UnsignedLongLongAccumObj};

Type::TypeCR const Type::SatShortFract   {&Type_Fixed::SatShortFractObj};
Type::TypeCR const Type::SatFract        {&Type_Fixed::SatFractObj};
Type::TypeCR const Type::SatLongFract    {&Type_Fixed::SatLongFractObj};
Type::TypeCR const Type::SatLongLongFract{&Type_Fixed::SatLongLongFractObj};
Type::TypeCR const Type::SatShortAccum   {&Type_Fixed::SatShortAccumObj};
Type::TypeCR const Type::SatAccum        {&Type_Fixed::SatAccumObj};
Type::TypeCR const Type::SatLongAccum    {&Type_Fixed::SatLongAccumObj};
Type::TypeCR const Type::SatLongLongAccum{&Type_Fixed::SatLongLongAccumObj};

Type::TypeCR const Type::SatUnsignedShortFract   {&Type_Fixed::SatUnsignedShortFractObj};
Type::TypeCR const Type::SatUnsignedFract        {&Type_Fixed::SatUnsignedFractObj};
Type::TypeCR const Type::SatUnsignedLongFract    {&Type_Fixed::SatUnsignedLongFractObj};
Type::TypeCR const Type::SatUnsignedLongLongFract{&Type_Fixed::SatUnsignedLongLongFractObj};
Type::TypeCR const Type::SatUnsignedShortAccum   {&Type_Fixed::SatUnsignedShortAccumObj};
Type::TypeCR const Type::SatUnsignedAccum        {&Type_Fixed::SatUnsignedAccumObj};
Type::TypeCR const Type::SatUnsignedLongAccum    {&Type_Fixed::SatUnsignedLongAccumObj};
Type::TypeCR const Type::SatUnsignedLongLongAccum{&Type_Fixed::SatUnsignedLongLongAccumObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Fixed::Type_Fixed
//
Type_Fixed::Type_Fixed(unsigned size_, bool satu_, bool sign_, bool frac_) :
   size{size_}, accu{!frac_}, frac{frac_}, prim{!satu_}, psig{!satu_ && sign_},
   puns{!satu_ && !sign_}, sacc{!frac_ && sign_}, satu{satu_}, sfra{frac_ && sign_},
   sign{sign_}, ssig{satu_ && sign_}, suns{satu_ && !sign_}, uacc{!frac_ && !sign_},
   ufra{frac_ && !sign_}, unsi{!sign_}
{
   ++refCount;
}

//
// Type_Fixed::getAlignment
//
bigsint Type_Fixed::getAlignment() const
{
   if(Target == TARGET_MageCraft) return size ? 4 : 2;

   return 1;
}

//
// Type_Fixed::getBase
//
auto Type_Fixed::getBase() const -> TypeCR
{
   if(prim) Error_Np("internal error: primary");

   return static_cast<TypeCR>(PrimaryTable[sign][frac][size]);
}

//
// Type_Fixed::getNameMangleBase
//
void Type_Fixed::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << (frac ? "BR" : "BK");

   if(satu) out << 'S';
   if(unsi) out << 'U';

   if(size != 1) out << "H DQ"[size];
}

//
// Type_Fixed::getSizeBits
//
bigsint Type_Fixed::getSizeBits() const
{
   if(Target == TARGET_MageCraft) return SizeBitsTable_MageCraft[frac][size];

   return SizeBitsTable_ACS[frac][size];
}

//
// Type_Fixed::getSizeBytes
//
bigsint Type_Fixed::getSizeBytes() const
{
   if(Target == TARGET_MageCraft) return SizeBytesTable_MageCraft[frac][size];

   return SizeBytesTable_ACS[frac][size];
}

//
// Type_Fixed::getSizePtr
//
bigsint Type_Fixed::getSizePtr() const
{
   if(Target == TARGET_MageCraft) return SizePtrTable_MageCraft[frac][size];

   return SizePtrTable_ACS[frac][size];
}

//
// Type_Fixed::getSizeWords
//
bigsint Type_Fixed::getSizeWords() const
{
   if(Target == TARGET_MageCraft) return SizeWordsTable_MageCraft[frac][size];

   return SizeWordsTable_ACS[frac][size];
}

//
// Type_Fixed::saveObject
//
ObjectSave &Type_Fixed::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << SaveObjectTable[satu][sign][frac][size]);
}

// EOF

