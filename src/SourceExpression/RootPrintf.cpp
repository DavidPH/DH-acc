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
// SourceExpression handling of "operator printf".
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_RootPrintf
//
class SourceExpression_RootPrintf : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_RootPrintf,
                                   SourceExpression);

public:
   //
   // PrintfType
   //
   enum PrintfType
   {
      PT_BOLD,
      PT_ERROR,
      PT_HUD,
      PT_HUD_BOLD,
      PT_LOG,
      PT_PRINT,
      PT_STRING
   };

   //
   // ::SourceExpression_RootPrintf
   //
   SourceExpression_RootPrintf
   (std::string const &type, std::string const &_format,
    SourceExpression::Vector const &_expressions, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), expressions(_expressions), format(_format)
   {
      expressions.push_back(NULL);

      if (type.empty() || type == "__printf_print")
         printfType = PT_PRINT;
      else if (type == "__printf_bold")
         printfType = PT_BOLD;
      else if (type == "__printf_error")
         printfType = PT_ERROR;
      else if (type == "__printf_hud")
         printfType = PT_HUD;
      else if (type == "__printf_hud_bold")
         printfType = PT_HUD_BOLD;
      else if (type == "__printf_log")
         printfType = PT_LOG;
      else if (type == "__printf_string")
         printfType = PT_STRING;
      else
         throw SourceException("unrecognized printfType", position, __func__);
   }

   //
   // ::getType
   //
   virtual VariableType const *getType() const
   {
      switch (printfType)
      {
      case PT_BOLD:
      case PT_ERROR:
      case PT_HUD:
      case PT_HUD_BOLD:
      case PT_LOG:
      case PT_PRINT:
         return VariableType::get_vt_void();

      case PT_STRING:
         return VariableType::get_vt_string();
      }

      throw SourceException("unrecognized printfType", position, __func__);
   }

private:
   //
   // ::init
   //
   void init()
   {
      expr = &expressions[0];
   }

   //
   // ::makeExpr
   //
   // Evaluates the next expression onto the stack.
   //
   void makeExpr(ObjectVector *objects, VariableType const *type)
   {
      VariableData::Pointer tmp =
         VariableData::create_stack(type->size(position));

      create_value_cast_implicit(nextExpr(type), type, context, position)
         ->makeObjects(objects, tmp);
   }

   //
   // ::makeString
   //
   // Generates a string literal and pushes its index.
   //
   void makeString(ObjectVector *objects, std::string const &string)
   {
      VariableType const *type = VariableType::get_vt_string();

      VariableData::Pointer tmp =
         VariableData::create_stack(type->size(position));

      create_value_string(string, context, position)
         ->makeObjects(objects, tmp);
   }

   //
   // ::nextExpr
   //
   SourceExpression::Pointer nextExpr(VariableType const *type)
   {
      if (!*expr) throw SourceException("insufficient arguments for printf",
                                        position, __func__);

      return create_value_cast_implicit(*expr++, type, context, position);
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      VariableData::Pointer src =
         VariableData::create_stack(getType()->size(position));

      std::string string;

      make_objects_memcpy_prep(objects, dst, src, position);

      init();

      // Start the print.
      objects->addToken(OCODE_ACSP_START);

      for (char const *c = format.c_str(); *c; ++c)
      {
         if (*c == '%' && *++c != '%')
         {
            makeString(objects, string);
            objects->addToken(OCODE_ACSP_STRING);

            string.clear();

            switch (*c)
            {
            case 'S':
               makeExpr(objects, VariableType::get_vt_string());
               objects->addToken(OCODE_ACSP_STRING);
               continue;

            case 'f':
               makeExpr(objects, VariableType::get_vt_real());
               objects->addToken(OCODE_ACSP_NUM_DEC32F);
               continue;

            case 'i':
               makeExpr(objects, VariableType::get_vt_int());
               objects->addToken(OCODE_ACSP_NUM_DEC32I);
               continue;

            case 'p':
               makeExpr(objects, VariableType::get_pointer(VariableType::get_vt_void()->constType));
               objects->addToken(OCODE_ACSP_NUM_HEX32U);
               continue;

            case 'u':
               makeExpr(objects, VariableType::get_vt_uint());
               objects->addToken(OCODE_ACSP_NUM_DEC32U);
               continue;

            default:
               throw SourceException("unrecognized printf format char",
                                     position, __func__);
            }
         }

         string += *c;
      }

      if (!string.empty())
      {
         makeString(objects, string);
         objects->addToken(OCODE_ACSP_STRING);
      }

      // Print options.
      switch (printfType)
      {
      case PT_BOLD:
      case PT_ERROR:
      case PT_LOG:
      case PT_PRINT:
      case PT_STRING:
         break;

      case PT_HUD:
      case PT_HUD_BOLD:
      {
         objects->addToken(OCODE_ACSP_START_OPT);

         ObjectExpression::Pointer msgtypeObj =
            nextExpr(VariableType::get_vt_int())->makeObject();
         bigsint msgtype = msgtypeObj->resolveInt();

         objects->addToken(OCODE_GET_LITERAL32I, msgtypeObj);
         makeExpr(objects, VariableType::get_vt_int());
         makeExpr(objects, VariableType::get_vt_int());
         makeExpr(objects, VariableType::get_vt_real());
         makeExpr(objects, VariableType::get_vt_real());
         makeExpr(objects, VariableType::get_vt_real());

         objects->addToken(OCODE_ACSP_END_OPT);

         switch (static_cast<int>(msgtype & 0xFFFF))
         {
         case 0:
            break;

         case 1:
            makeExpr(objects, VariableType::get_vt_real());
            break;

         case 2:
            makeExpr(objects, VariableType::get_vt_real());
            makeExpr(objects, VariableType::get_vt_real());
            break;

         case 3:
            makeExpr(objects, VariableType::get_vt_real());
            makeExpr(objects, VariableType::get_vt_real());
            break;

         default:
            throw SourceException("unrecognized printf hud msgtype",
                                  position, __func__);
         }
      }
         break;
      }

      // End the print.
      switch (printfType)
      {
      case PT_BOLD:
         objects->addToken(OCODE_ACSP_END_BOLD);
         break;

      case PT_ERROR:
         objects->addToken(OCODE_ACSP_END_ERROR);
         break;

      case PT_HUD:
         objects->addToken(OCODE_ACSP_END_HUD);
         break;

      case PT_HUD_BOLD:
         objects->addToken(OCODE_ACSP_END_HUD_BOLD);
         break;

      case PT_LOG:
         objects->addToken(OCODE_ACSP_END_LOG);
         break;

      case PT_PRINT:
         objects->addToken(OCODE_ACSP_END);
         break;

      case PT_STRING:
         objects->addToken(OCODE_ACSP_END_STRING);
         break;
      }

      make_objects_memcpy_post(objects, dst, src, position);
   }

   SourceExpression::Vector expressions;
   std::string format;
   SourceExpression::Pointer *expr;
   PrintfType printfType;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_printf
//
SourceExpression::Pointer SourceExpression::
create_root_printf
(std::string const &type, std::string const &format,
 SourceExpression::Vector const &expressions, SRCEXP_EXPR_ARGS)
{
   return new SourceExpression_RootPrintf
              (type, format, expressions, context, position);
}

// EOF

