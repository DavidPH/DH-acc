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
// C attribute parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionC.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::ParseAttributeAsmfunc
//
void SourceExpressionC::ParseAttributeAsmfunc(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1)
{
   funcAttr.asmfun = true;

   if(!in->dropType(SourceTokenC::TT_PAREN_O)) return;

   SourceTokenC::Reference tok = in->get(SourceTokenC::TT_NAM);
   funcAttr.asmfunCode.ocode = ocode_get_code(tok->data, tok->pos);

   if(in->dropType(SourceTokenC::TT_COMMA))
   {
      tok = in->get(SourceTokenC::TT_NAM);
      funcAttr.asmfunCode.ocode_imm = ocode_get_code(tok->data, tok->pos);
   }

   in->get(SourceTokenC::TT_PAREN_C);
}

//
// SourceExpressionC::ParseAttributeLinespec
//
void SourceExpressionC::ParseAttributeLinespec(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1)
{
   funcAttr.lnspec = true;

   if(!in->dropType(SourceTokenC::TT_PAREN_O)) return;

   SourceTokenC::Reference tok = in->get(SourceTokenC::TT_INT);
   funcAttr.scriptAddr = ParseInt(tok->data, context, tok->pos)->makeObject()->resolveINT();

   in->get(SourceTokenC::TT_PAREN_C);
}

//
// SourceExpressionC::ParseAttributeNative
//
void SourceExpressionC::ParseAttributeNative(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1)
{
   funcAttr.native = true;

   if(!in->dropType(SourceTokenC::TT_PAREN_O)) return;

   SourceTokenC::Reference tok = in->get(SourceTokenC::TT_INT);
   funcAttr.scriptAddr = ParseInt(tok->data, context, tok->pos)->makeObject()->resolveINT();

   in->get(SourceTokenC::TT_PAREN_C);
}

//
// SourceExpressionC::ParseAttributeScript
//
void SourceExpressionC::ParseAttributeScript(FunctionAttributes &funcAttr, SRCEXPC_PARSE_ARG1)
{
   funcAttr.script = true;

   if(!in->dropType(SourceTokenC::TT_PAREN_O)) return;

   // script-type

   SourceTokenC::Reference tok = in->get(SourceTokenC::TT_NAM);

   if(tok->data == "closed" || tok->data == "__closed" || tok->data == "__closed__")
      funcAttr.scriptType = ObjectData::ST_CLOSED;

   else if(tok->data == "open" || tok->data == "__open" || tok->data == "__open__")
      funcAttr.scriptType = ObjectData::ST_OPEN;

   else if(tok->data == "respawn" || tok->data == "__respawn" || tok->data == "__respawn__")
      funcAttr.scriptType = ObjectData::ST_RESPAWN;

   else if(tok->data == "death" || tok->data == "__death" || tok->data == "__death__")
      funcAttr.scriptType = ObjectData::ST_DEATH;

   else if(tok->data == "enter" || tok->data == "__enter" || tok->data == "__enter__")
      funcAttr.scriptType = ObjectData::ST_ENTER;

   else if(tok->data == "lightning" || tok->data == "__lightning" || tok->data == "__lightning__")
      funcAttr.scriptType = ObjectData::ST_LIGHTNING;

   else if(tok->data == "unloading" || tok->data == "__unloading" || tok->data == "__unloading__")
      funcAttr.scriptType = ObjectData::ST_UNLOADING;

   else if(tok->data == "disconnect" || tok->data == "__disconnect" || tok->data == "__disconnect__")
      funcAttr.scriptType = ObjectData::ST_DISCONNECT;

   else if(tok->data == "return" || tok->data == "__return" || tok->data == "__return__")
      funcAttr.scriptType = ObjectData::ST_RETURN;

   else
      Error(tok->pos, "unrecognized script-type '%s'", tok->data.c_str());

   // script-flag

   while(in->dropType(SourceTokenC::TT_IOR))
   {
      tok = in->get(SourceTokenC::TT_NAM);

      if(tok->data == "net" || tok->data == "__net" || tok->data == "__net__")
         funcAttr.scriptFlag |= ObjectData::SF_NET;

      else if(tok->data == "clientside" || tok->data == "__clientside" || tok->data == "__clientside__")
         funcAttr.scriptFlag |= ObjectData::SF_CLIENTSIDE;

      else
         Error(tok->pos, "unrecognized script-flag '%s'", tok->data.c_str());
   }

   // script-addr

   if(in->dropType(SourceTokenC::TT_COMMA))
   {
      if(in->peekType(SourceTokenC::TT_STR))
      {
         funcAttr.scriptName = in->get()->data;
         funcAttr.scriptAddr = -2;
      }
      else
      {
         tok = in->get(SourceTokenC::TT_INT);
         funcAttr.scriptName = "";
         funcAttr.scriptAddr = ParseInt(tok->data, context, tok->pos)->makeObject()->resolveINT();
      }
   }

   in->get(SourceTokenC::TT_PAREN_C);
}

//
// SourceExpressionC::ParseFunctionAttributes
//
SourceExpressionC::FunctionAttributes SourceExpressionC::ParseFunctionAttributes(SRCEXPC_PARSE_ARG1)
{
   FunctionAttributes funcAttr;

   while(in->dropType(SourceTokenC::TT_NAM, "__attribute__"))
   {
      in->get(SourceTokenC::TT_PAREN_O); in->get(SourceTokenC::TT_PAREN_O);

      if(!in->peekType(SourceTokenC::TT_PAREN_C)) do
      {
         SourceTokenC::Reference tok = in->get(SourceTokenC::TT_NAM);

         if(tok->data == "asmfunc" || tok->data == "__asmfunc" || tok->data == "__asmfunc__")
            {ParseAttributeAsmfunc(funcAttr, in, context); continue;}

         if(tok->data == "linespec" || tok->data == "__linespec" || tok->data == "__linespec__")
            {ParseAttributeLinespec(funcAttr, in, context); continue;}

         if(tok->data == "native" || tok->data == "__native" || tok->data == "__native__")
            {ParseAttributeNative(funcAttr, in, context); continue;}

         if(tok->data == "script" || tok->data == "__script" || tok->data == "__script__")
            {ParseAttributeScript(funcAttr, in, context); continue;}
      }
      while(in->dropType(SourceTokenC::TT_COMMA));

      in->get(SourceTokenC::TT_PAREN_C); in->get(SourceTokenC::TT_PAREN_C);
   }

   return funcAttr;
}

// EOF

