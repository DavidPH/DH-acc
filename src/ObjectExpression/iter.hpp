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
static inline void _iterator_function(TableType & table, IterFunc iterFunc, IterData iterData, std::string const & library)
{
	typedef std::vector<ObjectData_Function *> vec_t;
	typedef std::map<std::string, vec_t> map_t;

	typename vec_t::iterator vecit;
	typename map_t::iterator mapit;

	vec_t internals;
	map_t externals;

	for (typename TableType::iterator it(table.begin()); it != table.end(); ++it)
	{
		if (!it->second.external || it->second.library.empty() || it->second.library == library)
			internals.push_back(&it->second);
		else
			externals[it->second.library].push_back(&it->second);
	}

	for (vecit = internals.begin(); vecit != internals.end(); ++vecit)
		iterFunc(iterData, **vecit);

	for (mapit = externals.begin(); mapit != externals.end(); ++mapit)
		for (vecit = mapit->second.begin(); vecit != mapit->second.end(); ++vecit)
			iterFunc(iterData, **vecit);
}

template<typename TableType, typename IterFunc, typename IterData>
static inline void _iterator_map(TableType & table, IterFunc iterFunc, IterData iterData)
{
	for (typename TableType::iterator it(table.begin()); it != table.end(); ++it)
		iterFunc(iterData, it->second);
}



#endif//HPP_iter__ObjectExpression_


