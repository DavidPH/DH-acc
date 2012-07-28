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
// C preprocessing.
//
//-----------------------------------------------------------------------------

#include "SourceTokenizerC.hpp"

#include "ObjectExpression.hpp"
#include "option.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"

#include <cctype>
#include <cstring>
#include <sstream>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static int add_define_base
(char const *opt, int optf, int argc, char const *const *argv);

static int rem_define_base
(char const *opt, int optf, int argc, char const *const *argv);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_call option_define_handler
('D', "define", NULL, "Adds a define.", NULL, add_define_base);

static option::option_call option_undef_handler
('U', "undef", NULL, "Removes a define.", NULL, rem_define_base);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceTokenizerC::DefMap SourceTokenizerC::defines_base;
SourceTokenizerC::MacroMap SourceTokenizerC::macros_base;
extern bool option_function_autoargs;
extern bool option_script_autoargs;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// add_define_base
//
static int add_define_base
(char const *opt, int optf, int argc, char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   char const *base = argv[0], *arg = 0, *eq;

   // Search for '(' and '='.
   for (eq = base; *eq && *eq != '='; ++eq)
      if (*eq == '(') {arg = eq; break;}

   for (; *eq && *eq != '='; ++eq);

   if (arg)
   {
      std::string name(base, arg);
      std::vector<std::string> args;

      if (*++arg != ')') for (char const *itr = arg;;)
      {
         if (*itr == ',')
         {
            args.push_back(std::string(arg, itr)); arg = ++itr;
         }
         else if (*itr == ')')
         {
            args.push_back(std::string(arg, itr)); break;
         }
         else if (itr++ == eq)
            option::exception::error(opt, optf, "missing ')'");
      }

      if (*eq)
         SourceTokenizerC::add_macro_base(name, args, std::string(eq+1));
      else
         SourceTokenizerC::add_macro_base(name, args, std::string());
   }
   else
   {
      std::string name(base, eq);

      if (*eq)
         SourceTokenizerC::add_define_base(name, std::string(eq+1));
      else
         SourceTokenizerC::add_define_base(name, std::string());
   }

   return 1;
}

//
// find_arg
//
static SourceTokenizerC::MacroArg const *find_arg(
   SourceTokenizerC::MacroArgs const &args,
   SourceTokenizerC::MacroParm const &parm, std::string const &name)
{
   if (name == "__VA_ARGS__")
   {
      if (parm.back().empty())
         return &args.back();
      else
         return NULL;
   }

   for (size_t i = 0, e = parm.size(); i != e; ++i)
      if (parm[i] == name)
         return &args[i];

   return NULL;
}

