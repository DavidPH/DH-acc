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

#include "SourceExpressionC.hpp"

#include "ObjectExpression.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"
#include "SourceTokenC.hpp"
#include "SourceTokenizerC.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::DeclarationSpecifiers::DeclarationSpecifiers
//
SourceExpressionC::DeclarationSpecifiers::DeclarationSpecifiers() : sc(SC_NONE), fs(0)
{
}

//
// SourceExpressionC::IsDeclarator
//
bool SourceExpressionC::IsDeclarator(SRCEXPC_PARSE_ARG1)
{
   SourceTokenC::Reference tok = in->peek();

   if(tok->type == SourceTokenC::TT_MUL)     return true;
   if(tok->type == SourceTokenC::TT_BRACK_O) return true;
   if(tok->type == SourceTokenC::TT_PAREN_O) return true;

   if(tok->type == SourceTokenC::TT_NAM && !IsType(in, context)) return true;

   return false;
}

//
// SourceExpressionC::IsType
//
bool SourceExpressionC::IsType(SRCEXPC_PARSE_ARG1)
{
   SourceTokenC::Reference tok = in->peek();

   if(tok->type != SourceTokenC::TT_NAM) return false;

   // type-specifier
   if(tok->data == "void")       return true;
   if(tok->data == "char")       return true;
   if(tok->data == "short")      return true;
   if(tok->data == "int")        return true;
   if(tok->data == "long")       return true;
   if(tok->data == "float")      return true;
   if(tok->data == "double")     return true;
   if(tok->data == "signed")     return true;
   if(tok->data == "unsigned")   return true;
   if(tok->data == "_Complex")   return true;
   if(tok->data == "_Imaginary") return true;
   if(tok->data == "_Bool")      return true;

   // struct-or-union-specifier
   if(tok->data == "struct") return true;
   if(tok->data == "union")  return true;

   // enum-specifier
   if(tok->data == "enum") return true;

   // typedef-name
   if(context->getVariableTypeNull(tok->data)) return true;

   // type-qualifier
   if(tok->data == "const")    return true;
   if(tok->data == "restrict") return true;
   if(tok->data == "volatile") return true;

   return false;
}

