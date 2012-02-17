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
// DS handling of symbol.
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
// SourceExpressionDS::make_expression_single_symbol
//
SRCEXPDS_EXPRSINGLE_DEFN(symbol)
{
   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   VariableType::Reference type = make_expression_type(in, blocks, context);
   in->get(SourceTokenC::TT_OP_COMMA);
   std::string name = in->get(SourceTokenC::TT_STRING).getData();
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   SourceVariable::Pointer var =
      SourceVariable::create_literal(type, name, token.getPosition());

   return create_value_variable(var, context, token.getPosition());
}

// EOF

