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

/* SourceException.hpp
**
** SourceException class.
*/

#ifndef HPP_SourceException_
#define HPP_SourceException_

#include "SourcePosition.hpp"

#include <stdexcept>



class SourceException : public std::exception
{
public:
	SourceException();
	SourceException(std::string const & what, SourcePosition const & where, std::string const & who);
	virtual ~SourceException() throw();

	virtual char const * what() const throw();
	SourcePosition const & where() const;
	std::string const & who() const;

private:
	std::string _what;
	SourcePosition _where;
	std::string _who;
};



#endif /* HPP_SourcePosition_ */



