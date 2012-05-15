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
   typedef std::map<std::string, std::string> DefMap;

   typedef std::vector<SourceTokenC::Reference> MacroVec;
   typedef std::vector<SourceTokenC::Reference> MacroArg;
   typedef std::vector<MacroArg> MacroArgs;
   typedef std::vector<std::string> MacroParm;
   typedef std::pair<MacroParm, std::string> MacroData;
   typedef std::map<std::string, MacroData> MacroMap;


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


   static void add_define_base(std::string const &name, std::string const &data);

   static void add_macro_base(std::string const &name, MacroParm const &parm,
                              std::string const &data);

   static void rem_define_base(std::string const &name);

private:
   void addDefine(std::string const &name, std::string const &data);

   void addMacro(std::string const &name, MacroData const &data);

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

   void expand(MacroVec &out, std::set<std::string> &used,
               SourcePosition const &pos, SourceTokenC::Reference tok,
               SourceStream *in);
   void expand(MacroVec &out, std::set<std::string> &used,
               SourcePosition const &pos, std::string const &data);
   void expand(MacroVec &out, std::set<std::string> &used,
      SourcePosition const &pos, MacroData const &data, MacroArgs const &args);

   void expandDefine(SourceTokenC *tok);
   void expandMacro(SourceTokenC *tok);

   bool getIf();
   CounterPointer<ObjectExpression> getIfMultiple();
   CounterPointer<ObjectExpression> getIfSingle();

   SourceTokenC::Reference getRaw();

   bool hasDefine(std::string const &name);
   bool hasMacro(std::string const &name);

   bool isSkip();

   SourceTokenC::Reference peekRaw();

   void readArgs(MacroArgs &args, SourceStream *in, MacroData const &data,
                 SourcePosition const &pos);

   void remDefine(std::string const &name);
   void remSkip();

   DefMap defines;
   MacroMap macros;

   std::vector<SourceStream *> inStack;
   std::vector<bool> skipStack;
   std::vector<SourceTokenC::Reference> ungetStack;
   std::vector<bool> unskipStack;

   bool canExpand : 1;


   static DefMap defines_base;
   static MacroMap macros_base;
};

#endif//HPP_SourceTokenizerC_

