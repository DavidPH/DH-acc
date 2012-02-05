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
// DS handling of __printf.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_printf
//
SRCEXPDS_EXPRSINGLE_DEFN(printf)
{
   std::string type;

   if (in->peek().getType() == SourceTokenC::TT_OP_CMP_LT)
   {
      in->get(SourceTokenC::TT_OP_CMP_LT);

      type = in->get(SourceTokenC::TT_IDENTIFIER).getData();

      in->get(SourceTokenC::TT_OP_CMP_GT);
   }

   SourceExpression::Vector expressions;

   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

   std::string const format = in->get(SourceTokenC::TT_STRING).getData();

   while (in->peek().getType() == SourceTokenC::TT_OP_COMMA)
   {
      in->get(SourceTokenC::TT_OP_COMMA);
      expressions.push_back(make_expression(in, blocks, context));
   }

   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   return create_root_printf
          (type, format, expressions, context, token.getPosition());
}

// EOF

