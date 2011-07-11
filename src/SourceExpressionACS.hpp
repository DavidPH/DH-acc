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

/* SourceExpressionACS.hpp
**
** SourceExpressionACS class.
*/

#ifndef HPP_SourceExpressionACS_
#define HPP_SourceExpressionACS_

#include "SourcePosition.hpp"

#include <vector>

class ObjectToken;
class SourceBlockC;
class SourceContext;
class SourceTokenC;



class SourceExpressionACSBase
{
public:
	enum ExpressionType
	{
		ET_FIXED,
		ET_INT
	};



	SourceExpressionACSBase(SourcePosition const & position);
	virtual ~SourceExpressionACSBase();

	virtual SourceExpressionACSBase * clone() const = 0;

	SourcePosition const & getPosition() const;

	virtual ExpressionType getType() const = 0;

	virtual bool isConstant() const = 0;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const = 0;

	virtual void printDebug(std::ostream * const out) const = 0;



	friend void print_debug(std::ostream * const out, ExpressionType const type);

	static ExpressionType get_promoted_type(ExpressionType const type1, ExpressionType const type2);

private:
	std::string _labels;
	SourcePosition _position;
};

class SourceExpressionACS
{
public:
	SourceExpressionACS();
	SourceExpressionACS(SourceExpressionACS const & expr);
	SourceExpressionACS(SourceExpressionACSBase * const expr);
	~SourceExpressionACS();

	SourcePosition const & getPosition() const;

	SourceExpressionACSBase::ExpressionType getType() const;

	bool isConstant() const;

	void makeObjects(std::vector<ObjectToken> * const objects) const;

	SourceExpressionACS & operator = (SourceExpressionACS const & expr);



	friend void print_debug(std::ostream * const out, SourceExpressionACS const & expression);

	static void make_expressions(SourceBlockC const & block, std::vector<SourceExpressionACS> * const expressions);

	static void make_objects(std::vector<SourceExpressionACS> const & expressions, std::vector<ObjectToken> * const objects);

	static SourceExpressionACS const nop;

private:
	SourceExpressionACSBase * _expr;



	static SourceExpressionACS make_expression(std::vector<SourceTokenC> const & tokens, SourceContext const & context);

	static SourceExpressionACS make_expression_binary_add(SourceExpressionACS const & exprL, SourceExpressionACS const & exprR);
};



#endif /* HPP_SourceExpressionACS_ */



