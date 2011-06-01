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

/* SourceVariableC.hpp
**
** SourceVariableC class.
*/

#ifndef HPP_SourceVariableC_
#define HPP_SourceVariableC_

#include <string>

class SourceContextC;



class SourceVariableC
{
public:
	enum StorageClass
	{
		SC_AUTO,
		SC_REGISTER,
		SC_STATIC
	};

	enum VariableType
	{
		VT_INT
	};



	SourceVariableC()/* = delete*/;
	SourceVariableC(std::string const & name, int const address, StorageClass const storageClass, VariableType const type);

	StorageClass getClass() const;

	std::string getNameObject() const;
	std::string getNameSource() const;

	VariableType getType() const;

private:
	int _address;
	std::string _name;
	StorageClass _storageClass;
	VariableType _type;
};



#endif /* HPP_SourceVariableC_ */