//
// rem_define_base
//
static int rem_define_base(char const *opt, int optf, int argc,
                           char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   SourceTokenizerC::rem_define_base(argv[0]);

   return 1;
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenizerC::SourceTokenizerC
//
SourceTokenizerC::SourceTokenizerC(SourceStream *_in)
 : defines(defines_base), macros(macros_base),
   canExpand(true)
{
   switch (target_type)
   {
   case TARGET_Eternity: addDefine("__TARGET_Eternity__", ""); break;
   case TARGET_Hexen:    addDefine("__TARGET_Hexen__",    ""); break;
   case TARGET_HexPP:    addDefine("__TARGET_HexPP__",    ""); break;
   case TARGET_ZDoom:    addDefine("__TARGET_ZDoom__",    ""); break;
   case TARGET_UNKNOWN:  addDefine("__TARGET_UNKNOWN__",  ""); break;
   }

   if (option_function_autoargs)
      addDefine("__FUNCTION_AUTOARGS__", "");

   if (option_script_autoargs)
      addDefine("__SCRIPT_AUTOARGS__", "");

   inStack.push_back(_in);
   ungetStack.push_back(static_cast<SourceTokenC::Reference>(
      new SourceTokenC(SourcePosition::builtin(), SourceTokenC::TT_ENDL)));
}

//
// SourceTokenizerC::~SourceTokenizerC
//
SourceTokenizerC::~SourceTokenizerC()
{
   // All but the first SourceStream was alloc'd by this.
   while (inStack.size() > 1)
   {
      delete inStack.back(); inStack.pop_back();
   }
}

//
// SourceTokenizerC::add_define_base
//
void SourceTokenizerC::add_define_base(std::string const &name,
                                       std::string const &data)
{
   rem_define_base(name);
   (defines_base[name] = data) += "  ";
}

//
// SourceTokenizerC::add_macro_base
//
void SourceTokenizerC::add_macro_base(std::string const &name,
   MacroParm const &parm, std::string const &data)
{
   rem_define_base(name);
   MacroData &dat = macros_base[name];
   dat.first = parm;
   (dat.second = data) += "  ";
}

//
// SourceTokenizerC::addDefine
//
void SourceTokenizerC::addDefine(std::string const &name,
                                 std::string const &data)
{
   remDefine(name);
   (defines[name] = data) += "  ";
}

//
// SourceTokenizerC::addMacro
//
void SourceTokenizerC::addMacro
(std::string const &name, MacroData const &data)
{
   remDefine(name);
   (macros[name] = data).second += "  ";
}

//
// SourceTokenizerC::addSkip
//
void SourceTokenizerC::addSkip(bool skip)
{
   skipStack.push_back(skip);
   unskipStack.push_back(!skip);
}

//
// SourceTokenizerC::doAssert
//
void SourceTokenizerC::doAssert(SourceTokenC *tok, SourceTokenC::TokenType type)
{
   if (tok->type != type)
      ERROR(tok->pos, "expected %s got %s", make_string(type).c_str(),
            make_string(tok->type).c_str());
}

//
// SourceTokenizerC::doAssert
//
void SourceTokenizerC::doAssert(SourceTokenC *tok, std::string const &data)
{
   if (tok->data != data)
      ERROR(tok->pos, "expected %s got %s", data.c_str(), tok->data.c_str());
}

//
// SourceTokenizerC::doCommand
//
void SourceTokenizerC::doCommand()
{
   SourceTokenC::Reference tok = getRaw(); doAssert(tok, SourceTokenC::TT_NAM);

        if (tok->data == "define")  doCommand_define(tok);
   else if (tok->data == "else")    doCommand_else(tok);
   else if (tok->data == "elif")    doCommand_elif(tok);
   else if (tok->data == "endif")   doCommand_endif(tok);
   else if (tok->data == "error")   doCommand_error(tok);
   else if (tok->data == "if")      doCommand_if(tok);
   else if (tok->data == "ifdef")   doCommand_ifdef(tok);
   else if (tok->data == "ifndef")  doCommand_ifndef(tok);
   else if (tok->data == "include") doCommand_include(tok);
   else if (tok->data == "undef")   doCommand_undef(tok);

   else ERROR(tok->pos, "unknown command: %s", tok->data.c_str());
}

//
// SourceTokenizerC::doCommand_define
//
void SourceTokenizerC::doCommand_define(SourceTokenC *)
{
   SourceTokenC::Reference name = getRaw(); doAssert(name, SourceTokenC::TT_NAM);

   if (inStack.back()->peek() == '(')
   {
      MacroData data;

      doAssert(getRaw(), SourceTokenC::TT_PAREN_O);
      SourceTokenC::Reference arg = getRaw();
      if (arg->type != SourceTokenC::TT_PAREN_C) for (;;)
      {
         if (arg->type == SourceTokenC::TT_DOT3)
         {
            data.first.push_back(std::string());
            doAssert((arg = getRaw()), SourceTokenC::TT_PAREN_C);
            break;
         }

         doAssert(arg, SourceTokenC::TT_NAM);
         data.first.push_back(arg->data);

         if ((arg = getRaw())->type == SourceTokenC::TT_PAREN_C) break;
         doAssert(arg, SourceTokenC::TT_COMMA);
         arg = getRaw();
      }

      for (char c; (c = inStack.back()->get()) != '\n';)
         data.second += c;

      inStack.back()->unget('\n');

      if (isSkip()) return;

      addMacro(name->data, data);
   }
   else
   {
      std::string data;

      for (char c; (c = inStack.back()->get()) != '\n';)
         data += c;

      inStack.back()->unget('\n');

      if (isSkip()) return;

      addDefine(name->data, data);
   }
}

//
// SourceTokenizerC::doCommand_else
//
void SourceTokenizerC::doCommand_else(SourceTokenC *tok)
{
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   if (skipStack.empty())
      ERROR(tok->pos, "unmatched #else");

   skipStack.back() = unskipStack.back();
   unskipStack.back() = true; // If it wasn't, it is now.
}

//
// SourceTokenizerC::doCommand_elif
//
void SourceTokenizerC::doCommand_elif(SourceTokenC *tok)
{
   if (skipStack.empty())
      ERROR(tok->pos, "unmatched #elif");

   bool ifResult = !!getExpr()->resolveINT();
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

     skipStack.back() = unskipStack.back() || !ifResult;
   unskipStack.back() = unskipStack.back() ||  ifResult;
}

//
// SourceTokenizerC::doCommand_endif
//
void SourceTokenizerC::doCommand_endif(SourceTokenC *tok)
{
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   if (skipStack.empty())
      ERROR(tok->pos, "unmatched #endif");

   remSkip();
}

//
// SourceTokenizerC::doCommand_error
//
void SourceTokenizerC::doCommand_error(SourceTokenC *)
{
   SourceTokenC::Reference msg = getRaw(); doAssert(msg, SourceTokenC::TT_STR);
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   if (isSkip()) return;

   ERROR(msg->pos, "#error %s", msg->data.c_str());
}

//
// SourceTokenizerC::doCommand_if
//
void SourceTokenizerC::doCommand_if(SourceTokenC *)
{
   addSkip(!getExpr()->resolveINT());
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);
}

