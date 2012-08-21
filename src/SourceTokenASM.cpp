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

#include "SourceTokenASM.hpp"

#include "SourceException.hpp"
#include "SourceStream.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string const tt_names[SourceTokenASM::TT_NONE+1] =
{
   "TT_CHR",
   "TT_INT",
   "TT_STR",

   "TT_ENDL", // \n

   "TT_ADD",    // +
   "TT_AN2",    // &&
   "TT_AND",    // &
   "TT_CMP_EQ", // ==
   "TT_CMP_GE", // >=
   "TT_CMP_GT", // >
   "TT_CMP_LE", // <=
   "TT_CMP_LT", // <
   "TT_CMP_NE", // !=
   "TT_COLON",  // :
   "TT_COMMA",  // ,
   "TT_DIV",    // /
   "TT_IO2",    // ||
   "TT_IOR",    // |
   "TT_LSH",    // <<
   "TT_MOD",    // %
   "TT_MUL",    // *
   "TT_NOTBIT", // ~
   "TT_NOTLOG", // !
   "TT_QUERY",  // ?
   "TT_RSH",    // >>
   "TT_SUB",    // -
   "TT_XO2",    // ^^
   "TT_XOR",    // ^

   "TT_PAREN_O", // (
   "TT_PAREN_C", // )

   "TT_NONE"
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenASM::read_token
//
void SourceTokenASM::read_token(SourceStream *in, SourceTokenASM *token)
{
   char c;

   // Discard any whitespace before token.
   while(isspace(c = in->get())) if(c == '\n') break;

   token->pos.filename = in->getFilename();
   token->pos.line = in->getLineCount();
   token->pos.column = in->getColumn();

   token->data.clear();

   switch(c)
   {
   case '\n': token->type = TT_ENDL; return;
   case '+': token->type = TT_ADD;    return;
   case ':': token->type = TT_COLON;  return;
   case ',': token->type = TT_COMMA;  return;
   case '/': token->type = TT_DIV;    return;
   case '%': token->type = TT_MOD;    return;
   case '*': token->type = TT_MUL;    return;
   case '~': token->type = TT_NOTBIT; return;
   case '?': token->type = TT_QUERY;  return;
   case '-': token->type = TT_SUB;    return;
   case '(': token->type = TT_PAREN_O; return;
   case ')': token->type = TT_PAREN_C; return;

   case '&':
      c = in->get();
      if(c == '&') {token->type = TT_AN2; return;}
      in->unget(c); token->type = TT_AND; return;

   case '>':
      c = in->get();
      if(c == '=') {token->type = TT_CMP_GE; return;}
      if(c == '>') {token->type = TT_RSH;    return;}
      in->unget(c); token->type = TT_CMP_GT; return;

   case '<':
      c = in->get();
      if(c == '=') {token->type = TT_CMP_LE; return;}
      if(c == '<') {token->type = TT_LSH;    return;}
      in->unget(c); token->type = TT_CMP_LT; return;

   case '=':
      c = in->get();
      if(c == '=') {token->type = TT_CMP_EQ; return;}
      goto error_c;

   case '|':
      c = in->get();
      if(c == '|') {token->type = TT_IO2; return;}
      in->unget(c); token->type = TT_IOR; return;

   case '!':
      c = in->get();
      if(c == '=') {token->type = TT_CMP_NE; return;}
      in->unget(c); token->type = TT_NOTLOG; return;

   case '^':
      c = in->get();
      if(c == '^') {token->type = TT_XO2; return;}
      in->unget(c); token->type = TT_XOR; return;
   }

   // Identifier.
   if(isalpha(c) || c == '_')
   {
      for(token->type = TT_STR; isalnum(c) || c == '_'; c = in->get())
         token->data += c;

      in->unget(c);

      return;
   }

   // Number.
   if(c == '0')
   {
      token->type = TT_INT;

      token->data += c;

      int base;

      // Select base.
      switch((c = in->get()))
      {
      case 'B': case 'b': base =  2; break;
      case 'O': case 'o': base =  8; break;
      case 'D': case 'd': base = 10; break;
      case 'X': case 'x': base = 16; break;
      default: goto error_c;
      }

      token->data += c;

      // Numeric part.
      for(;; token->data += c)
      {
         c = in->get();

         if('0' <= c && c <= '1' && base >=  2) continue;
         if('2' <= c && c <= '7' && base >=  8) continue;
         if('8' <= c && c <= '9' && base >= 10) continue;
         if('A' <= c && c <= 'F' && base >= 16) continue;
         if('a' <= c && c <= 'f' && base >= 16) continue;

         break;
      }

      in->unget(c);

      return;
   }

   // String.
   if(c == '"' || c == '\'')
   {
      token->type = c == '"' ? TT_STR : TT_CHR;

      for(c = in->get(); in->isInQuote(); c = in->get())
         token->data += c;

      return;
   }

error_c:
   Error(token->pos, "unexpected character '%c' (%i)", c, static_cast<int>(c));
}

//
// make_string<SourceTokenASM::TokenType>
//
std::string const &make_string(SourceTokenASM::TokenType type)
{
   return tt_names[type];
}

// EOF

