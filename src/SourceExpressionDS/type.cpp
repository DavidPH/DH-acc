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
// DS handling of types.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define TYPE_BASE_CHAR   1
#define TYPE_BASE_INT    2
#define TYPE_BASE_FLOAT  3
#define TYPE_BASE_DOUBLE 4
#define TYPE_BASE_FIXED  5
#define TYPE_BASE_REAL   6


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef SourceExpression::Pointer (*expr_make_t)
(SourceTokenizerDS *, SourceExpression::Vector * blocks,
 SourceContext * context);


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// do_qualifier
//
static void do_qualifier
(VariableType::Pointer *type, VariableType::Qualifier qual,
 SourceTokenizerDS *in)
{
   SourceTokenC tokenQual = in->get(SourceTokenC::TT_IDENTIFIER);

   if ((*type)->getQualifier(qual))
      ERROR(tokenQual.pos, "redundant qualifier");

   *type = (*type)->setQualifier(qual);
}

//
// do_storage
//
static void do_storage(VariableType::Pointer *type, SourceTokenizerDS *in,
   SourceExpression::Vector *blocks, SourceContext *context)
{
   ObjectExpression::Pointer storeArea;
   StoreType storeType = SourceExpressionDS::
      make_store(in, blocks, context, &storeArea);

   if (storeArea)
      *type = (*type)->setStorage(storeType, storeArea->resolveSymbol());
   else
      *type = (*type)->setStorage(storeType);
}

//
// make_basic
//
// Tries to parse a basic type, returning NULL on failue.
//
static VariableType::Pointer make_basic
(SourceTokenC token, SourceTokenizerDS *in)
{
   if (token.data == "void")
      return VariableType::get_bt_void();

   if (token.data == "bool" || token.data == "_Bool")
      return VariableType::get_bt_boolhard();

   if (token.data == "__boolsoft")
      return VariableType::get_bt_boolsoft();

   if (token.data == "__label")
      return VariableType::get_bt_label();

   if (token.data == "__string")
      return VariableType::get_bt_string();

   if (token.data != "char"   && token.data != "int"     &&
       token.data != "float"  && token.data != "double"  &&
       token.data != "__real" && token.data != "__fixed" &&
       token.data != "short"  && token.data != "long"    &&
       token.data != "signed" && token.data != "unsigned")
      return NULL;

   int typeBase  = 0;
   int typeLong  = 0;
   int typeShort = 0;
   int typeSign  = 0;

   while (true)
   {
      if (token.data == "char" && !typeBase)
         typeBase = TYPE_BASE_CHAR;

      else if (token.data == "int" && !typeBase)
         typeBase = TYPE_BASE_INT;

      else if (token.data == "float" && !typeBase)
         typeBase = TYPE_BASE_FLOAT;

      else if (token.data == "double" && !typeBase)
         typeBase = TYPE_BASE_DOUBLE;

      else if (token.data == "__fixed" && !typeBase)
         typeBase = TYPE_BASE_FIXED;

      else if (token.data == "__real" && !typeBase)
         typeBase = TYPE_BASE_REAL;

      else if (token.data == "short")
         ++typeShort;

      else if (token.data == "long")
         ++typeLong;

      else if (token.data == "signed" && !typeSign)
         typeSign = -1;

      else if (token.data == "unsigned" && !typeSign)
         typeSign = +1;

      else
      {
         in->unget(token);
         break;
      }

      if (!in->peekType(SourceTokenC::TT_IDENTIFIER))
         break;

      token = in->get(SourceTokenC::TT_IDENTIFIER);
   }

   // Default to int.
   if (!typeBase) typeBase = TYPE_BASE_INT;

   switch (typeBase)
   {
   case TYPE_BASE_CHAR:
      if (typeLong)
         ERROR(token.pos, "long char");

      if (typeShort)
         ERROR(token.pos, "short char");

      if (typeSign < 0)
         return VariableType::get_bt_schar();

      if (typeSign > 0)
         return VariableType::get_bt_uchar();

      return VariableType::get_bt_char();

   case TYPE_BASE_INT:
      if (typeShort && typeLong)
         ERROR(token.pos, "short long int");

      if (typeShort > 1)
         ERROR(token.pos, "short short int");

      if (typeLong > 2)
         ERROR(token.pos, "long long long int");

      if (typeSign > 0)
      {
         if (typeShort)
            return VariableType::get_bt_ushort();

         if (typeLong == 1)
            return VariableType::get_bt_ulong();

         if (typeLong == 2)
            return VariableType::get_bt_ullong();

         return VariableType::get_bt_uint();
      }
      else
      {
         if (typeShort)
            return VariableType::get_bt_short();

         if (typeLong == 1)
            return VariableType::get_bt_long();

         if (typeLong == 2)
            return VariableType::get_bt_llong();

         return VariableType::get_bt_int();
      }

   case TYPE_BASE_DOUBLE:
      ++typeLong;
   case TYPE_BASE_FLOAT:
      if (typeShort)
         ERROR(token.pos, "short float");

      if (typeSign > 0)
         ERROR(token.pos, "unsigned float");

      if (typeSign < 0)
         ERROR(token.pos, "signed float");

      if (typeLong == 0)
         return VariableType::get_bt_float();

      if (typeLong == 1)
         return VariableType::get_bt_lfloat();

      if (typeLong == 2)
         return VariableType::get_bt_llfloat();

      ERROR(token.pos, "long long long float");

   case TYPE_BASE_FIXED:
      if (typeShort)
         ERROR(token.pos, "short fixed");

      if (typeLong)
         ERROR(token.pos, "long fixed");

      if (typeSign > 0)
         ERROR(token.pos, "unsigned fixed");

      if (typeSign < 0)
         ERROR(token.pos, "signed fixed");

      return VariableType::get_bt_fixed();

   case TYPE_BASE_REAL:
      if (typeShort)
         ERROR(token.pos, "short real");

      if (typeLong)
         ERROR(token.pos, "long real");

      if (typeSign > 0)
         ERROR(token.pos, "unsigned real");

      if (typeSign < 0)
         ERROR(token.pos, "signed real");

      return VariableType::get_bt_real();
   }

   // Should be unreachable.
   return NULL;
}

