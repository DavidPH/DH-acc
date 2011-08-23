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

/* ObjectData.hpp
**
** Defines the ObjectCode enum and related functions.
*/

#ifndef HPP_ObjectData_
#define HPP_ObjectData_

#include "bignum.hpp"

#include <istream>
#include <ostream>
#include <string>

class SourceTokenC;



struct ObjectData_Function
{
	std::string label;
	std::string library;
	std::string name;
	bigsint argCount;
	bigsint number;
	bigsint retCount;
	bigsint varCount;
	bool external;
};

struct ObjectData_Register
{
	std::string name;
	bigsint number;
	bigsint size;
};

struct ObjectData_RegisterArray
{
	std::string name;
	bigsint number;
	bigsint size;
};

struct ObjectData_Script
{
	enum ScriptFlag
	{
		SF_NET        = 1,
		SF_CLIENTSIDE = 2
	};

	enum ScriptType
	{
		ST_CLOSED     =  0,
		ST_OPEN       =  1,
		ST_RESPAWN    =  2,
		ST_DEATH      =  3,
		ST_ENTER      =  4,
		ST_LIGHTNING  = 12,
		ST_UNLOADING  = 13,
		ST_DISCONNECT = 14,
		ST_RETURN     = 15,

		ST_NONE
	};

	std::string label;
	std::string name;
	ScriptType stype;
	bigsint argCount;
	bigsint flags;
	bigsint number;
	bigsint varCount;
};

struct ObjectData_Static
{
	std::string name;
	bigsint number;
	bigsint size;
};

struct ObjectData_String
{
	std::string name;
	std::string string;
	bigsint offset;
};



extern ObjectData_Script::ScriptFlag odata_get_ScriptFlag(SourceTokenC const & token);
extern ObjectData_Script::ScriptType odata_get_ScriptType(SourceTokenC const & token);

extern bool override_object(ObjectData_Function      * out, ObjectData_Function      const & in);
extern bool override_object(ObjectData_Register      * out, ObjectData_Register      const & in);
extern bool override_object(ObjectData_RegisterArray * out, ObjectData_RegisterArray const & in);
extern bool override_object(ObjectData_Script        * out, ObjectData_Script        const & in);
extern bool override_object(ObjectData_Static        * out, ObjectData_Static        const & in);
extern bool override_object(ObjectData_String        * out, ObjectData_String        const & in);

extern void read_object(std::istream * in, ObjectData_Function           * out);
extern void read_object(std::istream * in, ObjectData_Register           * out);
extern void read_object(std::istream * in, ObjectData_RegisterArray      * out);
extern void read_object(std::istream * in, ObjectData_Script             * out);
extern void read_object(std::istream * in, ObjectData_Script::ScriptType * out);
extern void read_object(std::istream * in, ObjectData_Static             * out);
extern void read_object(std::istream * in, ObjectData_String             * out);

extern void write_object(std::ostream * out, ObjectData_Function           const & in);
extern void write_object(std::ostream * out, ObjectData_Register           const & in);
extern void write_object(std::ostream * out, ObjectData_RegisterArray      const & in);
extern void write_object(std::ostream * out, ObjectData_Script             const & in);
extern void write_object(std::ostream * out, ObjectData_Script::ScriptType const & in);
extern void write_object(std::ostream * out, ObjectData_Static             const & in);
extern void write_object(std::ostream * out, ObjectData_String             const & in);



#endif /* HPP_ObjectData_ */



