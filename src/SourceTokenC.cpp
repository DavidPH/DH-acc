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

#include "SourceTokenC.hpp"

#include "SourceStream.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string const tt_names[SourceTokenC::TT_NONE+1] =
{
   "TT_CHARACTER",
   "TT_FLOAT",
   "TT_IDENTIFIER",
   "TT_INTEGER",
   "TT_STRING",

   "TT_OP_AND",                // &
   "TT_OP_AND_EQUALS",         // &=
   "TT_OP_AND2",               // &&
   "TT_OP_AND2_EQUALS",        // &&=
   "TT_OP_AT",                 // @
   "TT_OP_ASTERISK",           // *
   "TT_OP_ASTERISK_EQUALS",    // *=
   "TT_OP_BRACE_C",            // }
   "TT_OP_BRACE_O",            // {
   "TT_OP_BRACKET_C",          // ]
   "TT_OP_BRACKET_O",          // [
   "TT_OP_CARET",              // ^
   "TT_OP_CARET_EQUALS",       // ^=
   "TT_OP_CARET2",             // ^^
   "TT_OP_CARET2_EQUALS",      // ^^=
   "TT_OP_CMP_EQ",             // ==
   "TT_OP_CMP_GE",             // >=
   "TT_OP_CMP_GT",             // >
   "TT_OP_CMP_GT2",            // >>
   "TT_OP_CMP_GT2_EQUALS",     // >>=
   "TT_OP_CMP_LE",             // <=
   "TT_OP_CMP_LT",             // <
   "TT_OP_CMP_LT2",            // <<
   "TT_OP_CMP_LT2_EQUALS",     // <<=
   "TT_OP_CMP_NE",             // !=
   "TT_OP_COLON",              // :
   "TT_OP_COMMA",              // ,
   "TT_OP_ELLIPSIS",           // ...
   "TT_OP_EQUALS",             // =
   "TT_OP_EXCLAMATION",        // !
   "TT_OP_HASH",               // #
   "TT_OP_HASH2",              // ##
   "TT_OP_HASH3",              // ###
   "TT_OP_MINUS",              // -
   "TT_OP_MINUS_EQUALS",       // -=
   "TT_OP_MINUS_GT",           // ->
   "TT_OP_MINUS2",             // --
   "TT_OP_PARENTHESIS_C",      // )
   "TT_OP_PARENTHESIS_O",      // (
   "TT_OP_PERCENT",            // %
   "TT_OP_PERCENT_EQUALS",     // %=
   "TT_OP_PERIOD",             // .
   "TT_OP_PIPE",               // |
   "TT_OP_PIPE_EQUALS",        // |=
   "TT_OP_PIPE2",              // ||
   "TT_OP_PIPE2_EQUALS",       // ||=
   "TT_OP_PLUS",               // +
   "TT_OP_PLUS_EQUALS",        // +=
   "TT_OP_PLUS2",              // ++
   "TT_OP_QUERY",              // ?
   "TT_OP_SEMICOLON",          // ;
   "TT_OP_SLASH",              // /
   "TT_OP_SLASH_EQUALS",       // /=
   "TT_OP_TILDE",              // ~

   "TT_NONE"
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenC::read_token
//
void SourceTokenC::read_token(SourceStream *in, SourceTokenC *token)
{
   char c;

   // Discard any whitespace before token.
   while (isspace(c = in->get()));

   token->pos.filename = in->getFilename();
   token->pos.line = in->getLineCount();
   token->pos.column = in->getColumn();

   token->data.clear();

   switch (c)
   {
   case '@': token->type = TT_OP_AT;            return;
   case '}': token->type = TT_OP_BRACE_C;       return;
   case '{': token->type = TT_OP_BRACE_O;       return;
   case ']': token->type = TT_OP_BRACKET_C;     return;
   case '[': token->type = TT_OP_BRACKET_O;     return;
   case ':': token->type = TT_OP_COLON;         return;
   case ',': token->type = TT_OP_COMMA;         return;
   case ')': token->type = TT_OP_PARENTHESIS_C; return;
   case '(': token->type = TT_OP_PARENTHESIS_O; return;
   case '?': token->type = TT_OP_QUERY;         return;
   case ';': token->type = TT_OP_SEMICOLON;     return;
   case '~': token->type = TT_OP_TILDE;         return;

   case '&':
      c = in->get();

      if (c == '=') {token->type = TT_OP_AND_EQUALS; return;}
      if (c == '&')
      {
         c = in->get();

         if (c == '=') {token->type = TT_OP_AND2_EQUALS; return;}

         in->unget(c);

         token->type = TT_OP_AND2; return;
      }

      in->unget(c);

      token->type = TT_OP_AND; return;

   case '*':
      c = in->get();

      if (c == '=') {token->type = TT_OP_ASTERISK_EQUALS; return;}

      in->unget(c);

      token->type = TT_OP_ASTERISK; return;

   case '^':
      c = in->get();

      if (c == '=') {token->type = TT_OP_CARET_EQUALS; return;}
      if (c == '^')
      {
         c = in->get();

         if (c == '=') {token->type = TT_OP_CARET2_EQUALS; return;}

         in->unget(c);

         token->type = TT_OP_CARET2; return;
      }

      in->unget(c);

      token->type = TT_OP_CARET; return;

   case '=':
      c = in->get();

      if (c == '=') {token->type = TT_OP_CMP_EQ; return;}

      in->unget(c);

      token->type = TT_OP_EQUALS; return;

   case '>':
      c = in->get();

      if (c == '=') {token->type = TT_OP_CMP_GE; return;}
      if (c == '>')
      {
         c = in->get();

         if (c == '=') {token->type = TT_OP_CMP_GT2_EQUALS; return;}

         in->unget(c);

         token->type = TT_OP_CMP_GT2; return;
      }

      in->unget(c);

      token->type = TT_OP_CMP_GT; return;

   case '<':
      c = in->get();

      if (c == '=') {token->type = TT_OP_CMP_LE; return;}
      if (c == '<')
      {
         c = in->get();

         if (c == '=') {token->type = TT_OP_CMP_LT2_EQUALS; return;}

         in->unget(c);

         token->type = TT_OP_CMP_LT2; return;
      }

      in->unget(c);

      token->type = TT_OP_CMP_LT; return;

   case '#':
      c = in->get();

      if (c == '#')
      {
         c = in->get();

         if (c == '#') {token->type = TT_OP_HASH3; return;}

         in->unget(c);

         token->type = TT_OP_HASH2; return;
      }

      in->unget(c);

      token->type = TT_OP_HASH; return;

   case '-':
      c = in->get();

      if (c == '=') {token->type = TT_OP_MINUS_EQUALS; return;}
      if (c == '>') {token->type = TT_OP_MINUS_GT;     return;}
      if (c == '-') {token->type = TT_OP_MINUS2;       return;}

      in->unget(c);

      token->type = TT_OP_MINUS; return;

   case '!':
      c = in->get();

      if (c == '=') {token->type = TT_OP_CMP_NE; return;}

      in->unget(c);

      token->type = TT_OP_EXCLAMATION; return;

   case '%':
      c = in->get();

      if (c == '=') {token->type = TT_OP_PERCENT_EQUALS; return;}

      in->unget(c);

      token->type = TT_OP_PERCENT; return;

   case '.':
      c = in->get();

      if (c == '.')
      {
         c = in->get();

         if (c == '.') {token->type = TT_OP_ELLIPSIS; return;}

         in->unget(c);

         c = '.';
      }

      in->unget(c);

      token->type = TT_OP_PERIOD; return;

   case '|':
      c = in->get();

      if (c == '=') {token->type = TT_OP_PIPE_EQUALS; return;}
      if (c == '|')
      {
         c = in->get();

         if (c == '=') {token->type = TT_OP_PIPE2_EQUALS; return;}

         in->unget(c);

         token->type = TT_OP_PIPE2; return;
      }

      in->unget(c);

      token->type = TT_OP_PIPE; return;

   case '+':
      c = in->get();

      if (c == '=') {token->type = TT_OP_PLUS_EQUALS; return;}
      if (c == '+') {token->type = TT_OP_PLUS2;       return;}

      in->unget(c);

      token->type = TT_OP_PLUS; return;

   case '/':
      c = in->get();

      if (c == '=') {token->type = TT_OP_SLASH_EQUALS; return;}

      in->unget(c);

      token->type = TT_OP_SLASH; return;
   }

   if (isalpha(c) || c == '_')
   {
      token->type = TT_IDENTIFIER;

      while (isalnum(c) || c == '_')
      {
         token->data += c;

         c = in->get();
      }

      in->unget(c);

      return;
   }

   // Number.
   if (isdigit(c) || c == '.')
   {
      token->type = TT_INTEGER;

      bool foundDot = false;
      bool foundExp = false;

      int base = 10;

      if (c == '0')
      {
         if (c == 'x' || c == 'X')
            base = 16;
         else if (c == 'b' || c == 'B')
            base = 2;
         else
            base = 8;

         if (base != 8)
         {
            token->data += c;
            c = in->get();
         }
      }

      // Numeric part.
      while (true)
      {
         token->data += c;
         c = in->get();

         // Fractional component.
         if (c == '.' && !foundDot)
         {
            foundDot = true;
            token->type = TT_FLOAT;

            // Float literals can start with 0 and are still decimal.
            if (base == 8) base = 10;

            continue;
         }

         // Exponent.
         if (!foundExp &&
            ((base ==  8 && (c == 'E' || c == 'e')) ||
             (base == 10 && (c == 'E' || c == 'e')) ||
             (base == 16 && (c == 'P' || c == 'p'))))
         {
            foundExp = true;
            token->type = TT_FLOAT;

            // Float literals can start with 0 and are still decimal.
            if (base == 8) base = 10;

            // The exponent for a hex-float is decimal.
            if (base == 16) base = 10;

            token->data += c;
            c = in->get();

            // Allow an optional sign.
            if (c == '+' || c == '-') continue;

            // This intentionally falls through to the digit checks.
         }

         if ('0' <= c && c <= '1' && base >=  2) continue;
         if ('2' <= c && c <= '7' && base >=  8) continue;
         if ('8' <= c && c <= '9' && base >=  8) continue;
         if ('A' <= c && c <= 'F' && base >= 16) continue;
         if ('a' <= c && c <= 'f' && base >= 16) continue;

         break;
      }

      // Check for suffix.
      if (token->type == TT_FLOAT)
      {
         if (c == 'F' || c == 'f' || c == 'L' || c == 'l')
            token->data += c;
         else
            in->unget(c);
      }
      else
      {
         bool foundU = false;
         bool foundL = false;

         in->unget(c);

         while (true)
         {
            c = in->get();

            if ((c == 'U' || c == 'u') && !foundU)
            {
               foundU = true;
               token->data += c;

               continue;
            }

            if ((c == 'L' || c == 'l') && !foundL)
            {
               foundL = true;
               token->data += c;

               char oldc = c;
               c = in->get();

               // LL or ll
               if (c == oldc) token->data += c;
               else in->unget(c);

               continue;
            }

            break;
         }

         in->unget(c);
      }

      return;
   }

   if (c == '"' || c == '\'')
   {
      token->type = c == '"' ? TT_STRING : TT_CHARACTER;

      c = in->get();

      while (in->isInQuote())
      {
         token->data += c;

         c = in->get();
      }

      return;
   }
}

//
// make_string<SourceTokenC::TokenType>
//
std::string const &make_string(SourceTokenC::TokenType type)
{
   return tt_names[type];
}

// EOF

