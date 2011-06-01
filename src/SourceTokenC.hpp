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

/* SourceTokenC.hpp
**
** Defines the SourceTokenC class.
*/

#ifndef HPP_SourceTokenC_
#define HPP_SourceTokenC_

#include "SourcePosition.hpp"

#include <string>
#include <vector>

class SourceStream;



class SourceTokenC
{
public:
	enum TokenType
	{
		TT_IDENTIFIER,
		TT_NUMBER,
		TT_STRING,

		TT_OP_AND,                // &
		TT_OP_AND_EQUALS,         // &=
		TT_OP_AND2,               // &&
		TT_OP_AND2_EQUALS,        // &&=
		TT_OP_ASTERISK,           // *
		TT_OP_ASTERISK_EQUALS,    // *=
		TT_OP_BRACE_C,            // }
		TT_OP_BRACE_O,            // {
		TT_OP_BRACKET_C,          // ]
		TT_OP_BRACKET_O,          // [
		TT_OP_CMP_EQ,             // ==
		TT_OP_CMP_GE,             // >=
		TT_OP_CMP_GT,             // >
		TT_OP_CMP_LE,             // <=
		TT_OP_CMP_LT,             // <
		TT_OP_CMP_NE,             // !=
		TT_OP_COLON,              // :
		TT_OP_COMMA,              // ,
		TT_OP_EQUALS,             // =
		TT_OP_HASH,               // #
		TT_OP_MINUS,              // -
		TT_OP_MINUS_EQUALS,       // -=
		TT_OP_MINUS2,             // --
		TT_OP_NOT,                // !
		TT_OP_PARENTHESIS_C,      // )
		TT_OP_PARENTHESIS_O,      // (
		TT_OP_PERCENT,            // %
		TT_OP_PERCENT_EQUALS,     // %=
		TT_OP_PERIOD,             // .
		TT_OP_PIPE,               // |
		TT_OP_PIPE_EQUALS,        // |=
		TT_OP_PIPE2,              // ||
		TT_OP_PIPE2_EQUALS,       // ||=
		TT_OP_PLUS,               // +
		TT_OP_PLUS_EQUALS,        // +=
		TT_OP_PLUS2,              // ++
		TT_OP_SEMICOLON,          // ;
		TT_OP_SHIFT_LEFT,         // <<
		TT_OP_SHIFT_LEFT_EQUALS,  // <<=
		TT_OP_SHIFT_RIGHT,        // >>
		TT_OP_SHIFT_RIGHT_EQUALS, // >>=
		TT_OP_SLASH,              // /
		TT_OP_SLASH_EQUALS,       // /=

		TT_NONE
	};



	SourceTokenC();
	SourceTokenC(SourceStream * const in);

	std::string const & getData() const;

	SourcePosition const & getPosition() const;

	TokenType getType() const;



	friend void print_debug(std::ostream * const out, SourceTokenC const & token);
	friend void print_debug(std::ostream * const out, SourceTokenC::TokenType const & type);

private:
	std::string _data;
	SourcePosition _position;
	TokenType _type;
};



#endif /* HPP_SourceTokenC_ */



