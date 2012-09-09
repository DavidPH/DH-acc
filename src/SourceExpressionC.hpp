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

#include "ObjectCode.hpp"
#include "ObjectData.hpp"
#include "SourceExpression.hpp"
#include "VariableType.hpp"


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
   // DeclarationSpecifiers
   //
   // Used to store both declaration-specifiers and specifier-qualifier-list.
   //
   struct DeclarationSpecifiers
   {
      //
      // DeclarationSpecifiers
      //
      DeclarationSpecifiers() : storage(SC_NONE), external(false),
         functionInline(false)
      {
      }

      //
      // hasFunctionSpecifier
      //
      // Checks if this has any function-specifier.
      //
      bool hasFunctionSpecifier() const
      {
         return functionInline;
      }

      //
      // isSpecifierQualifier
      //
      // Checks that this is a specifier-qualifier-list.
      //
      bool isSpecifierQualifier() const
      {
         return !storage && !hasFunctionSpecifier();
      }

      CounterPointer<VariableType> type;
      StorageClass storage;

      bool external : 1; // external-declaration

      // function-specifier
      bool functionInline : 1;
   };

   //
   // FunctionAttributes
   //
   struct FunctionAttributes
   {
      typedef ObjectData_Script::ScriptType ScriptType;

      FunctionAttributes()
       : scriptAddr(-1), scriptFlag(0), scriptType(ObjectData_Script::ST_CLOSED),
         asmfun(false), lnspec(false), native(false), script(false)
      {
      }

      ObjectCodeSet asmfunCode;
      std::string   scriptName;
      bigsint       scriptAddr;
      bigsint       scriptFlag;
      ScriptType    scriptType;
      bool          asmfun : 1;
      bool          lnspec : 1;
      bool          native : 1;
      bool          script : 1;
   };

   //
   // Declarator
   //
   // Used for both declarator and abstract-declarator.
   //
   struct Declarator
   {
      std::string name;
      CounterPointer<VariableType> type;

      // parameter-type-list
      std::vector<Parameter> param;

      // function-attributes
      FunctionAttributes funcAttr;
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

   //
   // Qualifier
   //
   // Stores a type-qualifier.
   //
   struct Qualifier
   {
      Qualifier() : quals(0), storeType(STORE_NONE), store(false) {}
      Qualifier(unsigned _quals) : quals(_quals), storeType(STORE_NONE), store(false) {}
      Qualifier(StoreType _storeType) : quals(0), storeType(_storeType), store(true) {}
      Qualifier(StoreType _storeType, std::string const &_storeArea)
       : quals(0), storeType(_storeType), storeArea(_storeArea), store(true) {}

      unsigned    quals;
      StoreType   storeType;
      std::string storeArea;
      bool        store : 1;
   };


   static bool IsDeclaration(SRCEXPC_PARSE_ARG1);
   static bool IsDeclarator(SRCEXPC_PARSE_ARG1);
   static bool IsQualifier(SRCEXPC_PARSE_ARG1);
   static bool IsType(SRCEXPC_PARSE_ARG1);

   // Attributes.
   static void ParseAttributeAsmfunc(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1);
   static void ParseAttributeLinespec(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1);
   static void ParseAttributeNative(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1);
   static void ParseAttributeScript(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1);
   static FunctionAttributes ParseFunctionAttributes(SRCEXPC_PARSE_ARG1);

   static CounterReference<VariableType> ParseEnum(SRCEXPC_PARSE_ARG1);
   static CounterReference<VariableType> ParseStruct(SRCEXPC_PARSE_ARG1);
   static DeclarationSpecifiers ParseDeclarationSpecifiers(SRCEXPC_PARSE_ARG1);
   static Declarator ParseDeclarator(VariableType *typeBase, SRCEXPC_PARSE_ARG1);
   static void ParseDeclaratorSuffix(Declarator &decl, SRCEXPC_PARSE_ARG1);
   static SourceExpression::Pointer ParseFloat(std::string const &value, SRCEXP_EXPR_ARGS);
   static SourceExpression::Pointer ParseInt(std::string const &value, SRCEXP_EXPR_ARGS);
   static Qualifier ParseQualifier(SRCEXPC_PARSE_ARG1);
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

   SRCEXPC_PARSE_DECL(AddressSpace);
   SRCEXPC_PARSE_DECL_EXT(Initializer, CounterPointer<VariableType> &type, bool root);
   SRCEXPC_PARSE_DECL_EXT(InitDeclarator, DeclarationSpecifiers const &spec);
   SRCEXPC_PARSE_DECL_EXT(InitDeclarator, DeclarationSpecifiers const &spec, Declarator &decl);
   SRCEXPC_PARSE_DECL(Library);
   SRCEXPC_PARSE_DECL(StaticAssert);
   SRCEXPC_PARSE_DECL_EXT(Typedef, DeclarationSpecifiers const &spec);
   SRCEXPC_PARSE_DECL_EXT(Declaration, bool external);
   SRCEXPC_PARSE_DECL(Declaration);

   SRCEXPC_PARSE_DECL_EXT(Function, DeclarationSpecifiers const &spec, Declarator &decl);
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

