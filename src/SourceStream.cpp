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

/* SourceStream.cpp
**
** SourceStream methods.
*/

#include "SourceStream.hpp"



SourceStream::SourceStream(std::istream * const in, std::string const & filename, SourceType const type) :
_oldC(-2), _curC(-2), _newC(-2),
_in(in),
_filename(filename),

_countLine(1),

_depthComment(0),

_inComment(false),

_inQuoteDouble(false),
_inQuoteSingle(false)
{
	switch (type)
	{
	case ST_ASMPLX:
		_doCommentASM = true;
		_doCommentC   = false;
		_doCommentCPP = false;

		_doQuoteDouble = false;
		_doQuoteSingle = false;
		break;

	case ST_C:
		_doCommentASM = false;
		_doCommentC   = true;
		_doCommentCPP = true;

		_doQuoteDouble = true;
		_doQuoteSingle = true;
		break;
	}
}

char SourceStream::get()
{
	prepareC();

	if (_curC == -1) throw EndOfStream();

	return _curC;
}

int SourceStream::getC()
{
	if (!_ungetStack.empty())
	{
		char c = _ungetStack.top();
		_ungetStack.pop();
		return c;
	}

	if (_in) return _in->get();

	return -1;
}

std::string const & SourceStream::getFilename() const
{
	return _filename;
}

long SourceStream::getLineCount() const
{
	return _countLine;
}

bool SourceStream::isInComment() const
{
	return _inComment && !_depthComment;
}

bool SourceStream::isInQuote() const
{
	return _inQuoteDouble || _inQuoteSingle;
}

void SourceStream::prepareC()
{
	if (!_ungetStack.empty())
	{
		_oldC = -2;
		_curC = _ungetStack.top();
		_ungetStack.pop();
		return;
	}

	while (true)
	{
		_oldC = _curC;
		_curC = _newC != -2 ? _newC : _in->get();
		_newC = _in->get();



		// \n end of line
		if (_curC == '\n')
		{
			_inComment = false;
			++_countLine;
		}

		// ; line comment start
		if (_curC == ';' && _doCommentASM && !isInComment() && !isInQuote())
		{
			_inComment = true;
		}

		// // line comment start?
		if (_curC == '/' && _newC == '/' && _doCommentCPP && !isInComment() && !isInQuote())
		{
			_inComment = true;
		}

		// /* comment start?
		if (_curC == '/' && _newC == '*' && _doCommentC && !isInComment() && !isInQuote())
		{
			++_depthComment;
		}

		// */ commend end?
		if (_curC == '*' && _newC == '/' && _doCommentC && !_inComment && _depthComment && !isInQuote())
		{
			--_depthComment;
		}

		// " double quote
		if (_curC == '"' && _doQuoteDouble && !isInComment() && !_inQuoteSingle)
		{
			_inQuoteDouble = !_inQuoteDouble;
		}

		// " single quote
		if (_curC == '\'' && _doQuoteSingle && !isInComment() && !_inQuoteDouble)
		{
			_inQuoteSingle = !_inQuoteSingle;
		}



		// Comments are stripped.
		if (isInComment())
			continue;



		return;
	}
}

bool SourceStream::skipHWS()
{
	bool found(false);

	if (_newC == -2) _newC = getC();

	while (!_ungetStack.empty())
	{
		char const & c(_ungetStack.top());

		if (c == ' ' || c == '\t')
		{
			found = true;
			_ungetStack.pop();
		}
		else
			return found;
	}

	while (_newC == ' ' || _newC == '\t')
	{
		found = true;

		prepareC();
	}

	return found;
}

void SourceStream::unget(char const c)
{
	_ungetStack.push(c);
	_oldC = -2;
	_curC = -2;
}



