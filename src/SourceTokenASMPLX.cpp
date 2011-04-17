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

#include "SourceException.hpp"
#include "SourceStream.hpp"



std::map<std::string, std::pair<ObjectToken::ObjectCode, int> > SourceTokenASMPLX::_arg_counts;



SourceTokenASMPLX::SourceTokenASMPLX(SourceStream * const in) : _data(), _name(), _position(in->getFilename(), in->getLineCount()), _type('\0')
{
	char c;

	// Skip any empty lines.
	while ((c = in->get()) == '\n');
	// If we skipped lines, need to reset position.
	_position.line = in->getLineCount();

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

std::string const & SourceTokenASMPLX::getData(uintptr_t const index) const
{
	static std::string const s;

	if (index < _data.size()) return _data[index];

	return s;
}
int32_t SourceTokenASMPLX::getDataInt32(uintptr_t const index) const
{
	return string_to_int(getData(index), _position);
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
	_arg_counts[#NAME] = std::pair<ObjectToken::ObjectCode, int>(ObjectToken::OCODE_##NAME, ARGC)

	// BinaryTokenACS
	DO_INIT(ADD,             0);
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAYDIRECT,     1);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(LSPEC1,          1);
	DO_INIT(LSPEC1DIRECT,    2);
	DO_INIT(LSPEC2,          1);
	DO_INIT(LSPEC2DIRECT,    3);
	DO_INIT(LSPEC3,          1);
	DO_INIT(LSPEC3DIRECT,    4);
	DO_INIT(LSPEC4,          1);
	DO_INIT(LSPEC4DIRECT,    5);
	DO_INIT(LSPEC5,          1);
	DO_INIT(LSPEC5DIRECT,    6);
	DO_INIT(NOP,             0);
	DO_INIT(PRINTNUMBER,     0);
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(RESTART,         0);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(PUSHGLOBALARRAY,   1);

	#undef DO_INIT
}

void SourceTokenASMPLX::initObject() const
{

}

void SourceTokenASMPLX::makeObject(std::vector<ObjectToken> * const objects) const
{
	std::map<std::string, std::pair<ObjectToken::ObjectCode, int> >::iterator argIt(_arg_counts.find(_name));

	if (argIt == _arg_counts.end())
		throw SourceException("unknown name", _position, "SourceTokenASMPLX");

	ObjectToken::ObjectCode const & code(argIt->second.first);
	int const & argC(argIt->second.second);

	if (argC == -1)
	{

	}
	else
	{
		std::vector<int32_t> args;

		for (int i(0); i < argC; ++i)
			args.push_back(getDataInt32(i));

		objects->push_back(ObjectToken(code, _position, args));
	}
}

bool SourceTokenASMPLX::isexprc(char const c)
{
	return isalnum(c) || c == '_' || c == '.' ||
		c == '*' || c == '/' || c == '%' || c == '+' ||
		c == '-' || c == '&' || c == '|' || c == '^';
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
		throw SourceException("bad idstring", SourcePosition(in->getFilename(), in->getLineCount()), "SourceTokenASMPLX");
	}

	while (true)
	{
		SourceTokenASMPLX token(in);

		if (token.getType() == '#' && token.getName() == "EOF") break;

		tokens->push_back(token);
	}
}

int32_t SourceTokenASMPLX::string_to_int(std::string const & s, SourcePosition const & position)
{
	if (s.empty() || s == "0") return 0;

	if (s.size() < 2 || s[0] != '0')
		throw SourceException("invalid number", position, "SourceTokenASMPLX");

	int32_t i(0);

	switch (s[1])
	{
	case 'b':
		for (uintptr_t index(2); index < s.size(); ++index)
		{
			char const & c(s[index]);

			i *= 2;

			if (c >= '0' && c <= '1')
				i += c - '0';
			else
				throw SourceException("invalid binary number", position, "SourceTokenASMPLX");
		}
		break;

	case 'o':
		for (uintptr_t index(2); index < s.size(); ++index)
		{
			char const & c(s[index]);

			i *= 8;

			if (c >= '0' && c <= '7')
				i += c - '0';
			else
				throw SourceException("invalid octal number", position, "SourceTokenASMPLX");
		}
		break;

	case 'd':
		for (uintptr_t index(2); index < s.size(); ++index)
		{
			char const & c(s[index]);

			i *= 10;

			if (c >= '0' && c <= '9')
				i += c - '0';
			else
				throw SourceException("invalid decimal number", position, "SourceTokenASMPLX");
		}
		break;

	case 'x':
		for (uintptr_t index(2); index < s.size(); ++index)
		{
			char const & c(s[index]);

			i *= 16;

			if (c >= '0' && c <= '9')
				i += c - '0';
			else if (c >= 'A' && c <= 'F')
				i += (c - 'A') + 10;
			else
				throw SourceException("invalid hex number", position, "SourceTokenASMPLX");
		}
		break;

	default:
		throw SourceException("invalid number", position, "SourceTokenASMPLX");
	}

	return i;
}



