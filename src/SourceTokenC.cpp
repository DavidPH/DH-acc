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

/* SourceTokenC.cpp
**
** Defines the SourceTokenC methods.
*/

#include "SourceTokenC.hpp"

#include "print_debug.hpp"
#include "SourceStream.hpp"



SourceTokenC::SourceTokenC() : _data(), _position(), _type(TT_NONE)
{

}
SourceTokenC::SourceTokenC(SourceStream * const in) : _data(), _position(in->getFilename(), in->getLineCount()), _type(TT_NONE)
{
	char c(in->get());

	// Discard any whitespace before token.
	while (isspace(c)) c = in->get();
	// If we skipped lines, need to reset position.
	_position.line = in->getLineCount();

	switch (c)
	{
	case '@': _type = TT_OP_AT;            return;
	case '}': _type = TT_OP_BRACE_C;       return;
	case '{': _type = TT_OP_BRACE_O;       return;
	case ']': _type = TT_OP_BRACKET_C;     return;
	case '[': _type = TT_OP_BRACKET_O;     return;
	case ':': _type = TT_OP_COLON;         return;
	case ',': _type = TT_OP_COMMA;         return;
	case '#': _type = TT_OP_HASH;          return;
	case ')': _type = TT_OP_PARENTHESIS_C; return;
	case '(': _type = TT_OP_PARENTHESIS_O; return;
	case '.': _type = TT_OP_PERIOD;        return;
	case '?': _type = TT_OP_QUERY;         return;
	case ';': _type = TT_OP_SEMICOLON;     return;
	case '~': _type = TT_OP_TILDE;         return;

	case '&':
		c = in->get();

		if (c == '=') {_type = TT_OP_AND_EQUALS; return;}
		if (c == '&')
		{
			c = in->get();

			if (c == '=') {_type = TT_OP_AND2_EQUALS; return;}

			in->unget(c);

			_type = TT_OP_AND2; return;
		}

		in->unget(c);

		_type = TT_OP_AND; return;

	case '*':
		c = in->get();

		if (c == '=') {_type = TT_OP_ASTERISK_EQUALS; return;}

		in->unget(c);

		_type = TT_OP_ASTERISK; return;

	case '=':
		c = in->get();

		if (c == '=') {_type = TT_OP_CMP_EQ; return;}

		in->unget(c);

		_type = TT_OP_EQUALS; return;

	case '>':
		c = in->get();

		if (c == '=') {_type = TT_OP_CMP_GE; return;}
		if (c == '>')
		{
			c = in->get();

			if (c == '=') {_type = TT_OP_SHIFT_RIGHT_EQUALS; return;}

			in->unget(c);

			_type = TT_OP_SHIFT_RIGHT; return;
		}

		in->unget(c);

		_type = TT_OP_CMP_GT; return;

	case '<':
		c = in->get();

		if (c == '=') {_type = TT_OP_CMP_LE; return;}
		if (c == '<')
		{
			c = in->get();

			if (c == '=') {_type = TT_OP_SHIFT_LEFT_EQUALS; return;}

			in->unget(c);

			_type = TT_OP_SHIFT_LEFT; return;
		}

		in->unget(c);

		_type = TT_OP_CMP_LT; return;

	case '-':
		c = in->get();

		if (c == '=') {_type = TT_OP_MINUS_EQUALS; return;}
		if (c == '>') {_type = TT_OP_MINUS_GT;     return;}
		if (c == '-') {_type = TT_OP_MINUS2;       return;}

		in->unget(c);

		_type = TT_OP_MINUS; return;

	case '!':
		c = in->get();

		if (c == '=') {_type = TT_OP_CMP_NE; return;}

		in->unget(c);

		_type = TT_OP_EXCLAMATION; return;

	case '%':
		c = in->get();

		if (c == '=') {_type = TT_OP_PERCENT_EQUALS; return;}

		in->unget(c);

		_type = TT_OP_PERCENT; return;

	case '|':
		c = in->get();

		if (c == '=') {_type = TT_OP_PIPE_EQUALS; return;}
		if (c == '|')
		{
			c = in->get();

			if (c == '=') {_type = TT_OP_PIPE2_EQUALS; return;}

			in->unget(c);

			_type = TT_OP_PIPE2; return;
		}

		in->unget(c);

		_type = TT_OP_PIPE; return;

	case '+':
		c = in->get();

		if (c == '=') {_type = TT_OP_PLUS_EQUALS; return;}
		if (c == '+') {_type = TT_OP_PLUS2;       return;}

		in->unget(c);

		_type = TT_OP_PLUS; return;

	case '/':
		c = in->get();

		if (c == '=') {_type = TT_OP_SLASH_EQUALS; return;}

		in->unget(c);

		_type = TT_OP_SLASH; return;
	}

	if (isalpha(c) || c == '_')
	{
		_type = TT_IDENTIFIER;

		while (isalnum(c) || c == '_')
		{
			_data += c;

			c = in->get();
		}

		in->unget(c);

		return;
	}

	if (isdigit(c) || c == '.')
	{
		_type = c == '.' ? TT_FLOAT : TT_INTEGER;

		bool foundDot   (c == '.');
		bool foundPlus  (false);
		bool foundZero  (c == '0');
		bool foundPrefix(!foundZero);

		char cOld;

		while (true)
		{
			_data += c;

			cOld = c;
			c = in->get();

			if (!foundPrefix)
			{
				// Can only find prefix at start.
				foundPrefix = true;

				if (c == 'X' || c == 'x') continue;
			}

			if (isxdigit(c)) continue;

			if (!foundDot && c == '.')
			{
				_type = TT_FLOAT;
				foundDot = true;
				continue;
			}

			if (!foundPlus && ((cOld == 'E') || (cOld == 'e')) && ((c == '+') || (c == '-')))
			{
				_type = TT_FLOAT;
				foundPlus = true;
				continue;
			}

			break;
		}

		in->unget(c);

		return;
	}

	if (c == '"' || c == '\'')
	{
		_type = c == '"' ? TT_STRING : TT_CHARACTER;

		c = in->get();

		while (in->isInQuote())
		{
			_data += c;

			c = in->get();
		}

		return;
	}
}

