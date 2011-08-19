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



SourceContext SourceContext::global_context;



SourceContext::SourceContext() : _allowLabel(true), _caseDefault(false), _countAuto(0), _countRegister(0), _labelCount(0), _limitAuto(0), _limitRegister(0), _parent(NULL), _type(CT_BLOCK), _inheritLocals(false)
{
	_types.resize(VariableType::VT_VOID+1);

	_types[VariableType::VT_VOID] = new VariableType;
	_types[VariableType::VT_VOID]->vt = VariableType::VT_VOID;
	_types[VariableType::VT_VOID]->complete = true;
	_types[VariableType::VT_VOID]->callType = _types[VariableType::VT_VOID];
	_types[VariableType::VT_VOID]->refType  = _types[VariableType::VT_VOID];

	for (VariableType::Type vt((VariableType::Type)0); vt < VariableType::VT_VOID; ++vt)
	{
		_types[vt] = new VariableType;
		_types[vt]->vt = vt;
		_types[vt]->complete = true;
		_types[vt]->callType = _types[VariableType::VT_VOID];
		_types[vt]->refType  = _types[VariableType::VT_VOID];
	}

	_types[VariableType::VT_STRING]->refType = _types[VariableType::VT_CHAR];

	_typenames.resize(VariableType::VT_VOID+1);
	_typenames[VariableType::VT_BOOLHARD] = "bool";
	_typenames[VariableType::VT_BOOLSOFT] = "softbool";
	_typenames[VariableType::VT_CHAR]     = "char";
	_typenames[VariableType::VT_INT]      = "int";
	_typenames[VariableType::VT_REAL]     = "real";
	_typenames[VariableType::VT_STRING]   = "string";
	_typenames[VariableType::VT_VOID]     = "void";

	_returnType = _types[VariableType::VT_VOID];
}
SourceContext::SourceContext(SourceContext * parent, ContextType type) : _allowLabel(true), _caseDefault(false), _countAuto(0), _countRegister(0), _label(parent->makeLabelShort()), _labelCount(0), _limitAuto(0), _limitRegister(0), _parent(parent), _returnType(NULL), _type(type), _inheritLocals(type == CT_BLOCK || type == CT_LOOP || type == CT_SWITCH)
{

}
SourceContext::~SourceContext()
{

}

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
std::string SourceContext::addLabelGoto(SourceTokenC const & token)
{
	return getLabelGoto(token);
}

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

void SourceContext::addVariable(SourceVariable const & var)
{
	_vars.push_back(var);
	_varnames.push_back(var.getNameSource());

	SourceVariable::StorageClass sc(var.getClass());
	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_CONSTANT:
	case SourceVariable::SC_REGISTER:
	case SourceVariable::SC_REGISTER_GLOBAL:
	case SourceVariable::SC_REGISTER_MAP:
	case SourceVariable::SC_REGISTER_WORLD:
	case SourceVariable::SC_STATIC:
		addCount(var.getType()->size(), sc);
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
	case SourceVariable::SC_REGISTERARRAY_WORLD:
		// Register arrays only require a single allocation regardless of size.
		addCount(1, sc);
		break;
	}
}

bool SourceContext::getAllowLabel() const
{
	if (_parent && _inheritLocals)
		return _allowLabel && _parent->getAllowLabel();
	else
		return _allowLabel;
}

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

	throw SourceException("getCount", SourcePosition::none, "SourceContext");
}

std::string SourceContext::getLabel() const
{
	if (_parent)
		return _parent->getLabel() + _label;
	else
		return _label;
}
std::string SourceContext::getLabelBreak(SourcePosition const & position) const
{
	if (_type == CT_LOOP || _type == CT_SWITCH)
		return getLabel() + "_break";

	if (_parent && _inheritLocals)
		return _parent->getLabelBreak(position);

	throw SourceException("getLabelBreak", position, "SourceContext");
}
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
std::string SourceContext::getLabelCaseDefault(SourcePosition const & position) const
{
	if (_type == CT_SWITCH)
		return getLabel() + "_casedefault";

	if (_parent && _inheritLocals)
		return _parent->getLabelCaseDefault(position);

	throw SourceException("getLabelCaseDefault", position, "SourceContext");
}
std::string SourceContext::getLabelContinue(SourcePosition const & position) const
{
	if (_type == CT_LOOP)
		return getLabel() + "_continue";

	if (_parent && _inheritLocals)
		return _parent->getLabelContinue(position);

	throw SourceException("getLabelContinue", position, "SourceContext");
}
std::string SourceContext::getLabelGoto(SourceTokenC const & token) const
{
	if (_parent && _inheritLocals)
		return _parent->getLabelGoto(token);

	return getLabel() + "_goto" + token.getData();
}

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

	throw SourceException("getCount", SourcePosition::none, "SourceContext");
}