//
// SourceTokenizerC::doCommand_ifdef
//
void SourceTokenizerC::doCommand_ifdef(SourceTokenC *)
{
   SourceTokenC::Reference name = getRaw(); doAssert(name, SourceTokenC::TT_NAM);
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   addSkip(!hasDefine(name->data) && !hasMacro(name->data));
}

//
// SourceTokenizerC::doCommand_ifndef
//
void SourceTokenizerC::doCommand_ifndef(SourceTokenC *)
{
   SourceTokenC::Reference name = getRaw(); doAssert(name, SourceTokenC::TT_NAM);
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   addSkip(hasDefine(name->data) || hasMacro(name->data));
}

//
// SourceTokenizerC::doCommand_include
//
void SourceTokenizerC::doCommand_include(SourceTokenC *)
{
   SourceTokenC::Reference inc = getRaw();
   std::string filename;

   if(inc->type == SourceTokenC::TT_STR)
      filename = inc->data;
   else if(inc->type == SourceTokenC::TT_CMP_LT)
   {
      for(char c; (c = inStack.back()->get()) != '>';)
      {
         if(c == '\n') ERROR(inc->pos, "unterminated include");
         filename += c;
      }
   }
   else
      ERROR(inc->pos, "expected TT_STR or TT_CMP_LT");

   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   if (isSkip()) return;

   try
   {
      inStack.push_back(new SourceStream(filename, SourceStream::ST_C));
      ungetStack.push_back(static_cast<SourceTokenC::Reference>(
         new SourceTokenC(SourcePosition::builtin(), SourceTokenC::TT_ENDL)));
   }
   catch (std::exception const &)
   {
      ERROR(inc->pos, "file not found: %s", filename.c_str());
   }
}

