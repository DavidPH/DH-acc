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



class option
{
public:
	class exception : public std::exception
	{
	public:
		exception(std::string const & name, std::string const & description);
		exception(std::string const & name, std::string const & arg, std::string const & description);
		virtual ~exception() throw();

		virtual char const * what();

	private:
		std::string _what;
	};



	virtual ~option();

	// Must return true if arg is consumed.
	virtual bool handle(std::string const & name, std::string const & arg, bool barg) = 0;



	static void assert_arg(std::string const & name, std::string const & arg);

	static bool less(option * l, option * r);

	static void print_help(std::ostream * out);
	static void print_help(std::ostream * out, int width);

	static void process(int argc, char const * const * argv);
	static bool process(std::string const & name, std::string const & arg);
	static void process(std::vector<std::string> const & args);

	// Holds arguments not consumed by options. The handler is passed the
	// argument as both the name and the arg and the return value ignored.
	static std::vector<std::string> args_vector;
	static bool (*args_handler)(std::string const & name, std::string const & arg, bool barg, std::vector<std::string> * data);

	static std::string const no_arg;

	static std::string program;
	static std::string usage;
	static std::string version;

protected:
	option(std::string const & name, std::string const & group, std::string const & description);

	virtual void printHelp(std::ostream * out, int width, int padlen);

	std::string description;
	std::string group;
	std::string name;

private:
	option()/* = delete*/;
	option(option const & opt)/* = delete*/;
};

template<typename T> class option_auto : public option
{
public:
	typedef T option_t;
	typedef bool (*handler_t)(std::string const & name, std::string const & arg, bool barg, option_t * data);



	option_auto(std::string const & name, std::string const & group, std::string const & description, option_t * data);
	option_auto(std::string const & name, std::string const & group, std::string const & description, option_t * data, handler_t handler);

	virtual bool handle(std::string const & name, std::string const & arg, bool barg);



	static bool handler_default(std::string const & name, std::string const & arg, bool barg, option_t * data);

	static option_t parse(std::string const & name, std::string const & arg);

protected:
	option_t * data_p;
	handler_t handler;

private:
	option_auto()/* = delete*/;
	option_auto(option_auto<T> const & opt)/* = delete*/;
};

template<typename T> class option_data : public option_auto<T>
{
public:
	typedef typename option_auto<T>::handler_t handler_t;
	typedef typename option_auto<T>::option_t option_t;

	option_data(std::string const & name, std::string const & group, std::string const & description);
	option_data(std::string const & name, std::string const & group, std::string const & description, handler_t handler);
	option_data(std::string const & name, std::string const & group, std::string const & description, option_t const & data);
	option_data(std::string const & name, std::string const & group, std::string const & description, option_t const & data, handler_t handler);

	option_t data;

private:
	option_data()/* = delete*/;
	option_data(option_data<T> const & opt)/* = delete*/;
};



template<typename T> option_auto<T>::option_auto(std::string const & name, std::string const & group, std::string const & description, option_t * data) : option(name, group, description), data_p(data), handler(handler_default)
{

}
template<typename T> option_auto<T>::option_auto(std::string const & name, std::string const & group, std::string const & description, option_t * data, handler_t handler) : option(name, group, description), data_p(data), handler(handler)
{

}

template<typename T> bool option_auto<T>::handle(std::string const & name, std::string const & arg, bool barg)
{
	return handler(name, arg, barg, data_p);
}



template<typename T> option_data<T>::option_data(std::string const & name, std::string const & group, std::string const & description) : option_auto<T>(name, group, description, &data)
{

}
template<typename T> option_data<T>::option_data(std::string const & name, std::string const & group, std::string const & description, handler_t handler) : option_auto<T>(name, group, description, &data, handler)
{

}
template<typename T> option_data<T>::option_data(std::string const & name, std::string const & group, std::string const & description, option_t const & data) : option_auto<T>(name, group, description, &this->data), data(data)
{

}
template<typename T> option_data<T>::option_data(std::string const & name, std::string const & group, std::string const & description, option_t const & data, handler_t handler) : option_auto<T>(name, group, description, &this->data, handler), data(data)
{

}



#endif /* HPP_option_ */


