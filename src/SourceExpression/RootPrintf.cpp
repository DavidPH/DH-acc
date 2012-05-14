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
         ERROR_NP("unrecognized printfType: %s", type.c_str());
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      switch (printfType)
      {
      case PT_BOLD:
      case PT_ERROR:
      case PT_HUD:
      case PT_HUD_BOLD:
      case PT_LOG:
      case PT_PRINT:
         return VariableType::get_bt_void();

      case PT_STRING:
         return VariableType::get_bt_str();
      }

      ERROR_NP("unrecognized printfType");
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
   // ::makeChar
   //
   void makeChar(ObjectVector *objects, char c)
   {
      VariableType::Reference type = VariableType::get_bt_chr();

      VariableData::Pointer tmp =
         VariableData::create_stack(type->getSize(pos));

      create_value_char(c, context, pos)->makeObjects(objects, tmp);
   }

   //
   // ::makeExpr
   //
   // Evaluates the next expression onto the stack.
   //
   void makeExpr(ObjectVector *objects, VariableType *type)
   {
      VariableData::Pointer tmp =
         VariableData::create_stack(type->getSize(pos));

      nextExpr(type)->makeObjects(objects, tmp);
   }

   //
   // ::makeInt
   //
   // Generates an int literal and pushes it.
   //
   void makeInt(ObjectVector *objects, bigsint i)
   {
      VariableType::Reference type = VariableType::get_bt_int();

      VariableData::Pointer tmp =
         VariableData::create_stack(type->getSize(pos));

      create_value_int(i, context, pos)->makeObjects(objects, tmp);
   }

   //
   // ::makeString
   //
   // Generates a string literal and pushes its index.
   //
   void makeString(ObjectVector *objects, std::string const &s)
   {
      VariableType::Reference type = VariableType::get_bt_str();

      VariableData::Pointer tmp =
         VariableData::create_stack(type->getSize(pos));

      create_value_string(s, context, pos)->makeObjects(objects, tmp);
   }

   //
   // ::nextExpr
   //
   SourceExpression::Pointer nextExpr()
   {
      if (!*expr) ERROR_NP("insufficient arguments");

      return *expr++;
   }

   //
   // ::nextExpr
   //
   SourceExpression::Pointer nextExpr(VariableType *type)
   {
      return create_value_cast_implicit(nextExpr(), type, context, pos);
   }

   //
   // ::makeFormat_p
   //
   void makeFormat_p(ObjectVector *objects)
   {
      SourceExpression::Pointer argExpr = nextExpr();
      VariableType::Reference   argType = argExpr->getType();
      VariableType::BasicType   argBT   = argType->getBasicType();
      bigsint                   argSize = argType->getSize(pos);

      if (argBT == VariableType::BT_ARR)
      {
         argBT = VariableType::BT_PTR;
         argType = argType->getReturn()->getPointer();
         argExpr = create_value_cast_implicit(argExpr, argType, context, pos);
         argSize = argType->getSize(pos);
      }

      if (argBT != VariableType::BT_PTR)
         ERROR_NP("expected pointer got %s", make_string(argType).c_str());

      argExpr->makeObjects(objects, VariableData::create_stack(argSize));
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
   }

   //
   // ::makeFormat_s
   //
   void makeFormat_s(ObjectVector *objects)
   {
      SourceExpression::Pointer argExpr = nextExpr();
      VariableType::Reference   argType = argExpr->getType();
      VariableType::BasicType   argBT   = argType->getBasicType();

      VariableData::Pointer tmp =
         VariableData::create_stack(argType->getSize(pos));

      if (argBT == VariableType::BT_STR)
      {
         argExpr->makeObjects(objects, tmp);
         objects->addToken(OCODE_ACSP_STR);
         return;
      }

      if (argBT == VariableType::BT_ARR)
      {
         argBT = VariableType::BT_PTR;
         argType = argType->getReturn()->getPointer();
         argExpr = create_value_cast_implicit(argExpr, argType, context, pos);
      }

      if (argBT != VariableType::BT_PTR)
         ERROR_NP("expected pointer got %s", make_string(argType).c_str());

      // Convert auto* to static*.
      StoreType argStore = argType->getReturn()->getStoreType();
      if (argStore == STORE_AUTO) argStore = STORE_STATIC;

      argType = argType->getReturn();
      argType = VariableType::get_bt_chr()
                ->setQualifier(argType->getQualifiers())
                ->setStorage(argStore, argType->getStoreArea())
                ->getPointer();
      argExpr = create_value_cast_implicit(argExpr, argType, context, pos);

      argExpr->makeObjects(objects, tmp);

      switch (argType->getReturn()->getStoreType())
      {
      case STORE_STATIC:
         makeInt(objects, option_addr_array);
         objects->addToken(OCODE_ACSP_STR_GBLARR);
         break;

      case STORE_AUTO:
      case STORE_CONST:
         ERROR_NP("cannot %%s pointer");

      case STORE_REGISTER:
      case STORE_MAPREGISTER:
      case STORE_WORLDREGISTER:
      case STORE_GLOBALREGISTER:
         ERROR_NP("cannot %%s register-pointer");

      case STORE_MAPARRAY:
         objects->addToken(OCODE_GET_IMM,
            objects->getValue(argType->getReturn()->getStoreArea()));
         objects->addToken(OCODE_ACSP_STR_MAPARR);
         break;

      case STORE_WORLDARRAY:
         objects->addToken(OCODE_GET_IMM,
            objects->getValue(argType->getReturn()->getStoreArea()));
         objects->addToken(OCODE_ACSP_STR_WLDARR);
         break;

      case STORE_GLOBALARRAY:
         objects->addToken(OCODE_GET_IMM,
            objects->getValue(argType->getReturn()->getStoreArea()));
         objects->addToken(OCODE_ACSP_STR_GBLARR);
         break;
      }
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      VariableType::Reference type = getType();
      VariableData::Pointer   src  =
         VariableData::create_stack(type->getSize(pos));

      std::string string;

      make_objects_memcpy_prep(objects, dst, src, pos);

      init();

      // Start the print.
      objects->addToken(OCODE_ACSP_START);

      for (char const *c = format.c_str(); *c; ++c)
      {
         if (*c == '%' && *++c != '%')
         {
            makeString(objects, string);
            objects->addToken(OCODE_ACSP_STR);

            string.clear();

            switch (*c)
            {
            case 'K':
               makeExpr(objects, VariableType::get_bt_int());
               objects->addToken(OCODE_ACSP_KEYBIND);
               continue;

            case 'L':
               makeExpr(objects, VariableType::get_bt_int());
               objects->addToken(OCODE_ACSP_STR_LOCALIZED);
               continue;

            case 'N':
               makeExpr(objects, VariableType::get_bt_int());
               objects->addToken(OCODE_ACSP_PLAYER_NAME);
               continue;

            case 'X':
               makeExpr(objects, VariableType::get_bt_fix());
               objects->addToken(OCODE_ACSP_NUM_DEC_X);
               continue;

            case 'c':
               makeExpr(objects, VariableType::get_bt_chr());
               objects->addToken(OCODE_ACSP_CHARACTER);
               continue;

            case 'i':
               makeExpr(objects, VariableType::get_bt_int());
               objects->addToken(OCODE_ACSP_NUM_DEC_I);
               continue;

            case 'p':
               makeFormat_p(objects);
               continue;

            case 's':
               makeFormat_s(objects);
               continue;

            case 'u':
               makeExpr(objects, VariableType::get_bt_uns());
               objects->addToken(OCODE_ACSP_NUM_DEC_U);
               continue;

            default:
               ERROR_NP("unrecognized format char: %c", *c);
            }
         }

         string += *c;
      }

      if (string.size() == 1)
      {
         makeChar(objects, string[0]);
         objects->addToken(OCODE_ACSP_CHARACTER);
      }
      else if (!string.empty())
      {
         makeString(objects, string);
         objects->addToken(OCODE_ACSP_STR);
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
            nextExpr(VariableType::get_bt_int())->makeObject();
         bigsint msgtype = msgtypeObj->resolveInt();

         objects->addToken(OCODE_GET_IMM, msgtypeObj);
         makeExpr(objects, VariableType::get_bt_int());
         makeExpr(objects, VariableType::get_bt_int());
         makeExpr(objects, VariableType::get_bt_fix());
         makeExpr(objects, VariableType::get_bt_fix());
         makeExpr(objects, VariableType::get_bt_fix());

         objects->addToken(OCODE_ACSP_END_OPT);

         switch (static_cast<int>(msgtype & 0xFFFF))
         {
         case 0:
            break;

         case 1:
            makeExpr(objects, VariableType::get_bt_fix());
            break;

         case 2:
            makeExpr(objects, VariableType::get_bt_fix());
            makeExpr(objects, VariableType::get_bt_fix());
            break;

         case 3:
            makeExpr(objects, VariableType::get_bt_fix());
            makeExpr(objects, VariableType::get_bt_fix());
            break;

         default:
            ERROR_NP("unrecognized hud msgtype");
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
         objects->addToken(OCODE_ACSP_END_STR);
         break;
      }

      make_objects_memcpy_post(objects, dst, src, type, context, pos);
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
      (type, format, expressions, context, pos);
}

// EOF

