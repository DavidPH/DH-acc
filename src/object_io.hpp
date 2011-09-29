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

/* object_io.hpp
**
** Declares the read_object and write_object functions.
*/

#ifndef HPP_object_io_
#define HPP_object_io_

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>



template<typename Tk, typename Tv> bool override_object(std::map<Tk, Tv> * out, std::pair<Tk, Tv> const & in);
bool override_object(std::string * out, std::string const & in);
template<typename T> bool override_object(std::vector<T> * out, T const & in);

void read_object(std::istream * in, bool * out);
void read_object(std::istream * in, int * out);
void read_object(std::istream * in, long * out);
void read_object(std::istream * in, long double * out);
template<typename Tk, typename Tv> void read_object(std::istream * in, std::map<Tk, Tv> * out);
template<typename Tk, typename Tv> void read_object(std::istream * in, std::pair<Tk, Tv> * out);
void read_object(std::istream * in, std::string * out);
template<typename T> void read_object(std::istream * in, std::vector<T> * out);
void read_object(std::istream * in, unsigned int * out);
void read_object(std::istream * in, unsigned long * out);

void read_object_raw(std::istream * in, char * out, size_t size);

void write_object(std::ostream * out, bool const & in);
void write_object(std::ostream * out, int const & in);
void write_object(std::ostream * out, long const & in);
void write_object(std::ostream * out, long double const & in);
template<typename Tk, typename Tv> void write_object(std::ostream * out, std::map<Tk, Tv> const & in);
template<typename Tk, typename Tv> void write_object(std::ostream * out, std::pair<Tk, Tv> const & in);
void write_object(std::ostream * out, std::string const & in);
template<typename T> void write_object(std::ostream * out, std::vector<T> const & in);
void write_object(std::ostream * out, unsigned int const & in);
void write_object(std::ostream * out, unsigned long const & in);

void write_object_raw(std::ostream * out, char const * in, size_t size);



template<typename Tk, typename Tv> bool override_object(std::map<Tk, Tv> * out, std::pair<Tk, Tv> const & in)
{
	typename std::map<Tk, Tv>::iterator it(out->find(in.first));

	if (it == out->end())
	{
		out->insert(in);
		return false;
	}
	else
		return override_object(&it->second, in.second);
}
template<typename T> bool override_object(std::vector<T> * out, T const & in)
{
	for (typename std::vector<T>::iterator it(out->begin()); it != out->end(); ++it)
		if (override_object(&*it, in))
			return true;

	return false;
}

template<typename Tk, typename Tv> void read_object(std::istream * in, std::map<Tk, Tv> * out)
{
	typename std::map<Tk, Tv>::size_type size;
	read_object(in, &size);

	while (size--)
	{
		std::pair<Tk, Tv> o;
		read_object(in, &o);

		override_object(out, o);
	}

}
template<typename Tk, typename Tv> void read_object(std::istream * in, std::pair<Tk, Tv> * out)
{
	read_object(in, &out->first);
	read_object(in, &out->second);
}
template<typename T> void read_object(std::istream * in, std::vector<T> * out)
{
	typename std::vector<T>::size_type size;
	read_object(in, &size);

	while (size--)
	{
		T o;
		read_object(in, &o);

		if (!override_object(out, o))
			out->push_back(o);
	}
}

template<typename Tk, typename Tv> void write_object(std::ostream * out, std::map<Tk, Tv> const & in)
{
	write_object(out, in.size());

	for (typename std::map<Tk, Tv>::const_iterator it(in.begin()); it != in.end(); ++it)
		write_object(out, *it);
}
template<typename Tk, typename Tv> void write_object(std::ostream * out, std::pair<Tk, Tv> const & in)
{
	write_object(out, in.first);
	write_object(out, in.second);
}
template<typename T> void write_object(std::ostream * out, std::vector<T> const & in)
{
	write_object(out, in.size());

	for (typename std::vector<T>::const_iterator it(in.begin()); it != in.end(); ++it)
		write_object(out, *it);
}



#endif /* HPP_object_io_ */


