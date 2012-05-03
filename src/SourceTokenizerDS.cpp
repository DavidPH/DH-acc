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

#include "SourceTokenizerDS.hpp"

#include "ObjectExpression.hpp"
#include "option.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"

#include <cstring>
#include <sstream>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static int add_define_base
(char const *opt, int optf, int argc, char const *const *argv);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_call option_define_handler
('D', "define", NULL, "Adds a define.", NULL, add_define_base);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceTokenizerDS::DefMap SourceTokenizerDS::defines_base;
SourceTokenizerDS::MacroMap SourceTokenizerDS::macros_base;
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
      {
         std::string data(eq+1); data += ' ';
         SourceTokenizerDS::add_macro_base(name, args, data);
      }
      else
         SourceTokenizerDS::add_macro_base(name, args);
   }
   else
   {
      std::string name(base, eq);

      if (*eq)
      {
         std::string data(eq+1); data += ' ';
         SourceTokenizerDS::add_define_base(name, data);
      }
      else
         SourceTokenizerDS::add_define_base(name);
   }

   return 1;
}

//
// find_tok
//
static SourceTokenizerDS::MacroVec *find_tok
(std::vector<SourceTokenizerDS::MacroVec> &tok,
 SourceTokenizerDS::MacroArg const &arg, std::string const &name)
{
   for (size_t i = 0, e = arg.size(); i != e; ++i)
      if (arg[i] == name)
         return &tok[i];

   return NULL;
}

