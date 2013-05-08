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
// Character type handling.
//
//-----------------------------------------------------------------------------

#include "Char.hpp"

#include "ost_type.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Char const Type_Char::CharObj{false, false};
Type_Char const Type_Char::SignedCharObj{false, true};
Type_Char const Type_Char::UnsignedCharObj{true, false};

Type_WChar const Type_WChar::Char16Obj{false, false};
Type_WChar const Type_WChar::Char32Obj{true, false};
Type_WChar const Type_WChar::WCharObj{true, true};

Type::TypeCR const Type::Char{&Type_Char::CharObj};
Type::TypeCR const Type::SignedChar{&Type_Char::SignedCharObj};
Type::TypeCR const Type::UnsignedChar{&Type_Char::UnsignedCharObj};

Type::TypeCR const Type::Char16{&Type_WChar::Char16Obj};
Type::TypeCR const Type::Char32{&Type_WChar::Char32Obj};
Type::TypeCR const Type::WChar{&Type_WChar::WCharObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Char::Type_Char
//
Type_Char::Type_Char(bool unsi_, bool sign_) : sign{sign_}, unsi{unsi_}
{
   ++refCount;
}

//
// Type_Char::getNameMangleBase
//
void Type_Char::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BC";
   if(sign) out << 'S';
   if(unsi) out << 'U';
}

//
// Type_Char::getSizeBits
//
bigsint Type_Char::getSizeBits() const
{
   return GetSizeByte();
}

//
// Type_Char::isSigned
//
bool Type_Char::isSigned() const
{
   if(sign) return true;
   if(unsi) return false;

   // Type "char" may be signed or unsigned.

   // For MageCraft, an unsigned char is faster, so use that.
   if(Target == TARGET_MageCraft) return false;

   // For ACS targets, signed is faster.
   return true;
}

//
// Type_Char::saveObject
//
ObjectSave &Type_Char::saveObject(ObjectSave &save) const
{
        if(sign) save << KWRD_SignedChar;
   else if(unsi) save << KWRD_UnsignedChar;
   else          save << KWRD_Char;
   return Super::saveObject(save);
}

//
// Type_WChar::Type_WChar
//
Type_WChar::Type_WChar(bool wide_, bool sign_) : sign{sign_}, wide{wide_}
{
   ++refCount;
}

//
// Type_WChar::getAlignment
//
bigsint Type_WChar::getAlignment() const
{
   return wide ? GetAlignmentWord() : GetAlignmentHWord();
}

//
// Type_WChar::getNameMangleBase
//
void Type_WChar::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << (wide ? sign ? "BCW" : "BCQ" : "BCD");
}

//
// Type_WChar::getSizeBits
//
bigsint Type_WChar::getSizeBits() const
{
   return Type_WChar::getSizeBytes() * GetSizeByte();
}

//
// Type_WChar::getSizeBytes
//
bigsint Type_WChar::getSizeBytes() const
{
   return wide ? GetSizeWord() : GetSizeHWord();
}

//
// Type_WChar::getSizePtr
//
bigsint Type_WChar::getSizePtr() const
{
   if(Target == TARGET_MageCraft)
      return wide ? 1 : 2;

   return 1;
}

//
// Type_WChar::isSigned
//
bool Type_WChar::isSigned() const
{
   if(!sign) return false;

   // Type "wchar_t" may be signed or unsigned.

   // For MageCraft, an unsigned wchar_t is no slower (and sometimes faster).
   if(Target == TARGET_MageCraft) return false;

   // For ACS targets, signed is faster.
   return true;
}

//
// Type_WChar::saveObject
//
ObjectSave &Type_WChar::saveObject(ObjectSave &save) const
{
   save << (wide ? sign ? KWRD_WChar : KWRD_Char32 : KWRD_Char16);
   return Super::saveObject(save);
}

// EOF

