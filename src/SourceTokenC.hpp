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
class SourceTokenC
{
public:
   //
   // ::TokenType
   //
   enum TokenType
   {
      TT_CHARACTER,
      TT_FLOAT,
      TT_IDENTIFIER,
      TT_INTEGER,
      TT_STRING,

      TT_OP_AND,                // &
      TT_OP_AND_EQUALS,         // &=
      TT_OP_AND2,               // &&
      TT_OP_AND2_EQUALS,        // &&=
      TT_OP_AT,                 // @
      TT_OP_ASTERISK,           // *
      TT_OP_ASTERISK_EQUALS,    // *=
      TT_OP_BRACE_C,            // }
      TT_OP_BRACE_O,            // {
      TT_OP_BRACKET_C,          // ]
      TT_OP_BRACKET_O,          // [
      TT_OP_CARET,              // ^
      TT_OP_CARET_EQUALS,       // ^=
      TT_OP_CARET2,             // ^^
      TT_OP_CARET2_EQUALS,      // ^^=
      TT_OP_CMP_EQ,             // ==
      TT_OP_CMP_GE,             // >=
      TT_OP_CMP_GT,             // >
      TT_OP_CMP_GT2,            // >>
      TT_OP_CMP_GT2_EQUALS,     // >>=
      TT_OP_CMP_LE,             // <=
      TT_OP_CMP_LT,             // <
      TT_OP_CMP_LT2,            // <<
      TT_OP_CMP_LT2_EQUALS,     // <<=
      TT_OP_CMP_NE,             // !=
      TT_OP_COLON,              // :
      TT_OP_COMMA,              // ,
      TT_OP_ELLIPSIS,           // ...
      TT_OP_EQUALS,             // =
      TT_OP_EXCLAMATION,        // !
      TT_OP_HASH,               // #
      TT_OP_HASH2,              // ##
      TT_OP_HASH3,              // ###
      TT_OP_MINUS,              // -
      TT_OP_MINUS_EQUALS,       // -=
      TT_OP_MINUS_GT,           // ->
      TT_OP_MINUS2,             // --
      TT_OP_PARENTHESIS_C,      // )
      TT_OP_PARENTHESIS_O,      // (
      TT_OP_PERCENT,            // %
      TT_OP_PERCENT_EQUALS,     // %=
      TT_OP_PERIOD,             // .
      TT_OP_PIPE,               // |
      TT_OP_PIPE_EQUALS,        // |=
      TT_OP_PIPE2,              // ||
      TT_OP_PIPE2_EQUALS,       // ||=
      TT_OP_PLUS,               // +
      TT_OP_PLUS_EQUALS,        // +=
      TT_OP_PLUS2,              // ++
      TT_OP_QUERY,              // ?
      TT_OP_SEMICOLON,          // ;
      TT_OP_SLASH,              // /
      TT_OP_SLASH_EQUALS,       // /=
      TT_OP_TILDE,              // ~

      TT_NONE
   };


   SourceTokenC() : type(TT_NONE) {}

   void readToken(SourceStream *in) {read_token(in, this);}

   SourcePosition pos;
   std::string data;
   TokenType type;


   static void read_token(SourceStream *in, SourceTokenC *token);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

std::string const &make_string(SourceTokenC::TokenType type);

#endif//HPP_SourceTokenC_

