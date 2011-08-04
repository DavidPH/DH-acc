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

/* SourceTokenizerDS.cpp
**
** Defines the SourceTokenizerDS methods.
*/

#include "SourceTokenizerDS.hpp"

#include "SourceException.hpp"
#include "SourceStream.hpp"

#include <fstream>
#include <sstream>



SourceTokenizerDS::SourceTokenizerDS(SourceStream * const in)
{
	_in.push(in);
	_is.push(NULL);
}
SourceTokenizerDS::~SourceTokenizerDS()
{
	// All but the first SourceStream was alloc'd by this.
	while (_in.size() > 1)
	{
		delete _in.top();
		_in.pop();

		delete _is.top();
		_is.pop();
	}
}

void SourceTokenizerDS::doCommand(SourceTokenC const & token)
{
	if (token.getType() != SourceTokenC::TT_IDENTIFIER)
		throw SourceException("expected TT_IDENTIFIER", token.getPosition(), "SourceTokenizerDS");

	std::string const & command(token.getData());

	if (command == "include") doCommandInclude();

	else throw SourceException("unknown command", token.getPosition(), "SourceTokenizerDS");
}
void SourceTokenizerDS::doCommandInclude()
{
	SourceTokenC includeToken(_in.top());

	if (includeToken.getType() != SourceTokenC::TT_STRING)
		throw SourceException("expected TT_STRING", includeToken.getPosition(), "SourceTokenizerDS");

	std::string const & include(includeToken.getData());

	std::ifstream * is;
	try
	{
		is = new std::ifstream(include.c_str());
	}
	catch (std::exception & e)
	{
		throw SourceException("file not found", includeToken.getPosition(), "SourceTokenizerDS");
	}
	SourceStream  * in(new SourceStream(is, include, SourceStream::ST_C));

	_is.push(is);
	_in.push(in);
}

SourceTokenC SourceTokenizerDS::get()
{
	if (!_ungetStack.empty())
	{
		SourceTokenC token(_ungetStack.top());
		_ungetStack.pop();
		return token;
	}

	SourceTokenC token;

	try
	{
		token = SourceTokenC(_in.top());
	}
	catch (SourceStream::EndOfStream & e)
	{
		if (_in.size() == 1) throw;

		delete _in.top();
		_in.pop();

		delete _is.top();
		_is.pop();

		return get();
	}

	// Preprocessor directive.
	if (token.getType() == SourceTokenC::TT_OP_HASH)
	{
		doCommand(SourceTokenC(_in.top()));

		return get();
	}

	return token;
}
SourceTokenC SourceTokenizerDS::get(SourceTokenC::TokenType const type)
{
	SourceTokenC token(get());

	if (token.getType() != type)
	{
		std::ostringstream out;
		out << "expected ";
		print_debug(&out, type);
		out << " got ";
		print_debug(&out, token.getType());

		throw SourceException(out.str(), token.getPosition(), "SourceTokenizerDS");
	}

	return token;
}

SourceTokenC SourceTokenizerDS::peek()
{
	if (_ungetStack.empty())
		_ungetStack.push(get());

	return _ungetStack.top();
}

void SourceTokenizerDS::unget(SourceTokenC const & token)
{
	_ungetStack.push(token);
}