//
// SourceTokenizerC::doCommand_undef
//
void SourceTokenizerC::doCommand_undef(SourceTokenC *)
{
   SourceTokenC::Reference name = getRaw(); doAssert(name, SourceTokenC::TT_NAM);
   doAssert(peekRaw(), SourceTokenC::TT_ENDL);

   if (isSkip()) return;

   remDefine(name->data);
}

//
// SourceTokenizerC::expand
//
// Expands a token.
//
void SourceTokenizerC::expand(MacroVec &out, std::set<std::string> &used,
   SourcePosition const &pos, SourceTokenC::Reference tok, SourceStream *in,
   MacroArgs const *altArgs, MacroParm const *altParm)
{
   // Check for function-like macro expansion.
   // That is, a function-like macro name followed by an open parenthesis.
   // And of course, that name must not have been expanded yet.
   if(tok->type == SourceTokenC::TT_PAREN_O &&
      !out.empty() && out.back()->type == SourceTokenC::TT_NAM &&
      hasMacro(out.back()->data) && !used.count(out.back()->data))
   {
      tok = out.back(); out.pop_back();
      MacroData const &mdata = macros[tok->data];
      MacroArgs margs;
      readArgs(margs, in, mdata, pos, altArgs, altParm);

      used.insert(tok->data);
      expand(out, used, pos, mdata, margs);
      used.erase(tok->data);

      return;
   }

   // Check for object-like macro expansion.
   // Like above, but it only needs one token to trigger.
   if(tok->type == SourceTokenC::TT_NAM && hasDefine(tok->data) && !used.count(tok->data))
   {
      used.insert(tok->data);
      expand(out, used, pos, defines[tok->data]);
      used.erase(tok->data);

      return;
   }

   // Then check for __FILE__ specifically.
   if(tok->type == SourceTokenC::TT_NAM && tok->data == "__FILE__")
   {
      out.push_back(SourceTokenC::create(pos, pos.filename, SourceTokenC::TT_STR));

      return;
   }

   // And __LINE__.
   if(tok->type == SourceTokenC::TT_NAM && tok->data == "__LINE__")
   {
      std::ostringstream oss; oss << pos.line;
      out.push_back(SourceTokenC::create(pos, oss.str(), SourceTokenC::TT_INT));

      return;
   }

   // If it actually isn't any of those things, then consider the token expanded.
   out.push_back(tok);
}

//
// SourceTokenizerC::expand
//
// Expands an object-like macro.
//
void SourceTokenizerC::expand(MacroVec &out, std::set<std::string> &used,
   SourcePosition const &pos, std::string const &data)
{
   // Create a stream out of the macro data.
   SourceStream in(data, SourceStream::ST_C|SourceStream::STF_STRING);

   // Read and expand tokens until EOF.
   try { for(;;)
   {
      // Read a token and set its position to the caller's
      SourceTokenC::Reference tok = SourceTokenC::create(&in); tok->pos = pos;

      // And then expand the token, because it might be a macro invocation.
      expand(out, used, pos, tok, &in);
   }
   } catch(SourceStream::EndOfStream const &) {}
}

