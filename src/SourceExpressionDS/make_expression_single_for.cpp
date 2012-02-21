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
// DS handling of for.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_for
//
SRCEXPDS_EXPRSINGLE_DEFN(for)
{
   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

   SourceContext::Reference contextInit =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprInit =
      make_expression(in, blocks, contextInit);

   in->get(SourceTokenC::TT_OP_SEMICOLON);

   SourceContext::Reference contextCond =
      SourceContext::create(contextInit, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);

   in->get(SourceTokenC::TT_OP_SEMICOLON);

   SourceContext::Reference contextIter =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprIter =
      make_expression(in, blocks, contextIter);

   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_LOOP);
   SourceExpression::Pointer exprBody =
      make_expression_single(in, blocks, contextBody);

   return create_branch_for(exprCond, exprBody, exprIter, exprInit, contextBody, token.pos);
}

// EOF

