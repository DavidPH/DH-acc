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

/* SourceStream.hpp
**
** SourceStream class.
*/

#ifndef HPP_SourceStream_
#define HPP_SourceStream_

#include <istream>
#include <stack>
#include <stdexcept>
#include <string>



class SourceStream
{
public:
	class EndOfStream : public std::exception
	{

	};

	enum SourceType
	{
		ST_ASMPLX,
		ST_C
	};



	SourceStream();
	SourceStream(std::string const & filename, SourceType type);
	~SourceStream();

	char get();

	long getColumn() const;
	std::string const & getFilename() const;
	long getLineCount() const;

	bool isInComment() const;
	bool isInQuote() const;

	// Skips horizontal whitespace, returning true if any found.
	bool skipHWS();

	void unget(char const c);

private:
	int getC();

	void prepareC();

	int _oldC, _curC, _newC;
	std::istream * _in;
	std::string _filename;
	std::stack<char> _ungetStack;

	long _countColumn;
	long _countLine;

	long _depthComment;

	unsigned _inComment : 1; // Single-line comment.

	unsigned _inQuoteDouble : 1; // "
	unsigned _inQuoteSingle : 1; // '

	unsigned _doCommentASM : 1; // ;
	unsigned _doCommentC   : 1; // /* */
	unsigned _doCommentCPP : 1; // //

	unsigned _doQuoteDouble : 1; // "
	unsigned _doQuoteSingle : 1; // '
};



#endif /* HPP_SourceStream_ */


