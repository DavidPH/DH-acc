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
// Integer type handling.
//
//-----------------------------------------------------------------------------

#include "Integer.hpp"

#include "ost_type.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static constexpr KeywordIndex SaveObjectTable[2][4] =
{
   {KWRD_UnsignedShortInt, KWRD_UnsignedInt, KWRD_UnsignedLongInt, KWRD_UnsignedLongLongInt},
   {KWRD_ShortInt, KWRD_Int, KWRD_LongInt, KWRD_LongLongInt},
};

static constexpr int SizeBitsTable_ACS[4] = {32, 32, 64, 64};
static constexpr int SizeBytesTable_ACS[4] = {1, 1, 2, 2};
static constexpr int SizePtrTable_ACS[4] = {1, 1, 2, 2};
static constexpr int SizeWordsTable_ACS[4] = {1, 1, 2, 2};

static constexpr int SizeBitsTable_MageCraft[4] = {16, 32, 64, 128};
static constexpr int SizeBytesTable_MageCraft[4] = {2, 4, 8, 16};
static constexpr int SizePtrTable_MageCraft[4] = {2, 1, 2, 4};
static constexpr int SizeWordsTable_MageCraft[4] = {1, 1, 2, 4};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Integer const Type_Integer::ShortIntObj{0, true};
Type_Integer const Type_Integer::IntObj{1, true};
Type_Integer const Type_Integer::LongIntObj{2, true};
Type_Integer const Type_Integer::LongLongIntObj{3, true};

Type_Integer const Type_Integer::UnsignedShortIntObj{0, false};
Type_Integer const Type_Integer::UnsignedIntObj{1, false};
Type_Integer const Type_Integer::UnsignedLongIntObj{2, false};
Type_Integer const Type_Integer::UnsignedLongLongIntObj{3, false};

Type::TypeCR const Type::ShortInt{&Type_Integer::ShortIntObj};
Type::TypeCR const Type::Int{&Type_Integer::IntObj};
Type::TypeCR const Type::LongInt{&Type_Integer::LongIntObj};
Type::TypeCR const Type::LongLongInt{&Type_Integer::LongLongIntObj};

Type::TypeCR const Type::UnsignedShortInt{&Type_Integer::UnsignedShortIntObj};
Type::TypeCR const Type::UnsignedInt{&Type_Integer::UnsignedIntObj};
Type::TypeCR const Type::UnsignedLongInt{&Type_Integer::UnsignedLongIntObj};
Type::TypeCR const Type::UnsignedLongLongInt{&Type_Integer::UnsignedLongLongIntObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Integer::Type_Integer
//
Type_Integer::Type_Integer(unsigned size_, bool sign_) : size{size_}, sign{sign_},
   unsi{!sign_}
{
   --refCount;
}

//
// Type_Integer::getAlignment
//
bigsint Type_Integer::getAlignment() const
{
   if(Target == TARGET_MageCraft) return size ? 4 : 2;

   return 1;
}

//
// Type_Integer::getNameMangleBase
//
void Type_Integer::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BI";

   if(unsi) out << 'U';

   if(size != 1) out << "H DQ"[size];
}

//
// Type_Integer::getSizeBits
//
bigsint Type_Integer::getSizeBits() const
{
   if(Target == TARGET_MageCraft) return SizeBitsTable_MageCraft[size];

   return SizeBitsTable_ACS[size];
}

//
// Type_Integer::getSizeBytes
//
bigsint Type_Integer::getSizeBytes() const
{
   if(Target == TARGET_MageCraft) return SizeBytesTable_MageCraft[size];

   return SizeBytesTable_ACS[size];
}

//
// Type_Integer::getSizePtr
//
bigsint Type_Integer::getSizePtr() const
{
   if(Target == TARGET_MageCraft) return SizePtrTable_MageCraft[size];

   return SizePtrTable_ACS[size];
}

//
// Type_Integer::getSizeWords
//
bigsint Type_Integer::getSizeWords() const
{
   if(Target == TARGET_MageCraft) return SizeWordsTable_MageCraft[size];

   return SizeWordsTable_ACS[size];
}

//
// Type_Integer::saveObject
//
ObjectSave &Type_Integer::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << SaveObjectTable[sign][size]);
}

// EOF

