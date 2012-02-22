//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Object-level data.
//
//-----------------------------------------------------------------------------

#include "ObjectData.hpp"

#include "object_io.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

bool override_object(ObjectData_Auto * out, ObjectData_Auto const & in)
{
	if (out->name != in.name)
		return false;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectData_Function * out, ObjectData_Function const & in)
{
	if (out->name != in.name)
		return false;

	out->externDef = out->externDef && in.externDef;

	if (out->number == -1)
		out->number = in.number;

	return true;
}
bool override_object(ObjectData_Register * out, ObjectData_Register const & in)
{
	if (out->name != in.name)
		return false;

	out->externDef = out->externDef && in.externDef;
	out->externVis = out->externVis || in.externVis;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectData_RegisterArray * out, ObjectData_RegisterArray const & in)
{
	if (out->name != in.name)
		return false;

	out->externDef = out->externDef && in.externDef;
	out->externVis = out->externVis || in.externVis;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectData_Static * out, ObjectData_Static const & in)
{
	if (out->name != in.name)
		return false;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectData_String * out, ObjectData_String const & in)
{
	if (out->name != in.name)
		return false;

	return true;
}

void read_object(std::istream * in, ObjectData_Auto * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
}
void read_object(std::istream * in, ObjectData_Function * out)
{
	read_object(in, &out->label);
	read_object(in, &out->name);
	read_object(in, &out->argCount);
	read_object(in, &out->number);
	read_object(in, &out->retCount);
	read_object(in, &out->varCount);
	read_object(in, &out->externDef);
}
void read_object(std::istream * in, ObjectData_Register * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
	read_object(in, &out->externDef);
	read_object(in, &out->externVis);
}
void read_object(std::istream * in, ObjectData_RegisterArray * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
	read_object(in, &out->externDef);
	read_object(in, &out->externVis);
}
void read_object(std::istream * in, ObjectData_Script::ScriptType * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));

	if (*out > ObjectData_Script::ST_NONE)
		*out = ObjectData_Script::ST_NONE;
}
void read_object(std::istream * in, ObjectData_Static * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
}
void read_object(std::istream * in, ObjectData_String * out)
{
	read_object(in, &out->name);
	read_object(in, &out->string);
}

void write_object(std::ostream * out, ObjectData_Auto const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
}
void write_object(std::ostream * out, ObjectData_Function const & in)
{
	write_object(out, in.label);
	write_object(out, in.name);
	write_object(out, in.argCount);
	write_object(out, in.number);
	write_object(out, in.retCount);
	write_object(out, in.varCount);
	write_object(out, in.externDef);
}
void write_object(std::ostream * out, ObjectData_Register const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
	write_object(out, in.externDef);
	write_object(out, in.externVis);
}
void write_object(std::ostream * out, ObjectData_RegisterArray const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
	write_object(out, in.externDef);
	write_object(out, in.externVis);
}
void write_object(std::ostream * out, ObjectData_Script::ScriptType const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, ObjectData_Static const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
}
void write_object(std::ostream * out, ObjectData_String const & in)
{
	write_object(out, in.name);
	write_object(out, in.string);
}

// EOF

