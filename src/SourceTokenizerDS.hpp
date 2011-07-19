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

/* SourceTokenizerDS.hpp
**
** Defines the SourceTokenizerDS class.
*/

#ifndef HPP_SourceTokenizerDS_
#define HPP_SourceTokenizerDS_

#include "SourceTokenC.hpp"

#include <istream>
#include <stack>

class SourceStream;



class SourceTokenizerDS
{
public:
	SourceTokenizerDS(SourceStream * const in);
	~SourceTokenizerDS();

	SourceTokenC get();
	SourceTokenC get(SourceTokenC::TokenType const type);

	SourceTokenC peek();

	void unget(SourceTokenC const & token);

private:
	SourceTokenizerDS(SourceTokenizerDS const & tokenizer)/* = delete*/;

	void doCommand(SourceTokenC const & token);
	void doCommandInclude();

	SourceTokenizerDS & operator = (SourceTokenizerDS const & tokenizer)/* = delete*/;

	std::stack<SourceStream *> _in;
	std::stack<std::istream *> _is;
	std::stack<SourceTokenC> _ungetStack;
};



#endif /* HPP_SourceTokenizerDS_ */



