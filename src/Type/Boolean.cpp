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
// Boolean type handling.
//
//-----------------------------------------------------------------------------

#include "Boolean.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Boolean const Type_Boolean::BoolCObj{true, true};
Type_Boolean const Type_Boolean::BoolCXXObj{true, false};
Type_Boolean const Type_Boolean::FastBoolObj{false, false};

Type::TypeCR const Type::BoolC{&Type_Boolean::BoolCObj};
Type::TypeCR const Type::BoolCXX{&Type_Boolean::BoolCXXObj};
Type::TypeCR const Type::FastBool{&Type_Boolean::FastBoolObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Boolean::Type_Boolean
//
Type_Boolean::Type_Boolean(bool hard_, bool unsi_) : hard{hard_}, unsi{unsi_}
{
   ++refCount;
}

//
// Type_Boolean::getAlignment
//
bigsint Type_Boolean::getAlignment() const
{
   return GetAlignmentWord();
}

//
// Type_Boolean::getNameMangleBase
//
void Type_Boolean::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << (hard ? unsi ? "BBC" : "BB" : "BBF");
}

//
// Type_Boolean::getSizeBits
//
bigsint Type_Boolean::getSizeBits() const
{
   if(hard) return 1;

   return GetSizeWord() * GetSizeByte();
}

//
// Type_Boolean::getSizeBytes
//
bigsint Type_Boolean::getSizeBytes() const
{
   return GetSizeWord();
}

//
// Type_Boolean::saveObject
//
ObjectSave &Type_Boolean::saveObject(ObjectSave &save) const
{
   save << (hard ? unsi ? KWRD_BoolC : KWRD_BoolCXX : KWRD_FastBool);
   return Super::saveObject(save);
}

// EOF

