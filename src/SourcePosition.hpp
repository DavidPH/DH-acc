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

/* SourcePosition.hpp
**
** SourcePosition class.
*/

#ifndef HPP_SourcePosition_
#define HPP_SourcePosition_

#include <ostream>
#include <string>



class SourcePosition
{
public:
	SourcePosition();
	SourcePosition(std::string const & filename, long const line);

	std::string filename;
	long line;



	static SourcePosition const none;
};



std::ostream & operator << (std::ostream & out, SourcePosition const & in);



#endif /* HPP_SourcePosition_ */