//
// SourceExpressionC::ParseDeclarationSpecifiers
//
SourceExpressionC::DeclarationSpecifiers SourceExpressionC::
   ParseDeclarationSpecifiers(SRCEXPC_PARSE_ARG1)
{
   SourcePosition pos = in->peek()->pos;

   DeclarationSpecifiers spec;
   VariableType::Pointer type;

   enum
   {
      // C types.
      BASE_VOID = 1, BASE_CHAR, BASE_INT, BASE_FLT, BASE_DBL, BASE_BOOL,
   };

   int numBase = 0;
   int numCplx = 0;
   int numImag = 0;
   int numLong = 0;
   int numShrt = 0;
   int numSign = 0;
   int numUnsi = 0;

   unsigned quals = 0;

   for(; in->peekType(SourceTokenC::TT_NAM); in->get())
   {
      SourceTokenC::Reference tok = in->peek();

      //
      // storage-class-specifier
      //

      #define DO_SC(DATA,SC) \
         if(tok->data == DATA) \
         { \
            if(spec.sc) Error(tok->pos, "multiple storage-class-specifier"); \
            spec.sc = SC; \
            continue; \
         }

      DO_SC("typedef",  SC_TYPEDEF);
      DO_SC("extern",   SC_EXTERN);
      DO_SC("static",   SC_STATIC);
      DO_SC("auto",     SC_AUTO);
      DO_SC("register", SC_REGISTER);

      #undef DO_SC

      //
      // type-specifier
      //

      #define DO_BASE(DATA,BASE) \
         if(tok->data == DATA) \
         { \
            if(numBase) Error(tok->pos, "multiple type-specifier"); \
            numBase = BASE; \
            continue; \
         }

      DO_BASE("void",    BASE_VOID);
      DO_BASE("char",    BASE_CHAR);
      DO_BASE("int",     BASE_INT);
      DO_BASE("float",   BASE_FLT);
      DO_BASE("double",  BASE_DBL);
      DO_BASE("_Bool",   BASE_BOOL);

      #undef DO_BASE

      if(tok->data == "_Complex")   { ++numCplx; continue; }
      if(tok->data == "_Imaginary") { ++numImag; continue; }
      if(tok->data == "long")       { ++numLong; continue; }
      if(tok->data == "short")      { ++numShrt; continue; }
      if(tok->data == "signed")     { ++numSign; continue; }
      if(tok->data == "unsigned")   { ++numUnsi; continue; }

      if(tok->data == "struct") Error(tok->pos, "stub");
      if(tok->data == "union")  Error(tok->pos, "stub");
      if(tok->data == "enum")   Error(tok->pos, "stub");

      if((type = context->getVariableTypeNull(tok->data)))
      {
         if(spec.type) Error(tok->pos, "multiple type-specifier (typedef-name)");
         spec.type = type;
         continue;
      }

      //
      // type-qualifier
      //

      #define DO_QUAL(DATA,QUAL) \
         if(tok->data == DATA) \
         { \
            quals |= VariableType::QUAL; \
            continue; \
         }

      DO_QUAL("const",    QUAL_CONST);
      DO_QUAL("restrict", QUAL_RESTRICT);
      DO_QUAL("volatile", QUAL_VOLATILE);

      #undef DO_QUAL

      //
      // function-specifier
      //

      #define DO_FS(DATA,FS) \
         if(tok->data == DATA) \
         { \
            spec.fs |= FS; \
            continue; \
         }

      DO_FS("inline", FS_INLINE);

      #undef DO_FS

      break;
   }

   if(!spec.type)
   {
      VariableType::BasicType bt;

      if(numBase == BASE_DBL) { numBase = BASE_FLT; ++numLong; }

      if(numCplx && numImag) Error_P("complex imaginary");
      if(numLong && numShrt) Error_P("long short");
      if(numSign && numUnsi) Error_P("signed unsigned");

      if(numCplx > 1) Error_P("complex complex");
      if(numImag > 1) Error_P("imaginary imaginary");
      if(numLong > 2) Error_P("long long long");
      if(numShrt > 1) Error_P("short short");
      if(numSign > 1) Error_P("signed signed");
      if(numUnsi > 1) Error_P("unsigned unsigned");

      switch(numBase)
      {
      case BASE_VOID:
         if(numCplx || numImag || numLong || numShrt || numSign || numUnsi)
            Error_P("invalid void type specification");

         bt = VariableType::BT_VOID;
         break;

      case BASE_CHAR:
         if(numCplx || numImag || numLong || numShrt)
            Error_P("invalid char type specification");

              if(numSign) bt = VariableType::BT_INT_HH;
         else if(numUnsi) bt = VariableType::BT_UNS_HH;
         else             bt = VariableType::BT_CHR;

         break;

      default:
      case BASE_INT:
         if(numCplx || numImag)
            Error_P("invalid int type specification");

         if(numUnsi)
         {
                 if(numLong == 2) bt = VariableType::BT_UNS_LL;
            else if(numLong == 1) bt = VariableType::BT_UNS_L;
            else if(numShrt == 1) bt = VariableType::BT_UNS_H;
            else                  bt = VariableType::BT_UNS;
         }
         else
         {
                 if(numLong == 2) bt = VariableType::BT_INT_LL;
            else if(numLong == 1) bt = VariableType::BT_INT_L;
            else if(numShrt == 1) bt = VariableType::BT_INT_H;
            else                  bt = VariableType::BT_INT;
         }

         break;

      case BASE_FLT:
         if(numShrt || numSign || numUnsi)
            Error_P("invalid float type specification");

              if(numLong == 2) bt = VariableType::BT_FLT_LL;
         else if(numLong == 1) bt = VariableType::BT_FLT_L;
         else                  bt = VariableType::BT_FLT;

         break;

      case BASE_BOOL:
         if(numCplx || numImag || numLong || numShrt || numSign || numUnsi)
            Error_P("invalid bool type specification");

         bt = VariableType::BT_BIT_HRD;
         break;
      }

           if(numCplx) spec.type = VariableType::get_bt_clx(bt);
      else if(numImag) spec.type = VariableType::get_bt_clx_im(bt);
      else             spec.type = VariableType::get_bt(bt);
   }
   else if(numBase || numCplx || numImag || numLong || numShrt || numSign || numUnsi)
   {
      Error_P("invalid declaration type specification");
   }

   spec.type = spec.type->setQualifier(quals);

   return spec;
}

//
// SourceExpressionC::ParseDeclarator
//
SourceExpressionC::Declarator SourceExpressionC::ParseDeclarator(
   VariableType *typeBase, SRCEXPC_PARSE_ARG1)
{
   Declarator decl;

   decl.type = typeBase;

   // * type-qualifier-list(opt)
   // * type-qualifier-list(opt) pointer
   for(;; in->get())
   {
      if(in->peekType(SourceTokenC::TT_MUL))
      {
         decl.type = decl.type->getPointer();
         continue;
      }

      if(in->peekType(SourceTokenC::TT_NAM, "const"))
      {
         decl.type = decl.type->addQualifier(VariableType::QUAL_CONST);
         continue;
      }

      if(in->peekType(SourceTokenC::TT_NAM, "restrict"))
      {
         decl.type = decl.type->addQualifier(VariableType::QUAL_RESTRICT);
         continue;
      }

      if(in->peekType(SourceTokenC::TT_NAM, "volatile"))
      {
         decl.type = decl.type->addQualifier(VariableType::QUAL_VOLATILE);
         continue;
      }

      break;
   }

   // direct-declarator

   std::vector<SourceTokenC::Reference> toks;

   // identifier
   if(in->peekType(SourceTokenC::TT_NAM))
   {
      decl.name = in->get()->data;
   }
   // ( declarator )
   else if(in->peekType(SourceTokenC::TT_PAREN_O))
   {
      SourceTokenC::Reference tok = in->get();

      if(IsDeclarator(in, context))
      {
         // Store the tokens for parsing later.

         for(int depth = 1; depth;)
         {
            toks.push_back(in->get());

            if(toks.back()->type == SourceTokenC::TT_PAREN_O) ++depth;
            if(toks.back()->type == SourceTokenC::TT_PAREN_C) --depth;
         }
      }
      else
      {
         // Haha, this is actually an abstract-declarator with function-call.

         in->unget(tok);
      }
   }
   else if(!in->peekType(SourceTokenC::TT_BRACK_O))
      Error(in->peek()->pos, "expected direct-declarator");

   // [ assignment-expression ]
   // ( parameter-type-list )
   ParseDeclaratorSuffix(decl, in, context);

   // ( declarator )
   if(!toks.empty())
   {
      for(std::vector<SourceTokenC::Reference>::iterator itr = toks.end(),
          end = toks.begin(); itr-- != end;)
      {
         in->unget(*itr);
      }

      decl = ParseDeclarator(decl.type, in, context);
      in->get(SourceTokenC::TT_PAREN_C);
   }

   return decl;
}

