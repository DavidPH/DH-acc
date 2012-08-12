/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* SourceTokenASMPLX.cpp
**
** SourceTokenASMPLX methods.
*/

#include "SourceTokenASMPLX.hpp"

#include "ObjectData.hpp"
#include "ObjectCode.hpp"
#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"



SourceTokenASMPLX::SourceTokenASMPLX(SourceStream * const in) : _data(), _name(), _position(in->getFilename(), 0, 0), _type('\0')
{
	char c;

	// Skip any empty lines.
	while ((c = in->get()) == '\n');
	// If we skipped lines, need to reset position.
	_position.line = in->getLineCount();
	_position.column = in->getColumn();

	in->unget(c);

	in->skipHWS();

	// Read type.
	c = in->get();

	if (isalpha(c) || c == '_')
	{
		_type = ' ';
	}
	else if (c <= 0x20 || c >= 0x7F)
	{
      Error(_position, "invalid statement type");
	}
	else
	{
		_type = c;
		c = in->get();
	}

	// Read name.
	if (!isalpha(c) && c != '_')
      Error(_position, "missing statement name");

	while (isalnum(c) || c == '_')
	{
		_name += c;

		c = in->get();
	}

	if (c == '\n') return;

	in->unget(c);

	// Check for and skip HWS after name.
	if (!in->skipHWS())
      Error(_position, "statement name not followed by HWS");

	// Read expression list.
	while (true)
	{
		in->skipHWS();

		std::string expr;

		while (isexprc(c = in->get())) expr += c;

		_data.push_back(expr);

		if (c == ',') continue;
		if (c == '\n') break;

      Error(_position, "invalid expression");
	}
}

bigsint SourceTokenASMPLX::char_to_int(char c, bigsint base, SourcePosition const & position)
{
	switch (base)
	{
	case 2:
		if (c >= '0' && c <= '1')
			return c - '0';
		else
         Error(position, "invalid bin number");

	case 8:
		if (c >= '0' && c <= '7')
			return c - '0';
		else
         Error(position, "invalid oct number");

	case 10:
		if (c >= '0' && c <= '9')
			return c - '0';
		else
         Error(position, "invalid dec number");

	case 16:
		if (c >= '0' && c <= '9')
			return c - '0';
		else if (c >= 'A' && c <= 'F')
			return (c - 'A') + 10;
		else
         Error(position, "invalid hex number");

	default:
      Error(position, "invalid base number");
	}
}

bool SourceTokenASMPLX::isexprc(char const c)
{
	return isalnum(c) || c == '_' || c == '.' ||
		c == '*' || c == '/' || c == '%' || c == '+' ||
		c == '-' || c == '&' || c == '|' || c == '^';
}

ObjectExpression::Pointer SourceTokenASMPLX::make_expression(std::string const & expr, SourcePosition const & position)
{
	if (expr.empty()) return ObjectExpression::create_value_int(0, position);

	// Start with the last operator because the last op found is the first
	// one evaluated.
	size_t index = expr.find_last_of("*/%+-&|^");

	if (index == std::string::npos)
	{
		if (expr[0] == '0')
		{
			if (expr.find_first_of('.') == std::string::npos)
				return ObjectExpression::create_value_int(string_to_int(expr, position), position);
			else
				return ObjectExpression::create_value_fix(string_to_real(expr, position), position);
		}
		else
		{
			return ObjectExpression::create_value_symbol(expr, position);
		}
	}
	else if (index == 0)
	{
		switch (expr[0])
		{
		case '+': return ObjectExpression::create_unary_add(make_expression(expr.substr(1), position), position);
		case '-': return ObjectExpression::create_unary_sub(make_expression(expr.substr(1), position), position);

      default: Error(position, "unknown prefix operator");
		}
	}
	else
	{
		// Check for any prefix operators.
		if
		(
			expr[index-1] == '*' || expr[index-1] == '/' ||
			expr[index-1] == '%' || expr[index-1] == '+' ||
			expr[index-1] == '-' || expr[index-1] == '&' ||
			expr[index-1] == '|' || expr[index-1] == '^'
		)
		{
			--index;
		}

		std::string exprL(expr, 0,index);
		std::string exprR(expr, index+1);

		switch (expr[index])
		{
		case '*': return ObjectExpression::create_binary_mul(make_expression(exprL, position), make_expression(exprR, position), position);
		case '/': return ObjectExpression::create_binary_div(make_expression(exprL, position), make_expression(exprR, position), position);
		case '%': return ObjectExpression::create_binary_mod(make_expression(exprL, position), make_expression(exprR, position), position);
		case '+': return ObjectExpression::create_binary_add(make_expression(exprL, position), make_expression(exprR, position), position);
		case '-': return ObjectExpression::create_binary_sub(make_expression(exprL, position), make_expression(exprR, position), position);
		case '&': return ObjectExpression::create_binary_and(make_expression(exprL, position), make_expression(exprR, position), position);
		case '|': return ObjectExpression::create_binary_ior(make_expression(exprL, position), make_expression(exprR, position), position);
		case '^': return ObjectExpression::create_binary_xor(make_expression(exprL, position), make_expression(exprR, position), position);
      default: Error(position, "unknown operator");
		}
	}
}

