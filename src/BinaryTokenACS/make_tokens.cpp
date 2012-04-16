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
// Object to ACS translation.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenACS.hpp"
#include "make_tokens.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define TOKEN_CLASS BinaryTokenACS


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenACS::make_tokens
//
void BinaryTokenACS::make_tokens
(ObjectVector const &objects, std::vector<BinaryTokenACS> *instructions)
{
   static ObjectExpression::Pointer const fracbits =
      ObjectExpression::create_value_int(16, SourcePosition::builtin());

   static std::vector<std::string> const nolabels;

   std::vector<ObjectExpression::Pointer> args;

   ObjectVector::const_iterator object;
   for (object = objects.begin(); object != objects.end(); ++object)
   {
   SourcePosition const &pos = object->pos;

   std::vector<std::string> const *labels = &object->labels;

   switch (object->code)
   {
   // Direct Mappings
   BINTOKACS_TOKENS_MAP_ALL_ALL();

   // Translations
   BINTOKACS_TOKENS_TRAN_ALL_ALL();

   case OCODE_NONE:
   default:
      ERROR_P("unknown OCODE: %s", make_string(object->code));
   }
   }
}

// EOF

