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
// DS handling of do...while.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_do
//
SRCEXPDS_EXPRSINGLE_DEFN(do)
{
   SourceContext::Reference contextBody =
      SourceContext::create(context, SourceContext::CT_LOOP);
   SourceExpression::Pointer exprBody =
      make_expression_single(in, blocks, contextBody);

   SourceTokenC tokenWhile(in->get(SourceTokenC::TT_IDENTIFIER));

   if (tokenWhile.getData() != "while")
      throw SourceException("expected 'while' got '" + tokenWhile.getData() +
                            "'", token.getPosition(), __func__);

   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   SourceContext::Reference contextCond =
      SourceContext::create(contextBody, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   return create_branch_do(exprCond, exprBody, contextCond, token.getPosition());
}

// EOF

