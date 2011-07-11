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

/* SourceExpressionDS/Base.hpp
**
** Defines the SourceExpressionDS_Base class.
*/

#ifndef HPP_SourceExpressionDS__Base_
#define HPP_SourceExpressionDS__Base_

#include "../SourceExpressionDS.hpp"
#include "../SourcePosition.hpp"



class SourceExpressionDS_Base
{
public:
	SourceExpressionDS_Base(SourcePosition const & position);
	virtual ~SourceExpressionDS_Base();

	virtual SourceExpressionDS_Base * clone() const = 0;

	virtual ObjectExpression createObject() const;

	std::vector<std::string> const & getLabels() const;
	SourcePosition const & getPosition() const;

	virtual SourceExpressionDS::ExpressionType getType() const = 0;

	virtual bool isConstant() const = 0;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const = 0;

	virtual void printDebug(std::ostream * const out) const;

private:
	std::vector<std::string> _labels;
	SourcePosition _position;
};



#endif /* HPP_SourceExpressionDS__Base_ */



