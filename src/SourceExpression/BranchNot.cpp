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

/* SourceExpression/BranchNot.cpp
**
** Defines the SourceExpression_BranchNot class and methods.
*/

#include "Unary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchNot
//
class SourceExpression_BranchNot : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchNot,
                                   SourceExpression_Unary);

public:
   SourceExpression_BranchNot(SRCEXP_EXPRUNA_ARGS);

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_not
//
SRCEXP_EXPRBRA_DEFN(u, not)
{
   return new SourceExpression_BranchNot(expr, context, position);
}

//
// SourceExpression_BranchNot::SourceExpression_BranchNot
//
SourceExpression_BranchNot::
SourceExpression_BranchNot(SRCEXP_EXPRUNA_PARM)
                           : Super(VariableType::get_vt_boolsoft(),
                                   SRCEXP_EXPRUNA_PASS)
{
}

void SourceExpression_BranchNot::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	// FIXME: Shuld be based on type.
	objects->addToken(OCODE_LOGICAL_NOT32I);

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}

// EOF

