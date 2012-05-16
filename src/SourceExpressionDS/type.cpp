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
#include "../SourceTokenizerC.hpp"
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
// Static Functions                                                           |
//

//
// do_qualifier
//
static void do_qualifier
(VariableType::Pointer *type, VariableType::Qualifier qual,
 SourceTokenizerC *in)
{
   SourceTokenC::Reference tokenQual = in->get(SourceTokenC::TT_NAM);

   *type = (*type)->setQualifier(qual);
}

//
// do_storage
//
static void do_storage(VariableType::Pointer *type, SourceTokenizerC *in,
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
(SourceTokenC::Reference tok, SourceTokenizerC *in)
{
   if (tok->data == "void")
      return VariableType::get_bt_void();

   if (tok->data == "bool" || tok->data == "_Bool")
      return VariableType::get_bt_bit_hrd();

   if (tok->data == "__boolsoft")
      return VariableType::get_bt_bit_sft();

   if (tok->data == "__label")
      return VariableType::get_bt_label();

   if (tok->data == "__string")
      return VariableType::get_bt_str();

   if (tok->data != "char"   && tok->data != "int"     &&
       tok->data != "float"  && tok->data != "double"  &&
       tok->data != "__real" && tok->data != "__fixed" &&
       tok->data != "short"  && tok->data != "long"    &&
       tok->data != "signed" && tok->data != "unsigned")
      return NULL;

   int typeBase  = 0;
   int typeLong  = 0;
   int typeShort = 0;
   int typeSign  = 0;

   while (true)
   {
      if (tok->data == "char" && !typeBase)
         typeBase = TYPE_BASE_CHAR;

      else if (tok->data == "int" && !typeBase)
         typeBase = TYPE_BASE_INT;

      else if (tok->data == "float" && !typeBase)
         typeBase = TYPE_BASE_FLOAT;

      else if (tok->data == "double" && !typeBase)
         typeBase = TYPE_BASE_DOUBLE;

      else if (tok->data == "__fixed" && !typeBase)
         typeBase = TYPE_BASE_FIXED;

      else if (tok->data == "__real" && !typeBase)
         typeBase = TYPE_BASE_REAL;

      else if (tok->data == "short")
         ++typeShort;

      else if (tok->data == "long")
         ++typeLong;

      else if (tok->data == "signed" && !typeSign)
         typeSign = -1;

      else if (tok->data == "unsigned" && !typeSign)
         typeSign = +1;

      else {in->unget(tok); break;}

      if (!in->peekType(SourceTokenC::TT_NAM)) break;

      tok = in->get();
   }

   // Default to int.
   if (!typeBase) typeBase = TYPE_BASE_INT;

   switch (typeBase)
   {
   case TYPE_BASE_CHAR:
      if (typeLong)
         ERROR(tok->pos, "long char");

      if (typeShort)
         ERROR(tok->pos, "short char");

      if (typeSign < 0)
         return VariableType::get_bt_int_hh();

      if (typeSign > 0)
         return VariableType::get_bt_uns_hh();

      return VariableType::get_bt_chr();

   case TYPE_BASE_INT:
      if (typeShort && typeLong)
         ERROR(tok->pos, "short long int");

      if (typeShort > 1)
         ERROR(tok->pos, "short short int");

      if (typeLong > 2)
         ERROR(tok->pos, "long long long int");

      if (typeSign > 0)
      {
         if (typeShort)
            return VariableType::get_bt_uns_h();

         if (typeLong == 1)
            return VariableType::get_bt_uns_l();

         if (typeLong == 2)
            return VariableType::get_bt_uns_ll();

         return VariableType::get_bt_uns();
      }
      else
      {
         if (typeShort)
            return VariableType::get_bt_int_h();

         if (typeLong == 1)
            return VariableType::get_bt_int_l();

         if (typeLong == 2)
            return VariableType::get_bt_int_ll();

         return VariableType::get_bt_int();
      }

   case TYPE_BASE_DOUBLE:
      ++typeLong;
   case TYPE_BASE_FLOAT:
      if (typeShort)
         ERROR(tok->pos, "short float");

      if (typeSign > 0)
         ERROR(tok->pos, "unsigned float");

      if (typeSign < 0)
         ERROR(tok->pos, "signed float");

      if (typeLong == 0)
         return VariableType::get_bt_flt();

      if (typeLong == 1)
         return VariableType::get_bt_flt_l();

      if (typeLong == 2)
         return VariableType::get_bt_flt_ll();

      ERROR(tok->pos, "long long long float");

   case TYPE_BASE_FIXED:
      if (typeShort)
         ERROR(tok->pos, "short fixed");

      if (typeLong)
         ERROR(tok->pos, "long fixed");

      if (typeSign > 0)
         ERROR(tok->pos, "unsigned fixed");

      if (typeSign < 0)
         ERROR(tok->pos, "signed fixed");

      return VariableType::get_bt_fix();

   case TYPE_BASE_REAL:
      if (typeShort)
         ERROR(tok->pos, "short real");

      if (typeLong)
         ERROR(tok->pos, "long real");

      if (typeSign > 0)
         ERROR(tok->pos, "unsigned real");

      if (typeSign < 0)
         ERROR(tok->pos, "signed real");

      return VariableType::get_bt_fix();
   }

   // Should be unreachable.
   return NULL;
}

//
// make_struct_lists
//
bool make_struct_lists(VariableType::VecStr *names,
   VariableType::Vector *types, SourceTokenizerC *in,
   SourceExpression::Vector *blocks, SourceContext *context)
{
   if (!in->peekType(SourceTokenC::TT_BRACE_O)) return false;

   VariableType::Pointer type;

   in->get(SourceTokenC::TT_BRACE_O);

   while (!in->peekType(SourceTokenC::TT_BRACE_C))
   {
      type = SourceExpressionDS::make_type(in, blocks, context);

      types->push_back(type);
      names->push_back(in->get(SourceTokenC::TT_NAM)->data);

      while (in->peekType(SourceTokenC::TT_COMMA))
      {
         in->get();
         types->push_back(type);
         names->push_back(in->get(SourceTokenC::TT_NAM)->data);
      }

      in->get(SourceTokenC::TT_SEMICOLON);
   }

   in->get(SourceTokenC::TT_BRACE_C);

   return true;
}

//
// make_struct
//
VariableType::Reference make_struct(bool isUnion, SourceTokenizerC *in,
   SourceExpression::Vector *blocks, SourceContext *context)
{
   std::string           name;
   VariableType::VecStr  names;
   VariableType::Vector  types;
   SourcePosition        pos;

   // The name must be declared for the body.
   if (in->peekType(SourceTokenC::TT_NAM))
   {
      SourceTokenC::Reference tok = in->get(SourceTokenC::TT_NAM);
      name = tok->data;
      pos  = tok->pos;

      if (isUnion)
         context->getVariableType_union(name, pos);
      else
         context->getVariableType_struct(name, pos);
   }
   else
      pos = in->peek(SourceTokenC::TT_BRACE_O)->pos;

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
VariableType::Reference SourceExpressionDS::make_type(SourceTokenizerC *in,
   Vector *blocks, SourceContext *context)
{
   SourceTokenC::Reference tok = in->peek();

   ArgList args;
   bigsint width;
   VariableType::Pointer type;

   if (tok->type == SourceTokenC::TT_PAREN_O)
   {
      in->get(SourceTokenC::TT_PAREN_O);
      type = make_type(in,blocks, context);
      in->get(SourceTokenC::TT_PAREN_C);
   }
   else
      in->get(SourceTokenC::TT_NAM);

   if (type || (type = make_basic(tok, in)) != NULL)
   {
   }
   else if (tok->data == "__asmfunc_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_asmfunc(args.types, args.retn);

      if (!args.name.empty())
         type = context->getVariableType_typedef(args.name, type, tok->pos);
   }
   else if (tok->data == "__block")
   {
      in->get(SourceTokenC::TT_BRACE_O);

      while (!in->peekType(SourceTokenC::TT_BRACE_C))
      {
         args.types.push_back(make_type(in, blocks, context));
         in->get(SourceTokenC::TT_SEMICOLON);
      }

      in->get(SourceTokenC::TT_BRACE_C);

      type = VariableType::get_bt_block(args.types);
   }
   else if (tok->data == "decltype")
   {
      type = make_prefix(in, blocks, context)->getType();
   }
   else if (tok->data == "enum")
   {
      if (in->peekType(SourceTokenC::TT_NAM))
         args.name = in->get(SourceTokenC::TT_NAM)->data;

      if (in->peekType(SourceTokenC::TT_BRACE_O))
      {
         bigsint enumVal = 0;

         type = context->getVariableType_enum(args.name, true, tok->pos);

         in->get(SourceTokenC::TT_BRACE_O);

         if (!in->peekType(SourceTokenC::TT_BRACE_C)) while (true)
         {
            SourceTokenC::Reference enumTok = in->get(SourceTokenC::TT_NAM);

            if (in->peekType(SourceTokenC::TT_EQUALS))
            {
               in->get();

               enumVal = make_assignment(in, blocks, context)
                  ->makeObject()->resolveInt();
            }

            ObjectExpression::Pointer enumObj =
               ObjectExpression::create_value_int(enumVal++, enumTok->pos);

            context->addVar(SourceVariable::create_constant
               (enumTok->data, type, enumObj, enumTok->pos), false, true);

            if (!in->peekType(SourceTokenC::TT_COMMA))
               break;

            in->get(SourceTokenC::TT_COMMA);

            if (in->peekType(SourceTokenC::TT_BRACE_C))
               break;
         }

         in->get(SourceTokenC::TT_BRACE_C);
      }
      else
         type = context->getVariableType_enum(args.name, false, tok->pos);
   }
   else if (tok->data == "__func_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_function(args.types, args.retn);

      if (!args.name.empty())
         type = context->getVariableType_typedef(args.name, type, tok->pos);
   }
   else if (tok->data == "__lnspec_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_linespec(args.types, args.retn);

      if (!args.name.empty())
         type = context->getVariableType_typedef(args.name, type, tok->pos);
   }
   else if (tok->data == "__native_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_native(args.types, args.retn);

      if (!args.name.empty())
         type = context->getVariableType_typedef(args.name, type, tok->pos);
   }
   else if (tok->data == "__script_t")
   {
      make_arglist(in, blocks, context, &args);

      type = VariableType::get_bt_script(args.types, args.retn);

      if (!args.name.empty())
         type = context->getVariableType_typedef(args.name, type, tok->pos);
   }
   else if (tok->data == "struct" || tok->data == "union")
      type = make_struct(tok->data == "union", in, blocks, context);

   else if (tok->data == "typedef")
   {
      type = make_type(in, blocks, context);
      tok = in->get(SourceTokenC::TT_NAM);
      type = context->getVariableType_typedef(tok->data, type, tok->pos);
   }
   else if (tok->data == "typename")
   {
      tok = in->get(SourceTokenC::TT_NAM);
      type = context->getVariableType(tok->data, tok->pos);
   }
   else
      type = context->getVariableType(tok->data, tok->pos);

   // Suffix modifiers.
   while (true) switch (in->peek()->type)
   {
   case SourceTokenC::TT_NAM:
      if (in->peek()->data == "const")
         do_qualifier(&type, VariableType::QUAL_CONST, in);

      else if (in->peek()->data == "volatile")
         do_qualifier(&type, VariableType::QUAL_VOLATILE, in);

      else if (in->peek()->data == "restrict")
         do_qualifier(&type, VariableType::QUAL_RESTRICT, in);

      else if (is_store(in->peek()->data))
         do_storage(&type, in, blocks, context);

      else
         return static_cast<VariableType::Reference>(type);

      break;

   case SourceTokenC::TT_MUL:
      in->get(SourceTokenC::TT_MUL);
      type = type->getPointer();
      break;

   case SourceTokenC::TT_BRACK_O:
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
            in->get(SourceTokenC::TT_BRACK_O);
            if (in->peekType(SourceTokenC::TT_BRACK_C))
               widths.push_back(0);
            else
               widths.push_back(make_expression(in, blocks, context)
                  ->makeObject()->resolveInt());
            in->get(SourceTokenC::TT_BRACK_C);
         }
         while (in->peekType(SourceTokenC::TT_BRACK_O));

         std::vector<bigsint>::size_type i = widths.size();
         while (i--) type = type->getArray(widths[i]);
      }
      else
      {
         in->get(SourceTokenC::TT_BRACK_O);
         if (in->peekType(SourceTokenC::TT_BRACK_C))
            width = 0;
         else
            width = make_expression(in, blocks, context)
               ->makeObject()->resolveInt();
         in->get(SourceTokenC::TT_BRACK_C);

         type = type->getArray(width);
      }
      break;

   default:
      return static_cast<VariableType::Reference>(type);
   }
}

// EOF

