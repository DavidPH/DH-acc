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

namespace option
{



union option_u
{
	option_b * b;
	option_f * f;
	option_i * i;
	option_s * s;

	option_bv * bv;
	option_fv * fv;
	option_iv * iv;
	option_sv * sv;
};

union option_handler_u
{
	option_handler_b b;
	option_handler_f f;
	option_handler_i i;
	option_handler_s s;

	option_handler_bv bv;
	option_handler_fv fv;
	option_handler_iv iv;
	option_handler_sv sv;
};

struct option
{
	option();

	option(option_b * data, option_handler_b handler, char const * desc, char const * name);
	option(option_f * data, option_handler_f handler, char const * desc, char const * name);
	option(option_i * data, option_handler_i handler, char const * desc, char const * name);
	option(option_s * data, option_handler_s handler, char const * desc, char const * name);

	option(option_bv * data, option_handler_bv handler, char const * desc, char const * name);
	option(option_fv * data, option_handler_fv handler, char const * desc, char const * name);
	option(option_iv * data, option_handler_iv handler, char const * desc, char const * name);
	option(option_sv * data, option_handler_sv handler, char const * desc, char const * name);

	enum option_t
	{
		OPT_B,
		OPT_F,
		OPT_I,
		OPT_S,

		OPT_BV,
		OPT_FV,
		OPT_IV,
		OPT_SV
	};

	option_t type;
	option_u data;
	option_handler_u handler;

