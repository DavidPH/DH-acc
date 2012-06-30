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
// DS handling/parsing.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceExpressionDS_
#define HPP_SourceExpressionDS_

#include "SourceExpression.hpp"
#include "StoreType.hpp"

#include <map>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// SRCEXPDS_EXPR_ARGS
//
#define SRCEXPDS_EXPR_ARG1 \
SourceTokenizerC *in, SourceExpression::Vector *blocks, \
SourceContext *context

//
// SRCEXPDS_EXPR_ARG2
//
#define SRCEXPDS_EXPR_ARG2 \
SRCEXPDS_EXPR_ARG1, SourceExpression::Pointer expr

//
// SRCEXPDS_EXPR_DECL
//
#define SRCEXPDS_EXPR_DECL(NAME) \
static SourceExpression::Pointer \
make_##NAME(SRCEXPDS_EXPR_ARG1); \
static SourceExpression::Pointer \
make_##NAME(SRCEXPDS_EXPR_ARG2)

//
// SRCEXPDS_EXPR_DEFN
//
// Convenience macro for DEF1/DEF2.
//
#define SRCEXPDS_EXPR_DEFN(NAME1,NAME2)     \
SRCEXPDS_EXPR_DEF1(NAME1) {                 \
   return make_##NAME1(in, blocks, context, \
      make_##NAME2(in, blocks, context)); } \
SRCEXPDS_EXPR_DEF2(NAME1)

//
// SRCEXPDS_EXPR_DEF1
//
#define SRCEXPDS_EXPR_DEF1(NAME)               \
SourceExpression::Pointer SourceExpressionDS:: \
make_##NAME(SRCEXPDS_EXPR_ARG1)

//
// SRCEXPDS_EXPR_DEF2
//
#define SRCEXPDS_EXPR_DEF2(NAME)               \
SourceExpression::Pointer SourceExpressionDS:: \
make_##NAME(SRCEXPDS_EXPR_ARG2)

//
// SRCEXPDS_EXTERN_ARGS
//
#define SRCEXPDS_EXTERN_ARGS \
SRCEXPDS_KEYWORD_ARGS, LinkageSpecifier linkSpec

//
// SRCEXPDS_EXTERN_DECL
//
#define SRCEXPDS_EXTERN_DECL(NAME) \
static SourceExpression::Pointer   \
make_extern_##NAME(SRCEXPDS_EXTERN_ARGS)

//
// SRCEXPDS_EXTERN_DEFN
//
#define SRCEXPDS_EXTERN_DEFN(NAME)             \
SourceExpression::Pointer SourceExpressionDS:: \
make_extern_##NAME(SRCEXPDS_EXTERN_ARGS)

//
// SRCEXPDS_KEYWORD_ARGS
//
#define SRCEXPDS_KEYWORD_ARGS             \
SourceTokenizerC *in, SourceTokenC *tok, \
SourceExpression::Vector *blocks, SourceContext *context

//
// SRCEXPDS_KEYWORD_DECL
//
#define SRCEXPDS_KEYWORD_DECL(NAME) \
static SourceExpression::Pointer    \
make_keyword_##NAME(SRCEXPDS_KEYWORD_ARGS)

//
// SRCEXPDS_KEYWORD_DECL
//
#define SRCEXPDS_KEYWORD_DEFN(NAME)            \
SourceExpression::Pointer SourceExpressionDS:: \
make_keyword_##NAME(SRCEXPDS_KEYWORD_ARGS)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceTokenC;
class SourceTokenizerC;

class SourceExpressionDS : public SourceExpression
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpressionDS, SourceExpression);

