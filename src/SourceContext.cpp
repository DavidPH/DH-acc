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

/* SourceContext.cpp
**
** Defines the SourceContext methods.
*/

#include "SourceContext.hpp"

#include "ObjectExpression.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceVariable.hpp"
#include "VariableType.hpp"

#include <cstring>
#include <sstream>


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceContext::Pointer SourceContext::global_context;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceContext::SourceContext
//
SourceContext::SourceContext() : _allowLabel(true), _caseDefault(false), _countAuto(0), _countRegister(0), _labelCount(0), _limitAuto(0), _limitRegister(0), _parent(NULL), _returnType(VariableType::get_vt_void()), _type(CT_BLOCK), _inheritLocals(false)
{
	getVariableType_typedef("bool",     VariableType::get_vt_boolhard(), SourcePosition::builtin());
	getVariableType_typedef("softbool", VariableType::get_vt_boolsoft(), SourcePosition::builtin());
	getVariableType_typedef("char",     VariableType::get_vt_char(),     SourcePosition::builtin());
	getVariableType_typedef("int",      VariableType::get_vt_int(),      SourcePosition::builtin());
	getVariableType_typedef("__label",  VariableType::get_vt_label(),    SourcePosition::builtin());
	getVariableType_typedef("real",     VariableType::get_vt_real(),     SourcePosition::builtin());
	getVariableType_typedef("string",   VariableType::get_vt_string(),   SourcePosition::builtin());
	getVariableType_typedef("void",     VariableType::get_vt_void(),     SourcePosition::builtin());


	addVariable(SourceVariable::create_constant("false", VariableType::get_vt_boolhard(), ObjectExpression::create_value_int(0, SourcePosition::builtin()), SourcePosition::builtin()));
	addVariable(SourceVariable::create_constant("true",  VariableType::get_vt_boolhard(), ObjectExpression::create_value_int(1, SourcePosition::builtin()), SourcePosition::builtin()));
}

//
// SourceContext::SourceContext
//
SourceContext::SourceContext(SourceContext * parent, ContextType type) : _allowLabel(true), _caseDefault(false), _countAuto(0), _countRegister(0), _label(parent->makeLabelShort()), _labelCount(0), _limitAuto(0), _limitRegister(0), _parent(parent), _returnType(NULL), _type(type), _inheritLocals(type == CT_BLOCK || type == CT_LOOP || type == CT_SWITCH)
{
	if (_inheritLocals)
	{
		_limitAuto     = _parent->_limitAuto;
		_limitRegister = _parent->_limitRegister;
	}
}

//
// SourceContext::~SourceContext
//
SourceContext::~SourceContext()
{
}

//
// SourceContext::addCount
//
void SourceContext::addCount(int count, SourceVariable::StorageClass sc)
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
		_countAuto += count;
		addLimit(getCount(sc), sc);
		break;

	case SourceVariable::SC_REGISTER:
		_countRegister += count;
		addLimit(getCount(sc), sc);
		break;

	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
	case SourceVariable::SC_STATIC:
		break;
	}
}

//
// SourceContext::addLabelCase
//
std::string SourceContext::addLabelCase(bigsint value, SourcePosition const & position)
{
	if (_type == CT_SWITCH)
	{
		if (_cases.find(value) == _cases.end() || !_cases[value])
			_cases[value] = true;
		else
			throw SourceException("case redefined", position, "SourceContext");

		return getLabelCase(value, position);
	}

	if (_parent && _inheritLocals)
		return _parent->addLabelCase(value, position);

	throw SourceException("addLabelCase", position, "SourceContext");
}

//
// SourceContext::addLabelCaseDefault
//
std::string SourceContext::addLabelCaseDefault(SourcePosition const & position)
{
	if (_type == CT_SWITCH)
	{
		if (!_caseDefault)
			_caseDefault = true;
		else
			throw SourceException("casedefault redefined", position, "SourceContext");

		return getLabelCaseDefault(position);
	}

	if (_parent && _inheritLocals)
		return _parent->addLabelCaseDefault(position);

	throw SourceException("addLabelCaseDefault", position, "SourceContext");
}

//
// SourceContext::addLabelGoto
//
std::string SourceContext::addLabelGoto(SourceTokenC const & token)
{
	return getLabelGoto(token);
}

//
// SourceContext::addLimit
//
void SourceContext::addLimit(int limit, SourceVariable::StorageClass sc)
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
		if (limit > _limitAuto)
			_limitAuto = limit;

		if (_inheritLocals && _parent)
			_parent->addLimit(limit, sc);

		break;

	case SourceVariable::SC_REGISTER:
		if (limit > _limitRegister)
			_limitRegister = limit;

		if (_inheritLocals && _parent)
			_parent->addLimit(limit, sc);

		break;

	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
	case SourceVariable::SC_STATIC:
		break;
	}
}

