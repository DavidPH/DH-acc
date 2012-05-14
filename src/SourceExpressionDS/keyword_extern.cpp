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
// DS handling of extern.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerC.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_keyword_extern
//
SRCEXPDS_KEYWORD_DEFN(extern)
{
   ExternMap::iterator externFunc;
   LinkageSpecifier linkSpec;

   if (tok->data != "extern")
      linkSpec = LS_INTERN;
   else if (in->peekType(SourceTokenC::TT_STR))
      linkSpec = make_linkspec(in);
   else
      linkSpec = LS_DS;

   if (in->peekType(SourceTokenC::TT_BRACE_O))
   {
      Vector exprs;

      in->get(SourceTokenC::TT_BRACE_O);

      while (!in->peekType(SourceTokenC::TT_BRACE_C))
      {
         SourceTokenC::Reference externToken = in->get(SourceTokenC::TT_NAM);

         externFunc = expr_extern.find(externToken->data);

         if (externFunc == expr_extern.end())
            ERROR(externToken->pos, "unknown extern type %s",
                  externToken->data.c_str());

         exprs.push_back(externFunc->second(
            in, externToken, blocks, context, linkSpec));

         in->get(SourceTokenC::TT_SEMICOLON);
      }

      in->get(SourceTokenC::TT_BRACE_C);

      return create_value_block(exprs, context, tok->pos);
   }
   else
   {
      SourceTokenC::Reference externToken = in->get(SourceTokenC::TT_NAM);

      externFunc = expr_extern.find(externToken->data);

      if (externFunc == expr_extern.end())
         ERROR(externToken->pos, "unknown extern type %s",
               externToken->data.c_str());

      return externFunc->second(in, externToken, blocks, context, linkSpec);
   }
}

// EOF

