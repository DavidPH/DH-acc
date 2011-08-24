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

#include <sstream>



SourceTokenizerDS::SourceTokenizerDS(SourceStream * const in)
{
	_in.push(in);
}
SourceTokenizerDS::~SourceTokenizerDS()
{
	// All but the first SourceStream was alloc'd by this.
	while (_in.size() > 1)
	{
		delete _in.top();
		_in.pop();
	}
}

void SourceTokenizerDS::addDefine(std::string const & name, SourcePosition const & position, std::vector<SourceTokenC> const & tokens)
{
	if (hasDefine(name))
		throw SourceException("attempt to redefine define", position, "SourceTokenizerDS");

	_defines[name] = tokens;
}

void SourceTokenizerDS::assert(SourceTokenC::TokenType type)
{
	if (_token.getType() != type)
	{
		std::ostringstream out;
		out << "expected ";
		print_debug(&out, type);
		out << " got ";
		print_debug(&out, _token.getType());

		throw SourceException(out.str(), _token.getPosition(), "SourceTokenizerDS");
	}
}

void SourceTokenizerDS::doCommand()
{
	prep(false, SourceTokenC::TT_IDENTIFIER);

	std::string const & command(_token.getData());

	if (command == "define") doCommand_define();
	else if (command == "else") doCommand_else();
	else if (command == "endif") doCommand_endif();
	else if (command == "ifdef") doCommand_ifdef();
	else if (command == "ifndef") doCommand_ifndef();
	else if (command == "include") doCommand_include();
	else if (command == "undef") doCommand_undef();

	else throw SourceException("unknown command", _token.getPosition(), "SourceTokenizerDS");
}
void SourceTokenizerDS::doCommand_define()
{
	prep(false, SourceTokenC::TT_IDENTIFIER);

	std::string name(_token.getData());
	SourcePosition position(_token.getPosition());

	std::vector<SourceTokenC> tokens;

	for (prep(false); _token.getType() != SourceTokenC::TT_OP_HASH; prep(false))
		tokens.push_back(_token);

	if (isSkip()) return;

	addDefine(name, position, tokens);
}
void SourceTokenizerDS::doCommand_else()
{
	if (_skipStack.empty())
		throw SourceException("unmatched #else", _token.getPosition(), "SourceTokenizerDS");

	_skipStack.top() = !_skipStack.top();
}
void SourceTokenizerDS::doCommand_endif()
{
	if (_skipStack.empty())
		throw SourceException("unmatched #endif", _token.getPosition(), "SourceTokenizerDS");

	_skipStack.pop();
}
void SourceTokenizerDS::doCommand_ifdef()
{
	prep(false, SourceTokenC::TT_IDENTIFIER);

	_skipStack.push(!hasDefine(_token.getData()));
}
void SourceTokenizerDS::doCommand_ifndef()
{
	prep(false, SourceTokenC::TT_IDENTIFIER);

	_skipStack.push(hasDefine(_token.getData()));
}
void SourceTokenizerDS::doCommand_include()
{
	prep(false, SourceTokenC::TT_STRING);

	if (isSkip()) return;

	try
	{
		_in.push(new SourceStream(_token.getData(), SourceStream::ST_C));
	}
	catch (std::exception & e)
	{
		throw SourceException("file not found", _token.getPosition(), "SourceTokenizerDS");
	}
}
void SourceTokenizerDS::doCommand_undef()
{
	prep(false, SourceTokenC::TT_IDENTIFIER);

	if (isSkip()) return;

	remDefine();
}

SourceTokenC const & SourceTokenizerDS::get()
{
	prep(true);

	return _token;
}
SourceTokenC const & SourceTokenizerDS::get(SourceTokenC::TokenType type)
{
	prep(true, type);

	return _token;
}

bool SourceTokenizerDS::hasDefine()
{
	if (_token.getType() != SourceTokenC::TT_IDENTIFIER)
		return false;

	return hasDefine(_token.getData());
}
bool SourceTokenizerDS::hasDefine(std::string const & name)
{
	return _defines.find(name) != _defines.end();
}

bool SourceTokenizerDS::isSkip()
{
	return !_skipStack.empty() && _skipStack.top();
}

SourceTokenC const & SourceTokenizerDS::peek()
{
	if (_ungetStack.empty())
		_ungetStack.push(get());

	return _ungetStack.top();
}

void SourceTokenizerDS::prep(bool preprocess)
{
	start:

	if (!_ungetStack.empty())
	{
		_token = _ungetStack.top();
		_ungetStack.pop();
	}
	else try
	{
		_token = SourceTokenC(_in.top());
	}
	catch (SourceStream::EndOfStream & e)
	{
		if (_in.size() == 1) throw;

		delete _in.top();
		_in.pop();

		goto start;
	}

	if (!preprocess) return;

	// Preprocessor directive.
	if (_token.getType() == SourceTokenC::TT_OP_HASH)
	{
		doCommand();
		goto start;
	}

	if (hasDefine())
	{
		prepDefine();
		goto start;
	}

	if (isSkip())
		goto start;
}
void SourceTokenizerDS::prep(bool preprocess, SourceTokenC::TokenType type)
{
	prep(preprocess);
	assert(type);
}

void SourceTokenizerDS::prepDefine()
{
	std::vector<SourceTokenC> const & tokens(_defines[_token.getData()]);

	for (size_t i(tokens.size()); i--;)
		_ungetStack.push(tokens[i]);
}

void SourceTokenizerDS::remDefine()
{
	_defines.erase(_token.getData());
}

void SourceTokenizerDS::unget(SourceTokenC const & token)
{
	_ungetStack.push(token);
}



