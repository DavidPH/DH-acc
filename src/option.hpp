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

/* option.hpp
**
** Declares the option-handling functions.
*/

#ifndef HPP_option_
#define HPP_option_

#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace option
{



class option_exception : public std::exception
{
public:
	option_exception(char const * name, char const * arg, char const * description);
	option_exception(char const * name, char const * description);
	virtual ~option_exception() throw();

	virtual char const * what();

private:
	std::string _what;
};



typedef bool          option_b;
typedef long double   option_f;
typedef long long int option_i;
typedef std::string   option_s;

typedef std::vector<bool>          option_bv;
typedef std::vector<long double>   option_fv;
typedef std::vector<long long int> option_iv;
typedef std::vector<std::string>   option_sv;

// Handlers return true if they consume the associated arg.
typedef bool (*option_handler_b)(char const * name, char const * arg, bool barg, option_b * data);
typedef bool (*option_handler_f)(char const * name, char const * arg, bool barg, option_f * data);
typedef bool (*option_handler_i)(char const * name, char const * arg, bool barg, option_i * data);
typedef bool (*option_handler_s)(char const * name, char const * arg, bool barg, option_s * data);

typedef bool (*option_handler_bv)(char const * name, char const * arg, bool barg, option_bv * data);
typedef bool (*option_handler_fv)(char const * name, char const * arg, bool barg, option_fv * data);
typedef bool (*option_handler_iv)(char const * name, char const * arg, bool barg, option_iv * data);
typedef bool (*option_handler_sv)(char const * name, char const * arg, bool barg, option_sv * data);



void option_add(char const * name, char const * group, char const * description, option_b * data, option_handler_b handler);
void option_add(char const * name, char const * group, char const * description, option_f * data, option_handler_f handler);
void option_add(char const * name, char const * group, char const * description, option_i * data, option_handler_i handler);
void option_add(char const * name, char const * group, char const * description, option_s * data, option_handler_s handler);

void option_add(char const * name, char const * group, char const * description, option_bv * data, option_handler_bv handler);
void option_add(char const * name, char const * group, char const * description, option_fv * data, option_handler_fv handler);
void option_add(char const * name, char const * group, char const * description, option_iv * data, option_handler_iv handler);
void option_add(char const * name, char const * group, char const * description, option_sv * data, option_handler_sv handler);

void option_assert_arg(char const * name, char const * arg);

bool option_handler_default_b(char const * name, char const * arg, bool barg, option_b * data);
bool option_handler_default_f(char const * name, char const * arg, bool barg, option_f * data);
bool option_handler_default_i(char const * name, char const * arg, bool barg, option_i * data);
bool option_handler_default_s(char const * name, char const * arg, bool barg, option_s * data);

bool option_handler_default_bv(char const * name, char const * arg, bool barg, option_bv * data);
bool option_handler_default_fv(char const * name, char const * arg, bool barg, option_fv * data);
bool option_handler_default_iv(char const * name, char const * arg, bool barg, option_iv * data);
bool option_handler_default_sv(char const * name, char const * arg, bool barg, option_sv * data);

option_f option_parse_f(char const * name, char const * arg);
option_i option_parse_i(char const * name, char const * arg);

void option_print(std::ostream * out, option_b data);
void option_print(std::ostream * out, option_f data);
void option_print(std::ostream * out, option_i data);
void option_print(std::ostream * out, option_s const & data);
void option_print(std::ostream * out, option_bv const & data);
void option_print(std::ostream * out, option_fv const & data);
void option_print(std::ostream * out, option_iv const & data);
void option_print(std::ostream * out, option_sv const & data);
void option_print_help(std::ostream * out);
void option_print_values(std::ostream * out);

bool option_process(char const * name, char const * arg);
void option_process(int argc, char const * const * argv);

void option_set_name(char const * name);
void option_set_usage(char const * usage);
void option_set_version(char const * version);



// Holds arguments not consumed by options. The handler is passed the argument
// as both the name and the arg and the return value ignored.
extern option_sv         option_args;
extern option_handler_sv option_args_handler;



}

#endif /* HPP_option_ */