void SourceTokenASMPLX::make_objects(std::vector<SourceTokenASMPLX> const & tokens, ObjectVector * objects)
{
	for (std::vector<SourceTokenASMPLX>::const_iterator token(tokens.begin()); token != tokens.end(); ++token)
	{
		if (token->_type == ' ')
		{
			ObjectCode ocode(ocode_get_code(token->_name, token->_position));

			std::vector<ObjectExpression::Pointer> oargs;

			for (std::vector<std::string>::const_iterator it(token->_data.begin()); it != token->_data.end(); ++it)
				oargs.push_back(make_expression(*it, token->_position));

         objects->setPosition(token->_position);
         objects->addToken(ocode, oargs);
		}
		else if (token->_type == '=')
		{
         ObjectExpression::Pointer expr = make_expression(token->_data.empty() ? "" : token->_data[0], token->_position);

			ObjectExpression::add_symbol(token->_name, expr);
		}
		else if (token->_type == ':')
		{
			if (token->_data.empty())
			{
				objects->addLabel(token->_name);
			}
			else
			{
				std::string value;
				for (std::vector<std::string>::const_iterator it(token->_data.begin()); it != token->_data.end(); ++it)
					value += (char)string_to_int(*it, token->_position);

				std::string const & name(ObjectData_String::add(value));

				ObjectExpression::Pointer expr(ObjectExpression::create_value_symbol(name, token->_position));

				ObjectExpression::add_symbol(token->_name, expr);
			}
		}
	}
}

void SourceTokenASMPLX::read_tokens(SourceStream * const in, std::vector<SourceTokenASMPLX> * const tokens)
{
	std::string idstring;

	{
		char c;

		while ((c = in->get()) == '\n');

		for (; c != '\n'; c = in->get())
			idstring += c;
	}

	if (idstring != "ASMPLX")
	{
      SourcePosition pos(in->getFilename(), in->getLineCount(), in->getColumn());
      Error_P("bad idstring");
	}

	while (true)
	{
		SourceTokenASMPLX token(in);

		if (token._type == '#' && token._name == "EOF") break;

		tokens->push_back(token);
	}
}

bigsint SourceTokenASMPLX::string_to_base(std::string const & s, SourcePosition const & position)
{
	if (s.empty() || s == "0") return -1;

	if (s.size() < 2 || s[0] != '0')
      Error(position, "invalid number");

	switch (s[1])
	{
	case 'b': return  2;
	case 'o': return  8;
	case 'd': return 10;
	case 'x': return 16;

   default: Error(position, "invalid base");
	}
}
bigsint SourceTokenASMPLX::string_to_int(std::string const & s, SourcePosition const & position)
{
	bigsint base(string_to_base(s, position));

	if (base == -1) return 0;

	bigsint i(0);
	for (size_t index(2); index < s.size(); ++index)
	{
		i *= base;
		i += char_to_int(s[index], base, position);
	}

	return i;
}
bigreal SourceTokenASMPLX::string_to_real(std::string const & s, SourcePosition const & position)
{
	bigsint basei(string_to_base(s, position));

	if (basei == -1) return 0;

	bigreal basef(basei);
	size_t index(2);

	bigreal fInt(0);
	for (; index < s.size() && s[index] != '.'; ++index)
	{
		fInt *= basef;
		fInt += char_to_int(s[index], basei, position);
	}

	bigreal fFrac(0); ++index;
	for (size_t indexFrac(s.size()); --indexFrac != index;)
	{
		fFrac += char_to_int(s[indexFrac], basei, position);
		fFrac /= basef;
	}

	return fInt + fFrac;
}

// EOF

