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
// Generic tokenizer wrapper.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceTokenizer_
#define HPP_SourceTokenizer_

#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceStream;

//
// SourceTokenizer
//
template<typename SourceToken> class SourceTokenizer
{
public:
   typedef typename SourceToken::Pointer   TokenPtr;
   typedef typename SourceToken::Reference TokenRef;
   typedef typename SourceToken::TokenType TokenType;


   //
   // ::SourceTokenizer
   //
   explicit SourceTokenizer(SourceStream *_in) : in(_in)
   {
   }

   //
   // ::dropType
   //
   bool dropType(TokenType type)
   {
      if(peekType(type))
      {
         get();
         return true;
      }
      else
         return false;
   }

   //
   // ::get
   //
   TokenRef get()
   {
      if(!ungetStack.empty())
      {
         TokenRef tok = ungetStack.back();
         ungetStack.pop_back();

         return tok;
      }
      else
         return SourceToken::create(in);
   }

   //
   // ::get
   //
   TokenRef get(TokenType type)
   {
      TokenRef tok = get();
      doAssert(tok, type);
      return tok;
   }

   //
   // ::get
   //
   TokenRef get(TokenType type, std::string const &data)
   {
      TokenRef tok = get();
      doAssert(tok, type); doAssert(tok, data);
      return tok;
   }

   //
   // ::peek
   //
   TokenRef peek()
   {
      TokenRef tok = get(); unget(tok);
      return tok;
   }

   //
   // ::peek
   //
   TokenRef peek(TokenType type)
   {
      TokenRef tok = get(); unget(tok);
      doAssert(tok, type);
      return tok;
   }

   //
   // ::peekType
   //
   bool peekType(TokenType type)
   {
      TokenRef tok = peek();
      return tok->type == type;
   }

   //
   // ::peekType
   //
   bool peekType(TokenType type, std::string const &data)
   {
      TokenRef tok = peek();
      return tok->type == type && tok->data == data;
   }

   //
   // ::unget
   //
   void unget(SourceToken *token)
   {
      ungetStack.push_back(static_cast<TokenRef>(token));
   }

private:
   //
   // ::doAssert
   //
   void doAssert(SourceToken *tok, TokenType type)
   {
      if(tok->type != type)
      {
         Error(tok->pos, "expected %s got %s", make_string(type).c_str(),
               make_string(tok->type).c_str());
      }
   }

   //
   // ::doAssert
   //
   void doAssert(SourceToken *tok, std::string const &data)
   {
      if(tok->data != data)
         Error(tok->pos, "expected %s got %s", data.c_str(), tok->data.c_str());
   }

   std::vector<TokenRef> ungetStack;
   SourceStream *in;
};

#endif//HPP_SourceTokenizer_