//
// make_expression
//
static ObjectExpression::Pointer
make_expression(std::vector<ObjectExpression::Pointer> const &expressions,
                std::vector<SourceTokenC> const &operators, size_t begin,
                size_t end)
{
   #define EXPRL make_expression(expressions, operators, begin, iter)
   #define EXPRR make_expression(expressions, operators, iter+1, end)

   #define CARGS operators[iter].pos

   // Terminating case. Only one expression, so just return it.
   if (begin == end) return expressions[begin];

   size_t iter;

   // ?:

   // ||
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_PIPE2:
         return ObjectExpression::create_branch_ior(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ^^
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CARET2:
         return ObjectExpression::create_branch_xor(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // &&
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_AND2:
         return ObjectExpression::create_branch_and(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // |
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_PIPE:
         return ObjectExpression::create_binary_ior(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ^
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CARET:
         return ObjectExpression::create_binary_xor(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // &
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_AND:
         return ObjectExpression::create_binary_and(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // == !=

   // >= > <= <

   // >> <<

   // - +
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_MINUS:
         return ObjectExpression::create_binary_sub(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PLUS:
         return ObjectExpression::create_binary_add(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // * % /
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_ASTERISK:
         return ObjectExpression::create_binary_mul(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PERCENT:
         return ObjectExpression::create_binary_mod(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_SLASH:
         return ObjectExpression::create_binary_div(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   ERROR(operators[begin].pos, "unexpected operator");

   #undef CARGS

   #undef EXPRR
   #undef EXPRL
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenizerDS::SourceTokenizerDS
//
SourceTokenizerDS::SourceTokenizerDS(SourceStream *_in)
 : defines(defines_base), macros(macros_base),
   canCommand(true), canExpand(true), canSkip(true), canString(true)
{
   switch (target_type)
   {
   case TARGET_Eternity: addDefine("__TARGET_Eternity__"); break;
   case TARGET_Hexen:    addDefine("__TARGET_Hexen__");    break;
   case TARGET_HexPP:    addDefine("__TARGET_HexPP__");    break;
   case TARGET_ZDoom:    addDefine("__TARGET_ZDoom__");    break;
   case TARGET_UNKNOWN:  addDefine("__TARGET_UNKNOWN__");  break;
   }

   if (option_function_autoargs)
      addDefine("__FUNCTION_AUTOARGS__");

   if (option_script_autoargs)
      addDefine("__SCRIPT_AUTOARGS__");

   in.push(_in);
}

//
// SourceTokenizerDS::~SourceTokenizerDS
//
SourceTokenizerDS::~SourceTokenizerDS()
{
	// All but the first SourceStream was alloc'd by this.
	while (in.size() > 1)
	{
		delete in.top();
		in.pop();
	}
}

//
// SourceTokenizerDS::add_define_base
//
void SourceTokenizerDS::add_define_base(std::string const &name)
{
   static DefVec const tokens;
   add_define_base(name, tokens);
}

//
// SourceTokenizerDS::add_define_base
//
void SourceTokenizerDS::add_define_base
(std::string const &name, DefVec const &tokens)
{
   defines_base[name] = tokens;
}

//
// SourceTokenizerDS::add_define_base
//
void SourceTokenizerDS::add_define_base
(std::string const &name, std::string const &source)
{
   DefVec vec;

   // Keep reading until end-of-stream.
   try
   {
      SourceStream in(source, SourceStream::ST_C|SourceStream::STF_STRING);
      SourceTokenC tok;

      while (true)
      {
         tok.readToken(&in);
         vec.push_back(tok);
      }
   }
   catch (SourceStream::EndOfStream &) {}

   add_define_base(name, vec);
}

//
// SourceTokenizerDS::add_macro_base
//
void SourceTokenizerDS::add_macro_base
(std::string const &name, MacroArg const &arg)
{
   MacroDat dat; dat.first = arg;

   add_macro_base(name, dat);
}

//
// SourceTokenizerDS::add_macro_base
//
void SourceTokenizerDS::add_macro_base
(std::string const &name, MacroArg const &arg, std::string const &data)
{
   MacroDat dat; dat.first = arg;

   // Keep reading until end-of-stream.
   try
   {
      SourceStream in(data, SourceStream::ST_C|SourceStream::STF_STRING);
      SourceTokenC tok;

      while (true)
      {
         tok.readToken(&in);
         dat.second.push_back(tok);
      }
   }
   catch (SourceStream::EndOfStream &) {}

   add_macro_base(name, dat);
}

//
// SourceTokenizerDS::add_macro_base
//
void SourceTokenizerDS::add_macro_base
(std::string const &name, MacroDat const &dat)
{
   macros_base[name] = dat;
}

//
// SourceTokenizerDS::addDefine
//
void SourceTokenizerDS::addDefine(std::string const &name)
{
   static DefVec const tokens;
   addDefine(name, SourcePosition::builtin(), tokens);
}

//
// SourceTokenizerDS::addDefine
//
void SourceTokenizerDS::addDefine
(std::string const &name, SourcePosition const &pos,
 std::vector<SourceTokenC> const &tokens)
{
   if (hasDefine(name))
      ERROR_P("attempt to redefine define");

   defines[name] = tokens;
}

//
// SourceTokenizerDS::addMacro
//
void SourceTokenizerDS::addMacro
(std::string const &name, SourcePosition const &pos, MacroDat const &dat)
{
   if (hasMacro(name))
      ERROR_P("attempt to redfine macro");

   macros[name] = dat;
}

//
// SourceTokenizerDS::addSkip
//
void SourceTokenizerDS::addSkip(bool skip)
{
   skipStack.push_back(skip);
   unskipStack.push(!skip);
}

//
// SourceTokenizerDS::doAssert
//
void SourceTokenizerDS::doAssert(SourceTokenC::TokenType type)
{
   if (token.type != type)
      ERROR(token.pos, "expected %s got %s", make_string(type).c_str(),
            make_string(token.type).c_str());
}

//
// SourceTokenizerDS::doAssert
//
void SourceTokenizerDS::doAssert(std::string const &data)
{
   if (token.data != data)
      ERROR(token.pos, "expected %s got %s", data.c_str(), token.data.c_str());
}

//
// SourceTokenizerDS::doCommand
//
void SourceTokenizerDS::doCommand()
{
   canCommand = false;
   canExpand  = false;
   canSkip    = false;

   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   std::string const &command = token.data;

        if (command == "define")  doCommand_define();
   else if (command == "else")    doCommand_else();
   else if (command == "elif")    doCommand_elif();
   else if (command == "endif")   doCommand_endif();
   else if (command == "error")   doCommand_error();
   else if (command == "if")      doCommand_if();
   else if (command == "ifdef")   doCommand_ifdef();
   else if (command == "ifndef")  doCommand_ifndef();
   else if (command == "include") doCommand_include();
   else if (command == "macro")   doCommand_macro();
   else if (command == "undef")   doCommand_undef();

   else ERROR(token.pos, "unknown command: %s", command.c_str());

   canCommand = true;
   canExpand  = true;
   canSkip    = true;
}

//
// SourceTokenizerDS::doCommand_define
//
void SourceTokenizerDS::doCommand_define()
{
   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   std::string name = token.data;
   SourcePosition position = token.pos;

   std::vector<SourceTokenC> tokens;

   for (prep(); token.type != SourceTokenC::TT_OP_HASH; prep())
      tokens.push_back(token);

   if (isSkip()) return;

   addDefine(name, position, tokens);
}

//
// SourceTokenizerDS::doCommand_else
//
void SourceTokenizerDS::doCommand_else()
{
   if (skipStack.empty())
      ERROR(token.pos, "unmatched #else");

   skipStack.back() = unskipStack.top();
   unskipStack.top() = true; // If it wasn't, it is now.
}

//
// SourceTokenizerDS::doCommand_elif
//
void SourceTokenizerDS::doCommand_elif()
{
   if (skipStack.empty())
      ERROR(token.pos, "unmatched #elif");

   bool ifResult = getIf();

   skipStack.back()  = unskipStack.top() || !ifResult;
   unskipStack.top() = unskipStack.top() ||  ifResult;
}

//
// SourceTokenizerDS::doCommand_endif
//
void SourceTokenizerDS::doCommand_endif()
{
   if (skipStack.empty())
      ERROR(token.pos, "unmatched #endif");

   remSkip();
}

//
// SourceTokenizerDS::doCommand_error
//
void SourceTokenizerDS::doCommand_error()
{
   prep(); doAssert(SourceTokenC::TT_STRING);

   if (isSkip()) return;

   ERROR(token.pos, "#error %s", token.data.c_str());
}

//
// SourceTokenizerDS::doCommand_if
//
void SourceTokenizerDS::doCommand_if()
{
   addSkip(!getIf());
}

//
// SourceTokenizerDS::doCommand_ifdef
//
void SourceTokenizerDS::doCommand_ifdef()
{
   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   addSkip(!hasDefine(token.data) && !hasMacro(token.data));
}

//
// SourceTokenizerDS::doCommand_ifndef
//
void SourceTokenizerDS::doCommand_ifndef()
{
   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   addSkip(hasDefine(token.data) || hasMacro(token.data));
}

//
// SourceTokenizerDS::doCommand_include
//
void SourceTokenizerDS::doCommand_include()
{
   canString = false;
   prep(); doAssert(SourceTokenC::TT_STRING);
   canString = true;

   if (isSkip()) return;

   try
   {
      in.push(new SourceStream(token.data, SourceStream::ST_C));
   }
   catch (std::exception & e)
   {
      ERROR(token.pos, "file not found: %s", token.data.c_str());
   }
}

//
// SourceTokenizerDS::doCommand_macro
//
void SourceTokenizerDS::doCommand_macro()
{
   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   std::string    name = token.data;
   SourcePosition pos  = token.pos;
   MacroDat       dat;

   prep(); doAssert(SourceTokenC::TT_OP_PARENTHESIS_O); prep();

   if (token.type != SourceTokenC::TT_OP_PARENTHESIS_C) for (;;)
   {
      doAssert(SourceTokenC::TT_IDENTIFIER);
      dat.first.push_back(token.data);

      prep();
      if (token.type == SourceTokenC::TT_OP_COMMA) {prep(); continue;}
      doAssert(SourceTokenC::TT_OP_PARENTHESIS_C);
      break;
   }

   for (prep(); token.type != SourceTokenC::TT_OP_HASH3; prep())
      dat.second.push_back(token);

   if (isSkip()) return;

   addMacro(name, pos, dat);
}

//
// SourceTokenizerDS::doCommand_undef
//
void SourceTokenizerDS::doCommand_undef()
{
   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   if (isSkip()) return;

   remDefine();
}

//
// SourceTokenizerDS::get
//
SourceTokenC const &SourceTokenizerDS::get()
{
   prep();

   return token;
}

//
// SourceTokenizerDS::get
//
SourceTokenC const &SourceTokenizerDS::get(SourceTokenC::TokenType type)
{
   prep(); doAssert(type);

   return token;
}

//
// SourceTokenizerDS::get
//
SourceTokenC const &SourceTokenizerDS::get
(SourceTokenC::TokenType type, std::string const &data)
{
   prep(); doAssert(type); doAssert(data);

   return token;
}

//
// SourceTokenizerDS::getIf
//
bool SourceTokenizerDS::getIf()
{
   canExpand = true;

   ObjectExpression::Pointer expr = getIfMultiple();

   canExpand = false;

   return !!expr->resolveInt();
}

//
// SourceTokenizerDS::getIfMultiple
//
ObjectExpression::Pointer SourceTokenizerDS::getIfMultiple()
{
   std::vector<ObjectExpression::Pointer> expressions;
   std::vector<SourceTokenC> operators;

   expressions.push_back(getIfSingle());

   for (prep(); token.type != SourceTokenC::TT_OP_HASH; prep())
   {
      switch (token.type)
      {
      case SourceTokenC::TT_OP_AND:
      case SourceTokenC::TT_OP_AND2:
      case SourceTokenC::TT_OP_ASTERISK:
      case SourceTokenC::TT_OP_CARET:
      case SourceTokenC::TT_OP_CARET2:
      case SourceTokenC::TT_OP_CMP_EQ:
      case SourceTokenC::TT_OP_CMP_GE:
      case SourceTokenC::TT_OP_CMP_GT:
      case SourceTokenC::TT_OP_CMP_GT2:
      case SourceTokenC::TT_OP_CMP_LE:
      case SourceTokenC::TT_OP_CMP_LT:
      case SourceTokenC::TT_OP_CMP_LT2:
      case SourceTokenC::TT_OP_CMP_NE:
      case SourceTokenC::TT_OP_MINUS:
      case SourceTokenC::TT_OP_PERCENT:
      case SourceTokenC::TT_OP_PIPE:
      case SourceTokenC::TT_OP_PIPE2:
      case SourceTokenC::TT_OP_PLUS:
      case SourceTokenC::TT_OP_SLASH:
      case_expr:
         operators.push_back(token);
         expressions.push_back(getIfSingle());
         break;

      case SourceTokenC::TT_OP_COLON:
      case SourceTokenC::TT_OP_PARENTHESIS_C:
         goto done;

      case SourceTokenC::TT_OP_QUERY:
         operators.push_back(token);
         expressions.push_back(getIfMultiple());
         prep(); doAssert(SourceTokenC::TT_OP_COLON);
         goto case_expr;

      default:
         ERROR(token.pos, "unexpected token type");
      }
   }

done:
   return make_expression(expressions, operators, 0, operators.size());
}

//
// SourceTokenizerDS::getIfSingle
//
ObjectExpression::Pointer SourceTokenizerDS::getIfSingle()
{
   prep();

   switch (token.type)
   {
   case SourceTokenC::TT_IDENTIFIER:
      if (token.data == "defined")
      {
         canExpand = false;

         prep();

         bool hasParentheses = token.type == SourceTokenC::TT_OP_PARENTHESIS_O;

         if (hasParentheses) prep();

         doAssert(SourceTokenC::TT_IDENTIFIER);

         bool isdef = hasDefine(token.data) || hasMacro(token.data);

         ObjectExpression::Pointer expr =
            ObjectExpression::create_value_int(isdef, token.pos);

         if (hasParentheses)
            {prep(); doAssert(SourceTokenC::TT_OP_PARENTHESIS_C);}

         canExpand = true;

         return expr;
      }
      else
      {
         // If this function sees any other identifier, that means it's not a
         // define. C says that undefined identifiers evaluate to 0.
         return ObjectExpression::create_value_int(0, token.pos);
      }

   case SourceTokenC::TT_INTEGER:
      return ObjectExpression::create_value_int
      (get_bigsint(token.data, token.pos), token.pos);

   case SourceTokenC::TT_OP_EXCLAMATION:
      return ObjectExpression::create_branch_not(getIfSingle(), token.pos);

   case SourceTokenC::TT_OP_PARENTHESIS_O:
   {
      ObjectExpression::Pointer expr = getIfMultiple();

      doAssert(SourceTokenC::TT_OP_PARENTHESIS_C);

      return expr;
   }

   default:
      ERROR(token.pos, "unexpected token type");
   }
}

//
// SourceTokenizerDS::hasDefine
//
bool SourceTokenizerDS::hasDefine(std::string const &name)
{
   return defines.find(name) != defines.end();
}

//
// SourceTokenizerDS::hasMacro
//
bool SourceTokenizerDS::hasMacro(std::string const &name)
{
   return macros.find(name) != macros.end();
}

//
// SourceTokenizerDS::isSkip
//
bool SourceTokenizerDS::isSkip()
{
   std::vector<bool>::iterator it;

   for (it = skipStack.begin(); it != skipStack.end(); ++it)
      if (*it) return true;

   return false;
}

//
// SourceTokenizerDS::peek
//
SourceTokenC const &SourceTokenizerDS::peek()
{
   prep();

   ungetStack.push(token);

   return token;
}

//
// SourceTokenizerDS::peek
//
SourceTokenC const &SourceTokenizerDS::peek(SourceTokenC::TokenType type)
{
   prep(); doAssert(type);

   ungetStack.push(token);

   return token;
}

//
// SourceTokenizerDS::peekType
//
bool SourceTokenizerDS::peekType(SourceTokenC::TokenType type)
{
   prep();

   ungetStack.push(token);

   return token.type == type;
}

//
// SourceTokenizerDS::peekType
//
bool SourceTokenizerDS::peekType
(SourceTokenC::TokenType type, std::string const &data)
{
   prep();

   ungetStack.push(token);

   return token.type == type && token.data == data;
}

//
// SourceTokenizerDS::prep
//
void SourceTokenizerDS::prep()
{
   while (true)
   {
      if (!ungetStack.empty())
      {
         token = ungetStack.top();
         ungetStack.pop();

         // Clear whenever the unget stack is emptied.
         if (ungetStack.empty()) definesUsed.clear();
      }
      else try
      {
         token.readToken(in.top());
      }
      catch (SourceStream::EndOfStream &e)
      {
         if (in.size() == 1) throw;

         delete in.top();
         in.pop();

         continue;
      }

      // Preprocessor directive.
      if (canCommand && token.type == SourceTokenC::TT_OP_HASH)
      {
         doCommand();
         continue;
      }

      // Macro expansion.
      if (canExpand && token.type == SourceTokenC::TT_IDENTIFIER)
      {
         // Check for macro.
         if (hasMacro(token.data) && definesUsed.insert(token.data).second)
         {
            SourceTokenC oldTok = token;
            prep(); unget(token);
            std::swap(token, oldTok);

            // Macro invocation!
            if (oldTok.type == SourceTokenC::TT_OP_PARENTHESIS_O)
            {
               prepMacro();
               continue;
            }
         }

         if (hasDefine(token.data) && definesUsed.insert(token.data).second)
         {
            prepDefine();
            continue;
         }

         if (token.data == "__FILE__" && definesUsed.insert(token.data).second)
         {
            token.type = SourceTokenC::TT_STRING;
            token.data = token.pos.filename;
            break;
         }

         if (token.data == "__LINE__" && definesUsed.insert(token.data).second)
         {
            std::ostringstream oss; oss << token.pos.line;

            token.type = SourceTokenC::TT_INTEGER;
            token.data = oss.str();

            break;
         }
      }

      if (canSkip && isSkip())
         continue;

      break;
   }

   // String literal concatenation.
   if (canString && token.type == SourceTokenC::TT_STRING)
   {
      SourceTokenC tokenTemp = token;

      prep();

      if (token.type == SourceTokenC::TT_STRING)
         tokenTemp.data += token.data;
      else
         unget(token);

      token = tokenTemp;
   }
}

//
// SourceTokenizerDS::prepDefine
//
void SourceTokenizerDS::prepDefine()
{
   std::vector<SourceTokenC> const &tokens(defines[token.data]);

   for (size_t i(tokens.size()); i--;)
   {
      // Copy the invoker's position, to enable better error reporting.
      // (Some day this will be additive, to keep both positions.)
      SourceTokenC tok = tokens[i];
      tok.pos = token.pos;
      ungetStack.push(tok);
   }
}

//
// SourceTokenizerDS::prepMacro
//
void SourceTokenizerDS::prepMacro()
{
   SourcePosition const pos = token.pos;

   MacroDat const &dat = macros[token.data];

   MacroArg const &arg = dat.first;
   MacroVec vec = dat.second, tmp, *tokVec;

   MacroVec::iterator itr, end;

   std::vector<MacroVec> tok;
   int pdepth = 0;

   canCommand = false;
   canExpand  = false;

   // Collect the macro arguments.
   prep();
   doAssert(SourceTokenC::TT_OP_PARENTHESIS_O);
   tok.push_back(std::vector<SourceTokenC>());
   while (true)
   {
      prep();

      if (token.type == SourceTokenC::TT_OP_PARENTHESIS_O) ++pdepth;
      if (token.type == SourceTokenC::TT_OP_PARENTHESIS_C && !pdepth--) break;

      if (token.type == SourceTokenC::TT_OP_COMMA && !pdepth)
      {
         tok.push_back(std::vector<SourceTokenC>());
         continue;
      }

      tok.back().push_back(token);
   }

   if (tok.size() != arg.size())
      ERROR_P("incorrect arg count for macro");

   // Copy the invoker's position, to enable better error reporting.
   // (Some day this will be additive, to keep both positions.)
   for ((itr = vec.begin()), (end = vec.end()); itr != end; ++itr)
      itr->pos = pos;

   // Process # tokens.
   for ((itr = vec.begin()), (end = vec.end()); itr != end; ++itr)
   {
      if (itr->type == SourceTokenC::TT_OP_HASH)
      {
         if (++itr == end || !(tokVec = find_tok(tok, arg, itr->data)))
            ERROR_P("# must be used on arg");

         tmp.push_back(SourceTokenC());

         tmp.back().type = SourceTokenC::TT_STRING;

         MacroVec::iterator tokItr, tokEnd = tokVec->end();
         for (tokItr = tokVec->begin(); tokItr != tokEnd; ++tokItr)
            tmp.back().data += tokItr->getDataString();
      }
      else
         tmp.push_back(*itr);
   }
   vec = tmp; tmp.clear();

   // Perform argument substitution.
   for ((itr = vec.begin()), (end = vec.end()); itr != end; ++itr)
   {
      if (itr->type == SourceTokenC::TT_IDENTIFIER &&
         (tokVec = find_tok(tok, arg, itr->data)))
      {
         MacroVec::iterator tokItr, tokEnd = tokVec->end();
         for (tokItr = tokVec->begin(); tokItr != tokEnd; ++tokItr)
            tmp.push_back(*tokItr);
      }
      else
         tmp.push_back(*itr);
   }
   vec = tmp; tmp.clear();

   // Process ## tokens.
   for ((itr = vec.begin()), (end = vec.end()); itr != end; ++itr)
   {
      if (itr->type == SourceTokenC::TT_OP_HASH2)
      {
         if (itr == vec.begin() || ++itr == end)
            ERROR_P("## cannot be at start or end");

         if (itr->type != SourceTokenC::TT_IDENTIFIER)
            ERROR_P("## must have identifier on right");

         // tmp.back() is the last token already.
         tmp.back().data += itr->data;
      }
      else
         tmp.push_back(*itr);
   }
   vec = tmp; tmp.clear();

   for ((itr = vec.end()), (end = vec.begin()); itr-- != end;)
      ungetStack.push(*itr);

   canCommand = true;
   canExpand  = true;
}

//
// SourceTokenizerDS::remDefine
//
void SourceTokenizerDS::remDefine()
{
   defines.erase(token.data);
   macros .erase(token.data);
}

//
// SourceTokenizerDS::remSkip
//
void SourceTokenizerDS::remSkip()
{
   skipStack.pop_back();
   unskipStack.pop();
}

//
// SourceTokenizerDS::unget
//
void SourceTokenizerDS::unget(SourceTokenC const &_token)
{
   ungetStack.push(_token);
}

// EOF

