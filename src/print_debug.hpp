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

/* print_debug.hpp
**
** Declares the print_debug functions.
*/

#ifndef HPP_print_debug_
#define HPP_print_debug_

#include <ostream>
#include <string>
#include <vector>



void print_debug(std::ostream * const out, bool b);
void print_debug(std::ostream * const out, std::string const & s);
template<typename T> void print_debug(std::ostream * const out, T const * const in)
{
	if (in)
		print_debug(out, *in);
	else
		*out << "NULL";
}
template<typename T> void print_debug(std::ostream * const out, std::vector<T> const & in)
{
	*out << "std::vector({";
	for (typename std::vector<T>::size_type i(0); i < in.size(); ++i)
	{
		print_debug(out, in[i]);

		if (i < in.size()-1)
			*out << ", ";
	}
	*out << "})";
}



#endif /* HPP_print_debug_ */