//
// SourceTokenizerC::expand
//
// Expands a function-like macro.
//
void SourceTokenizerC::expand(MacroVec &out, std::set<std::string> &used,
   SourcePosition const &pos, MacroData const &data, MacroArgs const &args)
{
   MacroArg::const_iterator tokEnd, tokItr;
   MacroArg const *arg;

   // Create a stream out of the macro data.
   SourceStream in(data.second, SourceStream::ST_C|SourceStream::STF_STRING);

   // Read and expand tokens until EOF.
   try { for(;;)
   {
      // Read a token and set its position to the caller's
      SourceTokenC::Reference tok = SourceTokenC::create(&in); tok->pos = pos;

      // Check for argument.
      if(tok->type == SourceTokenC::TT_NAM && (arg = find_arg(args, data.first, tok->data)))
      {
         // Expand all of the argument's tokens.
         for(tokEnd = arg->end(), tokItr = arg->begin(); tokItr != tokEnd; ++tokItr)
            expand(out, used, pos, *tokItr, &in);

         continue;
      }

      // Check for operator #.
      if(tok->type == SourceTokenC::TT_HASH1)
      {
         try
         {
            arg = NULL;
            tok = SourceTokenC::create(&in);
            if (tok->type == SourceTokenC::TT_NAM)
               arg = find_arg(args, data.first, tok->data);
         }
         catch(SourceStream::EndOfStream const &) {}

         if(!arg) ERROR_P("# must be used on arg");

         // Make a string out of the tokens. TT_NONEs are used to preserve whitespace.
         out.push_back(SourceTokenC::tt_str(tok->pos, *arg));

         continue;
      }

      // Check for operator ##.
      if(tok->type == SourceTokenC::TT_HASH2)
      {
         if(out.empty()) ERROR_P("## must not come at beginning");

         try
         {
            tok = SourceTokenC::create(&in);
         }
         catch(SourceStream::EndOfStream const &)
         {
            ERROR_P("## must not come at end");
         }

         // Check for argument.
         if(tok->type == SourceTokenC::TT_NAM && (arg = find_arg(args, data.first, tok->data)))
         {
            // If the next token is an argument, we only want to join its first token.
            tok = SourceTokenC::create_join(out.back(), *arg->begin());
            out.pop_back();

            // Expand the newly created token. It might be a macro name, now!
            expand(out, used, pos, tok, &in);

            // The rest of the argument is then expanded as above.
            for(tokEnd = arg->end(), tokItr = arg->begin() + 1; tokItr != tokEnd; ++tokItr)
               expand(out, used, pos, *tokItr, &in);
         }
         else
         {
            // Otherwise, just join the two tokens.
            tok = SourceTokenC::create_join(out.back(), tok);
            out.pop_back();

            // And then expand it.
            expand(out, used, pos, tok, &in);
         }

         continue;
      }

      // If it's not any of those other things, then it needs to be expanded.
      expand(out, used, pos, tok, &in, &args, &data.first);
   }
   } catch(SourceStream::EndOfStream const &) {}
}

//
// SourceTokenizerC::expandDefine
//
void SourceTokenizerC::expandDefine(SourceTokenC *tok)
{
   MacroVec out;
   std::set<std::string> used;

   // Invoke the macro expander, capturing the result in out.
   used.insert(tok->data);
   expand(out, used, tok->pos, defines[tok->data]);
   used.erase(tok->data);

   // Push out onto the unget stack.
   for(MacroVec::iterator end = out.begin(), itr = out.end(); itr-- != end;)
      ungetStack.push_back(*itr);
}

//
// SourceTokenizerC::expandMacro
//
void SourceTokenizerC::expandMacro(SourceTokenC *tok)
{
   MacroVec out;
   MacroArgs args;
   MacroData const &data = macros[tok->data];
   std::set<std::string> used;

   // Process the arguments, preserving whitespace in TT_NONEs.
   readArgs(args, inStack.back(), data, tok->pos);

   // Invoke the macro expander, capturing the result in out.
   used.insert(tok->data);
   expand(out, used, tok->pos, data, args);
   used.erase(tok->data);

   // Push out onto the unget stack.
   for(MacroVec::iterator end = out.begin(), itr = out.end(); itr-- != end;)
      ungetStack.push_back(*itr);
}

//
// SourceTokenizerC::get
//
SourceTokenC::Reference SourceTokenizerC::get()
{
   SourceTokenC::Pointer tok;

   for (;;)
   {
      tok = getExpand();

      // Preprocessor directive.
      if (tok->type == SourceTokenC::TT_ENDL)
      {
         while (tok->type == SourceTokenC::TT_ENDL)
            tok = getExpand();

         if (tok->type == SourceTokenC::TT_HASH1)
         {
            doCommand(); continue;
         }
      }

      if (isSkip())
         continue;

      if (tok->type == SourceTokenC::TT_NONE)
         continue;

      break;
   }

   // String literal concatenation.
   if (tok->type == SourceTokenC::TT_STR)
   {
      SourceTokenC::Reference tmpTok = get();

      if (tmpTok->type == SourceTokenC::TT_STR)
         tok->data += tmpTok->data;
      else
         unget(tmpTok);
   }

   return static_cast<SourceTokenC::Reference>(tok);
}