public:
   enum LinkageSpecifier
   {
      LS_INTERN,
      LS_ACS,
      LS_DS,
   };

   struct ArgList
   {
      ArgList(StoreType store = STORE_REGISTER);

      std::vector<std::string> names;
      std::vector<CounterPointer<VariableType> > types;
      SourceExpression::Vector args;
      std::string name;
      CounterPointer<SourceContext> context;
      CounterPointer<VariableType> retn;
      StoreType store;
      int count;
   };

   static void init();

   static SourceExpression::Pointer create_value_float
   (std::string const &value, SRCEXP_EXPR_ARGS);

   static SourceExpression::Pointer create_value_integer
   (std::string const &value, SRCEXP_EXPR_ARGS);

   static bool is_keyword(std::string const &data);

   static bool is_store(std::string const &data);

   static bool is_type(std::string const &data, SourceContext *context);

   static void make_arglist(SourceTokenizerC *in, Vector *blocks,
      SourceContext *context, ArgList *args);

   static LinkageSpecifier make_linkspec(SourceTokenizerC *in);

   static SourceExpression::Pointer make_statement(SourceTokenizerC *in,
      Vector *blocks, SourceContext *context);

   static SourceExpression::Pointer make_statements(SourceTokenizerC *in);

   static StoreType make_store(SourceTokenizerC *in, Vector *blocks,
      SourceContext *context, CounterPointer<ObjectExpression> *area);

   static CounterReference<VariableType> make_type(SourceTokenizerC *in,
      Vector *blocks, SourceContext *context);

   SRCEXPDS_EXPR_DECL(primary);
   SRCEXPDS_EXPR_DECL(suffix);
   SRCEXPDS_EXPR_DECL(prefix);
   SRCEXPDS_EXPR_DECL(multiplicative);
   SRCEXPDS_EXPR_DECL(additive);
   SRCEXPDS_EXPR_DECL(shift);
   SRCEXPDS_EXPR_DECL(relational);
   SRCEXPDS_EXPR_DECL(equality);
   SRCEXPDS_EXPR_DECL(bitwise_and);
   SRCEXPDS_EXPR_DECL(bitwise_xor);
   SRCEXPDS_EXPR_DECL(bitwise_ior);
   SRCEXPDS_EXPR_DECL(logical_and);
   SRCEXPDS_EXPR_DECL(logical_xor);
   SRCEXPDS_EXPR_DECL(logical_ior);
   SRCEXPDS_EXPR_DECL(conditional);
   SRCEXPDS_EXPR_DECL(assignment);
   SRCEXPDS_EXPR_DECL(expression);

protected:
   SourceExpressionDS(SRCEXP_EXPR_ARGS);

private:
   typedef SourceExpression::Pointer (*ExternHandler)(SRCEXPDS_EXTERN_ARGS);
   typedef std::map<std::string, ExternHandler> ExternMap;

   typedef SourceExpression::Pointer (*KeywordHandler)(SRCEXPDS_KEYWORD_ARGS);
   typedef std::map<std::string, KeywordHandler> KeywordMap;


   static void make_statements(SourceTokenizerC *in, Vector *exprs,
      Vector *blocks, SourceContext *context);

   SRCEXPDS_EXTERN_DECL(function);
   SRCEXPDS_EXTERN_DECL(script);
   SRCEXPDS_EXTERN_DECL(variable);

   SRCEXPDS_KEYWORD_DECL(break);
   SRCEXPDS_KEYWORD_DECL(cast);
   SRCEXPDS_KEYWORD_DECL(constexpr);
   SRCEXPDS_KEYWORD_DECL(continue);
   SRCEXPDS_KEYWORD_DECL(do);
   SRCEXPDS_KEYWORD_DECL(extern);
   SRCEXPDS_KEYWORD_DECL(for);
   SRCEXPDS_KEYWORD_DECL(function);
   SRCEXPDS_KEYWORD_DECL(goto);
   SRCEXPDS_KEYWORD_DECL(goto_dyn);
   SRCEXPDS_KEYWORD_DECL(if);
   SRCEXPDS_KEYWORD_DECL(library);
   SRCEXPDS_KEYWORD_DECL(linespec);
   SRCEXPDS_KEYWORD_DECL(ocode);
   SRCEXPDS_KEYWORD_DECL(output);
   SRCEXPDS_KEYWORD_DECL(printf);
   SRCEXPDS_KEYWORD_DECL(return);
   SRCEXPDS_KEYWORD_DECL(script);
   SRCEXPDS_KEYWORD_DECL(sizeof);
   SRCEXPDS_KEYWORD_DECL(switch);
   SRCEXPDS_KEYWORD_DECL(symbol);
   SRCEXPDS_KEYWORD_DECL(typestr);
   SRCEXPDS_KEYWORD_DECL(variable);
   SRCEXPDS_KEYWORD_DECL(variable_store);
   SRCEXPDS_KEYWORD_DECL(variable_type);
   SRCEXPDS_KEYWORD_DECL(while);

   static ExternMap expr_extern;
   static KeywordMap expr_keyword;
};

#endif//HPP_SourceExpressionDS_

