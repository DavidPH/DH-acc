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
// Void type handling.
//
//-----------------------------------------------------------------------------

#include "Void.hpp"

#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Label const Type_Label::LabelObj;
Type_String const Type_String::StringObj;
Type_Void const &Type_Void::VoidObj = GetVoidObj();

Type::TypeCR const Type::Label{&Type_Label::LabelObj};
Type::TypeCR const Type::String{&Type_String::StringObj};
Type::TypeCR const Type::Void{&Type_Void::VoidObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Label::Type_Label
//
Type_Label::Type_Label()
{
   ++refCount;
}

//
// Type_Label::getAlignment
//
bigsint Type_Label::getAlignment() const
{
   Error_Np("internal error: label");
}

//
// Type_Label::getNameMangleBase
//
void Type_Label::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BL";
}

//
// Type_Label::getSizeBytes
//
bigsint Type_Label::getSizeBytes() const
{
   Error_Np("internal error: label");
}

//
// Type_Label::getSizePtr
//
bigsint Type_Label::getSizePtr() const
{
   Error_Np("internal error: label");
}

//
// Type_Label::getSizeWords
//
bigsint Type_Label::getSizeWords() const
{
   Error_Np("internal error: label");
}

//
// Type_String::Type_String
//
Type_String::Type_String()
{
   ++refCount;
}

//
// Type_String::getAlignment
//
bigsint Type_String::getAlignment() const
{
   return GetAlignmentWord();
}

//
// Type_String::getNameMangleBase
//
void Type_String::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BS";
}

//
// Type_String::getSizeBytes
//
bigsint Type_String::getSizeBytes() const
{
   return getSizeWords() * GetSizeWord();
}

//
// Type_String::getSizePtr
//
bigsint Type_String::getSizePtr() const
{
   return getSizeWords();
}

//
// Type_String::getSizeWords
//
bigsint Type_String::getSizeWords() const
{
   return 1;
}

//
// Type_Void::Type_Void
//
Type_Void::Type_Void()
{
   ++refCount;
}

//
// Type_Void::getAlignment
//
bigsint Type_Void::getAlignment() const
{
   Error_Np("internal error: void");
}

//
// Type_Void::getNameMangleBase
//
void Type_Void::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BV";
}

//
// Type_Void::getSizeBytes
//
bigsint Type_Void::getSizeBytes() const
{
   Error_Np("internal error: void");
}

//
// Type_Void::getSizePtr
//
bigsint Type_Void::getSizePtr() const
{
   Error_Np("internal error: void");
}

//
// Type_Void::getSizeWords
//
bigsint Type_Void::getSizeWords() const
{
   Error_Np("internal error: void");
}

//
// Type_Void::GetVoidObj
//
Type_Void const &Type_Void::GetVoidObj()
{
   static Type_Void const VoidObj_;

   return VoidObj_;
}

// EOF

