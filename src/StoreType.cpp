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

static std::string const store_name[] =
{
   "far",
   "near",
   "auto",
   "literal",
   "register",
   "mapregister",
   "worldregister",
   "globalregister",
   "maparray",
   "worldarray",
   "globalarray",
   "stringarray",
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
   return STORE_MAPREGISTER;
}

//
// store_staticarray
//
StoreType store_staticarray()
{
   return STORE_MAPARRAY;
}

//
// store_autoregister
//
StoreType store_autoregister()
{
   return STORE_REGISTER;
}

//
// store_autoarray
//
StoreType store_autoarray()
{
   if(Target == TARGET_Hexen)
      return STORE_REGISTER;

   return STORE_AUTO;
}

// EOF

