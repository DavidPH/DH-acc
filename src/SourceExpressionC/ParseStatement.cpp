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
// C statement parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionC.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::ParseStatement
//
SRCEXPC_PARSE_DEFN_HALF(Statement)
{
   SourceTokenC::Reference tok = in->get();

   if(tok->type == SourceTokenC::TT_NAM)
   {
      //
      // selection-statement
      //

      // if ( expression ) statement
      // if ( expression ) statement else statement
      if(tok->data == "if")
      {
         // ( expression )
         in->get(SourceTokenC::TT_PAREN_O);
         SourceExpression::Pointer exprCond = ParseExpression(in, context);
         in->get(SourceTokenC::TT_PAREN_C);

         // statement
         SourceExpression::Pointer exprBody = ParseStatement(in, context);

         // else statement
         if(in->dropType(SourceTokenC::TT_NAM, "else"))
         {
            SourceExpression::Pointer exprElse = ParseStatement(in, context);

            return create_branch_if(exprCond, exprBody, exprElse, context, tok->pos);
         }
         else
            return create_branch_if(exprCond, exprBody, context, tok->pos);
      }

      // switch ( expression ) statement
      if(tok->data == "switch")
      {
         // Create a context for the entire switch.
         SourceContext::Reference contextSwitch = SourceContext::
            create(context, SourceContext::CT_SWITCH);

         // ( expression )
         in->get(SourceTokenC::TT_PAREN_O);
         SourceExpression::Pointer exprCond = ParseExpression(in, contextSwitch);
         in->get(SourceTokenC::TT_PAREN_C);

         // statement
         SourceExpression::Pointer exprBody = ParseStatement(in, contextSwitch);

         return create_branch_switch(exprCond, exprBody, contextSwitch, tok->pos);
      }

      //
      // iteration-statement
      //

      // while ( expression ) statement
      if(tok->data == "while")
      {
         // Create a context for the entire loop.
         SourceContext::Reference contextLoop = SourceContext::
            create(context, SourceContext::CT_LOOP);

         // ( expression )
         in->get(SourceTokenC::TT_PAREN_O);
         SourceExpression::Pointer exprCond = ParseExpression(in, contextLoop);
         in->get(SourceTokenC::TT_PAREN_C);

         // statement
         SourceExpression::Pointer exprBody = ParseStatement(in, contextLoop);

         return create_branch_while(exprCond, exprBody, contextLoop, tok->pos);
      }

      // do statement while ( expression ) ;
      if(tok->data == "do")
      {
         // Create a context for the entire loop.
         SourceContext::Reference contextLoop = SourceContext::
            create(context, SourceContext::CT_LOOP);

         // statement
         SourceExpression::Pointer exprBody = ParseStatement(in, contextLoop);

         // while
         in->get(SourceTokenC::TT_NAM, "while");

         // ( expression ) ;
         in->get(SourceTokenC::TT_PAREN_O);
         SourceExpression::Pointer exprCond = ParseExpression(in, contextLoop);
         in->get(SourceTokenC::TT_PAREN_C);
         in->get(SourceTokenC::TT_SEMICOLON);

         return create_branch_do(exprCond, exprBody, contextLoop, tok->pos);
      }

      // for ( expression(opt) ; expression(opt) ; expression(opt) ) statement
      // for ( declaration expression(opt) ; expression(opt) ) statement
      if(tok->data == "for")
      {
         // Create a context for the entire loop.
         SourceContext::Reference contextLoop = SourceContext::
            create(context, SourceContext::CT_LOOP);

         // (
         in->get(SourceTokenC::TT_PAREN_O);

         // expression(opt) ;
         // declaration
         SourceExpression::Pointer exprInit;
         if(IsDeclaration(in, context))
         {
            // declaration
            exprInit = ParseDeclaration(in, contextLoop);
         }
         else
         {
            // expression(opt)
            if(in->peekType(SourceTokenC::TT_SEMICOLON))
               exprInit = create_value_data(contextLoop, tok->pos);
            else
               exprInit = ParseExpression(in, contextLoop);

            // ;
            in->get(SourceTokenC::TT_SEMICOLON);
         }

         // expression(opt)
         SourceExpression::Pointer exprCond;
         if(in->peekType(SourceTokenC::TT_SEMICOLON))
            exprCond = create_value_int(1, contextLoop, tok->pos);
         else
            exprCond = ParseExpression(in, contextLoop);

         // ;
         in->get(SourceTokenC::TT_SEMICOLON);

         // expression(opt)
         SourceExpression::Pointer exprIter;
         if(in->peekType(SourceTokenC::TT_SEMICOLON))
            exprIter = create_value_data(contextLoop, tok->pos);
         else
            exprIter = ParseExpression(in, contextLoop);

         // )
         in->get(SourceTokenC::TT_PAREN_C);

         // statement
         SourceExpression::Pointer exprBody = ParseStatement(in, contextLoop);

         return create_branch_for(exprCond, exprBody, exprIter, exprInit, contextLoop, tok->pos);
      }

      //
      // jump-statement
      //

      // goto identifier ;
      if(tok->data == "goto")
      {
         // identifier ;
         SourceTokenC::Reference gotoTok = in->get(SourceTokenC::TT_NAM);
         in->get(SourceTokenC::TT_SEMICOLON);

         std::string label = context->getLabelGoto(gotoTok->data, gotoTok->pos);
         return create_branch_goto(label, context, tok->pos);
      }

      // continue ;
      if(tok->data == "continue")
      {
         // ;
         in->get(SourceTokenC::TT_SEMICOLON);

         return create_branch_continue(context, tok->pos);
      }

      // break ;
      if(tok->data == "break")
      {
         // ;
         in->get(SourceTokenC::TT_SEMICOLON);

         return create_branch_break(context, tok->pos);
      }

      // return expression(opt) ;
      if(tok->data == "return")
      {
         // expression(opt)
         SourceExpression::Pointer expr;
         if(in->peekType(SourceTokenC::TT_SEMICOLON))
            expr = create_value_data(context, tok->pos);
         else
            expr = ParseExpression(in, context);

         // ;
         in->get(SourceTokenC::TT_SEMICOLON);

         return create_branch_return(expr, context, tok->pos);
      }

      //
      // labeled-statement
      //

      // case constant-expression :
      if(tok->data == "case")
      {
         bigsint value = ParseConditional(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenC::TT_COLON);

         return ParseStatement(in, context)
            ->addLabel(context->addLabelCase(value, tok->pos));
      }

      // default :
      if(tok->data == "default")
      {
         in->get(SourceTokenC::TT_COLON);

         return ParseStatement(in, context)
            ->addLabel(context->addLabelCaseDefault(tok->pos));
      }

      // identifier :
      if(in->dropType(SourceTokenC::TT_COLON))
      {
         return ParseStatement(in, context)
            ->addLabel(context->addLabelGoto(tok->data, tok->pos));
      }
   }

   //
   // compound-statement
   //

   // { block-item-list(opt) }
   if(tok->type == SourceTokenC::TT_BRACE_O)
   {
      SourceContext::Reference blockContext =  SourceContext::
         create(context, SourceContext::CT_BLOCK);
      Vector exprs;

      while(!in->dropType(SourceTokenC::TT_BRACE_C))
      {
         if(IsDeclaration(in, blockContext))
            exprs.push_back(ParseDeclaration(in, blockContext));
         else
            exprs.push_back(ParseStatement(in, blockContext));
      }

      return create_value_block(exprs, context, tok->pos);
   }

   //
   // expression-statement
   //

   // ;
   if(tok->type == SourceTokenC::TT_SEMICOLON)
   {
      return create_value_data(context, tok->pos);
   }

   // expression ;
   {
      in->unget(tok);
      SourceExpression::Pointer expr = ParseExpression(in, context);
      in->get(SourceTokenC::TT_SEMICOLON);
      return expr;
   }
}

// EOF

