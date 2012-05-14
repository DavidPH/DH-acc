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

#include "SourceTokenizerC.hpp"

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
      {
         std::string data(eq+1); data += ' ';
         SourceTokenizerC::add_macro_base(name, args, data);
      }
      else
         SourceTokenizerC::add_macro_base(name, args);
   }
   else
   {
      std::string name(base, eq);

      if (*eq)
      {
         std::string data(eq+1); data += ' ';
         SourceTokenizerC::add_define_base(name, data);
      }
      else
         SourceTokenizerC::add_define_base(name);
   }

   return 1;
}

//
// find_tok
//
static SourceTokenizerC::MacroVec *find_arg
(std::vector<SourceTokenizerC::MacroVec> &args,
 SourceTokenizerC::MacroArg const &argNames, std::string const &name)
{
   for (size_t i = 0, e = argNames.size(); i != e; ++i)
      if (argNames[i] == name)
         return &args[i];

   return NULL;
}

//
// make_expression
//
static ObjectExpression::Pointer make_expression(
   std::vector<ObjectExpression::Pointer> const &expressions,
   std::vector<SourceTokenC::Reference> const &operators,
   size_t begin, size_t end)
{
   #define EXPRL make_expression(expressions, operators, begin, iter)
   #define EXPRR make_expression(expressions, operators, iter+1, end)

   #define CARGS operators[iter]->pos

   // Terminating case. Only one expression, so just return it.
   if (begin == end) return expressions[begin];

   size_t iter;

   // ?:

   // ||
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_IO2:
         return ObjectExpression::create_branch_ior(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ^^
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_XO2:
         return ObjectExpression::create_branch_xor(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // &&
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_AN2:
         return ObjectExpression::create_branch_and(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // |
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_IOR:
         return ObjectExpression::create_binary_ior(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ^
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_XOR:
         return ObjectExpression::create_binary_xor(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // &
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_AND:
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
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_SUB:
         return ObjectExpression::create_binary_sub(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_ADD:
         return ObjectExpression::create_binary_add(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // * % /
   for (iter = end; iter-- > begin;)
   {
      switch (operators[iter]->type)
      {
      case SourceTokenC::TT_MUL:
         return ObjectExpression::create_binary_mul(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_MOD:
         return ObjectExpression::create_binary_mod(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_DIV:
         return ObjectExpression::create_binary_div(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   ERROR(operators[begin]->pos, "unexpected operator");

   #undef CARGS

   #undef EXPRR
   #undef EXPRL
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenizerC::SourceTokenizerC
//
SourceTokenizerC::SourceTokenizerC(SourceStream *_in)
 : defines(defines_base), macros(macros_base),
   canCommand(true), canExpand(true), canSkip(true)
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

   in.push_back(_in);
   ungetStack.push_back(static_cast<SourceTokenC::Reference>(
      new SourceTokenC(SourcePosition::builtin(), SourceTokenC::TT_ENDL)));
}

//
// SourceTokenizerC::~SourceTokenizerC
//
SourceTokenizerC::~SourceTokenizerC()
{
   // All but the first SourceStream was alloc'd by this.
   while (in.size() > 1)
   {
      delete in.back(); in.pop_back();
   }
}

//
// SourceTokenizerC::add_define_base
//
void SourceTokenizerC::add_define_base(std::string const &name)
{
   static DefVec const tokens;
   add_define_base(name, tokens);
}

//
// SourceTokenizerC::add_define_base
//
void SourceTokenizerC::add_define_base
(std::string const &name, DefVec const &tokens)
{
   defines_base[name] = tokens;
}

//
// SourceTokenizerC::add_define_base
//
void SourceTokenizerC::add_define_base
(std::string const &name, std::string const &source)
{
   DefVec vec;

   // Keep reading until end-of-stream.
   try
   {
      SourceStream in(source, SourceStream::ST_C|SourceStream::STF_STRING);

      while (true)
      {
         SourceTokenC::Reference tok(new SourceTokenC);
         tok->readToken(&in);
         vec.push_back(tok);
      }
   }
   catch (SourceStream::EndOfStream &) {}

   add_define_base(name, vec);
}

//
// SourceTokenizerC::add_macro_base
//
void SourceTokenizerC::add_macro_base
(std::string const &name, MacroArg const &arg)
{
   MacroDat dat; dat.first = arg;

   add_macro_base(name, dat);
}

//
// SourceTokenizerC::add_macro_base
//
void SourceTokenizerC::add_macro_base
(std::string const &name, MacroArg const &arg, std::string const &data)
{
   MacroDat dat; dat.first = arg;

   // Keep reading until end-of-stream.
   try
   {
      SourceStream in(data, SourceStream::ST_C|SourceStream::STF_STRING);

      while (true)
      {
         SourceTokenC::Reference tok(new SourceTokenC);
         tok->readToken(&in);
         dat.second.push_back(tok);
      }
   }
   catch (SourceStream::EndOfStream &) {}

   add_macro_base(name, dat);
}

//
// SourceTokenizerC::add_macro_base
//
void SourceTokenizerC::add_macro_base
(std::string const &name, MacroDat const &dat)
{
   macros_base[name] = dat;
}

//
// SourceTokenizerC::addDefine
//
void SourceTokenizerC::addDefine(std::string const &name)
{
   static DefVec const tokens;
   addDefine(name, SourcePosition::builtin(), tokens);
}

//
// SourceTokenizerC::addDefine
//
void SourceTokenizerC::addDefine(std::string const &name,
   SourcePosition const &pos, DefVec const &vec)
{
   if (hasDefine(name))
      ERROR_P("attempt to redefine define %s", name.c_str());

   defines[name] = vec;
}

//
// SourceTokenizerC::addMacro
//
void SourceTokenizerC::addMacro
(std::string const &name, SourcePosition const &pos, MacroDat const &dat)
{
   if (hasMacro(name))
      ERROR_P("attempt to redfine macro");

   macros[name] = dat;
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
   canCommand = false;
   canExpand  = false;
   canSkip    = false;

   SourceTokenC::Reference tok = get(); doAssert(tok, SourceTokenC::TT_NAM);

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

   canCommand = true;
   canExpand  = true;
   canSkip    = true;
}

//
// SourceTokenizerC::doCommand_define
//
void SourceTokenizerC::doCommand_define(SourceTokenC *)
{
   SourceTokenC::Reference name = get(); doAssert(name, SourceTokenC::TT_NAM);

   if (in.back()->peek() == '(')
   {
      MacroDat dat;

      doAssert(get(), SourceTokenC::TT_PAREN_O);
      SourceTokenC::Reference arg = get();
      if (arg->type != SourceTokenC::TT_PAREN_C) for (;;)
      {
         doAssert(arg, SourceTokenC::TT_NAM);
         dat.first.push_back(arg->data);

         if ((arg = get())->type == SourceTokenC::TT_PAREN_C) break;
         doAssert(arg, SourceTokenC::TT_COMMA);
         arg = get();
      }

      SourceTokenC::Reference itr = get();
      for (; itr->type != SourceTokenC::TT_ENDL; itr = get())
         dat.second.push_back(itr);
      unget(itr);

      if (isSkip()) return;

      addMacro(name->data, name->pos, dat);
   }
   else
   {
      DefVec vec;

      SourceTokenC::Reference itr = get();
      for (; itr->type != SourceTokenC::TT_ENDL; itr = get())
         vec.push_back(itr);
      unget(itr);

      if (isSkip()) return;

      addDefine(name->data, name->pos, vec);
   }
}

//
// SourceTokenizerC::doCommand_else
//
void SourceTokenizerC::doCommand_else(SourceTokenC *tok)
{
   doAssert(peek(), SourceTokenC::TT_ENDL);

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

   bool ifResult = getIf();
   doAssert(peek(), SourceTokenC::TT_ENDL);

     skipStack.back() = unskipStack.back() || !ifResult;
   unskipStack.back() = unskipStack.back() ||  ifResult;
}

//
// SourceTokenizerC::doCommand_endif
//
void SourceTokenizerC::doCommand_endif(SourceTokenC *tok)
{
   doAssert(peek(), SourceTokenC::TT_ENDL);

   if (skipStack.empty())
      ERROR(tok->pos, "unmatched #endif");

   remSkip();
}

//
// SourceTokenizerC::doCommand_error
//
void SourceTokenizerC::doCommand_error(SourceTokenC *)
{
   SourceTokenC::Reference msg = get(); doAssert(msg, SourceTokenC::TT_STR);
   doAssert(peek(), SourceTokenC::TT_ENDL);

   if (isSkip()) return;

   ERROR(msg->pos, "#error %s", msg->data.c_str());
}

//
// SourceTokenizerC::doCommand_if
//
void SourceTokenizerC::doCommand_if(SourceTokenC *)
{
   addSkip(!getIf());
   doAssert(peek(), SourceTokenC::TT_ENDL);
}

//
// SourceTokenizerC::doCommand_ifdef
//
void SourceTokenizerC::doCommand_ifdef(SourceTokenC *)
{
   SourceTokenC::Reference name = get(); doAssert(name, SourceTokenC::TT_NAM);
   doAssert(peek(), SourceTokenC::TT_ENDL);

   addSkip(!hasDefine(name->data) && !hasMacro(name->data));
}

//
// SourceTokenizerC::doCommand_ifndef
//
void SourceTokenizerC::doCommand_ifndef(SourceTokenC *)
{
   SourceTokenC::Reference name = get(); doAssert(name, SourceTokenC::TT_NAM);
   doAssert(peek(), SourceTokenC::TT_ENDL);

   addSkip(hasDefine(name->data) || hasMacro(name->data));
}

//
// SourceTokenizerC::doCommand_include
//
void SourceTokenizerC::doCommand_include(SourceTokenC *)
{
   SourceTokenC::Reference inc = get(); doAssert(inc, SourceTokenC::TT_STR);
   doAssert(peek(), SourceTokenC::TT_ENDL);

   if (isSkip()) return;

   try
   {
      in.push_back(new SourceStream(inc->data, SourceStream::ST_C));
      ungetStack.push_back(static_cast<SourceTokenC::Reference>(
         new SourceTokenC(SourcePosition::builtin(), SourceTokenC::TT_ENDL)));
   }
   catch (std::exception const &)
   {
      ERROR(inc->pos, "file not found: %s", inc->data.c_str());
   }
}

//
// SourceTokenizerC::doCommand_undef
//
void SourceTokenizerC::doCommand_undef(SourceTokenC *)
{
   SourceTokenC::Reference name = get(); doAssert(name, SourceTokenC::TT_NAM);
   doAssert(peek(), SourceTokenC::TT_ENDL);

   if (isSkip()) return;

   remDefine(name->data);
}

//
// SourceTokenizerC::get
//
SourceTokenC::Reference SourceTokenizerC::get()
{
   SourceTokenC::Pointer tok;

   for (;;)
   {
      tok = getRaw();

      // Preprocessor directive.
      if (canCommand && tok->type == SourceTokenC::TT_ENDL)
      {
         while (tok->type == SourceTokenC::TT_ENDL)
            tok = getRaw();

         if (tok->type == SourceTokenC::TT_HASH1)
         {
            doCommand(); continue;
         }
      }

      // Macro expansion.
      if (canExpand && tok->type == SourceTokenC::TT_NAM)
      {
         // Check for macro.
         if (hasMacro(tok->data) && definesUsed.insert(tok->data).second)
         {
            SourceTokenC::Reference oldTok(tok);
            tok = get(); unget(tok);

            // Macro invocation!
            if (tok->type == SourceTokenC::TT_PAREN_O)
            {
               prepMacro(static_cast<SourceTokenC::Reference>(oldTok)); continue;
            }

            tok = oldTok;
         }

         if (hasDefine(tok->data) && definesUsed.insert(tok->data).second)
         {
            prepDefine(tok); continue;
         }

         if (tok->data == "__FILE__")
         {
            tok->data = tok->pos.filename;
            tok->type = SourceTokenC::TT_STR;
            break;
         }

         if (tok->data == "__LINE__")
         {
            std::ostringstream oss; oss << tok->pos.line;
            tok->data = oss.str();
            tok->type = SourceTokenC::TT_INT;
            break;
         }
      }

      if (canSkip && isSkip())
         continue;

      if (tok->type == SourceTokenC::TT_NONE)
         continue;

      break;
   }

   // String literal concatenation.
   if (tok->type == SourceTokenC::TT_STR)
   {
      SourceTokenC::Reference oldTok(tok);
      tok = get();

      if (tok->type == SourceTokenC::TT_STR)
         oldTok->data += tok->data;
      else
         unget(tok);

      tok = oldTok;
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
// SourceTokenizerC::getIf
//
bool SourceTokenizerC::getIf()
{
   canExpand = true;

   ObjectExpression::Pointer expr = getIfMultiple();

   canExpand = false;

   return !!expr->resolveInt();
}

//
// SourceTokenizerC::getIfMultiple
//
ObjectExpression::Pointer SourceTokenizerC::getIfMultiple()
{
   std::vector<ObjectExpression::Pointer> expressions;
   std::vector<SourceTokenC::Reference> operators;

   expressions.push_back(getIfSingle());

   SourceTokenC::Reference tok = get();
   for (; tok->type != SourceTokenC::TT_ENDL; tok = get())
   {
      switch (tok->type)
      {
      case SourceTokenC::TT_ADD:
      case SourceTokenC::TT_AN2:
      case SourceTokenC::TT_AND:
      case SourceTokenC::TT_CMP_EQ:
      case SourceTokenC::TT_CMP_GE:
      case SourceTokenC::TT_CMP_GT:
      case SourceTokenC::TT_CMP_LE:
      case SourceTokenC::TT_CMP_LT:
      case SourceTokenC::TT_CMP_NE:
      case SourceTokenC::TT_DIV:
      case SourceTokenC::TT_IO2:
      case SourceTokenC::TT_IOR:
      case SourceTokenC::TT_MOD:
      case SourceTokenC::TT_MUL:
      case SourceTokenC::TT_LSH:
      case SourceTokenC::TT_RSH:
      case SourceTokenC::TT_SUB:
      case SourceTokenC::TT_XOR:
      case SourceTokenC::TT_XO2:
      case_expr:
         operators.push_back(tok);
         expressions.push_back(getIfSingle());
         break;

      case SourceTokenC::TT_COLON:
      case SourceTokenC::TT_PAREN_C:
         ungetStack.push_back(tok);
         goto done;

      case SourceTokenC::TT_QUERY:
         operators.push_back(tok);
         expressions.push_back(getIfMultiple());
         tok = get(); doAssert(tok, SourceTokenC::TT_COLON);
         goto case_expr;

      default:
         ERROR(tok->pos, "unexpected token type");
      }
   }
   unget(tok);

done:
   return make_expression(expressions, operators, 0, operators.size());
}

//
// SourceTokenizerC::getIfSingle
//
ObjectExpression::Pointer SourceTokenizerC::getIfSingle()
{
   SourceTokenC::Reference tok = get();

   switch (tok->type)
   {
   case SourceTokenC::TT_NAM:
      if (tok->data == "defined")
      {
         canExpand = false;

         bool hasParen = (tok = get())->type == SourceTokenC::TT_PAREN_O;

         doAssert((hasParen ? (tok = get()) : tok), SourceTokenC::TT_NAM);

         bool isdef = hasDefine(tok->data) || hasMacro(tok->data);

         ObjectExpression::Pointer expr =
            ObjectExpression::create_value_int(isdef, tok->pos);

         if (hasParen) doAssert(get(), SourceTokenC::TT_PAREN_C);

         canExpand = true;

         return expr;
      }
      else
      {
         // If this function sees any other identifier, that means it's not a
         // define. C says that undefined identifiers evaluate to 0.
         return ObjectExpression::create_value_int(0, tok->pos);
      }

   case SourceTokenC::TT_INT:
      return ObjectExpression::create_value_int(
         get_bigsint(tok->data, tok->pos), tok->pos);

   case SourceTokenC::TT_NOTLOG:
      return ObjectExpression::create_branch_not(getIfSingle(), tok->pos);

   case SourceTokenC::TT_PAREN_O:
   {
      ObjectExpression::Pointer expr = getIfMultiple();

      doAssert(get(), SourceTokenC::TT_PAREN_C);

      return expr;
   }

   default:
      ERROR(tok->pos, "unexpected token type");
   }
}

//
// SourceTokenizerC::getRaw
//
SourceTokenC::Reference SourceTokenizerC::getRaw()
{
   if (!ungetStack.empty())
   {
      SourceTokenC::Reference tok = ungetStack.back();
      ungetStack.pop_back();

      // Clear whenever the unget stack is emptied.
      if (ungetStack.empty()) definesUsed.clear();

      return tok;
   }
   else try
   {
      SourceTokenC::Reference tok(new SourceTokenC);
      tok->readToken(in.back());
      return tok;
   }
   catch (SourceStream::EndOfStream &e)
   {
      if (in.size() == 1) throw;
      delete in.back(); in.pop_back();
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
// SourceTokenizerC::prepDefine
//
void SourceTokenizerC::prepDefine(SourceTokenC *tok)
{
   DefVec const &tokens = defines[tok->data];
   DefVec::const_iterator itr, end = tokens.begin();

   for (itr = tokens.end(); itr-- != end;)
   {
      ungetStack.push_back(static_cast<SourceTokenC::Reference>(
         new SourceTokenC(tok->pos, (*itr)->data, (*itr)->type)));
   }
}

//
// SourceTokenizerC::prepMacro
//
void SourceTokenizerC::prepMacro(SourceTokenC::Reference tok)
{
   SourcePosition const &pos = tok->pos;

   MacroDat const &dat = macros[tok->data];

   MacroArg const &argNames = dat.first;
   MacroVec argData = dat.second, tmpData, *arg;
   tmpData.reserve(argData.size());

   MacroVec::iterator itr, end;

   std::vector<MacroVec> args;
   int pdepth = 0;

   canCommand = false;
   canExpand  = false;

   // Collect the macro arguments.
   doAssert((tok = get()), SourceTokenC::TT_PAREN_O);
   args.push_back(MacroVec());
   while (true)
   {
      tok = get();

      if (tok->type == SourceTokenC::TT_PAREN_O) ++pdepth;
      if (tok->type == SourceTokenC::TT_PAREN_C && !pdepth--) break;

      if (tok->type == SourceTokenC::TT_COMMA && !pdepth)
      {
         args.push_back(MacroVec()); continue;
      }

      args.back().push_back(tok);
   }

   if (args.size() != argNames.size())
      ERROR_P("incorrect arg count for macro, expected %i got %i",
              (int)argNames.size(), (int)args.size());

   // Copy the invoker's position, to enable better error reporting.
   // (Some day this will be additive, to keep both positions.)
   for (itr = argData.begin(), end = argData.end(); itr != end; ++itr)
      (*itr)->pos = pos;

   // Process # tokens.
   for (itr = argData.begin(), end = argData.end(); itr != end; ++itr)
   {
      if ((*itr)->type == SourceTokenC::TT_HASH1)
      {
         if (++itr == end || !(arg = find_arg(args, argNames, (*itr)->data)))
            ERROR_P("# must be used on arg");

         tok = static_cast<SourceTokenC::Reference>(
            new SourceTokenC((*itr)->pos, SourceTokenC::TT_STR));

         MacroVec::iterator tokItr, tokEnd = arg->end();
         for (tokItr = arg->begin(); tokItr != tokEnd; ++tokItr)
            tok->data += (*tokItr)->getDataString();

         tmpData.push_back(tok);
      }
      else
         tmpData.push_back(*itr);
   }
   argData.clear(); argData.reserve(tmpData.size());

   // Perform argument substitution.
   for (itr = tmpData.begin(), end = tmpData.end(); itr != end; ++itr)
   {
      if ((*itr)->type == SourceTokenC::TT_NAM &&
         (arg = find_arg(args, argNames, (*itr)->data)))
      {
         MacroVec::iterator tokItr, tokEnd = arg->end();
         for (tokItr = arg->begin(); tokItr != tokEnd; ++tokItr)
            argData.push_back(*tokItr);
      }
      else
         argData.push_back(*itr);
   }
   tmpData.clear(); tmpData.reserve(argData.size());

   // Process ## tokens.
   for (itr = argData.begin(), end = argData.end(); itr != end; ++itr)
   {
      if ((*itr)->type == SourceTokenC::TT_HASH2)
      {
         if (itr == argData.begin() || ++itr == end)
            ERROR_P("## cannot be at start or end");

         if ((*itr)->type != SourceTokenC::TT_NAM)
            ERROR_P("## must have identifier on right");

         // tmp.back() is the last token already.
         tmpData.back()->data += (*itr)->data;
      }
      else
         tmpData.push_back(*itr);
   }

   for (itr = tmpData.end(), end = tmpData.begin(); itr-- != end;)
      ungetStack.push_back(*itr);

   canCommand = true;
   canExpand  = true;
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

