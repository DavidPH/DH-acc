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

/* object_io.cpp
**
** Defines the read_object and write_object functions.
*/

#include "object_io.hpp"



bool override_object(int *, int const &)
{
   return false;
}
bool override_object(std::string *, std::string const &)
{
	return false;
}

void read_object(std::istream * in, bool * out)
{
	*out = !!in->get();
}
void read_object(std::istream * in, int * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));
}
void read_object(std::istream * in, long * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));
}
void read_object(std::istream * in, long double * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));
}
void read_object(std::istream * in, std::string * out)
{
	std::string::size_type size;
	read_object(in, &size);

	out->reserve(size);
	while (size--)
		*out += (char)in->get();
}
void read_object(std::istream * in, unsigned int * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));
}
void read_object(std::istream * in, unsigned long * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));
}

void read_object_raw(std::istream * in, char * out, size_t size)
{
	while (size--)
		*out++ = (char)in->get();
}

void write_object(std::ostream * out, bool const & in)
{
	out->put((char)in);
}
void write_object(std::ostream * out, int const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, long const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, long double const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, std::string const & in)
{
	std::string::size_type size(in.size());
	write_object(out, size);

	write_object_raw(out, in.c_str(), size);
}
void write_object(std::ostream * out, unsigned int const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, unsigned long const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}

void write_object_raw(std::ostream * out, char const * in, size_t size)
{
	while (size--)
		out->put(*in++);
}


