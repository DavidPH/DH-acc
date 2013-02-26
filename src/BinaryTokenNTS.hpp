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

#ifndef HPP_BinaryTokenNTS_
#define HPP_BinaryTokenNTS_

#include "bignum.hpp"
#include "Counter.hpp"
#include "SourcePosition.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ObjectData
{
struct Function;
struct Static;
struct String;
}

class ObjectExpression;
class ObjectVector;

//
// BinaryTokenNTS
//
class BinaryTokenNTS
{
public:
   BinaryTokenNTS(std::vector<std::string> const &labels,
      std::vector<CounterPointer<ObjectExpression>> const &args,
      char const *code, SourcePosition const &pos);

   std::vector<std::string> labels;
   std::vector<CounterPointer<ObjectExpression>> args;
   std::string code;
   SourcePosition pos;


   static void MakeTokens(std::vector<BinaryTokenNTS> *tokens, ObjectVector const &objects);

   static void WriteAll(std::ostream *out, std::vector<BinaryTokenNTS> const &tokens);

   static void WriteFunction(std::ostream *out, ObjectData::Function const &f);
   static void WriteInt(std::ostream *out, bigsint i);
   static void WriteKeyInt(std::ostream *out, char const *key, bigsint val);
   static void WriteKeyStr(std::ostream *out, char const *key, std::string const &val);
   static void WriteStatic(std::ostream *out, ObjectData::Static const &s);
   static void WriteStr(std::ostream *out, std::string const &s);
   static void WriteStr(std::ostream *out, char const *s);
   static void WriteString(std::ostream *out, ObjectData::String const &s);
};

#endif//HPP_BinaryTokenNTS_

