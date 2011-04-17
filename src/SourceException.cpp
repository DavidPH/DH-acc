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

/* SourceException.cpp
**
** SourceException methods.
*/

#include "SourceException.hpp"



SourceException::SourceException() : _what(), _where(), _who()
{

}
SourceException::SourceException(std::string const & what, SourcePosition const & where, std::string const & who) : _what(what), _where(where), _who(who)
{

}
SourceException::~SourceException() throw()
{

}

char const * SourceException::what() const throw()
{
	return _what.c_str();
}
SourcePosition const & SourceException::where() const
{
	return _where;
}
std::string const & SourceException::who() const
{
	return _who;
}



