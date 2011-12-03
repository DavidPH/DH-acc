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

/* ObjectExpression/iter.hpp
**
** Defines iteration helpers for ObjectExpression.
*/

#ifndef HPP_iter__ObjectExpression_
#define HPP_iter__ObjectExpression_

#include "../ObjectExpression.hpp"



template<typename TableType, typename IterFunc, typename IterData>
static inline void _iterator_map(TableType & table, IterFunc iterFunc, IterData iterData)
{
	for (typename TableType::iterator it(table.begin()); it != table.end(); ++it)
		iterFunc(iterData, it->second);
}



#endif//HPP_iter__ObjectExpression_


