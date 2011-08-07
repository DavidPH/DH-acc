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

#include "BinaryTokenZDACS.hpp"
#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "ObjectVector.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"



std::map<std::string, std::pair<ObjectCode, int> > SourceTokenASMPLX::_arg_counts;



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
		throw SourceException("invalid statement type", _position, "SourceTokenASMPLX");
	}
	else
	{
		_type = c;
		c = in->get();
	}

	// Read name.
	if (!isalpha(c) && c != '_')
		throw SourceException("missing statement name", _position, "SourceTokenASMPLX");

	while (isalnum(c) || c == '_')
	{
		_name += c;

		c = in->get();
	}

	if (c == '\n') return;

	in->unget(c);

	// Check for and skip HWS after name.
	if (!in->skipHWS())
		throw SourceException("statement name not followed by HWS", _position, "SourceTokenASMPLX");

	// Read expression list.
	while (true)
	{
		in->skipHWS();

		std::string expr;

		while (isexprc(c = in->get())) expr += c;

		_data.push_back(expr);

		if (c == ',') continue;
		if (c == '\n') break;

		throw SourceException("invalid expression", _position, "SourceTokenASMPLX");
	}
}

int32_t SourceTokenASMPLX::char_to_int(char const c, int32_t const base, SourcePosition const & position)
{
	switch (base)
	{
	case 2:
		if (c >= '0' && c <= '1')
			return c - '0';
		else
			throw SourceException("invalid bin number", position, "SourceTokenASMPLX");

	case 8:
		if (c >= '0' && c <= '7')
			return c - '0';
		else
			throw SourceException("invalid oct number", position, "SourceTokenASMPLX");

	case 10:
		if (c >= '0' && c <= '9')
			return c - '0';
		else
			throw SourceException("invalid dec number", position, "SourceTokenASMPLX");

	case 16:
		if (c >= '0' && c <= '9')
			return c - '0';
		else if (c >= 'A' && c <= 'F')
			return (c - 'A') + 10;
		else
			throw SourceException("invalid hex number", position, "SourceTokenASMPLX");

	default:
		throw SourceException("invalid base number", position, "SourceTokenASMPLX");
	}
}

std::string const & SourceTokenASMPLX::getData(uintptr_t const index) const
{
	static std::string const s;

	if (index < _data.size()) return _data[index];

	return s;
}

uintptr_t SourceTokenASMPLX::getDataSize() const
{
	return _data.size();
}

SourcePosition const & SourceTokenASMPLX::getPosition() const
{
	return _position;
}

std::string const & SourceTokenASMPLX::getName() const
{
	return _name;
}

SourceTokenASMPLX::TokenType SourceTokenASMPLX::getType() const
{
	return _type;
}