VariableType const * SourceContext::getReturnType() const
{
	return _returnType ? _returnType : _parent->getReturnType();
}

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

SourceVariable const & SourceContext::getVariable(SourceTokenC const & token) const
{
	return getVariable(token.getData(), token.getPosition(), true);
}
SourceVariable const & SourceContext::getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const
{
	for (size_t i(_vars.size()); i--;)
	{
		if (name == _varnames[i])
		{
			switch (_vars[i].getClass())
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

bool SourceContext::hasLabelCaseDefault() const
{
	return _caseDefault;
}

std::string SourceContext::makeLabel()
{
	return getLabel() + makeLabelShort();
}
std::string SourceContext::makeLabelShort()
{
	std::ostringstream oss;

	oss << "block" << ++_labelCount;

	return oss.str();
}

std::string SourceContext::makeNameObject(SourceVariable::StorageClass sc, VariableType const * type, std::string const & nameSource, SourcePosition const & position) const
{
	std::string nameObject(getLabel() + nameSource);

	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		ObjectExpression::add_symbol(nameObject, ObjectExpression::create_value_int(getCount(sc), position));
		break;

	case SourceVariable::SC_CONSTANT:
		throw SourceException("makeNameObject on SC_CONSTANT", position, "SourceContext");

	case SourceVariable::SC_REGISTER_GLOBAL:
		ObjectExpression::add_register_global(nameObject, type->size());
		break;

	case SourceVariable::SC_REGISTER_MAP:
		ObjectExpression::add_register_map(nameObject, type->size());
		break;

	case SourceVariable::SC_REGISTER_WORLD:
		ObjectExpression::add_register_world(nameObject, type->size());
		break;

	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
		ObjectExpression::add_registerarray_global(nameObject, type->size());
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
		ObjectExpression::add_registerarray_map(nameObject, type->size());
		break;

	case SourceVariable::SC_REGISTERARRAY_WORLD:
		ObjectExpression::add_registerarray_world(nameObject, type->size());
		break;

	case SourceVariable::SC_STATIC:
		ObjectExpression::add_static(nameObject, type->size());
		break;
	}

	return nameObject;
}
std::string SourceContext::makeNameObject(SourceVariable::StorageClass sc, VariableType const * type, std::string const & nameSource, bigsint address, SourcePosition const & position) const
{
	std::string nameObject(getLabel() + nameSource);

	switch (sc)
	{
	case SourceVariable::SC_AUTO:
	case SourceVariable::SC_REGISTER:
		ObjectExpression::add_symbol(nameObject, ObjectExpression::create_value_int(getCount(sc), position));
		break;

	case SourceVariable::SC_CONSTANT:
		throw SourceException("makeNameObject on SC_CONSTANT", position, "SourceContext");

	case SourceVariable::SC_REGISTER_GLOBAL:
		ObjectExpression::add_register_global(nameObject, type->size(), address);
		break;

	case SourceVariable::SC_REGISTER_MAP:
		ObjectExpression::add_register_map(nameObject, type->size(), address);
		break;

	case SourceVariable::SC_REGISTER_WORLD:
		ObjectExpression::add_register_world(nameObject, type->size(), address);
		break;

	case SourceVariable::SC_REGISTERARRAY_GLOBAL:
		ObjectExpression::add_registerarray_global(nameObject, type->size(), address);
		break;

	case SourceVariable::SC_REGISTERARRAY_MAP:
		ObjectExpression::add_registerarray_map(nameObject, type->size(), address);
		break;

	case SourceVariable::SC_REGISTERARRAY_WORLD:
		ObjectExpression::add_registerarray_world(nameObject, type->size(), address);
		break;

	case SourceVariable::SC_STATIC:
		ObjectExpression::add_static(nameObject, type->size(), address);
		break;
	}

	return nameObject;
}

void SourceContext::setAllowLabel(bool allow)
{
	_allowLabel = allow;
}

void SourceContext::setReturnType(VariableType const * returnType)
{
	_returnType = returnType;
}



