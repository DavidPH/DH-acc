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

/* SourceExpressionDS/Binary.hpp
**
** Defines the SourceExpressionDS_Binary class.
*/

#ifndef HPP_SourceExpressionDS__Binary_
#define HPP_SourceExpressionDS__Binary_

#include "Base.hpp"



class SourceExpressionDS_Binary : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_Binary(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	SourceExpressionDS_Binary(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, bool const castL, SourcePosition const & position);

	virtual SourceExpressionDS_Binary * clone() const = 0;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

protected:
	SourceExpressionDS _exprL;
	SourceExpressionDS _exprR;
};



#endif /* HPP_SourceExpressionDS__Binary_ */



