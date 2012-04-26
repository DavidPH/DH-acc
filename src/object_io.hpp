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

#ifndef HPP_object_io_
#define HPP_object_io_

#include "bignum.hpp"

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

void read_object(std::istream *in, bool *out);
void read_object(std::istream *in, long double *out);
void read_object(std::istream *in, signed int *out);
void read_object(std::istream *in, signed long int *out);
void read_object(std::istream *in, signed long long int *out);
void read_object(std::istream *in, unsigned int *out);
void read_object(std::istream *in, unsigned long int *out);
void read_object(std::istream *in, unsigned long long int *out);
void read_object(std::istream *in, std::string *out);

bool    read_object_bit(std::istream *in);
bigreal read_object_flt(std::istream *in);
bigsint read_object_int(std::istream *in);
biguint read_object_uns(std::istream *in);

void write_object(std::ostream *out, bool const *in);
void write_object(std::ostream *out, long double const *in);
void write_object(std::ostream *out, signed int const *in);
void write_object(std::ostream *out, signed long int const *in);
void write_object(std::ostream *out, signed long long int const *in);
void write_object(std::ostream *out, unsigned int const *in);
void write_object(std::ostream *out, unsigned long int const *in);
void write_object(std::ostream *out, unsigned long long int const *in);
void write_object(std::ostream *out, std::string const *in);

void write_object_bit(std::ostream *out, bool    in);
void write_object_flt(std::ostream *out, bigreal in);
void write_object_int(std::ostream *out, bigsint in);
void write_object_uns(std::ostream *out, biguint in);

//
// read_object<std::map>
//
template<typename Tk, typename Tv>
void read_object(std::istream *in, std::map<Tk, Tv> *out)
{
   typename std::map<Tk, Tv>::iterator  iter;
   typename std::map<Tk, Tv>::size_type count;

   read_object(in, &count);

   while (count--)
   {
      std::pair<Tk, Tv> o;
      read_object(in, &o);

      if ((iter = out->find(o.first)) == out->end())
         out->insert(o);
      else
         override_object(&iter->second, &o.second);
   }
}

//
// read_object<std::pair>
//
template<typename Tk, typename Tv>
void read_object(std::istream *in, std::pair<Tk, Tv> *out)
{
   read_object(in, &out->first);
   read_object(in, &out->second);
}

//
// read_object<std::vector>
//
template<typename T>
void read_object(std::istream *in, std::vector<T> *out)
{
   typename std::vector<T>::iterator  iter, end;
   typename std::vector<T>::size_type size = out->size(), count;

   read_object(in, &count);

   out->resize(size + count);
   end = out->end();

   for (iter = out->begin() + size; iter != end; ++iter)
      read_object(in, &*iter);
}

//
// write_object<std::map>
//
template<typename Tk, typename Tv>
void write_object(std::ostream *out, std::map<Tk, Tv> const *in)
{
   typename std::map<Tk, Tv>::const_iterator iter, end = in->end();
   typename std::map<Tk, Tv>::size_type      size = in->size();

   write_object(out, &size);

   for (iter = in->begin(); iter != end; ++iter)
      write_object(out, &*iter);
}

//
// write_object<std::pair>
//
template<typename Tk, typename Tv>
void write_object(std::ostream *out, std::pair<Tk, Tv> const *in)
{
   write_object(out, &in->first);
   write_object(out, &in->second);
}

//
// write_object<std::vector>
//
template<typename T>
void write_object(std::ostream *out, std::vector<T> const *in)
{
   typename std::vector<T>::const_iterator iter, end = in->end();
   typename std::vector<T>::size_type      size = in->size();

   write_object(out, &size);

   for (iter = in->begin(); iter != end; ++iter)
      write_object(out, &*iter);
}

#endif//HPP_object_io_