void SourceTokenASMPLX::init()
{
	#define DO_INIT(NAME,ARGC)\
	_arg_counts[#NAME] = std::pair<ObjectCode, int>(OCODE_##NAME, ARGC)

	// BinaryTokenACS
	DO_INIT(ADD,             0);
	DO_INIT(ADDWORLDVAR,     1);
	DO_INIT(AND,             0);
	DO_INIT(ASSIGNMAPVAR,    1);
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(ASSIGNWORLDVAR,  1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(BRANCH,          1);
	DO_INIT(BRANCHCASE,      2);
	DO_INIT(BRANCHNOTZERO,   1);
	DO_INIT(BRANCHZERO,      1);
	DO_INIT(CMPEQ,           0);
	DO_INIT(CMPGE,           0);
	DO_INIT(CMPGT,           0);
	DO_INIT(CMPLE,           0);
	DO_INIT(CMPLT,           0);
	DO_INIT(CMPNE,           0);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAY_IMM,       1);
	DO_INIT(DIV,             0);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(IOR,             0);
	DO_INIT(LOGICALAND,      0);
	DO_INIT(LOGICALIOR,      0);
	DO_INIT(LOGICALNOT,      0);
	DO_INIT(LSPEC1,          1);
	DO_INIT(LSPEC1_IMM,      2);
	DO_INIT(LSPEC2,          1);
	DO_INIT(LSPEC2_IMM,      3);
	DO_INIT(LSPEC3,          1);
	DO_INIT(LSPEC3_IMM,      4);
	DO_INIT(LSPEC4,          1);
	DO_INIT(LSPEC4_IMM,      5);
	DO_INIT(LSPEC5,          1);
	DO_INIT(LSPEC5_IMM,      6);
	DO_INIT(MUL,             0);
	DO_INIT(MOD,             0);
	DO_INIT(NEGATE,          0);
	DO_INIT(NOP,             0);
	DO_INIT(PRINTCHARACTER,  0);
	DO_INIT(PRINTNUMBER,     0);
	DO_INIT(PRINTSTRING,     0);
	DO_INIT(PUSHMAPVAR,      1);
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(PUSHWORLDVAR,    1);
	DO_INIT(RANDOM,          0);
	DO_INIT(RANDOM_IMM,      2);
	DO_INIT(RESTART,         0);
	DO_INIT(SHIFTL,          0);
	DO_INIT(SHIFTR,          0);
	DO_INIT(SUB,             0);
	DO_INIT(SUBWORLDVAR,     1);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);
	DO_INIT(XOR,             0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(ASSIGNGLOBALVAR,   1);
	DO_INIT(ASSIGNMAPARRAY,    1);
	DO_INIT(ASSIGNWORLDARRAY,  1);
	DO_INIT(CALLZDACS,         1);
	DO_INIT(CALLZDACSDISCARD,  1);
	DO_INIT(CALLZDFUNC,        2);
	DO_INIT(DIVFIXED,          0);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(LSPEC5RESULT,      1);
	DO_INIT(MULFIXED,          0);
	DO_INIT(NOT,               0);
	DO_INIT(PRINTFIXED,        0);
	DO_INIT(PRINTHEX,          0);
	DO_INIT(PUSHGLOBALARRAY,   1);
	DO_INIT(PUSHGLOBALVAR,     1);
	DO_INIT(PUSHMAPARRAY,      1);
	DO_INIT(PUSHWORLDARRAY,    1);
	DO_INIT(RETURNZDACS,       0);
	DO_INIT(RETURNZDACSVOID,   0);
	DO_INIT(SETRESULTVALUE,    0);
	DO_INIT(STRLEN,            0);
	DO_INIT(SWAP,              0);

	// ObjectToken
	DO_INIT(ADDSTACK_IMM,       1);
	DO_INIT(ASSIGNGLOBALARRAY2, 2);
	DO_INIT(ASSIGNMAPARRAY2,    2);
	DO_INIT(ASSIGNSTACKARRAY2,  2);
	DO_INIT(ASSIGNSTACKVAR,     1);
	DO_INIT(ASSIGNSTATICARRAY2, 2);
	DO_INIT(ASSIGNSTATICVAR,    1);
	DO_INIT(ASSIGNWORLDARRAY2,  2);
	DO_INIT(LOGICALXOR,         0);
	DO_INIT(PUSHGLOBALARRAY2,   2);
	DO_INIT(PUSHMAPARRAY2,      2);
	DO_INIT(PUSHSTACKADDRESS,   1);
	DO_INIT(PUSHSTACKARRAY2,    2);
	DO_INIT(PUSHSTACKVAR,       1);
	DO_INIT(PUSHSTATICARRAY2,   2);
	DO_INIT(PUSHSTATICVAR,      1);
	DO_INIT(PUSHWORLDARRAY2,    2);
	DO_INIT(SUBSTACK_IMM,       1);

	#undef DO_INIT
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
	uintptr_t index = expr.find_last_of("*/%+-&|^");

	if (index == std::string::npos)
	{
		if (expr[0] == '0')
		{
			if (expr.find_first_of('.') == std::string::npos)
				return ObjectExpression::create_value_int(string_to_int(expr, position), position);
			else
				return ObjectExpression::create_value_float(string_to_real(expr, position), position);
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

		default: throw SourceException("unknown prefix operator", position, "SourceTokenASMPLX");
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
		default: throw SourceException("unknown operator", position, "SourceTokenASMPLX");
		}
	}
}

void SourceTokenASMPLX::make_objects(std::vector<SourceTokenASMPLX> const & tokens, ObjectVector * objects)
{
	std::vector<std::string> labels;

	for (uintptr_t index(0); index < tokens.size(); ++index)
	{
		SourceTokenASMPLX const & token(tokens[index]);

		if (token._type == ' ')
		{
			std::map<std::string, std::pair<ObjectCode, int> >::iterator argIt(_arg_counts.find(token._name));

			if (argIt == _arg_counts.end())
				throw SourceException("unknown name", token._position, "SourceTokenASMPLX");

			ObjectCode const & code(argIt->second.first);
			int const & argC(argIt->second.second);

			if (argC == -1)
			{

			}
			else
			{
				std::vector<ObjectExpression::Pointer> args;

				for (uintptr_t i(0); i < (uintptr_t)argC; ++i)
					args.push_back(make_expression(token._data[i], token._position));

				objects->addToken(ObjectToken(code, token._position, labels, args));

				labels.clear();
			}
		}
		else if (token._type == '=')
		{
			ObjectExpression::add_symbol(token._name, make_expression(token.getData(0), token._position));
		}
		else if (token._type == ':')
		{
			if (token._data.empty())
			{
				labels.push_back(token._name);
			}
			else
			{
				std::string value;
				for (uintptr_t index(0); index < token._data.size(); ++index)
					value += (char)string_to_int(token._data[index], token._position);

				ObjectExpression::add_string(token._name, value);
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
		throw SourceException("bad idstring", SourcePosition(in->getFilename(), in->getLineCount(), in->getColumn()), "SourceTokenASMPLX");
	}

	while (true)
	{
		SourceTokenASMPLX token(in);

		if (token.getType() == '#' && token.getName() == "EOF") break;

		tokens->push_back(token);
	}
}

int32_t SourceTokenASMPLX::string_to_base(std::string const & s, SourcePosition const & position)
{
	if (s.empty() || s == "0") return -1;

	if (s.size() < 2 || s[0] != '0')
		throw SourceException("invalid number", position, "SourceTokenASMPLX");

	switch (s[1])
	{
	case 'b': return  2;
	case 'o': return  8;
	case 'd': return 10;
	case 'x': return 16;

	default: throw SourceException("invalid base", position, "SourceTokenASMPLX");
	}
}
int32_t SourceTokenASMPLX::string_to_int(std::string const & s, SourcePosition const & position)
{
	int32_t base(string_to_base(s, position));

	if (base == -1) return 0;

	int32_t i(0);
	for (uintptr_t index(2); index < s.size(); ++index)
	{
		i *= base;
		i += char_to_int(s[index], base, position);
	}

	return i;
}
long double SourceTokenASMPLX::string_to_real(std::string const & s, SourcePosition const & position)
{
	long double base(string_to_base(s, position));

	if (base == -1) return 0;

	uintptr_t index(2);

	long double fInt(0);
	for (; index < s.size() && s[index] != '.'; ++index)
	{
		fInt *= base;
		fInt += char_to_int(s[index], base, position);
	}

	long double fFrac(0);
	for (++index; index < s.size(); ++index)
	{
		fFrac += char_to_int(s[index], base, position);
		fFrac /= base;
	}

	return fInt + fFrac;
}



