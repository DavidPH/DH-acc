//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// NTS0 (MageCraft CODEDEFS) handling.
//
//-----------------------------------------------------------------------------

#include "BinaryTokenNTS.hpp"

#include "ObjectData.hpp"
#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define TranslateDirect(OCODE,NTS) \
   case OCODE_##OCODE: \
      tokens->emplace_back(object.labels, object.args, NTS, object.pos); \
      break


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenNTS::BinaryTokenNTS
//
BinaryTokenNTS::BinaryTokenNTS(std::vector<std::string> const &_labels,
   ObjectExpression::Vector const &_args,  char const *_code,
   SourcePosition const &_pos) : labels{_labels}, args{_args}, code{_code}, pos{_pos}
{
}

//
// BinaryTokenNTS::MakeTokens
//
void BinaryTokenNTS::MakeTokens(std::vector<BinaryTokenNTS> *tokens, ObjectVector const &objects)
{
   for(auto const &object : objects)
   {
      switch(object.code)
      {
         TranslateDirect(NOP, "nop");

      case OCODE_ADD_STK_I:
         TranslateDirect(ADD_STK_U, "addu_stk");

         TranslateDirect(JMP_CAL, "call_stk");

         TranslateDirect(JMP_NIL, "cjmp_stk_nil");
         TranslateDirect(JMP_TRU, "cjmp_stk_tru");

      case OCODE_CMP_EQ_I:
         TranslateDirect(CMP_EQ_U, "cmpu_stk_eq");
         TranslateDirect(CMP_GE_U, "cmpu_stk_ge");
         TranslateDirect(CMP_GT_U, "cmpu_stk_gt");
         TranslateDirect(CMP_LE_U, "cmpu_stk_le");
         TranslateDirect(CMP_LT_U, "cmpu_stk_lt");
      case OCODE_CMP_NE_I:
         TranslateDirect(CMP_NE_U, "cmpu_stk_ne");

         TranslateDirect(SET_STATIC, "drop_dat");
         TranslateDirect(STK_DROP,   "drop_nul");
         TranslateDirect(SET_PTR,    "drop_ptr");
         TranslateDirect(SET_REG,    "drop_reg");

         TranslateDirect(JMP_IMM, "jump_imm");
         TranslateDirect(JMP,     "jump_stk");

         TranslateDirect(GET_STATIC, "push_dat");
      case OCODE_GET_FUNCP:
         TranslateDirect(GET_IMM,    "push_imm");
         TranslateDirect(GET_PTR,    "push_ptr");
         TranslateDirect(GET_REG,    "push_reg");

      case OCODE_JMP_RET_NIL:
      case OCODE_JMP_RET_SCR:
         TranslateDirect(JMP_RET, "retn");

      case OCODE_SUB_STK_I:
         TranslateDirect(SUB_STK_U, "subu_stk");

      default:
         Error(object.pos, "unknown OCODE: %s", make_string(object.code));
      }
   }
}

//
// BinaryTokenNTS::WriteAll
//
void BinaryTokenNTS::WriteAll(std::ostream *out, std::vector<BinaryTokenNTS> const &tokens)
{
   // Header.
   WriteStr(out, "MC_NTS0");
   WriteStr(out, "CODEDEFS");
   WriteStr(out, "");

   // Variable data.
   ObjectData::Static::Iterate(WriteStatic, out);
   ObjectData::String::Iterate(WriteString, out);

   // Function data.
   ObjectData::Function::Iterate(WriteFunction, out);

   // Instruction data.
   WriteStr(out, "code");
   WriteStr(out, "");
   WriteStr(out, "{");

   for(auto const &token : tokens)
   {
      // Labels.
      for(auto const &label : token.labels)
      {
         WriteStr(out, "label");
         WriteStr(out, "(");
         WriteStr(out, label);
         WriteStr(out, ")");
      }

      WriteStr(out, token.code);
      WriteStr(out, "(");
      if(!token.args.empty())
      {
         token.args[0]->writeNTS0(out);
         for(auto itr = token.args.begin() + 1, end = token.args.end(); itr != end; ++itr)
            WriteStr(out, ","), (*itr)->writeNTS0(out);
      }
      WriteStr(out, ")");
   }

   WriteStr(out, "}");
}