//
// SourceTokenizerC::get
//
SourceTokenC::Reference SourceTokenizerC::get(SourceTokenC::TokenType type)
{
   SourceTokenC::Reference tok = get();
   doAssert(tok, type);
   return tok;
}

//
// SourceTokenizerC::get
//
SourceTokenC::Reference SourceTokenizerC::get
(SourceTokenC::TokenType type, std::string const &data)
{
   SourceTokenC::Reference tok = get();
   doAssert(tok, type); doAssert(tok, data);
   return tok;
}

//
// SourceTokenizerC::getExpand
//
SourceTokenC::Reference SourceTokenizerC::getExpand()
{
   SourceTokenC::Reference tok = getRaw();

   if (canExpand && tok->type == SourceTokenC::TT_NAM)
   {
      if (hasMacro(tok->data))
      {
         SourceTokenC::Reference tmpTok = getExpand();

         // Macro invocation!
         if (tmpTok->type == SourceTokenC::TT_PAREN_O)
         {
            expandMacro(tok); return getExpand();
         }

         unget(tmpTok);
      }

      if (hasDefine(tok->data))
      {
         expandDefine(tok); return getExpand();
      }

      if (tok->data == "__FILE__")
         return SourceTokenC::create(tok->pos, tok->pos.filename,
                                     SourceTokenC::TT_STR);

      if (tok->data == "__LINE__")
      {
         std::ostringstream oss; oss << tok->pos.line;
         return SourceTokenC::create(tok->pos, oss.str(), SourceTokenC::TT_INT);
      }
   }

   return tok;
}

//
// SourceTokenizerC::getRaw
//
SourceTokenC::Reference SourceTokenizerC::getRaw()
{
   if (!ungetStack.empty())
   {
      canExpand = false;
      SourceTokenC::Reference tok = ungetStack.back();
      ungetStack.pop_back();

      return tok;
   }
   else try
   {
      canExpand = true;
      SourceTokenC::Reference tok(new SourceTokenC);
      tok->readToken(inStack.back());
      return tok;
   }
   catch (SourceStream::EndOfStream &e)
   {
      if (inStack.size() == 1) throw;
      delete inStack.back(); inStack.pop_back();
      return getRaw();
   }
}

//
// SourceTokenizerC::hasDefine
//
bool SourceTokenizerC::hasDefine(std::string const &name)
{
   return defines.find(name) != defines.end();
}

//
// SourceTokenizerC::hasMacro
//
bool SourceTokenizerC::hasMacro(std::string const &name)
{
   return macros.find(name) != macros.end();
}

//
// SourceTokenizerC::isSkip
//
bool SourceTokenizerC::isSkip()
{
   std::vector<bool>::iterator it;

   for (it = skipStack.begin(); it != skipStack.end(); ++it)
      if (*it) return true;

   return false;
}

//
// SourceTokenizerC::peek
//
SourceTokenC::Reference SourceTokenizerC::peek()
{
   SourceTokenC::Reference tok = get(); ungetStack.push_back(tok);
   return tok;
}

//
// SourceTokenizerC::peek
//
SourceTokenC::Reference SourceTokenizerC::peek(SourceTokenC::TokenType type)
{
   SourceTokenC::Reference tok = get(); ungetStack.push_back(tok);
   doAssert(tok, type);
   return tok;
}

//
// SourceTokenizerC::peekRaw
//
SourceTokenC::Reference SourceTokenizerC::peekRaw()
{
   SourceTokenC::Reference tok = getRaw();
   ungetStack.push_back(tok);
   return tok;
}

