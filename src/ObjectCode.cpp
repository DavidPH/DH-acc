//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Object Code handling.
//
//-----------------------------------------------------------------------------

#include "ObjectCode.hpp"

#include "ObjectArchive.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::map<std::string, ObjectCode> _ocode_map;
static char const * _ocode_str[OCODE_NONE+1];

static struct _ocode_init_s {_ocode_init_s();} _ocode_init;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// _ocode_init_s::_ocode_init_s
//
_ocode_init_s::_ocode_init_s()
{
   #define DO_INIT(NAME)             \
   _ocode_map[#NAME] = OCODE_##NAME; \
   _ocode_str[OCODE_##NAME] = "OCODE_"#NAME;

   #include "OCODE.hpp"

   DO_INIT(NONE)

   #undef DO_INIT
}

//
// ocode_get_code
//
ObjectCode ocode_get_code
(std::string const &data, SourcePosition const &pos)
{
   std::map<std::string, ObjectCode>::iterator codeIt(_ocode_map.find(data));

   if (codeIt == _ocode_map.end())
      Error_P("no such OCODE: %s", data.c_str());

   return codeIt->second;
}

//
// ocode_is_delay
//
bool ocode_is_delay(ObjectCode ocode)
{
   switch(ocode)
   {
   case OCODE_ACS_WAIT_POLYOBJECT:
   case OCODE_ACS_WAIT_POLYOBJECT_IMM:
   case OCODE_ACS_WAIT_SCRIPT:
   case OCODE_ACS_WAIT_SCRIPT_IMM:
   case OCODE_ACS_WAIT_STAG:
   case OCODE_ACS_WAIT_STAG_IMM:
   case OCODE_ACS_WAIT_TICS:
   case OCODE_ACS_WAIT_TICS_IMM:
      return true;

   default:
      return false;
   }
}

//
// ocode_is_push_noarg
//
bool ocode_is_push_noarg(ObjectCode ocode)
{
   switch (ocode)
   {
   case OCODE_GET_AUTPTR_IMM:

   case OCODE_GET_IMM:
   case OCODE_GET_STATIC:
   case OCODE_GET_AUTO:
   case OCODE_GET_REG:
   case OCODE_GET_MAPREG:
   case OCODE_GET_WLDREG:
   case OCODE_GET_GBLREG:
      return true;

   default:
      return false;
   }
}

//
// make_string<ObjectCode>
//
char const *make_string(ObjectCode ocode)
{
   if (ocode <= OCODE_NONE)
      return _ocode_str[ocode];
   else
      return "OCODE";
}

//
// operator ObjectSave << ObjectCode
//
ObjectSave &operator << (ObjectSave &arc, ObjectCode const &data)
{
   return arc.saveEnum(data);
}

//
// operator ObjectSave << ObjectCodeSet
//
ObjectSave &operator << (ObjectSave &arc, ObjectCodeSet const &data)
{
   return arc << data.ocode << data.ocode_imm;
}

//
// operator ObjectLoad >> ObjectCode
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectCode &data)
{
   return arc.loadEnum(data, OCODE_NONE);
}

//
// operator ObjectLoad >> ObjectCodeSet
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectCodeSet &data)
{
   return arc >> data.ocode >> data.ocode_imm;
}

// EOF

