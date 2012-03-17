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
// DS handling of sizeof.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_sizeof
//
SRCEXPDS_EXPRSINGLE_DEFN(sizeof)
{
   VariableType::Pointer type;
   bigsint size;

   if (in->peekType(SourceTokenC::TT_OP_PARENTHESIS_O))
   {
      in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

      if (in->peekType(SourceTokenC::TT_IDENTIFIER) &&
          is_expression_type(in->peek().data, context))
         type = make_expression_type(in, blocks, context);
      else
         type = make_expression(in, blocks, context)->getType();

      in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
   }
   else
   {
      type = make_expression_single(in, blocks, context)->getType();
   }

   size = type->getSize(token.pos);

   return create_value_uint(size, context, token.pos);
}

// EOF

