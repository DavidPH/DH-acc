//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
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
// Object-format I/O.
//
//-----------------------------------------------------------------------------

#include "object_io.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// read_object<bool>
//
void read_object(std::istream *in, bool *out)
{
   *out = read_object_bit(in);
}

//
// read_object<long double>
//
void read_object(std::istream *in, long double *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));
}

//
// read_object<signed int>
//
void read_object(std::istream *in, signed int *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));
}

//
// read_object<signed long int>
//
void read_object(std::istream *in, long *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));
}

//
// read_object<signed long long int>
//
void read_object(std::istream *in, signed long long int *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));
}

//
// read_object<unsigned int>
//
void read_object(std::istream *in, unsigned int *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));
}

//
// read_object<unsigned long int>
//
void read_object(std::istream *in, unsigned long int *out)
{
   read_object_raw(in, (char *)out, sizeof(*out));
}

//
// read_object<std::string>
//
void read_object(std::istream *in, std::string *out)
{
   std::string::iterator  iter, end;
   std::string::size_type size;

   read_object(in, &size);

   out->resize(size);
   end = out->end();

   for (iter = out->begin(); iter != end; ++iter)
      *iter = static_cast<char>(in->get());
}

//
// read_object_bit
//
bool read_object_bit(std::istream *in)
{
   return static_cast<bool>(in->get());
}

//
// read_object_raw
//
void read_object_raw(std::istream *in, char *out, size_t size)
{
   while (size--) *out++ = static_cast<char>(in->get());
}

//
// write_object<bool>
//
void write_object(std::ostream *out, bool const *in)
{
   write_object_bit(out, *in);
}

//
// write_object<long double>
//
void write_object(std::ostream *out, long double const *in)
{
   write_object_raw(out, (char const *)in, sizeof(*in));
}

//
// write_object<signed int>
//
void write_object(std::ostream *out, signed int const *in)
{
   write_object_raw(out, (char const *)in, sizeof(*in));
}

//
// write_object<signed long int>
//
void write_object(std::ostream *out, signed long int const *in)
{
   write_object_raw(out, (char const *)in, sizeof(*in));
}

//
// write_object<signed long long int>
//
void write_object(std::ostream *out, signed long long int const *in)
{
   write_object_raw(out, (char const *)in, sizeof(*in));
}

//
// write_object<unsigned int>
//
void write_object(std::ostream *out, unsigned int const *in)
{
   write_object_raw(out, (char const *)in, sizeof(*in));
}

//
// write_object<unsigned long int>
//
void write_object(std::ostream *out, unsigned long int const *in)
{
   write_object_raw(out, (char const *)in, sizeof(*in));
}

//
// write_object<std::string>
//
void write_object(std::ostream *out, std::string const *in)
{
   std::string::size_type size = in->size();

   write_object(out, &size);

   write_object_raw(out, in->c_str(), size);
}

//
// write_object_bit
//
void write_object_bit(std::ostream *out, bool in)
{
   out->put(static_cast<char>(in));
}

//
// write_object_raw
//
void write_object_raw(std::ostream *out, char const *in, size_t size)
{
   while (size--) out->put(*in++);
}

// EOF