//
// SourceExpressionC::ParseDeclaratorSuffix
//
void SourceExpressionC::ParseDeclaratorSuffix(Declarator &decl, SRCEXPC_PARSE_ARG1)
{
   // [ assignment-expression ]
   if(in->dropType(SourceTokenC::TT_BRACK_O))
   {
      bigsint width;

      if(!in->dropType(SourceTokenC::TT_BRACK_C))
      {
         width = ParseAssignment(in, context)->makeObject()->resolveINT();
         SourceTokenC::Reference tok = in->get(SourceTokenC::TT_BRACK_C);

         if(!width) Error(tok->pos, "0 width array");
      }
      else
         width = 0;

      ParseDeclaratorSuffix(decl, in, context);

      decl.type = decl.type->getArray(width);
      decl.param.clear();
   }
   // ( parameter-type-list )
   else if(in->dropType(SourceTokenC::TT_PAREN_O))
   {
      std::vector<Parameter> param;
      VariableType::Vector types;

      if(!in->dropType(SourceTokenC::TT_PAREN_C))
      {
         // ( void )
         if(in->peekType(SourceTokenC::TT_NAM, "void"))
         {
            in->get();
         }
         // ( identifier-list )
         else if(!IsDeclaration(in, context))
         {
            Error(in->peek()->pos, "stub");
         }
         // ( parameter-type-list )
         else do
         {
            // ... )
            if(in->dropType(SourceTokenC::TT_DOT3))
            {
               types.push_back(NULL);
               break;
            }

            param.push_back(Parameter());

            // declaration-specifiers
            param.back().spec = ParseDeclarationSpecifiers(in, context);

            // declarator
            // abstract-declarator(opt)
            if(IsDeclarator(in, context))
               param.back().decl = ParseDeclarator(param.back().spec.type, in, context);
            else
               param.back().decl.type = param.back().spec.type;

            // Change function to pointer-to-function.
            if(VariableType::is_bt_function(param.back().decl.type->getBasicType()))
               param.back().decl.type = param.back().decl.type->getPointer();

            // Change array-of-T to pointer-to-T.
            if(param.back().decl.type->getBasicType() == VariableType::BT_ARR)
               param.back().decl.type = param.back().decl.type->getReturn()->getPointer();

            types.push_back(param.back().decl.type);
         }
         while(in->dropType(SourceTokenC::TT_COMMA));

         in->get(SourceTokenC::TT_PAREN_C);
      }

      ParseDeclaratorSuffix(decl, in, context);

      decl.type = VariableType::get_bt_fun(types, decl.type);
      decl.param = param;
   }
}

//
// SourceExpressionC::ParseType
//
VariableType::Reference SourceExpressionC::ParseType(SRCEXPC_PARSE_ARG1)
{
   SourcePosition pos = in->peek()->pos;

   // specifier-qualifier-list
   DeclarationSpecifiers spec = ParseDeclarationSpecifiers(in, context);

   if(spec.sc) Error_P("unexpected storage-class-specifier");
   if(spec.fs) Error_P("unexpected function-specifier");

   // abstract-declarator(opt)
   if(IsDeclarator(in, context))
   {
      Declarator decl = ParseDeclarator(spec.type, in, context);

      if(!decl.name.empty()) Error_P("expected abstract-declarator");

      spec.type = decl.type;
   }

   return static_cast<VariableType::Reference>(spec.type);
}

//
// SourceExpressionC::ParseTranslationUnit
//
SRCEXPC_PARSE_DEFN_HALF(TranslationUnit)
{
   SourcePosition pos;
   Vector exprs;

   try { pos = in->peek()->pos; } catch(SourceStream::EndOfStream const &) {}

   for(;;)
   {
      try { in->peek(); } catch(SourceStream::EndOfStream const &) { break; }

      exprs.push_back(ParseExternalDeclaration(in, context));
   }

   return create_value_block(exprs, context, pos);
}

// EOF

