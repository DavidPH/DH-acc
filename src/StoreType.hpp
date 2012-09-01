//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// Storage-type/class handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_StoreType_
#define HPP_StoreType_

#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// StoreType
//
enum StoreType
{
   STORE_NONE,
   STORE_FAR,
   STORE_STATIC,
   STORE_AUTO,
   STORE_CONST,
   STORE_REGISTER,
   STORE_MAPREGISTER,
   STORE_WORLDREGISTER,
   STORE_GLOBALREGISTER,
   STORE_MAPARRAY,
   STORE_WORLDARRAY,
   STORE_GLOBALARRAY,
   STORE_STRING,
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

std::string const &make_string(StoreType store);

StoreType store_staticregister();
StoreType store_staticarray();

StoreType store_autoregister();
StoreType store_autoarray();

#endif//HPP_StoreType_

