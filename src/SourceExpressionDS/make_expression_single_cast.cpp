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
// DS handling of template-casts.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_cast
//
SRCEXPDS_EXPRSINGLE_DEFN(cast)
{
   in->get(SourceTokenC::TT_OP_CMP_LT);
   VariableType::Reference type = make_expression_type(in, blocks, context);
   in->get(SourceTokenC::TT_OP_CMP_GT);

   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   SourceExpression::Pointer expr = make_expression(in, blocks, context);
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   if (token.data == "const_cast")
      return create_value_cast_qualifier(expr, type, context, token.pos);

   if (token.data == "reinterpret_cast")
      return create_value_cast_reinterpret(expr, type, context, token.pos);

   if (token.data == "static_cast")
      return create_value_cast_static(expr, type, context, token.pos);

   if (token.data == "__force_cast")
      return create_value_cast_force(expr, type, context, token.pos);

   throw SourceException(token.data, token.pos, __func__);
}

// EOF

