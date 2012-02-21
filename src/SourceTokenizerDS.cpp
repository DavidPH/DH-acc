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
#include "ost_type.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

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

   throw SourceException("unexpected operator", operators[begin].pos, __func__);

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
 : canCommand(true), canExpand(true), canSkip(true)
{
   #define CASE_TARGET(TARGET)                \
   case TARGET_##TARGET:                      \
      addDefine("__TARGET_"#TARGET"__",       \
                SourcePosition::builtin(),    \
                std::vector<SourceTokenC>()); \
      break

   switch (target_type)
   {
   CASE_TARGET(Eternity);
   CASE_TARGET(Hexen);
   CASE_TARGET(HexPP);
   CASE_TARGET(ZDoom);
   CASE_TARGET(UNKNOWN);
   }

   #undef CASE_TARGET

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
// SourceTokenizerDS::addDefine
//
void SourceTokenizerDS::addDefine
(std::string const &name, SourcePosition const &position,
 std::vector<SourceTokenC> const &tokens)
{
   if (hasDefine(name))
      throw SourceException("attempt to redefine define", position, __func__);

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
      throw SourceException
      ("expected " + make_string(type) + " got " + make_string(token.type),
       token.pos, __func__);
}

//
// SourceTokenizerDS::doAssert
//
void SourceTokenizerDS::doAssert(std::string const &data)
{
   if (token.data != data)
      throw SourceException
      ("expected '" + data + "' got '" + token.data + "'", token.pos, __func__);
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

   else throw SourceException("unknown command", token.pos, __func__);

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
      throw SourceException("unmatched #else", token.pos, __func__);

   skipStack.back() = unskipStack.top();
   unskipStack.top() = true; // If it wasn't, it is now.
}

//
// SourceTokenizerDS::doCommand_elif
//
void SourceTokenizerDS::doCommand_elif()
{
   if (skipStack.empty())
      throw SourceException("unmatched #elif", token.pos, __func__);

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
      throw SourceException("unmatched #endif", token.pos, __func__);

   remSkip();
}

//
// SourceTokenizerDS::doCommand_error
//
void SourceTokenizerDS::doCommand_error()
{
   prep(); doAssert(SourceTokenC::TT_STRING);

   if (isSkip()) return;

   throw SourceException(token.data, token.pos, "#error");
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
   prep(); doAssert(SourceTokenC::TT_STRING);

   if (isSkip()) return;

   try
   {
      in.push(new SourceStream(token.data, SourceStream::ST_C));
   }
   catch (std::exception & e)
   {
      throw SourceException("file not found", token.pos, __func__);
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
         throw SourceException("unexpected token type", token.pos, __func__);
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
      return ObjectExpression::create_unary_lognot(getIfSingle(), token.pos);

   case SourceTokenC::TT_OP_PARENTHESIS_O:
   {
      ObjectExpression::Pointer expr = getIfMultiple();

      doAssert(SourceTokenC::TT_OP_PARENTHESIS_C);

      return expr;
   }

   default:
      throw SourceException("unexpected token type", token.pos, __func__);
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