//
// SourceContext::addVariable
//
void SourceContext::addVariable(SourceVariable * var)
{
	_vars.push_back(var);
	_varnames.push_back(var->getNameSource());

	SourceVariable::StorageClass sc(var->getClass());
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_STATIC:
		addCount(var->getType()->size(SourcePosition::none()), sc);
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		// Register arrays only require a single allocation regardless of size.
		addCount(1, sc);
		break;
	}
}

//
// SourceContext::create
//
SourceContext::Reference SourceContext::
create(SourceContext *parent, ContextType type)
{
   return Reference(new SourceContext(parent, type));
}

//
// SourceContext::getAllowLabel
//
bool SourceContext::getAllowLabel() const
{
	if (_parent && _inheritLocals)
		return _allowLabel && _parent->getAllowLabel();
	else
		return _allowLabel;
}

//
// SourceContext::getCases
//
std::vector<bigsint> SourceContext::getCases(SourcePosition const & position) const
{
	if (_type == CT_SWITCH)
	{
		std::vector<bigsint> cases(_cases.size());

		size_t i(0);
		for (std::map<bigsint, bool>::const_iterator it(_cases.begin()); it != _cases.end(); ++it, ++i)
		{
			if (!it->second)
				throw SourceException("case used but undefined", position, "SourceContext");

			cases[i] = it->first;
		}

		// No need for sort since std::map will have done that.
		return cases;
	}

	if (_parent && _inheritLocals)
		return _parent->getCases(position);

	throw SourceException("getCases", position, "SourceContext");
}

//
// SourceContext::getCount
//
int SourceContext::getCount(SourceVariable::StorageClass sc) const
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
		if (_inheritLocals && _parent)
			return _parent->getCount(sc) + _countAuto;
		else
			return _countAuto;

	case SourceVariable::SC_REGISTER:
		if (_inheritLocals && _parent)
			return _parent->getCount(sc) + _countRegister;
		else
			return _countRegister;

	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
	case SourceVariable::SC_STATIC:
		return 0;
	}

	throw SourceException("getCount", SourcePosition::none(), "SourceContext");
}

//
// SourceContext::getLabel
//
std::string SourceContext::getLabel() const
{
	if (_parent)
		return _parent->getLabel() + _label;
	else
		return ObjectExpression::get_filename() + _label;
}

//
// SourceContext::getLabelBreak
//
std::string SourceContext::getLabelBreak(SourcePosition const & position) const
{
	if (_type == CT_LOOP || _type == CT_SWITCH)
		return getLabel() + "_break";

	if (_parent && _inheritLocals)
		return _parent->getLabelBreak(position);

	throw SourceException("getLabelBreak", position, "SourceContext");
}

//
// SourceContext::getLabelCase
//
std::string SourceContext::getLabelCase(bigsint value, SourcePosition const & position)
{
	if (_type == CT_SWITCH)
	{
		if (_cases.find(value) == _cases.end())
			_cases[value] = false;

		std::ostringstream out;
		out << getLabel() << "_case" << value;
		return out.str();
	}

	if (_parent && _inheritLocals)
		return _parent->getLabelCase(value, position);

	throw SourceException("getLabelCase", position, "SourceContext");
}

//
// SourceContext::getLabelCaseDefault
//
std::string SourceContext::getLabelCaseDefault(SourcePosition const & position) const
{
	if (_type == CT_SWITCH)
		return getLabel() + "_casedefault";

	if (_parent && _inheritLocals)
		return _parent->getLabelCaseDefault(position);

	throw SourceException("getLabelCaseDefault", position, "SourceContext");
}

//
// SourceContext::getLabelContinue
//
std::string SourceContext::getLabelContinue(SourcePosition const & position) const
{
	if (_type == CT_LOOP)
		return getLabel() + "_continue";

	if (_parent && _inheritLocals)
		return _parent->getLabelContinue(position);

	throw SourceException("getLabelContinue", position, "SourceContext");
}

//
// SourceContext::getLabelGoto
//
std::string SourceContext::getLabelGoto(SourceTokenC const & token) const
{
	if (_parent && _inheritLocals)
		return _parent->getLabelGoto(token);

	return getLabel() + "_goto" + token.getData();
}

//
// SourceContext::getLimit
//
int SourceContext::getLimit(SourceVariable::StorageClass sc) const
{
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
		return _limitAuto;

	case SourceVariable::SC_REGISTER:
		return _limitRegister;

	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
	case SourceVariable::SC_STATIC:
		return 0;
	}

	throw SourceException("getCount", SourcePosition::none(), "SourceContext");
}

//
// SourceContext::getReturnType
//
VariableType const * SourceContext::getReturnType() const
{
	return _returnType ? _returnType : _parent->getReturnType();
}

//
// SourceContext::getTypeRoot
//
SourceContext::ContextType SourceContext::getTypeRoot() const
{
	switch (_type)
	{
	case CT_BLOCK:
	case CT_LOOP:
	case CT_SWITCH:
		if (_parent)
			return _parent->getTypeRoot();
		break;

	case CT_FUNCTION:
	case CT_SCRIPT:
		return _type;
	}
	return _type;
}

