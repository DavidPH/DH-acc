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
// C token handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceTokenC_
#define HPP_SourceTokenC_

#include "Counter.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceStream;

//
// SourceTokenC
//
class SourceTokenC : public PlainCounter
{
   MAKE_NOVIRTUAL_COUNTER_CLASS_BASE(SourceTokenC, PlainCounter);

public:
   //
   // ::TokenType
   //
   enum TokenType
   {
      TT_CHR,
      TT_FLT,
      TT_NAM,
      TT_INT,
      TT_STR,

      TT_ENDL,  // \n
      TT_HASH1, // #
      TT_HASH2, // ##
      TT_HASH3, // ###

      TT_AD2,    // ++
      TT_ADD,    // +
      TT_ADD_EQ, // +=
      TT_AN2,    // &&
      TT_AN2_EQ, // &&=
      TT_AND,    // &
      TT_AND_EQ, // &=
      TT_AT,     // @
      TT_CMP_EQ, // ==
      TT_CMP_GE, // >=
      TT_CMP_GT, // >
      TT_CMP_LE, // <=
      TT_CMP_LT, // <
      TT_CMP_NE, // !=
      TT_COLON,  // :
      TT_COMMA,  // ,
      TT_DIV,    // /
      TT_DIV_EQ, // /=
      TT_DOT,    // .
      TT_DOT3,   // ...
      TT_EQUALS, // =
      TT_IO2,    // ||
      TT_IO2_EQ, // ||=
      TT_IOR,    // |
      TT_IOR_EQ, // |=
      TT_LSH,    // <<
      TT_LSH_EQ, // <<=
      TT_MEM,    // ->
      TT_MOD,    // %
      TT_MOD_EQ, // %=
      TT_MUL,    // *
      TT_MUL_EQ, // *=
      TT_NOTBIT, // ~
      TT_NOTLOG, // !
      TT_QUERY,  // ?
      TT_RSH,    // >>
      TT_RSH_EQ, // >>=
      TT_SU2,    // --
      TT_SUB,    // -
      TT_SUB_EQ, // -=
      TT_XO2,    // ^^
      TT_XO2_EQ, // ^^=
      TT_XOR,    // ^
      TT_XOR_EQ, // ^=

      TT_BRACE_O, // {
      TT_BRACE_C, // }
      TT_BRACK_O, // [
      TT_BRACK_C, // ]
      TT_PAREN_O, // (
      TT_PAREN_C, // )

      TT_SEMICOLON, // ;

      TT_NONE
   };


   SourceTokenC() : type(TT_NONE) {}

   SourceTokenC(SourcePosition const &_pos, TokenType _type)
    : pos(_pos), type(_type) {}

   SourceTokenC(SourcePosition const &_pos, std::string const &_data,
      TokenType _type) : pos(_pos), data(_data), type(_type) {}

   void readToken(SourceStream *in) {read_token(in, this);}

   SourcePosition pos;
   std::string data;
   TokenType type;


   static Reference create(SourceStream *in)
   {
      Reference tok(new SourceTokenC); read_token(in, tok); return tok;
   }

   static Reference create(SourcePosition const &pos, std::string const &data,
                           TokenType type)
   {
      return static_cast<Reference>(new SourceTokenC(pos, data, type));
   }

   static Reference create_join(SourceTokenC const *l, SourceTokenC const *r);

   static void read_token(SourceStream *in, SourceTokenC *token);

   static Reference tt_none()
   {
      static Reference tok(new SourceTokenC); return tok;
   }

   static Reference tt_str(SourcePosition const &pos,
                           std::vector<Reference> const &args);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

std::string const &make_string(SourceTokenC::TokenType type);

#endif//HPP_SourceTokenC_

