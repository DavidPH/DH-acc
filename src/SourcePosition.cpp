//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2013 David Hill
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
// Storage of source-code positions.
//
//-----------------------------------------------------------------------------

#include "SourcePosition.hpp"

#include "ObjectArchive.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// operator ObjectSave << SourcePosition
//
ObjectSave &operator << (ObjectSave &arc, SourcePosition const &data)
{
   return arc << data.filename << data.line << data.column;
}

//
// operator std::ostream << SourcePosition
//
std::ostream &operator << (std::ostream &out, SourcePosition const &in)
{
   return out << in.filename << ':' << in.line << ':' << in.column;
}

//
// operator ObjectLoad >> SourcePosition
//
ObjectLoad &operator >> (ObjectLoad &arc, SourcePosition &data)
{
   return arc >> data.filename >> data.line >> data.column;
}

// EOF

