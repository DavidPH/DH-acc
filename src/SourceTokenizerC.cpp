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

/* SourceTokenizerC.cpp
**
** Defines the SourceTokenizerC methods.
*/

#include "SourceTokenizerC.hpp"

#include "SourceException.hpp"
#include "SourceStream.hpp"



SourceTokenizerC::SourceTokenizerC(SourceStream * const in)
{
	_in.push(in);
}

SourceTokenC SourceTokenizerC::get()
{
	if (!_ungetStack.empty())
	{
		SourceTokenC token(_ungetStack.top());
		_ungetStack.pop();
		return token;
	}

	return SourceTokenC(_in.top());
}
SourceTokenC SourceTokenizerC::get(SourceTokenC::TokenType const type)
{
	SourceTokenC token(get());

	if (token.getType() != type)
		throw SourceException("unexpected token type", token.getPosition(), "SourceTokenizerC");

	return token;
}

SourceTokenC SourceTokenizerC::peek()
{
	if (_ungetStack.empty())
		_ungetStack.push(get());

	return _ungetStack.top();
}

void SourceTokenizerC::unget(SourceTokenC const & token)
{
	_ungetStack.push(token);
}



