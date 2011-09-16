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

/* option.cpp
**
** Defines the option-handling functions.
*/

#include "option.hpp"

#include <map>
#include <set>



std::string const option::no_arg("\0\0\0NO\0\0_\0\0ARG\0\0\0", 16);

std::string option::program;
std::string option::usage;
std::string option::version;

typedef std::map<std::string, option *> _option_map_type;
static inline _option_map_type & _option_map()
{
	static _option_map_type * option_map(new _option_map_type);

	return *option_map;
}
typedef std::set<option *, bool (*)(option *, option *)> _option_set_type;
static inline _option_set_type & _option_set()
{
	static _option_set_type * option_set(new _option_set_type(option::less));

	return *option_set;
}



option::exception::exception(std::string const & name, std::string const & description) : _what("(--" + name + "): " + description)
{

}
option::exception::exception(std::string const & name, std::string const & arg, std::string const & description) : _what("(--" + name + "=" + arg + "): " + description)
{

}
option::exception::~exception() throw()
{

}

char const * option::exception::what() const throw()
{
	return _what.c_str();
}



option::option(std::string const & name, std::string const & group, std::string const & description) : description(description), group(group), name(name)
{
	_option_map()[name] = this;
	_option_set().insert(this);
}
option::~option()
{
	_option_map().erase(name);
	_option_set().erase(this);
}

void option::assert_arg(std::string const & name, std::string const & arg)
{
	if (arg == no_arg)
		throw exception(name, "missing argument");
}

bool option::less(option * l, option * r)
{
	if (l->group < r->group) return true;
	if (l->group > r->group) return false;

	return l->name < r->name;
}

void option::print_help(std::ostream * out)
{
	print_help(out, 80);
}
void option::print_help(std::ostream * out, int width)
{
	if (!program.empty())
	{
		if (version.empty())
			*out << program << std::endl;
		else
			*out << program << " " << version << std::endl;

		if (!usage.empty())
			*out << "usage: " << program << " " << usage << std::endl;
	}

	typedef _option_set_type::const_iterator optIt_t;

	_option_set_type const & optSet(_option_set());
	optIt_t optBegin(optSet.begin());
	optIt_t optEnd(optSet.end());

	std::string group;
	int padlen;

	for (optIt_t optIt(optBegin); optIt != optEnd; ++optIt)
	{
		option * opt(*optIt);

		if (opt->group != group)
		{
			if (!group.empty()) *out << std::endl;

			*out << (group = opt->group) << ':' << std::endl;

			padlen = 0;
			for (optIt_t grpIt(optIt); grpIt != optEnd; ++grpIt)
			{
				option * grp(*grpIt);

				if (grp->group != group) break;

				int len((int)grp->name.size());

				if (len > padlen) padlen = len;
			}

			// Normal prefix+suffix length.
			padlen += 6;
		}

		opt->printHelp(out, width, padlen);
	}
}

void option::printHelp(std::ostream * out, int width, int padlen)
{
	*out << "  --" << name;
	int len(4 + (int)name.size());
	while (len++ < padlen) *out << ' '; --len;

	for (std::string::iterator it(description.begin()); it != description.end(); ++it)
	{
		if (++len > width)
		{
			*out << std::endl;
			len = 0;
			while (len++ < padlen) *out << ' ';
		}

		*out << *it;
	}
	*out << std::endl;
}

void option::process(int argc, char const * const * argv)
{
	std::vector<std::string> args;
	args.reserve(argc);

	for (int i(0); i < argc; ++i)
		args.push_back(argv[i]);

	process(args);
}
bool option::process(std::string const & name, std::string const & arg)
{
	char const * name_c(name.c_str());

	if (name_c[0] != '-' || name_c[1] != '-')
	{
		args_handler(name, name, true, &args_vector);
		return false;
	}

	bool arg_used(true);
	char const * arg_c(arg.c_str());

	name_c += 2;

	if (arg_c[0] == '-' && arg_c[1] == '-')
		arg_c = NULL;

	bool barg(true);

	if (name_c[0] == 'n' && name_c[1] == 'o' && name_c[2] == '-')
	{
		barg = false;
		name_c += 3;
	}

	arg_used = arg_used && arg_c;
	std::string arg_s(arg_c ? (std::string)arg_c : no_arg);

	std::string name_s(name_c);

	_option_map_type::iterator optIt(_option_map().find(name_s));

	if (optIt != _option_map().end())
	{
		return optIt->second->handle(name_s, arg_c, barg) && arg_used;
	}

	throw exception(name_c, "unknown name");
}
void option::process(std::vector<std::string> const & args)
{
	for (std::vector<std::string>::size_type argi(0), argn(1), argc(args.size()); argi < argc; ++argi)
	{
		argn = argi + 1;

		argi += process(args[argi], (argn < argc) ? args[argn] : no_arg);
	}
}



template<> float option_auto<float>::parse(std::string const & name, std::string const & arg)
{
	assert_arg(name, arg);

	float f(0);

	char const * s(arg.c_str());

	for (; *s && *s != '.'; ++s)
	{
		if (!isdigit(*s))
			throw exception(name, arg, "invalid float");

		f *= 10;
		f += *s - '0';
	}

	if (*s == '.')
	{
		while (*++s);

		float fFrac(0);

		while (*--s != '.')
		{
			if (!isdigit(*s))
				throw exception(name, arg, "invalid float");

			fFrac += *s - '0';
			fFrac /= 10;
		}
	}

	return f;
}

template<> int option_auto<int>::parse(std::string const & name, std::string const & arg)
{
	assert_arg(name, arg);

	int i(0);

	for (char const * s(arg.c_str()); *s; ++s)
	{
		if (!isdigit(*s))
			throw exception(name, arg, "invalid int");

		i *= 10;
		i += *s - '0';
	}

	return i;
}

template<> bool option_auto<bool>::handler_default(std::string const & name, std::string const & arg, bool barg, bool * data)
{
	*data = barg;

	return false;
}
template<> bool option_auto<float>::handler_default(std::string const & name, std::string const & arg, bool barg, float * data)
{
	*data = parse(name, arg);

	return true;
}
template<> bool option_auto<int>::handler_default(std::string const & name, std::string const & arg, bool barg, int * data)
{
	*data = parse(name, arg);

	return true;
}
template<> bool option_auto<std::string>::handler_default(std::string const & name, std::string const & arg, bool barg, std::string * data)
{
	assert_arg(name, arg);

	*data = arg;

	return true;
}
template<> bool option_auto<std::vector<std::string> >::handler_default(std::string const & name, std::string const & arg, bool barg, std::vector<std::string> * data)
{
	assert_arg(name, arg);

	data->push_back(arg);

	return true;
}



// Must be after template specializations.
std::vector<std::string> option::args_vector;
option_auto<std::vector<std::string> >::handler_t option::args_handler(option_auto<std::vector<std::string> >::handler_default);


