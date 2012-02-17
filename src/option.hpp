//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
// Option Handling
//
//-----------------------------------------------------------------------------

#ifndef HPP_option_
#define HPP_option_

#include <cstddef>
#include <cstdio>
#include <stdexcept>



/// Base class for option processing.
class option
{
public:
   class exception;
   class option_args;
   class option_call;
   class option_copy;
   class option_cstr;
   template<typename T> class option_dptr;
   template<typename T> class option_data;

   enum
   {
      OPTF_FALSE = 1, // The option is negated.
      OPTF_SHORT = 2, // opt is a pointer to a single char.
      OPTF_FINAL = 4, // There are no more args. (Might never be set.)
      OPTF_KEEPA = 8, // Args may be kept.
   };


   /// Handles an option.
   /**
   *** @param opt  The name of the option.
   *** @param optf OPTF_ flags.
   *** @param argc Number of valid entries in argv.
   *** @param argv Vector of args.
   ***
   *** @return This function must return the number of args consumed.
   **/
   virtual int handle(char const *opt, int optf, int argc,
                      char const *const *argv) = 0;


   friend class option_args;

   /// Returns the option with the name nameS. Throws if not found.
   static option *find(char nameS);
   /// Returns the option with the name nameL. Throws if not found.
   static option *find(char const *nameL);

   /// Prints usage information.
   static void print_help(FILE *out, unsigned width = 0);

   /// Prints version information.
   static void print_version(FILE *out);

   static void process_options(int argc, char const *const *argv, int optf);

   /// Used to handle args not attached to options.
   /**
   *** When the handle method is called for this option, opt is "" and argv is
   *** a series of args not attached to options.
   **/
   static option *arg_handler;

   /// Used when printing help to identify the program.
   static char const *help_program;
   /// Used when printing help to identify the version.
   static char const *help_version;
   /// Used when printing help to provide a usage overview.
   static char const *help_usage;
   /// Used when printing help to provide a description.
   static char const *help_desc_s;
   /// Used when generating documentation.
   static char const *help_desc_l;

protected:
   option(char nameS, char const *nameL, char const *group, char const *descS,
          char const *descL);
   ~option();

   char const *descL;
   char const *descS;
   char const *group;
   char const *nameL;
   char        nameS;


   /// Finds the option after the end of the group.
   static option *find_group_end(char const *group, option *start);
   /// Finds the first option in the group.
   static option *find_group_start(char const *group);

private:
   option();
   option(option const &);

   option *next;
   option *prev;


   static int count_args(int argc, char const *const *argv);

   static int optstrcmp(char const *s1, char const *s2);

   static int process_option_arg(int argc, char const *const *argv, int optf);
   static int process_option_long(int argc, char const *const *argv, int optf);
   static int process_option_short(int argc, char const *const *argv, int optf);

   static size_t wordlen(char const *str);

   static void write_wrapped(FILE *out, size_t width, char const *str,
                             size_t baselen, char const *prefix = NULL);

   static option *head();
};

/// Class for indicating errors with options.
class option::exception : public std::exception
{
public:
   exception();
   exception(exception const &e);
   exception(char const *msg);
   exception(char const *msg, size_t len);
   ~exception() throw();

   char const *what() const throw() {return whatmsg;}


   static void error(char const *opt, int optf, char const *format, ...);

private:
   char *whatmsg;
   size_t whatlen;
};

/// Class for handling loose args.
class option::option_args : public option
{
public:
   virtual int handle(char const *opt, int optf, int argc,
                      char const *const *argv);


   friend class option;

   static size_t       arg_alloc;
   static size_t       arg_count;
   static char const **arg_vector;

private:
   option_args();
};

/// Class for calling a function for handling.
class option::option_call : public option
{
public:
   typedef int (*handler)(char const *opt, int optf, int argc,
                          char const *const *argv);

   option_call(char nameS, char const *nameL, char const *group,
               char const *descS, char const *descL, handler h);

   virtual int handle(char const *opt, int optf, int argc,
                      char const *const *argv);

protected:
   handler h;
};

/// Class for giving extra names to an option.
class option::option_copy : public option
{
public:
   option_copy(char nameS, char const *nameL, option *copy);

   virtual int handle(char const *opt, int optf, int argc,
                      char const *const *argv);

protected:
   option *copy;
};

/// Class for handling a string option.
/**
*** This class differs from option_data<char const *> in that it does not free
*** its payload on destruction. However, instead it only copies its arg if it's
*** not OPTF_KEEPA.
**/
class option::option_cstr : public option
{
public:
   option_cstr(char nameS, char const *nameL, char const *group,
               char const *descS, char const *descL);
   option_cstr(char nameS, char const *nameL, char const *group,
               char const *descS, char const *descL, char const *value);

   virtual int handle(char const *opt, int optf, int argc,
                      char const *const *argv);

   char const *data;
};

/// Class for handling a single piece of data, stored externally.
/**
*** Instantiations are provided for the following types:
***   bool, int, char const *, std::string, std::vector<std::string>.
**/
template<typename T> class option::option_dptr : public option
{
public:
   option_dptr(char nameS, char const *nameL, char const *group,
               char const *descS, char const *descL, T *dptr);

   virtual int handle(char const *opt, int optf, int argc,
                      char const *const *argv);

protected:
   T *dptr;
};

/// Class for handling a single piece of data, stored internally.
/**
*** Instantiations are provided for the same types as option_dptr.
**/
template<typename T> class option::option_data : public option_dptr<T>
{
public:
   option_data(char nameS, char const *nameL, char const *group,
               char const *descS, char const *descL);
   option_data(char nameS, char const *nameL, char const *group,
               char const *descS, char const *descL, T const &value);
   ~option_data();

   T data;
};



#endif//HPP_option_

