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

/* ObjectExpression/iter.cpp
**
** Defines the ObjectExpression::iter_* functions.
*/

#include "../ObjectExpression.hpp"

#include "iter.hpp"

#include "../ObjectData.hpp"



void ObjectExpression::iter_auto(void (*iter)(std::ostream *, ObjectData_Auto const &), std::ostream * out)
{
	_iterator_map(_auto_table, iter, out);
}
void ObjectExpression::iter_library(void (*iter)(std::ostream *, std::string const &), std::ostream * out)
{
	for (std::set<std::string>::iterator it(_library_table.begin()); it != _library_table.end(); ++it)
			iter(out, *it);
}
void ObjectExpression::iter_static(void (*iter)(std::ostream *, ObjectData_Static const &), std::ostream * out)
{
	_iterator_map(_static_table, iter, out);
}

// EOF

