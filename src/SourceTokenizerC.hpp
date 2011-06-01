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

/* SourceTokenizerC.hpp
**
** Defines the SourceTokenizerC class.
*/

#ifndef HPP_SourceTokenizerC_
#define HPP_SourceTokenizerC_

#include "SourceTokenC.hpp"

#include <stack>

class SourceStream;



class SourceTokenizerC
{
public:
	SourceTokenizerC(SourceStream * const in);

	SourceTokenC get();
	SourceTokenC get(SourceTokenC::TokenType const type);

	SourceTokenC peek();

	void unget(SourceTokenC const & token);

private:
	std::stack<SourceStream *> _in;
	std::stack<SourceTokenC> _ungetStack;
};



#endif /* HPP_SourceTokenizerC_ */



