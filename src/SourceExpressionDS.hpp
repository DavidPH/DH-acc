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

#include <map>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define SRCEXPDS_EXPREXTERN_ARGS \
SRCEXPDS_EXPRSINGLE_ARGS, LinkageSpecifier linkSpec

#define SRCEXPDS_EXPREXTERN_DECL(NAME) \
static SourceExpression::Pointer       \
make_expression_extern_##NAME(SRCEXPDS_EXPREXTERN_ARGS)

#define SRCEXPDS_EXPREXTERN_DEFN(NAME)         \
SourceExpression::Pointer SourceExpressionDS:: \
make_expression_extern_##NAME(SRCEXPDS_EXPREXTERN_ARGS)

#define SRCEXPDS_EXPRSINGLE_ARGS                  \
SourceTokenizerDS *in, SourceTokenC const &token, \
SourceExpression::Vector *blocks, SourceContext *context

#define SRCEXPDS_EXPRSINGLE_DECL(NAME) \
static SourceExpression::Pointer       \
make_expression_single_##NAME(SRCEXPDS_EXPRSINGLE_ARGS)

#define SRCEXPDS_EXPRSINGLE_DEFN(NAME)         \
SourceExpression::Pointer SourceExpressionDS:: \
make_expression_single_##NAME(SRCEXPDS_EXPRSINGLE_ARGS)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceTokenC;
class SourceTokenizerDS;

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

   static void init();

   static SourceExpression::Pointer create_value_float
   (std::string const &value, SRCEXP_EXPR_ARGS);

   static SourceExpression::Pointer create_value_integer
   (std::string const &value, SRCEXP_EXPR_ARGS);

   static SourceExpression::Pointer make_expression
   (SourceTokenizerDS *in, Vector *blocks, SourceContext *context);

   static void make_expression_arglist
   (SourceTokenizerDS *in, Vector *blocks, SourceContext *context,
    std::vector<CounterPointer<VariableType> > *argTypes,
    CounterPointer<VariableType> *returnType,
    SourceVariable::StorageClass argClass = SourceVariable::SC_REGISTER);

   static void make_expression_arglist
   (SourceTokenizerDS *in, Vector *blocks, SourceContext *context,
    std::vector<CounterPointer<VariableType> > *argTypes,
    std::vector<std::string> *argNames, int *argCount,
    SourceContext *argContext, CounterPointer<VariableType> *returnType,
    SourceVariable::StorageClass argClass = SourceVariable::SC_REGISTER);

   static SourceExpression::Pointer make_expression_single
   (SourceTokenizerDS *in, Vector *blocks, SourceContext *context);

   static CounterReference<VariableType> make_expression_type
   (SourceTokenizerDS *in, Vector *blocks, SourceContext *context);

   static SourceExpression::Pointer make_expressions(SourceTokenizerDS *in);

   static LinkageSpecifier make_linkage_specifier(SourceTokenizerDS *in);

protected:
   SourceExpressionDS(SRCEXP_EXPR_ARGS);

private:
   typedef SourceExpression::Pointer (*ExternHandler)(SRCEXPDS_EXPREXTERN_ARGS);
   typedef std::map<std::string, ExternHandler> ExternMap;

   typedef SourceExpression::Pointer (*SingleHandler)(SRCEXPDS_EXPRSINGLE_ARGS);
   typedef std::map<std::string, SingleHandler> SingleMap;


   static bool is_expression_type
   (std::string const &data, SourceContext *context);

   static void make_expressions
   (SourceTokenizerDS *in, Vector *expressions, Vector *blocks,
    SourceContext *context);

   SRCEXPDS_EXPREXTERN_DECL(function);
   SRCEXPDS_EXPREXTERN_DECL(script);
   SRCEXPDS_EXPREXTERN_DECL(variable);

   SRCEXPDS_EXPRSINGLE_DECL(asmfunc);
   SRCEXPDS_EXPRSINGLE_DECL(break);
   SRCEXPDS_EXPRSINGLE_DECL(case);
   SRCEXPDS_EXPRSINGLE_DECL(cast);
   SRCEXPDS_EXPRSINGLE_DECL(constexpr);
   SRCEXPDS_EXPRSINGLE_DECL(continue);
   SRCEXPDS_EXPRSINGLE_DECL(default);
   SRCEXPDS_EXPRSINGLE_DECL(delay);
   SRCEXPDS_EXPRSINGLE_DECL(do);
   SRCEXPDS_EXPRSINGLE_DECL(extern);
   SRCEXPDS_EXPRSINGLE_DECL(for);
   SRCEXPDS_EXPRSINGLE_DECL(function);
   SRCEXPDS_EXPRSINGLE_DECL(goto);
   SRCEXPDS_EXPRSINGLE_DECL(goto_dyn);
   SRCEXPDS_EXPRSINGLE_DECL(if);
   SRCEXPDS_EXPRSINGLE_DECL(library);
   SRCEXPDS_EXPRSINGLE_DECL(linespec);
   SRCEXPDS_EXPRSINGLE_DECL(native);
   SRCEXPDS_EXPRSINGLE_DECL(output);
   SRCEXPDS_EXPRSINGLE_DECL(printf);
   SRCEXPDS_EXPRSINGLE_DECL(return);
   SRCEXPDS_EXPRSINGLE_DECL(script);
   SRCEXPDS_EXPRSINGLE_DECL(sizeof);
   SRCEXPDS_EXPRSINGLE_DECL(switch);
   SRCEXPDS_EXPRSINGLE_DECL(symbol);
   SRCEXPDS_EXPRSINGLE_DECL(typedef);
   SRCEXPDS_EXPRSINGLE_DECL(typestr);
   SRCEXPDS_EXPRSINGLE_DECL(variable);
   SRCEXPDS_EXPRSINGLE_DECL(variable_type);
   SRCEXPDS_EXPRSINGLE_DECL(void);
   SRCEXPDS_EXPRSINGLE_DECL(while);

   static ExternMap expr_extern;
   static SingleMap expr_single;
};

#endif//HPP_SourceExpressionDS_

