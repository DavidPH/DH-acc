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
// DS preprocessing.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceTokenizerC_
#define HPP_SourceTokenizerC_

#include "Counter.hpp"
#include "SourceTokenC.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class SourceStream;

class SourceTokenizerC
{
public:
   typedef std::vector<SourceTokenC::Reference> DefVec;
   typedef std::map<std::string, DefVec> DefMap;

   typedef std::vector<std::string> MacroArg;
   typedef std::vector<SourceTokenC::Reference> MacroVec;
   typedef std::pair<MacroArg, MacroVec> MacroDat;
   typedef std::map<std::string, MacroDat> MacroMap;


   explicit SourceTokenizerC(SourceStream *in);
   ~SourceTokenizerC();

   SourceTokenC::Reference get();
   SourceTokenC::Reference get(SourceTokenC::TokenType type);
   SourceTokenC::Reference get(SourceTokenC::TokenType type,
                               std::string const &data);

   SourceTokenC::Reference peek();
   SourceTokenC::Reference peek(SourceTokenC::TokenType type);

   bool peekType(SourceTokenC::TokenType type);
   bool peekType(SourceTokenC::TokenType type, std::string const &data);

   void unget(SourceTokenC *token);


   static void add_define_base(std::string const &name);
   static void add_define_base(std::string const &name, DefVec const &tokens);
   static void add_define_base(std::string const &name, std::string const &source);

   static void add_macro_base(std::string const &name, MacroArg const &arg);
   static void add_macro_base(std::string const &name, MacroArg const &arg,
                              std::string const &data);
   static void add_macro_base(std::string const &name, MacroDat const &dat);

private:
   SourceTokenizerC(SourceTokenizerC const &tokenizer)/* = delete*/;

   void addDefine(std::string const &name);
   void addDefine(std::string const &name, SourcePosition const &pos,
                  DefVec const &vec);

   void addMacro(std::string const &name, SourcePosition const &pos,
                 MacroDat const &dat);

   void addSkip(bool skip);

   void doAssert(SourceTokenC *tok, SourceTokenC::TokenType type);
   void doAssert(SourceTokenC *tok, std::string const &data);

   void doCommand();
   void doCommand_define(SourceTokenC *tok);
   void doCommand_else(SourceTokenC *tok);
   void doCommand_elif(SourceTokenC *tok);
   void doCommand_endif(SourceTokenC *tok);
   void doCommand_error(SourceTokenC *tok);
   void doCommand_if(SourceTokenC *tok);
   void doCommand_ifdef(SourceTokenC *tok);
   void doCommand_ifndef(SourceTokenC *tok);
   void doCommand_include(SourceTokenC *tok);
   void doCommand_undef(SourceTokenC *tok);

   bool getIf();
   CounterPointer<ObjectExpression> getIfMultiple();
   CounterPointer<ObjectExpression> getIfSingle();

   SourceTokenC::Reference getRaw();

   bool hasDefine(std::string const &name);
   bool hasMacro(std::string const &name);

   bool isSkip();

   SourceTokenizerC & operator = (SourceTokenizerC const &tokenizer)/* = delete*/;

   void prepDefine(SourceTokenC *tok);
   void prepMacro(SourceTokenC::Reference tok);

   void remDefine(std::string const &name);
   void remSkip();

   DefMap defines;
   MacroMap macros;

   std::set<std::string> definesUsed;

   std::vector<SourceStream *> in;
   std::vector<bool> skipStack;
   std::vector<SourceTokenC::Reference> ungetStack;
   std::vector<bool> unskipStack;

   bool canCommand : 1;
   bool canExpand  : 1;
   bool canSkip    : 1;


   static DefMap defines_base;
   static MacroMap macros_base;
};

#endif//HPP_SourceTokenizerC_

