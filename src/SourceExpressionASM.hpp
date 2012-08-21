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
// ASM handling/parsing.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceExpressionASM_
#define HPP_SourceExpressionASM_

#include "ObjectCode.hpp"
#include "SourceExpression.hpp"

#include <map>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// SRCEXPASM_EXPR_ARGS
//
#define SRCEXPASM_EXPR_ARGS SourceTokenizerASM *in, SourceContext *context

//
// SRCEXPASM_EXPR_DECL
//
#define SRCEXPASM_EXPR_DECL(NAME) \
   static SourceExpression::Pointer Make_##NAME(SRCEXPASM_EXPR_ARGS)

//
// SRCEXPASM_EXPR_DEFN
//
#define SRCEXPASM_EXPR_DEFN(NAME) \
   SourceExpression::Pointer SourceExpressionASM::Make_##NAME(SRCEXPASM_EXPR_ARGS)

//
// SRCEXPASM_KEYWORD_ARGS
//
#define SRCEXPASM_KEYWORD_ARGS \
   SourceTokenizerASM *in, SourceTokenASM *tok, SourceContext *context

//
// SRCEXPASM_KEYWORD_DECL
//
#define SRCEXPASM_KEYWORD_DECL(NAME) \
   static SourceExpression::Pointer MakeKeyword_##NAME(SRCEXPASM_KEYWORD_ARGS)

//
// SRCEXPASM_KEYWORD_DECL
//
#define SRCEXPASM_KEYWORD_DEFN(NAME) \
   SourceExpression::Pointer SourceExpressionASM::MakeKeyword_##NAME(SRCEXPASM_KEYWORD_ARGS)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceTokenASM;
template<typename SourceToken> class SourceTokenizer;

typedef SourceTokenizer<SourceTokenASM> SourceTokenizerASM;

class SourceExpressionASM : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpressionASM, SourceExpression);

public:
   Vector     args;
   ObjectCode code;


   static void Init();

   static SourceExpression::Pointer CreateValueInteger(std::string const &value,
                                                       SRCEXP_EXPR_ARGS);

   static SourceExpression::Pointer MakeStatement(SourceTokenizerASM *in,
                                                  SourceContext *context);

   static SourceExpression::Pointer MakeStatements(SourceTokenizerASM *in);

   SRCEXPASM_EXPR_DECL(Primary);
   SRCEXPASM_EXPR_DECL(Prefix);
   SRCEXPASM_EXPR_DECL(Multiplicative);
   SRCEXPASM_EXPR_DECL(Additive);
   SRCEXPASM_EXPR_DECL(Shift);
   SRCEXPASM_EXPR_DECL(Relational);
   SRCEXPASM_EXPR_DECL(Equality);
   SRCEXPASM_EXPR_DECL(BitwiseAnd);
   SRCEXPASM_EXPR_DECL(BitwiseXor);
   SRCEXPASM_EXPR_DECL(BitwiseIor);
   SRCEXPASM_EXPR_DECL(LogicalAnd);
   SRCEXPASM_EXPR_DECL(LogicalXor);
   SRCEXPASM_EXPR_DECL(LogicalIor);
   SRCEXPASM_EXPR_DECL(Expression);

protected:
   //
   // ::SourceExpressionASM
   //
   SourceExpressionASM(ObjectCode _code, Vector const &_args, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), args(_args), code(_code)
   {
   }

private:
   typedef SourceExpression::Pointer (*KeywordHandler)(SRCEXPASM_KEYWORD_ARGS);
   typedef std::map<std::string, KeywordHandler> KeywordMap;


   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);


   static void MakeStatements(SourceTokenizerASM *in, Vector *exprs, SourceContext *context);

   SRCEXPASM_KEYWORD_DECL(Function);
   SRCEXPASM_KEYWORD_DECL(Script);

   static KeywordMap Keyword;
};

#endif//HPP_SourceExpressionASM_

