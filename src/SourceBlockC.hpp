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

/* SourceBlockC.hpp
**
** Defines the SourceBlockC class.
*/

#ifndef HPP_SourceBlockC_
#define HPP_SourceBlockC_

#include <ostream>
#include <stdint.h>
#include <vector>

class SourceTokenC;
class SourceTokenizerC;



class SourceBlockC
{
public:
	SourceBlockC();
	SourceBlockC(SourceTokenizerC * const in);

	SourceTokenC const & getToken(uintptr_t const index) const;

	std::vector<SourceTokenC> const & getTokens() const;



	friend void print_debug(std::ostream * const out, SourceBlockC const & block);

private:
	void readBlock(SourceTokenizerC * const in);
	void readBlocks(SourceTokenizerC * const in);

	std::vector<SourceBlockC> _blocks;
	std::vector<SourceTokenC> _tokens;
};



#endif /* HPP_SourceBlockC_ */



