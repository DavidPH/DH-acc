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

#include "ObjectExpression.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"

#include <sstream>



SourceTokenizerDS::SourceTokenizerDS(SourceStream * const in) : _canCommand(true), _canExpand(true), _canSkip(true)
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
void SourceTokenizerDS::addSkip(bool skip)
{
	_skipStack.push(skip);
	_unskipStack.push(!skip);
}

void SourceTokenizerDS::doAssert(SourceTokenC::TokenType type)
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
	_canCommand = false;
	_canExpand  = false;
	_canSkip    = false;

	prep(SourceTokenC::TT_IDENTIFIER);

	std::string const & command(_token.getData());

	if (command == "define") doCommand_define();
	else if (command == "else") doCommand_else();
	else if (command == "elif") doCommand_elif();
	else if (command == "endif") doCommand_endif();
	else if (command == "error") doCommand_error();
	else if (command == "if") doCommand_if();
	else if (command == "ifdef") doCommand_ifdef();
	else if (command == "ifndef") doCommand_ifndef();
	else if (command == "include") doCommand_include();
	else if (command == "undef") doCommand_undef();

	else throw SourceException("unknown command", _token.getPosition(), "SourceTokenizerDS");

	_canCommand = true;
	_canExpand  = true;
	_canSkip    = true;
}
void SourceTokenizerDS::doCommand_define()
{
	prep(SourceTokenC::TT_IDENTIFIER);

	std::string name(_token.getData());
	SourcePosition position(_token.getPosition());

	std::vector<SourceTokenC> tokens;

	for (prep(); _token.getType() != SourceTokenC::TT_OP_HASH; prep())
		tokens.push_back(_token);

	if (isSkip()) return;

	addDefine(name, position, tokens);
}
void SourceTokenizerDS::doCommand_else()
{
	if (_skipStack.empty())
		throw SourceException("unmatched #else", _token.getPosition(), "SourceTokenizerDS");

	_skipStack.top() = _unskipStack.top();
	_unskipStack.top() = true; // If it wasn't, it is now.
}
void SourceTokenizerDS::doCommand_elif()
{
	if (_skipStack.empty())
		throw SourceException("unmatched #elif", _token.getPosition(), "SourceTokenizerDS");

	bool ifResult(getIf());

	_skipStack.top() = _unskipStack.top() || !ifResult;
	_unskipStack.top() = _unskipStack.top() || ifResult;
}
void SourceTokenizerDS::doCommand_endif()
{
	if (_skipStack.empty())
		throw SourceException("unmatched #endif", _token.getPosition(), "SourceTokenizerDS");

	remSkip();
}
void SourceTokenizerDS::doCommand_error()
{
	prep(SourceTokenC::TT_STRING);

	throw SourceException(_token.getData(), _token.getPosition(), "#error");
}
void SourceTokenizerDS::doCommand_if()
{
	addSkip(!getIf());
}
void SourceTokenizerDS::doCommand_ifdef()
{
	prep(SourceTokenC::TT_IDENTIFIER);

	addSkip(!hasDefine(_token.getData()));
}
void SourceTokenizerDS::doCommand_ifndef()
{
	prep(SourceTokenC::TT_IDENTIFIER);

	addSkip(hasDefine(_token.getData()));
}
void SourceTokenizerDS::doCommand_include()
{
	prep(SourceTokenC::TT_STRING);

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
	prep(SourceTokenC::TT_IDENTIFIER);

	if (isSkip()) return;

	remDefine();
}

SourceTokenC const & SourceTokenizerDS::get()
{
	prep();

	return _token;
}
SourceTokenC const & SourceTokenizerDS::get(SourceTokenC::TokenType type)
{
	prep(type);

	return _token;
}

bool SourceTokenizerDS::getIf()
{
	_canExpand = true;

	ObjectExpression::Pointer expr(getIfMultiple());

	_canExpand = false;

	return !!expr->resolveInt();
}
ObjectExpression::Pointer SourceTokenizerDS::getIfMultiple()
{
	ObjectExpression::Pointer expr(getIfSingle());

	for (prep(); _token.getType() != SourceTokenC::TT_OP_HASH; prep())
	{
		SourcePosition position(_token.getPosition());

		switch (_token.getType())
		{
		case SourceTokenC::TT_OP_AND:
			expr = ObjectExpression::create_binary_and(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_ASTERISK:
			expr = ObjectExpression::create_binary_mul(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_CARET:
			expr = ObjectExpression::create_binary_xor(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_MINUS:
			expr = ObjectExpression::create_binary_sub(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_PARENTHESIS_C:
			return expr;

		case SourceTokenC::TT_OP_PERCENT:
			expr = ObjectExpression::create_binary_mod(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_PIPE:
			expr = ObjectExpression::create_binary_ior(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_PLUS:
			expr = ObjectExpression::create_binary_add(expr, getIfSingle(), position);
			break;

		case SourceTokenC::TT_OP_SLASH:
			expr = ObjectExpression::create_binary_div(expr, getIfSingle(), position);
			break;

		default:
			throw SourceException("unexpected token type", position, "SourceTokenizerDS::getIfMultiple");
		}
	}

	return expr;
}
ObjectExpression::Pointer SourceTokenizerDS::getIfSingle()
{
	prep();

	switch (_token.getType())
	{
	case SourceTokenC::TT_IDENTIFIER:
		if (_token.getData() == "defined")
		{
			_canExpand = false;

			prep();

			bool hasParentheses(_token.getType() == SourceTokenC::TT_OP_PARENTHESIS_O);

			if (hasParentheses) prep();

			doAssert(SourceTokenC::TT_IDENTIFIER);

			ObjectExpression::Pointer expr(ObjectExpression::create_value_int(hasDefine(_token.getData()), _token.getPosition()));

			if (hasParentheses) prep(SourceTokenC::TT_OP_PARENTHESIS_C);

			_canExpand = true;

			return expr;
		}
		else
		{
			return ObjectExpression::create_value_int(0, _token.getPosition());
		}

	case SourceTokenC::TT_INTEGER:
		return ObjectExpression::create_value_int(get_bigsint(_token), _token.getPosition());

	case SourceTokenC::TT_OP_PARENTHESIS_O:
	{
		ObjectExpression::Pointer expr(getIfMultiple());

		doAssert(SourceTokenC::TT_OP_PARENTHESIS_C);

		return expr;
	}

	default:
		throw SourceException("unexpected token type", _token.getPosition(), "SourceTokenizerDS::getIfSingle");
	}
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
	prep();

	_ungetStack.push(_token);

	return _token;
}

void SourceTokenizerDS::prep()
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

	// Preprocessor directive.
	if (_canCommand && _token.getType() == SourceTokenC::TT_OP_HASH)
	{
		doCommand();
		goto start;
	}

	if (_canExpand && hasDefine())
	{
		prepDefine();
		goto start;
	}

	if (_canSkip && isSkip())
		goto start;
}
void SourceTokenizerDS::prep(SourceTokenC::TokenType type)
{
	prep();
	doAssert(type);
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
void SourceTokenizerDS::remSkip()
{
	_skipStack.pop();
	_unskipStack.pop();
}

void SourceTokenizerDS::unget(SourceTokenC const & token)
{
	_ungetStack.push(token);
}



