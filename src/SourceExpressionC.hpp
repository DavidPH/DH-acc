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
// C handling/parsing.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceExpressionC_
#define HPP_SourceExpressionC_

#include "SourceExpression.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// SRCEXPC_PARSE_ARG1
//
#define SRCEXPC_PARSE_ARG1 \
   SourceTokenizerC *in, SourceContext *context

//
// SRCEXPC_PARSE_ARG2
//
#define SRCEXPC_PARSE_ARG2 \
   SRCEXPC_PARSE_ARG1, SourceExpression::Pointer expr

//
// SRCEXPC_PARSE_DECL
//
#define SRCEXPC_PARSE_DECL(NAME) \
   static SourceExpression::Pointer Parse##NAME(SRCEXPC_PARSE_ARG1); \
   static SourceExpression::Pointer Parse##NAME(SRCEXPC_PARSE_ARG2)

//
// SRCEXPC_PARSE_DECL_EXT
//
#define SRCEXPC_PARSE_DECL_EXT(NAME,...) \
   static SourceExpression::Pointer Parse##NAME(__VA_ARGS__, SRCEXPC_PARSE_ARG1)

//
// SRCEXPC_PARSE_DEFN
//
// Convenience macro for generic DEF1.
//
#define SRCEXPC_PARSE_DEFN(NAME1,NAME2) \
   SRCEXPC_PARSE_DEF1(NAME1) \
   { \
      return Parse##NAME1(in, context, Parse##NAME2(in, context)); \
   } \
   SRCEXPC_PARSE_DEF2(NAME1)

//
// SRCEXPC_PARSE_DEFN_HALF
//
// Convenience macro for stubbed DEF2.
//
#define SRCEXPC_PARSE_DEFN_HALF(NAME) \
   SRCEXPC_PARSE_DEF2(NAME) \
   { \
      (void)in; (void)context; (void)expr; \
      Error_p("stub"); \
   } \
   SRCEXPC_PARSE_DEF1(NAME)

//
// SRCEXPC_PARSE_DEF1
//
#define SRCEXPC_PARSE_DEF1(NAME) \
   SourceExpression::Pointer SourceExpressionC::Parse##NAME(SRCEXPC_PARSE_ARG1)

//
// SRCEXPC_PARSE_DEF2
//
#define SRCEXPC_PARSE_DEF2(NAME) \
   SourceExpression::Pointer SourceExpressionC::Parse##NAME(SRCEXPC_PARSE_ARG2)

//
// SRCEXPC_PARSE_DEFN_EXT
//
#define SRCEXPC_PARSE_DEFN_EXT(NAME,...) \
   SourceExpression::Pointer SourceExpressionC::Parse##NAME(__VA_ARGS__, SRCEXPC_PARSE_ARG1)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceTokenizerC;

//
// SourceExpressionC
//
class SourceExpressionC : public SourceExpression
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpressionC, SourceExpression);

public:
   struct Parameter;

   //
   // ::FunctionSpec
   //
   enum FunctionSpec
   {
      FS_INLINE = 0x00000001,
   };

   //
   // ::StorageClass
   //
   // Used to store storage-class-specifier.
   //
   enum StorageClass
   {
      SC_NONE,
      SC_TYPEDEF,
      SC_EXTERN,
      SC_STATIC,
      SC_AUTO,
      SC_REGISTER,
   };

   //
   // ::DeclarationSpecifiers
   //
   // Used to store both declaration-specifiers and specifier-qualifier-list.
   //
   struct DeclarationSpecifiers
   {
      DeclarationSpecifiers();

      CounterPointer<VariableType> type;
      StorageClass sc;
      unsigned fs;
   };

   //
   // ::Declarator
   //
   // Used for both declarator and abstract-declarator.
   //
   struct Declarator
   {
      std::string name;
      CounterPointer<VariableType> type;

      // parameter-type-list
      std::vector<Parameter> param;
   };

   //
   // Parameter
   //
   // Stores a parameter-declaration.
   //
   struct Parameter
   {
      DeclarationSpecifiers spec;
      Declarator            decl;
   };


   static bool IsDeclaration(SRCEXPC_PARSE_ARG1);
   static bool IsDeclarator(SRCEXPC_PARSE_ARG1);
   static bool IsType(SRCEXPC_PARSE_ARG1);

   static DeclarationSpecifiers ParseDeclarationSpecifiers(SRCEXPC_PARSE_ARG1);
   static Declarator ParseDeclarator(VariableType *typeBase, SRCEXPC_PARSE_ARG1);
   static void ParseDeclaratorSuffix(Declarator &decl, SRCEXPC_PARSE_ARG1);
   static SourceExpression::Pointer ParseInt(std::string const &value, SRCEXP_EXPR_ARGS);
   static CounterReference<VariableType> ParseType(SRCEXPC_PARSE_ARG1);

   SRCEXPC_PARSE_DECL(Primary);
   SRCEXPC_PARSE_DECL(Suffix);
   SRCEXPC_PARSE_DECL(Prefix);
   SRCEXPC_PARSE_DECL(Cast);
   SRCEXPC_PARSE_DECL(Multiplicative);
   SRCEXPC_PARSE_DECL(Additive);
   SRCEXPC_PARSE_DECL(Shift);
   SRCEXPC_PARSE_DECL(Relational);
   SRCEXPC_PARSE_DECL(Equality);
   SRCEXPC_PARSE_DECL(BitwiseAnd);
   SRCEXPC_PARSE_DECL(BitwiseXor);
   SRCEXPC_PARSE_DECL(BitwiseIor);
   SRCEXPC_PARSE_DECL(LogicalAnd);
   SRCEXPC_PARSE_DECL(LogicalIor);
   SRCEXPC_PARSE_DECL(Conditional);
   SRCEXPC_PARSE_DECL(Assignment);
   SRCEXPC_PARSE_DECL(Expression);

   SRCEXPC_PARSE_DECL(Statement);

   SRCEXPC_PARSE_DECL_EXT(Initializer, CounterPointer<VariableType> &type);
   SRCEXPC_PARSE_DECL_EXT(Typedef, DeclarationSpecifiers const &spec);
   SRCEXPC_PARSE_DECL(Declaration);

   SRCEXPC_PARSE_DECL(ExternalDeclaration);
   SRCEXPC_PARSE_DECL(TranslationUnit);

protected:
   //
   // ::SourceExpressionC
   //
   SourceExpressionC(SRCEXP_EXPR_PARM) : Super(SRCEXP_EXPR_PASS)
   {
   }

private:
};

#endif//HPP_SourceExpressionC_
