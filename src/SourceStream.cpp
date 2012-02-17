//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Low-level processing of source code.
//
//-----------------------------------------------------------------------------

#include "SourceStream.hpp"

#include "option.hpp"

#include <fstream>
#include <vector>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_data<std::vector<std::string> > option_include_dir
('i', "include-dir", "input",
 "Specifies a directory to search for includes in.", NULL,
 std::vector<std::string>(1));

static option::option_data<int> option_tab_columns
('\0', "tab-columns", "input",
 "How many columns a tab counts for in error reporting.", NULL, 1);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

SourceStream::SourceStream(std::string const & filename, SourceType const type) :
_oldC(-2), _curC(-2), _newC(-2),
_in(NULL),
_filename(filename),

_countColumn(0),
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

	for (std::vector<std::string>::iterator dir(option_include_dir.data.begin()); !_in && dir != option_include_dir.data.end(); ++dir)
	{
		_in = new std::ifstream((*dir + _filename).c_str());

		if (!*_in)
		{
			delete _in;
			_in = NULL;
		}
	}

	if (!_in) throw std::exception();
}
SourceStream::~SourceStream()
{
	delete _in;
}

char SourceStream::get()
{
	if (!_ungetStack.empty())
	{
		char c = _ungetStack.top();
		_ungetStack.pop();
		return c;
	}

	while (true)
	{
		_oldC = _curC;
		_curC = _newC != -2 ? _newC : _in->get();
		_newC = _in->get();



		// \t has special counting
		if (_curC == '\t')
			_countColumn += option_tab_columns.data;
		else
			++_countColumn;

		// \n end of line
		if (_curC == '\n')
		{
			_inComment = false;
			_countColumn = 0;
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

		// End of multi-line comments are stripped, unless quoted.
		if (((_curC == '*' && _newC == '/') || (_oldC == '*' && _curC == '/')) && !isInQuote())
			continue;



		// Quoted string escape sequences.
		if (isInQuote() && _curC == '\\')
		{
			int newC(_newC);
			_newC = -2;
			++_countColumn;

			switch (newC)
			{
			case 'r': _curC = '\r'; break;
			case 'n': _curC = '\n'; break;
			case 't': _curC = '\t'; break;

			case '\n':
				_countColumn = 0;
				++_countLine;
				_curC = newC;
				break;

			case '\t':
				_countColumn += option_tab_columns.data-1;
			case '\\':
			case '\'':
			case '"':
			case ' ':
				_curC = newC;
				break;

			// TODO: \xXX

			default:
				_newC = newC;
				break;
			}
		}



		if (!*_in) throw EndOfStream();
		return (char)_curC;
	}
}

long SourceStream::getColumn() const
{
	return _countColumn;
}

std::string const & SourceStream::getFilename() const
{
	return _filename;
}

long SourceStream::getLineCount() const
{
	return _countLine;
}

bool SourceStream::is_HWS(char c)
{
	return c == ' ' || c == '\t';
}

bool SourceStream::isInComment() const
{
	return _inComment || _depthComment;
}

bool SourceStream::isInQuote() const
{
	return _inQuoteDouble || _inQuoteSingle;
}

bool SourceStream::skipHWS()
{
	bool found(false);
	char c;

	while (is_HWS(c = get())) found = true;
	unget(c);

	return found;
}

void SourceStream::unget(char const c)
{
	_ungetStack.push(c);
	_oldC = -2;
	_curC = -2;
}