//
// SourceTokenizerC::peekType
//
bool SourceTokenizerC::peekType(SourceTokenC::TokenType type)
{
   SourceTokenC::Reference tok = get(); ungetStack.push_back(tok);
   return tok->type == type;
}

//
// SourceTokenizerC::peekType
//
bool SourceTokenizerC::peekType
(SourceTokenC::TokenType type, std::string const &data)
{
   SourceTokenC::Reference tok = get(); ungetStack.push_back(tok);
   return tok->type == type && tok->data == data;
}

//
// SourceTokenizerC::readArgs
//
// Process function-like macro arguments, preserving whitespace in TT_NONEs.
//
void SourceTokenizerC::readArgs(MacroArgs &args, SourceStream *in, MacroData const &data,
   SourcePosition const &pos, MacroArgs const *altArgs, MacroParm const *altParm)
{
   MacroArg const *arg;
   MacroParm const &parm = data.first;
   int pdepth = 0;

   // If there are no arguments expected, then this is just a simple assertion.
   if(parm.empty())
   {
      doAssert(SourceTokenC::create(in), SourceTokenC::TT_PAREN_C);
   }
   // Otherwise, start reading args. Each arg being a vector of tokens.
   // First thing is to create the first vector, though.
   else for(args.push_back(MacroArg());;)
   {
      // Convert each whitespace character into a TT_NONE token.
      // These are used by operator # to add spaces.
      while (std::isspace(in->peek()))
      {
         in->get();
         args.back().push_back(SourceTokenC::tt_none());
      }

      // Read the token.
      SourceTokenC::Reference tok = SourceTokenC::create(in);

      // If it's a parenthesis, terminate on the close-parenthesis that matches
      // the initial one that started the expansion.
      if(tok->type == SourceTokenC::TT_PAREN_O) ++pdepth;
      if(tok->type == SourceTokenC::TT_PAREN_C && !pdepth--) break;

      // A comma not in parentheses starts a new argument...
      if(tok->type == SourceTokenC::TT_COMMA && !pdepth &&
      // ... Unless we've reached the __VA_ARGS__ segment.
         (args.size() < parm.size() || !parm.back().empty()))
      {
         args.push_back(MacroArg());
         continue;
      }

      // If we're being called from a function-like macro expansion and we see
      // one of its arguments, expand it.
      if(tok->type == SourceTokenC::TT_NAM && altArgs && altParm &&
         (arg = find_arg(*altArgs, *altParm, tok->data)))
      {
         for(MacroArg::const_iterator tokEnd = arg->end(),
             tokItr = arg->begin(); tokItr != tokEnd; ++tokItr)
         {
            args.back().push_back(*tokItr);
         }
      }
      else
         args.back().push_back(tok);
   }

   // Must not have empty args.
   for(MacroArgs::iterator argsEnd = args.end(),
       argsItr = args.begin(); argsItr != argsEnd; ++argsItr)
   {
      if(argsItr->empty())
         argsItr->push_back(SourceTokenC::tt_none());
   }

   // And of course, must have the right number of arguments.
   if(args.size() != parm.size())
      ERROR_P("incorrect arg count for macro, expected %i got %i",
              (int)parm.size(), (int)args.size());
}

//
// SourceTokenizerC::rem_define_base
//
void SourceTokenizerC::rem_define_base(std::string const &name)
{
   defines_base.erase(name);
   macros_base .erase(name);
}

//
// SourceTokenizerC::remDefine
//
void SourceTokenizerC::remDefine(std::string const &name)
{
   defines.erase(name);
   macros .erase(name);
}

//
// SourceTokenizerC::remSkip
//
void SourceTokenizerC::remSkip()
{
   skipStack.pop_back();
   unskipStack.pop_back();
}

//
// SourceTokenizerC::unget
//
void SourceTokenizerC::unget(SourceTokenC *tok)
{
   ungetStack.push_back(static_cast<SourceTokenC::Reference>(tok));
}

// EOF

