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

#include "StoreType.hpp"

#include "ost_type.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

std::string const store_name[] =
{
   "STORE_STATIC",
   "STORE_AUTO",
   "STORE_CONST",
   "STORE_REGISTER",
   "STORE_MAPREGISTER",
   "STORE_WORLDREGISTER",
   "STORE_GLOBALREGISTER",
   "STORE_MAPARRAY",
   "STORE_WORLDARRAY",
   "STORE_GLOBALARRAY",
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// make_string<StoreType>
//
std::string const &make_string(StoreType store)
{
   return store_name[store];
}

//
// store_staticregister
//
StoreType store_staticregister()
{
   if (target_type == TARGET_Hexen || target_type == TARGET_ZDoom)
      return STORE_MAPREGISTER;

   return STORE_STATIC;
}

//
// store_staticarray
//
StoreType store_staticarray()
{
   if (target_type == TARGET_ZDoom)
      return STORE_MAPARRAY;

   return STORE_STATIC;
}

//
// store_autoregister
//
StoreType store_autoregister()
{
   if (target_type == TARGET_Hexen || target_type == TARGET_ZDoom)
      return STORE_REGISTER;

   return STORE_AUTO;
}

//
// store_autoarray
//
StoreType store_autoarray()
{
   if (target_type == TARGET_Hexen)
      return STORE_REGISTER;

   return STORE_AUTO;
}

// EOF