//
// make_struct_lists
//
bool make_struct_lists
(VariableType::VecStr *names, VariableType::Vector *types,
 SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context)
{
   if (!in->peekType(SourceTokenC::TT_OP_BRACE_O)) return false;

   VariableType::Pointer type;

   in->get(SourceTokenC::TT_OP_BRACE_O);

   while (!in->peekType(SourceTokenC::TT_OP_BRACE_C))
   {
      type = SourceExpressionDS::make_type(in, blocks, context);

      types->push_back(type);
      names->push_back(in->get(SourceTokenC::TT_IDENTIFIER).data);

      while (in->peekType(SourceTokenC::TT_OP_COMMA))
      {
         in->get(SourceTokenC::TT_OP_COMMA);
         types->push_back(type);
         names->push_back(in->get(SourceTokenC::TT_IDENTIFIER).data);
      }

      in->get(SourceTokenC::TT_OP_SEMICOLON);
   }

   in->get(SourceTokenC::TT_OP_BRACE_C);

   return true;
}

//
// make_struct
//
VariableType::Reference make_struct
(bool isUnion, SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context)
{
   std::string           name;
   VariableType::VecStr  names;
   VariableType::Vector  types;
   SourcePosition        pos;

   // The name must be declared for the body.
   if (in->peekType(SourceTokenC::TT_IDENTIFIER))
   {
      SourceTokenC const &token = in->get(SourceTokenC::TT_IDENTIFIER);
      name = token.data;
      pos  = token.pos;

      if (isUnion)
         context->getVariableType_union(name, token.pos);
      else
         context->getVariableType_struct(name, token.pos);
   }
   else
      pos = in->peek(SourceTokenC::TT_OP_BRACE_O).pos;

   if (isUnion)
   {
      if (make_struct_lists(&names, &types, in, blocks, context))
         return context->getVariableType_union(name, names, types, pos);
      else
         return context->getVariableType_union(name, pos);
   }
   else
   {
      if (make_struct_lists(&names, &types, in, blocks, context))
         return context->getVariableType_struct(name, names, types, pos);
      else
         return context->getVariableType_struct(name, pos);
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::is_type
//
bool SourceExpressionDS::is_type(std::string const &data,
   SourceContext *context)
{
   if (data == "void")
      return true;

   if (data == "bool" || data == "_Bool")
      return true;

   if (data == "__boolsoft")
      return true;

   if (data == "__label")
      return true;

   if (data == "__string")
      return true;

   if (data == "char"   || data == "int"     ||
       data == "float"  || data == "double"  ||
       data == "__real" || data == "__fixed" ||
       data == "short"  || data == "long"    ||
       data == "signed" || data == "unsigned")
      return true;

   if (data == "__asmfunc_t")return true;
   if (data == "__block")    return true;
   if (data ==   "decltype") return true;
   if (data ==   "enum")     return true;
   if (data == "__func_t")   return true;
   if (data == "__lnspec_t") return true;
   if (data == "__native_t") return true;
   if (data == "__script_t") return true;
   if (data ==   "struct")   return true;
   if (data ==   "typedef")  return true;
   if (data ==   "typename") return true;
   if (data ==   "union")    return true;

   return context->getVariableTypeNull(data);
}

//
// SourceExpressionDS::make_type
//
VariableType::Reference SourceExpressionDS::make_type(SourceTokenizerDS *in,
   Vector *blocks, SourceContext *context)
{
   SourceTokenC tok;

   ArgList args;

   bigsint width;
   std::string name;
   VariableType::Pointer type;

   if (in->peekType(SourceTokenC::TT_OP_PARENTHESIS_O))
   {
      in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
      type = make_type(in,blocks, context);
      in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
   }
   else
   {
      tok = in->get(SourceTokenC::TT_IDENTIFIER);
   }

   if (type || (type = make_basic(tok, in)) != NULL)
   {
   }
   else if (tok.data == "__asmfunc_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_asmfunc(args.types, args.retn);
   }
   else if (tok.data == "__block")
   {
      in->get(SourceTokenC::TT_OP_BRACE_O);

      while (true)
      {
         if (in->peekType(SourceTokenC::TT_OP_BRACE_C))
         break;

         args.types.push_back(make_type(in, blocks, context));

         in->get(SourceTokenC::TT_OP_SEMICOLON);
      }

      in->get(SourceTokenC::TT_OP_BRACE_C);

      type = VariableType::get_bt_block(args.types);
   }
   else if (tok.data == "decltype")
   {
      type = make_prefix(in, blocks, context)->getType();
   }
   else if (tok.data == "enum")
   {
      if (in->peekType(SourceTokenC::TT_IDENTIFIER))
         name = in->get(SourceTokenC::TT_IDENTIFIER).data;

      if (in->peekType(SourceTokenC::TT_OP_BRACE_O))
      {
         bigsint enumVal = 0;

         type = context->getVariableType_enum(name, true, tok.pos);

         in->get(SourceTokenC::TT_OP_BRACE_O);

         if (!in->peekType(SourceTokenC::TT_OP_BRACE_C)) while (true)
         {
            SourceTokenC enumTok = in->get(SourceTokenC::TT_IDENTIFIER);

            if (in->peekType(SourceTokenC::TT_OP_EQUALS))
            {
               in->get(SourceTokenC::TT_OP_EQUALS);

               enumVal = make_assignment(in, blocks, context)
                  ->makeObject()->resolveInt();
            }

            ObjectExpression::Pointer enumObj =
               ObjectExpression::create_value_int(enumVal++, enumTok.pos);

            context->addVar(SourceVariable::create_constant
               (enumTok.data, type, enumObj, enumTok.pos), false, true);

            if (!in->peekType(SourceTokenC::TT_OP_COMMA))
               break;

            in->get(SourceTokenC::TT_OP_COMMA);

            if (in->peekType(SourceTokenC::TT_OP_BRACE_C))
               break;
         }

         in->get(SourceTokenC::TT_OP_BRACE_C);
      }
      else
         type = context->getVariableType_enum(name, false, tok.pos);
   }
   else if (tok.data == "__func_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_function(args.types, args.retn);
   }
   else if (tok.data == "__lnspec_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_linespec(args.types, args.retn);
   }
   else if (tok.data == "__native_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_native(args.types, args.retn);
   }
   else if (tok.data == "__script_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_script(args.types, args.retn);
   }
   else if (tok.data == "struct" || tok.data == "union")
      type = make_struct(tok.data == "union", in, blocks, context);

   else if (tok.data == "typedef")
   {
      type = make_type(in, blocks, context);
      tok = in->get(SourceTokenC::TT_IDENTIFIER);
      type = context->getVariableType_typedef(tok.data, type, tok.pos);
   }
   else if (tok.data == "typename")
   {
      tok = in->get(SourceTokenC::TT_IDENTIFIER);
      type = context->getVariableType(tok.data, tok.pos);
   }
   else
      type = context->getVariableType(tok.data, tok.pos);

   // Suffix modifiers.
   while (true) switch (in->peek().type)
   {
   case SourceTokenC::TT_IDENTIFIER:
      if (in->peek().data == "const")
         do_qualifier(&type, VariableType::QUAL_CONST, in);

      else if (in->peek().data == "volatile")
         do_qualifier(&type, VariableType::QUAL_VOLATILE, in);

      else if (in->peek().data == "restrict")
         do_qualifier(&type, VariableType::QUAL_RESTRICT, in);

      else if (is_store(in->peek().data))
         do_storage(&type, in, blocks, context);

      else
         return type.ref();

      break;

   case SourceTokenC::TT_OP_ASTERISK:
      in->get(SourceTokenC::TT_OP_ASTERISK);
      type = type->getPointer();
      break;

   case SourceTokenC::TT_OP_BRACKET_O:
      // Time for type-fucking. This should be an option, but who'd use it?
      //
      // I want to make it clear that I think this syntax is at best completely
      // backwards to all other type-related syntax. As easily demonstrated by
      // using typedefs in place of directly using the multiple dimensions.
      //
      // And for what? The one benefit is that you specify the sizes in the
      // same order as the indexes. In exchange you open things up to subtle
      // syntax eccentricities and inconsistencies. Such as the typedef
      // scenario mentioned before. These two array types are not the same:
      //
      //   typedef int[1] arr;
      //   int[1][2]; // 1 of int[2]
      //   arr[2];    // 2 of int[1]
      //
      // Why is that a problem? Because they don't appear to be different. (I
      // certainly don't see them as being different, and that may be telling
      // of where I'm coming from.) They seem to get their sizes in the same
      // order, and yet they are two very different types. This results in a
      // more complicated syntax specification for a highly superficial gain.
      //
      // And the "intuitive" ordering only works when not put under scrutiny.
      // The [] operator applies to the expression on the left. The type
      // specifier does not. It applies either to the expression to the left or
      // to the array specifer to the right. Thus the specifiers must be taken
      // as a whole to be one specifier, which is semantically different from
      // the operator it's supposed to be similar to!
      //
      //   int[1][2]; // These are different...
      //   (int[1])[2];
      //
      //   __variable auto int[2][3] arr;
      //   arr[1][2]; // ... And yet, these are the same.
      //   (arr[1])[2];
      //
      //   __variable auto int[3] ar2 = arr[1];
      //   ar2[2]; // And this is still the same, unlike the typedef case.
      //
      // However, at some point in all this I have to explain why I'm putting
      // up with it. There are two basic reasons. Firstly, and the most gross,
      // is that this is how it's done. "Familiar is intuitive." Secondly, all
      // of the above only applies from a type-centric perspective. As in, the
      // one that you'd have while writing this piece of code. If you're not
      // thinking of it as arbitrary types being affected by arbitrary type
      // specifiers, then the point is lost.
      //
      // In short, I don't like it, but I'll do it anyway.
      //
      if (true)
      {
         std::vector<bigsint> widths;
         do
         {
            in->get(SourceTokenC::TT_OP_BRACKET_O);
            widths.push_back(make_expression(in, blocks, context)->makeObject()->resolveInt());
            in->get(SourceTokenC::TT_OP_BRACKET_C);
         }
         while (in->peekType(SourceTokenC::TT_OP_BRACKET_O));

         std::vector<bigsint>::size_type i = widths.size();
         while (i--) type = type->getArray(widths[i]);
      }
      else
      {
         in->get(SourceTokenC::TT_OP_BRACKET_O);
         width = make_expression(in, blocks, context)->makeObject()->resolveInt();
         in->get(SourceTokenC::TT_OP_BRACKET_C);

         type = type->getArray(width);
      }
      break;

   default:
      return type.ref();
   }
}

// EOF