	std::string desc;
	std::string name;
};



static std::map<std::string, std::map<std::string, option> > option_map;

static std::string option_name;
static std::string option_usage;
static std::string option_version;

option_sv         option_args;
option_handler_sv option_args_handler(option_handler_default_sv);



option::option()
{

}

option::option(option_b * data, option_handler_b handler, char const * desc, char const * name) : type(OPT_B), desc(desc), name(name)
{
	this->data.b    = data;
	this->handler.b = handler;
}
option::option(option_f * data, option_handler_f handler, char const * desc, char const * name) : type(OPT_F), desc(desc), name(name)
{
	this->data.f    = data;
	this->handler.f = handler;
}
option::option(option_i * data, option_handler_i handler, char const * desc, char const * name) : type(OPT_I), desc(desc), name(name)
{
	this->data.i    = data;
	this->handler.i = handler;
}
option::option(option_s * data, option_handler_s handler, char const * desc, char const * name) : type(OPT_S), desc(desc), name(name)
{
	this->data.s    = data;
	this->handler.s = handler;
}

option::option(option_bv * data, option_handler_bv handler, char const * desc, char const * name) : type(OPT_BV), desc(desc), name(name)
{
	this->data.bv    = data;
	this->handler.bv = handler;
}
option::option(option_fv * data, option_handler_fv handler, char const * desc, char const * name) : type(OPT_FV), desc(desc), name(name)
{
	this->data.fv    = data;
	this->handler.fv = handler;
}
option::option(option_iv * data, option_handler_iv handler, char const * desc, char const * name) : type(OPT_IV), desc(desc), name(name)
{
	this->data.iv    = data;
	this->handler.iv = handler;
}
option::option(option_sv * data, option_handler_sv handler, char const * desc, char const * name) : type(OPT_SV), desc(desc), name(name)
{
	this->data.sv    = data;
	this->handler.sv = handler;
}



option_exception::option_exception(char const * name, char const * arg, char const * description) : _what(std::string("(--") + name + "=" + arg + "): " + description)
{

}
option_exception::option_exception(char const * name, char const * description) : _what(std::string("(--") + name + "): " + description)
{

}
option_exception::~option_exception() throw()
{

}

char const * option_exception::what()
{
	return _what.c_str();
}



void option_add(char const * name, char const * group, char const * description, option_b * data, option_handler_b handler)
{
	option_map[group][name] = option(data, handler, description, name);
}
void option_add(char const * name, char const * group, char const * description, option_f * data, option_handler_f handler)
{
	option_map[group][name] = option(data, handler, description, name);
}
void option_add(char const * name, char const * group, char const * description, option_i * data, option_handler_i handler)
{
	option_map[group][name] = option(data, handler, description, name);
}
void option_add(char const * name, char const * group, char const * description, option_s * data, option_handler_s handler)
{
	option_map[group][name] = option(data, handler, description, name);
}

void option_add(char const * name, char const * group, char const * description, option_bv * data, option_handler_bv handler)
{
	option_map[group][name] = option(data, handler, description, name);
}
void option_add(char const * name, char const * group, char const * description, option_fv * data, option_handler_fv handler)
{
	option_map[group][name] = option(data, handler, description, name);
}
void option_add(char const * name, char const * group, char const * description, option_iv * data, option_handler_iv handler)
{
	option_map[group][name] = option(data, handler, description, name);
}
void option_add(char const * name, char const * group, char const * description, option_sv * data, option_handler_sv handler)
{
	option_map[group][name] = option(data, handler, description, name);
}

void option_assert_arg(char const * name, char const * arg)
{
	if (!arg) throw option_exception(name, "no arg");
}

bool option_handler_default_b(char const * name, char const * arg, bool barg, option_b * data)
{
	*data = barg;

	return false;
}
bool option_handler_default_f(char const * name, char const * arg, bool barg, option_f * data)
{
	*data = option_parse_f(name, arg);

	return true;
}
bool option_handler_default_i(char const * name, char const * arg, bool barg, option_i * data)
{
	*data = option_parse_i(name, arg);

	return true;
}
bool option_handler_default_s(char const * name, char const * arg, bool barg, option_s * data)
{
	option_assert_arg(name, arg);

	*data = arg;

	return true;
}

bool option_handler_default_bv(char const * name, char const * arg, bool barg, option_bv * data)
{
	data->push_back(barg);

	return false;
}
bool option_handler_default_fv(char const * name, char const * arg, bool barg, option_fv * data)
{
	data->push_back(option_parse_f(name, arg));

	return true;
}
bool option_handler_default_iv(char const * name, char const * arg, bool barg, option_iv * data)
{
	data->push_back(option_parse_i(name, arg));

	return true;
}
bool option_handler_default_sv(char const * name, char const * arg, bool barg, option_sv * data)
{
	option_assert_arg(name, arg);

	data->push_back(arg);

	return true;
}

option_f option_parse_f(char const * name, char const * arg)
{
	option_assert_arg(name, arg);

	option_f f(0);

	char const * s(arg);

	for (; *s && *s != '.'; ++s)
	{
		if (!isdigit(*s))
			throw option_exception(name, arg, "invalid float");

		f *= 10;
		f += *s - '0';
	}

	if (*s == '.')
	{
		while (*++s);

		option_f fFrac(0);

		while (*--s != '.')
		{
			if (!isdigit(*s))
				throw option_exception(name, arg, "invalid float");

			fFrac += *s - '0';
			fFrac /= 10;
		}
	}

	return f;
}
option_i option_parse_i(char const * name, char const * arg)
{
	option_assert_arg(name, arg);

	option_i i(0);

	for (char const * s(arg); *s; ++s)
	{
		if (!isdigit(*s))
			throw option_exception(name, arg, "invalid int");

		i *= 10;
		i += *s - '0';
	}

	return i;
}

void option_print(std::ostream * out, option const & opt)
{
	switch (opt.type)
	{
	case option::OPT_B:  if (opt.data.b ) option_print(out, opt.data.b ); break;
	case option::OPT_F:  if (opt.data.f ) option_print(out, opt.data.f ); break;
	case option::OPT_I:  if (opt.data.i ) option_print(out, opt.data.i ); break;
	case option::OPT_S:  if (opt.data.s ) option_print(out, opt.data.s ); break;
	case option::OPT_BV: if (opt.data.bv) option_print(out, opt.data.bv); break;
	case option::OPT_FV: if (opt.data.fv) option_print(out, opt.data.fv); break;
	case option::OPT_IV: if (opt.data.iv) option_print(out, opt.data.iv); break;
	case option::OPT_SV: if (opt.data.sv) option_print(out, opt.data.sv); break;
	}
}
void option_print(std::ostream * out, option_b data)
{
	*out << (data ? "true" : "false");
}
void option_print(std::ostream * out, option_f data)
{
	*out << data;
}
void option_print(std::ostream * out, option_i data)
{
	*out << data;
}
void option_print(std::ostream * out, option_s const & data)
{
	*out << data;
}
void option_print(std::ostream * out, option_bv const & data)
{
	*out << "{";

	for (size_t i(0); i < data.size(); ++i)
	{
		option_print(out, data[i]);

		if (i != data.size()-1)
			*out << ", ";
	}

	*out << "}";
}
void option_print(std::ostream * out, option_fv const & data)
{
	*out << "{";

	for (size_t i(0); i < data.size(); ++i)
	{
		option_print(out, data[i]);

		if (i != data.size()-1)
			*out << ", ";
	}

	*out << "}";
}
void option_print(std::ostream * out, option_iv const & data)
{
	*out << "{";

	for (size_t i(0); i < data.size(); ++i)
	{
		option_print(out, data[i]);

		if (i != data.size()-1)
			*out << ", ";
	}

	*out << "}";
}
void option_print(std::ostream * out, option_sv const & data)
{
	*out << "{";

	for (size_t i(0); i < data.size(); ++i)
	{
		option_print(out, data[i]);

		if (i != data.size()-1)
			*out << ", ";
	}

	*out << "}";
}

void option_print_help(std::ostream * out)
{
	if (!option_name.empty())
	{
		if (option_version.empty())
			*out << option_name << std::endl;
		else
			*out << option_name << " " << option_version << std::endl;

		if (!option_usage.empty())
			*out << "usage: " << option_name << " " << option_usage << std::endl;
	}

	for (std::map<std::string, std::map<std::string, option> >::iterator groupIt(option_map.begin()); groupIt != option_map.end(); ++groupIt)
	{
		*out << groupIt->first << ":" << std::endl;

		std::map<std::string, option> & group(groupIt->second);

		size_t optLen(0);

		for (std::map<std::string, option>::iterator optIt(group.begin()); optIt != group.end(); ++optIt)
		{
			if (optIt->first.size() > optLen)
				optLen = optIt->first.size();
		}

		for (std::map<std::string, option>::iterator optIt(group.begin()); optIt != group.end(); ++optIt)
		{
			*out << "  --" << optIt->first << " ";

			option & opt(optIt->second);

			switch (opt.type)
			{
			case option::OPT_B:  *out << "b "; break;
			case option::OPT_F:  *out << "f "; break;
			case option::OPT_I:  *out << "i "; break;
			case option::OPT_S:  *out << "s "; break;
			case option::OPT_BV: *out << "bv"; break;
			case option::OPT_FV: *out << "fv"; break;
			case option::OPT_IV: *out << "iv"; break;
			case option::OPT_SV: *out << "sv"; break;
			}

			for (size_t i(optIt->first.size()); i < optLen; ++i)
				out->put(' ');

			*out << "  " << opt.desc << std::endl;
		}

		*out << std::endl;
	}

	*out << std::endl;
}

void option_print_values(std::ostream * out)
{
	*out << "args=";
	option_print(out, option_args);
	*out << std::endl;

	for (std::map<std::string, std::map<std::string, option> >::iterator groupIt(option_map.begin()); groupIt != option_map.end(); ++groupIt)
	{
		*out << groupIt->first << ":" << std::endl;

		std::map<std::string, option> & group(groupIt->second);

		for (std::map<std::string, option>::iterator optIt(group.begin()); optIt != group.end(); ++optIt)
		{
			*out << "  --" << optIt->first << "=";

			option_print(out, optIt->second);

			*out << std::endl;
		}

		*out << std::endl;
	}

	*out << std::endl;
}

bool option_process(char const * name, char const * arg)
{
	if (name[0] != '-' || name[1] != '-')
	{
		option_args_handler(name, name, true, &option_args);
		return false;
	}

	name += 2;

	if (arg && arg[0] == '-' && arg[1] == '-')
		arg = NULL;

	bool barg(true);

	if (name[0] == 'n' && name[1] == 'o' && name[2] == '-')
	{
		barg = false;
		name += 3;
	}

	for (std::map<std::string, std::map<std::string, option> >::iterator groupIt(option_map.begin()); groupIt != option_map.end(); ++groupIt)
	{
		std::map<std::string, option> & group(groupIt->second);

		std::map<std::string, option>::iterator optIt(group.find(name));

		if (optIt != group.end())
		{
			option & opt(optIt->second);

			switch (opt.type)
			{
			case option::OPT_B: return opt.handler.b(name, arg, barg, opt.data.b);
			case option::OPT_F: return opt.handler.f(name, arg, barg, opt.data.f);
			case option::OPT_I: return opt.handler.i(name, arg, barg, opt.data.i);
			case option::OPT_S: return opt.handler.s(name, arg, barg, opt.data.s);

			case option::OPT_BV: return opt.handler.bv(name, arg, barg, opt.data.bv);
			case option::OPT_FV: return opt.handler.fv(name, arg, barg, opt.data.fv);
			case option::OPT_IV: return opt.handler.iv(name, arg, barg, opt.data.iv);
			case option::OPT_SV: return opt.handler.sv(name, arg, barg, opt.data.sv);
			}
		}
	}

	throw option_exception(name, "unknown name");
}
void option_process(int argc, char const * const * argv)
{
	for (int argi(0), argn(1); argi < argc; ++argi)
	{
		argn = argi + 1;

		argi += option_process(argv[argi], (argn < argc) ? argv[argn] : NULL);
	}
}

void option_set_name(char const * name)
{
	option_name = name;
}
void option_set_usage(char const * usage)
{
	option_usage = usage;
}
void option_set_version(char const * version)
{
	option_version = version;
}



}



