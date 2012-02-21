//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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

#include "object_io.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <map>
#include <string>


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
// ObjectCodeSet::ObjectCodeSet
//
ObjectCodeSet::ObjectCodeSet() : ocode(OCODE_NONE), ocode_imm(OCODE_NONE)
{
}

//
// ocode_get_code
//
ObjectCode ocode_get_code
(std::string const &data, SourcePosition const &position)
{
   std::map<std::string, ObjectCode>::iterator codeIt(_ocode_map.find(data));

   if (codeIt == _ocode_map.end())
      throw SourceException("no such OCODE '" + data + "'", position, __func__);

   return codeIt->second;
}

//
// ocode_is_push_noarg
//
bool ocode_is_push_noarg(ObjectCode ocode)
{
   switch (ocode)
   {
   case OCODE_ADDR_AUTO:
   case OCODE_GET_AUTO32F:
   case OCODE_GET_AUTO32I:
   case OCODE_GET_LITERAL32F:
   case OCODE_GET_LITERAL32I:
   case OCODE_GET_REGISTER32F:
   case OCODE_GET_REGISTER32I:
   case OCODE_GET_STATIC32F:
   case OCODE_GET_STATIC32I:
   case OCODE_ACS_GET_MAPREGISTER:
   case OCODE_ACS_GET_WORLDREGISTER:
   case OCODE_ACSE_GET_GLOBALREGISTER:
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
// read_object<ObjectCode>
//
void read_object(std::istream *in, ObjectCode *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));

   if (*out > OCODE_NONE)
      *out = OCODE_NONE;
}

//
// read_object<ObjectCodeSet>
//
void read_object(std::istream *in, ObjectCodeSet *out)
{
   read_object(in, &out->ocode);
   read_object(in, &out->ocode_imm);
}

//
// write_object<ObjectCode>
//
void write_object(std::ostream *out, ObjectCode const &in)
{
   write_object_raw(out, (char const *)&in, sizeof(in));
}

//
// write_object<ObjectCodeSet>
//
void write_object(std::ostream *out, ObjectCodeSet const &in)
{
   write_object(out, in.ocode);
   write_object(out, in.ocode_imm);
}

// EOF

