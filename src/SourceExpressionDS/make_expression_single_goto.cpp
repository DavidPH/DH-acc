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
// DS handling of goto.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_goto
//
SRCEXPDS_EXPRSINGLE_DEFN(goto)
{
   SourceTokenC gotoToken = in->get(SourceTokenC::TT_IDENTIFIER);

   std::string label;

   if (gotoToken.getData() == "case")
   {
      if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER &&
          in->peek().getData() == "default")
      {
         in->get(SourceTokenC::TT_IDENTIFIER);
         label = context->getLabelCaseDefault(token.getPosition());
      }
      else
      {
         label =
            context->getLabelCase
            (make_expression(in, blocks, context)->makeObject()->resolveInt(),
		   token.getPosition());
      }
   }
   else
   {
      label =
         context->getLabelGoto(gotoToken.getData(), gotoToken.getPosition());
   }

   return create_branch_goto(label, context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_goto_dyn
//
SRCEXPDS_EXPRSINGLE_DEFN(goto_dyn)
{
   return create_branch_goto
          (make_expression_single(in, blocks, context), context,
		 token.getPosition());
}

// EOF