std::string const & SourceTokenC::getData() const
{
	return _data;
}

SourcePosition const & SourceTokenC::getPosition() const
{
	return _position;
}

SourceTokenC::TokenType SourceTokenC::getType() const
{
	return _type;
}



void print_debug(std::ostream * const out, SourceTokenC const & token)
{
	*out << "SourceTokenC(";
		*out << "type=(";
		print_debug(out, token._type);
		*out << ")";

		*out << ", ";

		*out << "data=(";
		print_debug(out, token._data);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, SourceTokenC::TokenType const & type)
{
	switch (type)
	{
	case SourceTokenC::TT_CHARACTER:             *out << "TT_CHARACTER";             break;
	case SourceTokenC::TT_FLOAT:                 *out << "TT_FLOAT";                 break;
	case SourceTokenC::TT_IDENTIFIER:            *out << "TT_IDENTIFIER";            break;
	case SourceTokenC::TT_INTEGER:               *out << "TT_INTEGER";               break;
	case SourceTokenC::TT_OP_AT:                 *out << "TT_OP_AT";                 break;
	case SourceTokenC::TT_OP_EXCLAMATION:        *out << "TT_OP_EXCLAMATION";        break;
	case SourceTokenC::TT_OP_PLUS:               *out << "TT_OP_PLUS";               break;
	case SourceTokenC::TT_OP_QUERY:              *out << "TT_OP_QUERY";              break;
	case SourceTokenC::TT_OP_SEMICOLON:          *out << "TT_OP_SEMICOLON";          break;
	case SourceTokenC::TT_OP_SHIFT_RIGHT_EQUALS: *out << "TT_OP_SHIFT_RIGHT_EQUALS"; break;
	case SourceTokenC::TT_OP_TILDE:              *out << "TT_OP_TILDE";              break;
	default: *out << "TT_"; break;
	}
}



