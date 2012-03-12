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

   char const *eq = std::strchr(argv[0], '=');

   if (eq)
   {
      std::string name(argv[0], eq-argv[0]);
      SourceTokenizerDS::add_define_base(name, eq);
   }
   else
   {
      SourceTokenizerDS::add_define_base(argv[0]);
   }

   return 1;
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
 : defines(defines_base),
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
   DefVec tokens;
   SourceStream in(source, SourceStream::ST_C);

   try
   {
      tokens.resize(tokens.size()+1);
      tokens.back().readToken(&in);
   }
   catch (SourceStream::EndOfStream &)
   {
      // Keep reading until end-of-stream.
      // (I sure hope there aren't any incomplete tokens.)
   }

   add_define_base(name, tokens);
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

   addSkip(!hasDefine(token.data));
}

//
// SourceTokenizerDS::doCommand_ifndef
//
void SourceTokenizerDS::doCommand_ifndef()
{
   prep(); doAssert(SourceTokenC::TT_IDENTIFIER);

   addSkip(hasDefine(token.data));
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

         ObjectExpression::Pointer expr =
            ObjectExpression::create_value_int(hasDefine(token.data), token.pos);

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
bool SourceTokenizerDS::hasDefine()
{
   if (token.type != SourceTokenC::TT_IDENTIFIER)
      return false;

   return hasDefine(token.data);
}

//
// SourceTokenizerDS::hasDefine
//
bool SourceTokenizerDS::hasDefine(std::string const &name)
{
   return defines.find(name) != defines.end();
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

      if (canExpand && hasDefine())
      {
         prepDefine();
         continue;
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
      ungetStack.push(tokens[i]);
}

//
// SourceTokenizerDS::remDefine
//
void SourceTokenizerDS::remDefine()
{
   defines.erase(token.data);
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

