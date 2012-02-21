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
// DS handling of if and if...else.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_if
//
SRCEXPDS_EXPRSINGLE_DEFN(if)
{
   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   SourceContext::Reference contextCond =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
     make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody =
      make_expression_single(in, blocks, contextBody);

   if (in->peekType(SourceTokenC::TT_IDENTIFIER, "else"))
   {
      in->get();

      SourceContext::Reference contextElse =
         SourceContext::create(contextCond, SourceContext::CT_BLOCK);
      SourceExpression::Pointer exprElse =
         make_expression_single(in, blocks, contextElse);

      return create_branch_if(exprCond, exprBody, exprElse, context, token.pos);
   }
   else
   {
      return create_branch_if(exprCond, exprBody, context, token.pos);
   }
}

// EOF

