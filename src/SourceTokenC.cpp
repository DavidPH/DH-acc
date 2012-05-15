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

#include "SourceException.hpp"
#include "SourceStream.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::string const tt_datas[SourceTokenC::TT_NONE+1] =
{
   "",
   "",
   "",
   "",
   "",

   " ",
   "#",
   "##",
   "###",

   "++",
   "+",
   "+=",
   "&&",
   "&&=",
   "&",
   "&=",
   "@",
   "==",
   ">=",
   ">",
   "<=",
   "<",
   "!=",
   ":",
   ",",
   "/",
   "/=",
   ".",
   "...",
   "=",
   "||",
   "||=",
   "|",
   "|=",
   "<<",
   "<<=",
   "->",
   "%",
   "%=",
   "*",
   "*=",
   "~",
   "!",
   "?",
   ">>",
   ">>=",
   "--",
   "-",
   "-=",
   "^^",
   "^^=",
   "^",
   "^=",

   "{",
   "}",
   "[",
   "]",
   "(",
   ")",

   ";",

   " "
};

static std::string const tt_names[SourceTokenC::TT_NONE+1] =
{
   "TT_CHR",
   "TT_FLT",
   "TT_NAM",
   "TT_INT",
   "TT_STR",

   "TT_ENDL", // \n
   "TT_HASH1", // #
   "TT_HASH2", // ##
   "TT_HASH3", // ###

   "TT_AD2",    // ++
   "TT_ADD",    // +
   "TT_ADD_EQ", // +=
   "TT_AN2",    // &&
   "TT_AN2_EQ", // &&=
   "TT_AND",    // &
   "TT_AND_EQ", // &=
   "TT_AT",     // @
   "TT_CMP_EQ", // ==
   "TT_CMP_GE", // >=
   "TT_CMP_GT", // >
   "TT_CMP_LE", // <=
   "TT_CMP_LT", // <
   "TT_CMP_NE", // !=
   "TT_COLON",  // :
   "TT_COMMA",  // ,
   "TT_DIV",    // /
   "TT_DIV_EQ", // /=
   "TT_DOT",    // .
   "TT_DOT3",   // ...
   "TT_EQUALS", // =
   "TT_IO2",    // ||
   "TT_IO2_EQ", // ||=
   "TT_IOR",    // |
   "TT_IOR_EQ", // |=
   "TT_LSH",    // <<
   "TT_LSH_EQ", // <<=
   "TT_MEM",    // ->
   "TT_MOD",    // %
   "TT_MOD_EQ", // %=
   "TT_MUL",    // *
   "TT_MUL_EQ", // *=
   "TT_NOTBIT", // ~
   "TT_NOTLOG", // !
   "TT_QUERY",  // ?
   "TT_RSH",    // >>
   "TT_RSH_EQ", // >>=
   "TT_SU2",    // --
   "TT_SUB",    // -
   "TT_SUB_EQ", // -=
   "TT_XO2",    // ^^
   "TT_XO2_EQ", // ^^=
   "TT_XOR",    // ^
   "TT_XOR_EQ", // ^=

   "TT_BRACE_O", // {
   "TT_BRACE_C", // }
   "TT_BRACK_O", // [
   "TT_BRACK_C", // ]
   "TT_PAREN_O", // (
   "TT_PAREN_C", // )

   "TT_SEMICOLON", // ;

   "TT_NONE"
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// dequote
//
static void dequote(std::string &out, std::string const &in)
{
   for (std::string::const_iterator end = in.end(),
        itr = in.begin(); itr != end; ++itr) switch (*itr)
   {
   case '"':
      out += "\\\"";
      break;

   default:
      out += *itr;
      break;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenC::create_join
//
SourceTokenC::Reference SourceTokenC::create_join(SourceTokenC const *l,
                                                  SourceTokenC const *r)
{
   if (l->type != TT_NAM || r->type != TT_NAM)
      ERROR(l->pos, "TODO:can only join TT_NAM");

   Reference tok(new SourceTokenC(*l));
   tok->data += r->data;
   return tok;
}

//
// SourceTokenC::read_token
//
void SourceTokenC::read_token(SourceStream *in, SourceTokenC *token)
{
   char c;

   // Discard any whitespace before token.
   while (isspace(c = in->get())) if (c == '\n') break;

   token->pos.filename = in->getFilename();
   token->pos.line = in->getLineCount();
   token->pos.column = in->getColumn();

   token->data.clear();

   switch (c)
   {
   case '\n': token->type = TT_ENDL; return;
   case '@': token->type = TT_AT;     return;
   case ':': token->type = TT_COLON;  return;
   case ',': token->type = TT_COMMA;  return;
   case '~': token->type = TT_NOTBIT; return;
   case '?': token->type = TT_QUERY;  return;
   case '{': token->type = TT_BRACE_O; return;
   case '}': token->type = TT_BRACE_C; return;
   case '[': token->type = TT_BRACK_O; return;
   case ']': token->type = TT_BRACK_C; return;
   case '(': token->type = TT_PAREN_O; return;
   case ')': token->type = TT_PAREN_C; return;
   case ';': token->type = TT_SEMICOLON; return;

   case '+':
      c = in->get();
      if (c == '=') {token->type = TT_ADD_EQ; return;}
      if (c == '+') {token->type = TT_AD2;    return;}
      in->unget(c);  token->type = TT_ADD;    return;

   case '&':
      c = in->get();
      if (c == '=')    {token->type = TT_AND_EQ; return;}
      if (c == '&') { c = in->get();
         if (c == '=') {token->type = TT_AN2_EQ; return;}
         in->unget(c);  token->type = TT_AN2;    return;}
      in->unget(c);     token->type = TT_AND;    return;

   case '>':
      c = in->get();
      if (c == '=')    {token->type = TT_CMP_GE; return;}
      if (c == '>') { c = in->get();
         if (c == '=') {token->type = TT_RSH_EQ; return;}
         in->unget(c);  token->type = TT_RSH;    return;}
      in->unget(c);     token->type = TT_CMP_GT; return;

   case '<':
      c = in->get();
      if (c == '=')    {token->type = TT_CMP_LE; return;}
      if (c == '<') { c = in->get();
         if (c == '=') {token->type = TT_LSH_EQ; return;}
         in->unget(c);  token->type = TT_LSH;    return;}
      in->unget(c);     token->type = TT_CMP_LT; return;

   case '.':
      c = in->get();
      if (c == '.') { c = in->get();
         if (c == '.') {token->type = TT_DOT3;   return;}
         in->unget(c);}
      in->unget(c);     token->type = TT_DOT;    return;

   case '/':
      c = in->get();
      if (c == '=') {token->type = TT_DIV_EQ; return;}
      in->unget(c);  token->type = TT_DIV;    return;

   case '=':
      c = in->get();
      if (c == '=') {token->type = TT_CMP_EQ; return;}
      in->unget(c);  token->type = TT_EQUALS; return;

   case '|':
      c = in->get();
      if (c == '=')    {token->type = TT_IOR_EQ; return;}
      if (c == '|') { c = in->get();
         if (c == '=') {token->type = TT_IO2_EQ; return;}
         in->unget(c);  token->type = TT_IO2;    return;}
      in->unget(c);     token->type = TT_IOR;    return;

   case '%':
      c = in->get();
      if (c == '=') {token->type = TT_MOD_EQ; return;}
      in->unget(c);  token->type = TT_MOD;    return;

   case '*':
      c = in->get();
      if (c == '=') {token->type = TT_MUL_EQ; return;}
      in->unget(c);  token->type = TT_MUL;    return;

   case '!':
      c = in->get();
      if (c == '=') {token->type = TT_CMP_NE; return;}
      in->unget(c);  token->type = TT_NOTLOG; return;

   case '-':
      c = in->get();
      if (c == '=') {token->type = TT_SUB_EQ; return;}
      if (c == '>') {token->type = TT_MEM;    return;}
      if (c == '-') {token->type = TT_SU2;    return;}
      in->unget(c);  token->type = TT_SUB;    return;

   case '^':
      c = in->get();
      if (c == '=')    {token->type = TT_XOR_EQ; return;}
      if (c == '^') { c = in->get();
         if (c == '=') {token->type = TT_XO2_EQ; return;}
         in->unget(c);  token->type = TT_XO2;    return;}
      in->unget(c);     token->type = TT_XOR;    return;

   case '#':
      c = in->get();
      if (c == '#') { c = in->get();
         if (c == '#') {token->type = TT_HASH3; return;}
         in->unget(c);  token->type = TT_HASH2; return;}
      in->unget(c);     token->type = TT_HASH1; return;
   }

   if (isalpha(c) || c == '_')
   {
      token->type = TT_NAM;

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
      token->type = TT_INT;

      bool foundDot = false;
      bool foundExp = false;

      int base = 10;

      if (c == '0')
      {
         char cPeek = in->get();
         in->unget(cPeek);

         if (cPeek == 'x' || cPeek == 'X')
            base = 16;
         else if (cPeek == 'b' || cPeek == 'B')
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
            token->type = TT_FLT;

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
            token->type = TT_FLT;

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
      if (token->type == TT_FLT)
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

            if ((c == 'H' || c == 'h') && !foundL)
            {
               foundL = true;
               token->data += c;

               char oldc = c;
               c = in->get();

               // HH or hh
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
      token->type = c == '"' ? TT_STR : TT_CHR;

      c = in->get();

      while (in->isInQuote())
      {
         token->data += c;

         c = in->get();
      }

      return;
   }

   ERROR(token->pos, "unexpected character '%c' (%i)", c, static_cast<int>(c));
}

//
// SourceTokenC::tt_str
//
SourceTokenC::Reference SourceTokenC::tt_str(SourcePosition const &pos,
   std::vector<Reference> const &args)
{
   Reference tok(new SourceTokenC(pos, TT_STR));

   for (std::vector<Reference>::const_iterator end = args.end(),
        itr = args.begin(); itr != end; ++itr) switch ((*itr)->type)
   {
   case TT_CHR:
      tok->data.reserve(tok->data.size() + (*itr)->data.size() + 2);
      tok->data += '\'';
      dequote(tok->data, (*itr)->data);
      tok->data += '\'';
      break;

   case TT_FLT:
   case TT_NAM:
   case TT_INT:
      tok->data += (*itr)->data;
      break;

   case TT_STR:
      tok->data.reserve(tok->data.size() + (*itr)->data.size() + 2);
      tok->data += '"';
      dequote(tok->data, (*itr)->data);
      tok->data += '"';
      break;

   default:
      tok->data += tt_datas[(*itr)->type];
      break;
   }

   // Now search for trailing/leading whitespace and remove.
   std::string::iterator end = tok->data.end(), itr = tok->data.begin();

   while (itr != end && std::isspace(*itr)) ++itr;
   while (end != itr && std::isspace(*(end-1))) --end;

   if (end != tok->data.end() || itr != tok->data.begin())
      tok->data = std::string(itr, end);

   return tok;
}

//
// make_string<SourceTokenC::TokenType>
//
std::string const &make_string(SourceTokenC::TokenType type)
{
   return tt_names[type];
}

// EOF

