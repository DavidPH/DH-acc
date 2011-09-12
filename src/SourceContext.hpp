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

#include "bignum.hpp"
#include "SourceVariable.hpp"

#include <map>
#include <string>
#include <vector>

class SourceTokenC;
class VariableType;



class SourceContext
{
public:
	enum ContextType
	{
		CT_BLOCK,
		CT_FUNCTION,
		CT_LOOP,
		CT_SCRIPT,
		CT_SWITCH
	};



	SourceContext(SourceContext * parent, ContextType type);
	~SourceContext();

	std::string addLabelCase(bigsint value, SourcePosition const & position);
	std::string addLabelCaseDefault(SourcePosition const & position);
	std::string addLabelGoto(SourceTokenC const & token);

	void addVariable(SourceVariable * var);

	bool getAllowLabel() const;

	std::vector<bigsint> getCases(SourcePosition const & position) const;

	std::string getLabel() const;
	std::string getLabelBreak(SourcePosition const & position) const;
	std::string getLabelCase(bigsint value, SourcePosition const & position);
	std::string getLabelCaseDefault(SourcePosition const & position) const;
	std::string getLabelContinue(SourcePosition const & position) const;
	std::string getLabelGoto(SourceTokenC const & token) const;

	int getLimit(SourceVariable::StorageClass sc) const;

	VariableType const * getReturnType() const;

	ContextType getType() const;
	ContextType getTypeRoot() const;

	CounterPointer<SourceVariable> getVariable(SourceTokenC const & token) const;

	VariableType const * getVariableType(SourceTokenC const & token);
	VariableType const * getVariableType_enum(std::string const & name, bool block, SourcePosition const & position);
	VariableType const * getVariableType_struct(std::string const & name, SourcePosition const & position);
	VariableType const * getVariableType_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types, SourcePosition const & position);
	VariableType const * getVariableType_typedef(SourceTokenC const & token, VariableType const * type);
	VariableType const * getVariableType_typedef(std::string const & name, VariableType const * type, SourcePosition const & position);
	VariableType const * getVariableType_union(std::string const & name, SourcePosition const & position);
	VariableType const * getVariableType_union(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types, SourcePosition const & position);
	VariableType const * getVariableTypeNull(std::string const & name);

	bool hasLabelCaseDefault() const;

	std::string makeLabel();

	std::string makeNameObject(SourceVariable::StorageClass sc, VariableType const * type, std::string const & nameSource, SourcePosition const & position) const;
	std::string makeNameObject(SourceVariable::StorageClass sc, VariableType const * type, std::string const & nameSource, bigsint address, SourcePosition const & position) const;

	void setAllowLabel(bool allow);

	void setReturnType(VariableType const * returnType);



	static SourceContext global_context;

private:
	SourceContext();

	void addCount(int count, SourceVariable::StorageClass sc);
	void addLimit(int limit, SourceVariable::StorageClass sc);

	int getCount(SourceVariable::StorageClass sc) const;

	CounterPointer<SourceVariable> getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const;

	VariableType * getVariableType_enum(std::string const & name);
	VariableType * getVariableType_struct(std::string const & name);
	VariableType * getVariableType_union(std::string const & name);

	std::string makeLabelShort();

	bool _allowLabel;

	std::map<bigsint, bool> _cases;
	bool _caseDefault;

	int _countAuto;
	int _countRegister;

	std::string _label;
	int _labelCount;

	int _limitAuto;
	int _limitRegister;

	SourceContext * _parent;

	VariableType const * _returnType;

	ContextType _type;

	std::vector<VariableType const *> _types;
	std::vector<std::string> _typenames;

	std::vector<VariableType *> _enums;
	std::vector<std::string> _enumnames;

	std::vector<VariableType *> _structs;
	std::vector<std::string> _structnames;

	std::vector<VariableType *> _unions;
	std::vector<std::string> _unionnames;

	std::vector<CounterPointer<SourceVariable> > _vars;
	std::vector<std::string> _varnames;

	unsigned _inheritLocals : 1;
};



#endif /* HPP_SourceContext_ */


