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
      case OCODE_NOP:
         tokens->emplace_back(object.labels, object.args, "nop", object.pos);
         break;

      case OCODE_ADD_STK_I:
      case OCODE_ADD_STK_U:
         tokens->emplace_back(object.labels, object.args, "addu_stk", object.pos);
         break;

      case OCODE_GET_FUNCP:
      case OCODE_GET_IMM:
         tokens->emplace_back(object.labels, object.args, "push_imm", object.pos);
         break;

      case OCODE_GET_PTR:
         tokens->emplace_back(object.labels, object.args, "push_ptr", object.pos);
         break;

      case OCODE_GET_REG:
         tokens->emplace_back(object.labels, object.args, "push_reg", object.pos);
         break;

      case OCODE_GET_STATIC:
         tokens->emplace_back(object.labels, object.args, "push_dat", object.pos);
         break;

      case OCODE_JMP:
         tokens->emplace_back(object.labels, object.args, "jump_stk", object.pos);
         break;

      case OCODE_JMP_CAL:
         tokens->emplace_back(object.labels, object.args, "call_stk", object.pos);
         break;

      case OCODE_JMP_RET:
      case OCODE_JMP_RET_NIL:
      case OCODE_JMP_RET_SCR:
         tokens->emplace_back(object.labels, object.args, "retn", object.pos);
         break;

      case OCODE_JMP_TRU:
         tokens->emplace_back(object.labels, object.args, "cjmp_tru", object.pos);
         break;

      case OCODE_SET_PTR:
         tokens->emplace_back(object.labels, object.args, "drop_ptr", object.pos);
         break;

      case OCODE_SET_REG:
         tokens->emplace_back(object.labels, object.args, "drop_reg", object.pos);
         break;

      case OCODE_SET_STATIC:
         tokens->emplace_back(object.labels, object.args, "drop_dat", object.pos);
         break;

      case OCODE_STK_DROP:
         tokens->emplace_back(object.labels, object.args, "drop_nul", object.pos);
         break;

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

   WriteStr(out, "data");
   WriteStr(out, s.names[0]);
   WriteInt(out, s.string.size() + 1);
   WriteStr(out, "(");
   for(auto const &c : s.string)
      WriteInt(out, c), WriteStr(out, ",");
   WriteStr(out, "0");
   WriteStr(out, ")");
}

// EOF

