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

/* SourceContext.hpp
**
** Defines the SourceContext class.
*/

#ifndef HPP_SourceContext_
#define HPP_SourceContext_

#include "SourceVariable.hpp"

#include <map>
#include <string>

class SourceTokenC;



class SourceContext
{
public:
	SourceContext();

	void addVariable(SourceVariable const & var);

	int getAddress(SourceVariable::StorageClass const sc);

	SourceVariable const & getVariable(SourceTokenC const & token) const;

private:
	int _addressRegister;
	std::map<std::string, SourceVariable> _vars;
};



#endif /* HPP_SourceContext_ */



