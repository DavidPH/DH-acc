//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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
// Linkage specifier handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_LinkSpec_
#define HPP_LinkSpec_


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectLoad;
class ObjectSave;

//
// LinkageSpecifier
//
enum LinkageSpecifier
{
   LINKAGE_INTERN,
   LINKAGE_ACS,
   LINKAGE_C,
   LINKAGE_CPP,
   LINKAGE_DS,
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

ObjectSave &operator << (ObjectSave &arc, LinkageSpecifier const &data);
ObjectLoad &operator >> (ObjectLoad &arc, LinkageSpecifier &data);

#endif//HPP_LinkSpec_

