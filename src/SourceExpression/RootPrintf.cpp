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
   // FormatFlag
   //
   enum FormatFlag
   {
      FF_LEFT = 0x0001,
      FF_SIGN = 0x0002,
      FF_PADS = 0x0004,
      FF_ALTF = 0x0008,
      FF_PAD0 = 0x0010,
   };

   //
   // FormatLength
   //
   enum FormatLength
   {
      FL_NONE,
      FL_HH,
      FL_H,
      FL_L,
      FL_LL,
      FL_MAX,
      FL_SIZE,
      FL_DIFF,
   };

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
   // FormatData
   //
   struct FormatData
   {
      unsigned     flags;
      bigsint      width;
      bigsint      prec;
      FormatLength len;
      char         fmt;
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
         Error_NP("unrecognized printfType: %s", type.c_str());
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

      Error_NP("unrecognized printfType");
   }

private:
   //
   // ::doFormat
   //
   void doFormat(ObjectVector *objects, FormatData &data)
   {
      switch(data.fmt)
      {
      case 'B':
         makeExpr(objects, VariableType::get_bt_uns());
         objects->addToken(OCODE_ACSP_NUM_BIN_U);
         break;

      case 'I':
         makeExpr(objects, VariableType::get_bt_fix());
         objects->addToken(OCODE_ACSP_NUM_DEC_X);
         break;

      case 'K':
         makeExpr(objects, VariableType::get_bt_str());
         objects->addToken(OCODE_ACSP_KEYBIND);
         break;

      case 'S':
         makeExpr(objects, VariableType::get_bt_int());
         objects->addToken(OCODE_ACSP_STR_LOCALIZED);
         break;

      case 'N':
         makeExpr(objects, VariableType::get_bt_int());
         objects->addToken(OCODE_ACSP_PLAYER_NAME);
         break;

      case 'c':
         makeExpr(objects, VariableType::get_bt_chr());
         objects->addToken(OCODE_ACSP_CHARACTER);
         break;

      case 'd':
      case 'i':
         makeExpr(objects, VariableType::get_bt_int());
         objects->addToken(OCODE_ACSP_NUM_DEC_I);
         break;

      case 'p':
         doFormat_p(objects, data);
         break;

      case 's':
         doFormat_s(objects, data);
         break;

      case 'u':
         makeExpr(objects, VariableType::get_bt_uns());
         objects->addToken(OCODE_ACSP_NUM_DEC_I);
         break;

      case 'x':
      case 'X':
         doFormat_x(objects, data);
         break;

      default:
         Error_NP("unrecognized format char: %c", data.fmt);
      }
   }

   //
   // ::doFormat_p
   //
   void doFormat_p(ObjectVector *objects, FormatData &)
   {
      SourceExpression::Pointer argExpr = nextExpr();
      VariableType::Reference   argType = argExpr->getType();
      VariableType::BasicType   argBT   = argType->getBasicType();
      bigsint                   argSize = argType->getSize(pos);

      if(argBT == VariableType::BT_ARR)
      {
         argBT = VariableType::BT_PTR;
         argType = argType->getReturn()->getPointer();
         argExpr = create_value_cast_implicit(argExpr, argType, context, pos);
         argSize = argType->getSize(pos);
      }

      if(argType->getReturn()->getStoreType() == STORE_AUTO)
      {
         argType = argType->getReturn()->setStorage(STORE_STATIC)->getPointer();
         argExpr = create_value_cast_implicit(argExpr, argType, context, pos);
      }

      if(argBT != VariableType::BT_PTR)
         Error_NP("expected pointer got %s", make_string(argType).c_str());

      argExpr->makeObjects(objects, VariableData::create_stack(argSize));

      if(argSize == 1)
         objects->addToken(OCODE_ACSP_NUM_HEX_U);
      else
      {
         objects->addToken(OCODE_STK_SWAP);
         objects->addToken(OCODE_ACSP_NUM_HEX_U);
         makeChar(objects, '-');
         objects->addToken(OCODE_ACSP_CHARACTER);
         objects->addToken(OCODE_ACSP_NUM_HEX_U);
      }
   }

   //
   // ::doFormat_s
   //
   void doFormat_s(ObjectVector *objects, FormatData &)
   {
      SourceExpression::Pointer argExpr = nextExpr();
      VariableType::Reference   argType = argExpr->getType();
      VariableType::BasicType   argBT   = argType->getBasicType();

      VariableData::Pointer tmp =
         VariableData::create_stack(argType->getSize(pos));

      if(argBT == VariableType::BT_STR)
      {
         argExpr->makeObjects(objects, tmp);
         objects->addToken(OCODE_ACSP_STR);
         return;
      }

      if(argBT == VariableType::BT_ARR)
      {
         argBT = VariableType::BT_PTR;
         argType = argType->getReturn()->getPointer();
         argExpr = create_value_cast_implicit(argExpr, argType, context, pos);
      }

      if (argBT != VariableType::BT_PTR)
         Error_NP("expected pointer got %s", make_string(argType).c_str());

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

      case STORE_NONE:
      case STORE_AUTO:
      case STORE_CONST:
      case STORE_STRING:
         Error_NP("cannot %%s pointer");

      case STORE_REGISTER:
      case STORE_MAPREGISTER:
      case STORE_WORLDREGISTER:
      case STORE_GLOBALREGISTER:
         Error_NP("cannot %%s register-pointer");

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
   // ::doFormat_x
   //
   void doFormat_x(ObjectVector *objects, FormatData &data)
   {
      switch(data.len)
      {
      case FL_L:
      case FL_LL:
      case FL_MAX:
         makeExpr(objects, VariableType::get_bt_uns_ll());
         makeData(objects, data);
         objects->addToken(OCODE_JMP_CAL_NIL_IMM, objects->getValue("__Print_lx"));
         break;

      default:
         if(data.flags || data.width || data.prec || data.fmt == 'x')
         {
            makeExpr(objects, VariableType::get_bt_uns());
            makeData(objects, data);
            objects->addToken(OCODE_JMP_CAL_NIL_IMM, objects->getValue("__Print_x"));
         }
         else
         {
            makeExpr(objects, VariableType::get_bt_uns());
            objects->addToken(OCODE_ACSP_NUM_HEX_U);
         }
         break;
      }
   }

   //
   // ::doFormatLiteral
   //
   void doFormatLiteral(ObjectVector *objects, std::string &s)
   {
      if(s.size() == 1)
      {
         makeChar(objects, s[0]);
         objects->addToken(OCODE_ACSP_CHARACTER);
      }
      else if(!s.empty())
      {
         makeString(objects, s);
         objects->addToken(OCODE_ACSP_STR);
      }

      s.clear();
   }

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
   // ::makeData
   //
   // Pushes data members to the stack for an extended print call.
   //
   void makeData(ObjectVector *objects, FormatData &data)
   {
      makeInt(objects, data.flags);

      if(data.width == -1)
         makeExpr(objects, VariableType::get_bt_int());
      else
         makeInt(objects, data.width);

      if(data.prec == -1)
         makeExpr(objects, VariableType::get_bt_int());
      else
         makeInt(objects, data.prec);

      makeInt(objects, data.fmt);
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
      if (!*expr) Error_NP("insufficient arguments");

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

      for(char const *c = format.c_str(); *c;)
      {
         // If not a % (or if it's a %%), just add to output.
         if(*c != '%' || *++c == '%')
         {
            string += *c++;
            continue;
         }

         FormatData data;

         // Otherwise, handle the current literal and start reading format.
         doFormatLiteral(objects, string);

         // Read flags, if any.
         for(data.flags = 0; *c; ++c)
         {
            switch(*c)
            {
            case '-': data.flags |= FF_LEFT; continue;
            case '+': data.flags |= FF_SIGN; continue;
            case ' ': data.flags |= FF_PADS; continue;
            case '#': data.flags |= FF_ALTF; continue;
            case '0': data.flags |= FF_PAD0; continue;
            }

            break;
         }

         // Read field width, if any.
         if(*c == '*')
            data.width = -1, ++c;
         else for(data.width = 0; std::isdigit(*c);)
            data.width = (data.width * 10) + (*c++ - '0');

         // Read precision, if any.
         if(*c == '.')
         {
            if(*++c == '*')
               data.prec = -1, ++c;
            else for(data.prec = 0; std::isdigit(*c);)
               data.prec = (data.prec * 10) + (*c++ - '0');
         }
         else
            data.prec = 0;

         // Read length modifier, if any.
         switch(*c)
         {
         case 'h': data.len = *++c == 'h' ? ++c, FL_HH : FL_H; break;
         case 'l': data.len = *++c == 'l' ? ++c, FL_LL : FL_L; break;
         case 'j': ++c; data.len = FL_MAX; break;
         case 'z': ++c; data.len = FL_SIZE; break;
         case 't': ++c; data.len = FL_DIFF; break;
         case 'L': ++c; data.len = FL_L; break;
         default: data.len = FL_NONE; break;
         }

         // Read conversion specifier.
         data.fmt = *c++;

         doFormat(objects, data);
      }

      doFormatLiteral(objects, string);

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
         bigsint msgtype = msgtypeObj->resolveINT();

         objects->addToken(OCODE_GET_IMM, msgtypeObj);  // type
         makeExpr(objects, VariableType::get_bt_int()); // id
         makeExpr(objects, VariableType::get_bt_int()); // color
         makeExpr(objects, VariableType::get_bt_fix()); // x
         makeExpr(objects, VariableType::get_bt_fix()); // y
         makeExpr(objects, VariableType::get_bt_fix()); // holdTime

         objects->addToken(OCODE_ACSP_END_OPT);

         switch(static_cast<int>(msgtype & 0xFFF))
         {
         case 0: // PLAIN
            break;

         case 1: // FADEOUT
            makeExpr(objects, VariableType::get_bt_fix()); // fadeTime
            break;

         case 2: // TYPEON
            makeExpr(objects, VariableType::get_bt_fix()); // typeTime
            makeExpr(objects, VariableType::get_bt_fix()); // fadeTime
            break;

         case 3: // FADEINOUT
            makeExpr(objects, VariableType::get_bt_fix()); // inTime
            makeExpr(objects, VariableType::get_bt_fix()); // outTime
            break;

         default:
            Error_NP("unrecognized hud msgtype");
         }

         if(*expr) makeExpr(objects, VariableType::get_bt_fix()); // alpha
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

      // If there are any remaining expressions, evaluate them for side effects.
      if(*expr)
      {
         int i = 0;

         for(; *expr; ++i) makeExpr(objects, VariableType::get_bt_void());

         // And then whine at the user.
         Warn_NP("%i unused arguments", i);
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

