//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
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
// Assembly token handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceTokenASM_
#define HPP_SourceTokenASM_

#include "Counter.hpp"
#include "SourcePosition.hpp"

#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceStream;

//
// SourceTokenASM
//
class SourceTokenASM : public PlainCounter
{
   MAKE_NOVIRTUAL_COUNTER_CLASS_BASE(SourceTokenASM, PlainCounter);

public:
   //
   // ::TokenType
   //
   enum TokenType
   {
      TT_CHR,
      TT_INT,
      TT_STR,

      TT_ENDL, // \n

      TT_ADD,    // +
      TT_AN2,    // &&
      TT_AND,    // &
      TT_CMP_EQ, // ==
      TT_CMP_GE, // >=
      TT_CMP_GT, // >
      TT_CMP_LE, // <=
      TT_CMP_LT, // <
      TT_CMP_NE, // !=
      TT_COLON,  // :
      TT_COMMA,  // ,
      TT_DIV,    // /
      TT_IO2,    // ||
      TT_IOR,    // |
      TT_LSH,    // <<
      TT_MOD,    // %
      TT_MUL,    // *
      TT_NOTBIT, // ~
      TT_NOTLOG, // !
      TT_QUERY,  // ?
      TT_RSH,    // >>
      TT_SUB,    // -
      TT_XO2,    // ^^
      TT_XOR,    // ^

      TT_PAREN_O, // (
      TT_PAREN_C, // )

      TT_NONE
   };


   SourceTokenASM() : type(TT_NONE) {}

   void readToken(SourceStream *in) {read_token(in, this);}

   SourcePosition pos;
   std::string data;
   TokenType type;


   friend std::string const &make_string(TokenType type);

   static Reference create(SourceStream *in)
   {
      Reference tok(new SourceTokenASM); read_token(in, tok); return tok;
   }

   static void read_token(SourceStream *in, SourceTokenASM *token);

   static Reference tt_none()
   {
      static Reference tok(new SourceTokenASM); return tok;
   }
};

#endif//HPP_SourceTokenASM_