//
// SourceContext::getVariable
//
SourceVariable::Pointer SourceContext::getVariable(SourceTokenC const & token) const
{
	return getVariable(token.getData(), token.getPosition(), true);
}

//
// SourceContext::getVariable
//
SourceVariable::Pointer SourceContext::getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const
{
	for (size_t i(_vars.size()); i--;)
	{
		if (name == _varnames[i])
		{
			switch (_vars[i]->getClass())
			{
			case SourceVariable::SC_AUTO:
			case SourceVariable::SC_REGISTER:
				if (canLocal)
					return _vars[i];

				break;

			case SourceVariable::SC_CONSTANT:
			case SourceVariable::SC_REGISTER_GLOBAL:
			case SourceVariable::SC_REGISTER_MAP:
			case SourceVariable::SC_REGISTER_WORLD:
			case SourceVariable::SC_REGISTERARRAY_GLOBAL:
			case SourceVariable::SC_REGISTERARRAY_MAP:
			case SourceVariable::SC_REGISTERARRAY_WORLD:
			case SourceVariable::SC_STATIC:
				return _vars[i];
			}
		}
	}

	if (_parent) return _parent->getVariable(name, position, canLocal && _inheritLocals);

	throw SourceException("no such variable '" + name + "'", position, "SourceContext");
}

//
// SourceContext::hasLabelCaseDefault
//
bool SourceContext::hasLabelCaseDefault() const
{
	return _caseDefault;
}

//
// SourceContext::init
//
void SourceContext::init()
{
	global_context = new SourceContext;
}

//
// SourceContext::makeLabel
//
std::string SourceContext::makeLabel()
{
	return getLabel() + makeLabelShort();
}

//
// SourceContext::makeLabelShort
//
std::string SourceContext::makeLabelShort()
{
	std::ostringstream oss;

	oss << "block" << ++_labelCount;

	return oss.str();
}

//
// SourceContext::makeNameObject
//
std::string SourceContext::makeNameObject(NameType nameType, SourceVariable::StorageClass sc, VariableType const * type, std::string const & nameSource, SourcePosition const & position) const
{
	std::string nameObject;
	if (nameType == NT_LOCAL)
		nameObject += getLabel();
	nameObject += nameSource;

	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		// TODO: Need a separate list for register at some point eventually, but it's not that important.
		ObjectExpression::add_auto(nameObject, type->size(position), getCount(sc));
		break;

	case SourceVariable::SC_CONSTANT:
		throw SourceException("makeNameObject on SC_CONSTANT", position, "SourceContext");

	case SourceVariable::SC_REGISTER_GLOBAL:
		ObjectExpression::add_register_global(nameObject, type->size(position), nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTER_MAP:
		ObjectExpression::add_register_map(nameObject, type->size(position), nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTER_WORLD:
		ObjectExpression::add_register_world(nameObject, type->size(position), nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
		ObjectExpression::add_registerarray_global(nameObject, type->size(position), nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
		ObjectExpression::add_registerarray_map(nameObject, type->size(position), nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTERARRAY_WORLD:
		ObjectExpression::add_registerarray_world(nameObject, type->size(position), nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_STATIC:
		ObjectExpression::add_static(nameObject, type->size(position));
		break;
	}

	return nameObject;
}

//
// SourceContext::makeNameObject
//
std::string SourceContext::makeNameObject(NameType nameType, SourceVariable::StorageClass sc, VariableType const * type, std::string const & nameSource, bigsint address, SourcePosition const & position) const
{
	std::string nameObject;
	if (nameType == NT_LOCAL)
		nameObject += getLabel();
	nameObject += nameSource;

	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		// TODO: Need a separate list for register at some point eventually, but it's not that important.
		ObjectExpression::add_auto(nameObject, type->size(position), address);
		break;

	case SourceVariable::SC_CONSTANT:
		throw SourceException("makeNameObject on SC_CONSTANT", position, "SourceContext");

	case SourceVariable::SC_REGISTER_GLOBAL:
		ObjectExpression::add_register_global(nameObject, type->size(position), address, nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTER_MAP:
		ObjectExpression::add_register_map(nameObject, type->size(position), address, nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTER_WORLD:
		ObjectExpression::add_register_world(nameObject, type->size(position), address, nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
		ObjectExpression::add_registerarray_global(nameObject, type->size(position), address, nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
		ObjectExpression::add_registerarray_map(nameObject, type->size(position), address, nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_REGISTERARRAY_WORLD:
		ObjectExpression::add_registerarray_world(nameObject, type->size(position), address, nameType == NT_EXTERN, nameType != NT_LOCAL);
		break;

	case SourceVariable::SC_STATIC:
		ObjectExpression::add_static(nameObject, type->size(position), address);
		break;
	}

	return nameObject;
}

//
// SourceContext::setAllowLabel
//
void SourceContext::setAllowLabel(bool allow)
{
	_allowLabel = allow;
}

//
// SourceContext::setReturnType
//
void SourceContext::setReturnType(VariableType const * returnType)
{
	_returnType = returnType;
}

// EOF

