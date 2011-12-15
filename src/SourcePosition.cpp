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

/* SourcePosition.cpp
**
** SourcePosition methods.
*/

#include "SourcePosition.hpp"

#include "object_io.hpp"



SourcePosition::SourcePosition() : filename(), line(0), column(0)
{

}
SourcePosition::SourcePosition(std::string const & filename_, long line_, long column_) : filename(filename_), line(line_), column(column_)
{

}

SourcePosition const &SourcePosition::builtin()
{
   static SourcePosition const position("__builtin__", 0, 0);
   return position;
}

SourcePosition const &SourcePosition::none()
{
   static SourcePosition const position("(none)", 0, 0);
   return position;
}



std::ostream & operator << (std::ostream & out, SourcePosition const & in)
{
	return out << in.filename << ':' << in.line << ':' << in.column;
}

void print_debug(std::ostream * out, SourcePosition const & in)
{
	*out << in;
}
void read_object(std::istream * in, SourcePosition * out)
{
	read_object(in, &out->filename);
	read_object(in, &out->line);
	read_object(in, &out->column);
}
void write_object(std::ostream * out, SourcePosition const & in)
{
	write_object(out, in.filename);
	write_object(out, in.line);
	write_object(out, in.column);
}