//
// BinaryTokenNTS::WriteFunction
//
void BinaryTokenNTS::WriteFunction(std::ostream *out, ObjectData::Function const &f)
{
   if(f.externDef) return;

   WriteStr(out, "function");
   WriteStr(out, f.name);
   WriteStr(out, "{");
   WriteKeyInt(out, "argCount", f.argCount);
   WriteKeyStr(out, "label",    f.label);
   WriteKeyInt(out, "regCount", f.varCount - f.argCount);
   WriteKeyInt(out, "retCount", f.retCount);
   WriteStr(out, "}");
}

//
// BinaryTokenNTS::WriteInt
//
void BinaryTokenNTS::WriteInt(std::ostream *out, bigsint i)
{
   *out << std::hex << i << '\0';
}

//
// BinaryTokenNTS::WriteKeyInt
//
void BinaryTokenNTS::WriteKeyInt(std::ostream *out, char const *key, bigsint val)
{
   WriteStr(out, key);
   WriteStr(out, "=");
   WriteInt(out, val);
   WriteStr(out, ";");
}

//
// BinaryTokenNTS::WriteKeyStr
//
void BinaryTokenNTS::WriteKeyStr(std::ostream *out, char const *key, std::string const &val)
{
   WriteStr(out, key);
   WriteStr(out, "=");
   WriteStr(out, val);
   WriteStr(out, ";");
}

//
// BinaryTokenNTS::WriteStatic
//
void BinaryTokenNTS::WriteStatic(std::ostream *out, ObjectData::Static const &s)
{
   if(s.externDef) return;

   WriteStr(out, "data");
   WriteStr(out, s.name);
   WriteInt(out, s.size);

   if(!s.init.data.empty())
   {
      auto itr = s.init.data.begin(), end = s.init.data.end();
      WriteStr(out, "(");
      (*itr++)->writeNTS0(out);
      while(itr != end)
         WriteStr(out, ","), (*itr++)->writeNTS0(out);
      WriteStr(out, ")");
   }
   else
      WriteStr(out, ";");
}

//
// BinaryTokenNTS::WriteStr
//
void BinaryTokenNTS::WriteStr(std::ostream *out, std::string const &s)
{
   *out << s << '\0';
}

//
// BinaryTokenNTS::WriteStr
//
void BinaryTokenNTS::WriteStr(std::ostream *out, char const *s)
{
   *out << s << '\0';
}

//
// BinaryTokenNTS::WriteString
//
void BinaryTokenNTS::WriteString(std::ostream *out, ObjectData::String const &s)
{
   if(s.names.empty()) return;

   biguint bytes = s.string.size() + 1;
   biguint words = (bytes + 3) / 4;

   // Convert the string into words.
   std::vector<biguint> data(words, 0);
   for(auto i = s.string.size(); i--;)
      data[i / 4] |= static_cast<biguint>(s.string[i] & 0xFF) << (i % 4 * 8);

   // Add the string as a data.
   WriteStr(out, "data");
   WriteStr(out, s.names[0] + '_');
   WriteInt(out, words);
   WriteStr(out, "(");
   {
      auto itr = data.begin(), end = data.end();
      WriteInt(out, *itr++);
      while(itr != end)
         WriteStr(out, ","), WriteInt(out, *itr++);
   }
   WriteStr(out, ")");

   // Define the actual name as the byte-oriented address.
   WriteStr(out, "define");
   WriteStr(out, s.names[0]);
   WriteStr(out, "=");
   WriteStr(out, "*");
   WriteStr(out, '$' + s.names[0] + '_');
   WriteStr(out, "4");
   WriteStr(out, ";");
}

// EOF

