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

#include "option.hpp"

#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>



option *option::arg_handler = head();

char const *option::help_program = NULL;
char const *option::help_version = NULL;
char const *option::help_usage   = NULL;
char const *option::help_desc_s  = NULL;
char const *option::help_desc_l  = NULL;

size_t       option::option_args::arg_alloc = 8;
size_t       option::option_args::arg_count = 0;
char const **option::option_args::arg_vector = new char const*[8];

//=============================================================================
// option                                                                     |
//

//
// option::option
//
option::option() : descL(NULL), descS(NULL), group(NULL), nameL(NULL),
                   nameS('\0'), next(this), prev(this)
{
}

//
// option::option
//
option::option(char _nameS, char const *_nameL, char const *_group,
               char const *_descS, char const *_descL)
               : descL(_descL ? _descL : _descS), descS(_descS), group(_group),
                 nameL(_nameL), nameS(_nameS)
{
   option *start = find_group_start(group);
   option *end = find_group_end(group, start);

   option *opt;

   for (opt = start; opt != end; opt = opt->next)
   {
      if (optstrcmp(opt->nameL, nameL) > 0)
         break;
   }

   next = opt;
   prev = opt->prev;

   next->prev = this;
   prev->next = this;
}

//
// option::~option
//
option::~option()
{
   prev->next = next;
   next->prev = prev;
}

//
// option::count_args
//
int option::count_args(int argc, char const *const *argv)
{
   int argi;
   char const *arg;

   for (argi = 0; argi < argc; ++argi)
   {
      arg = argv[argi];
      if (arg[0] == '-' && arg[1] != '\0')
         break;
   }

   return argi;
}

//
// option::find
//
option *option::find(char nameS)
{
   option *end = head();
   option *opt = end;

   while ((opt = opt->next) != end)
   {
      if (opt->nameS && opt->nameS == nameS)
         return opt;
   }

   exception::error(&nameS, OPTF_SHORT, "no such option");
   return NULL;
}

//
// option::find
//
option *option::find(char const *nameL)
{
   option *end = head();
   option *opt = end;

   // shorthand longopts
   size_t  slen = strlen(nameL);
   option *sopt = NULL;
   int     scnt = 0;

   while ((opt = opt->next) != end)
   {
      if (!opt->nameL) continue;

      if (strcmp(opt->nameL, nameL) == 0)
         return opt;

      if (memcmp(opt->nameL, nameL, slen) == 0)
      {
         sopt = opt;
         scnt++;
      }
   }

   if (sopt)
   {
      if (scnt > 1)
         exception::error(nameL, 0, "ambiguous shorthand (%i matches)", scnt);

      return sopt;
   }

   exception::error(nameL, 0, "no such option");
   return NULL;
}

//
// option::find_group_end
//
option *option::find_group_end(char const *group, option *start)
{
   option *end = head();
   option *opt;

   for (opt = start; opt != end; opt = opt->next)
   {
      if (optstrcmp(opt->group, group) > 0)
         break;
   }

   return opt;
}

//
// option::find_group_start
//
option *option::find_group_start(char const *group)
{
   option *end = head();
   option *opt = end;

   if (!group)
      return opt->next;

   while ((opt = opt->next) != end)
   {
      if (optstrcmp(opt->group, group) >= 0)
         break;
   }

   return opt;
}

//
// option::head
//
option *option::head()
{
   static option *opt = new option_args;
   return opt;
}

//
// option::optstrcmp
//
int option::optstrcmp(char const *s1, char const *s2)
{
   if (s1 == s2) return 0;

   if (!s1) return -1;
   if (!s2) return +1;

   return strcmp(s1, s2);
}

//
// option::print_help
//
void option::print_help(FILE *out, unsigned width)
{
   // TODO: Determine terminal width.
   if (!width)
      width = 80;

   if (help_program)
   {
      fputs(help_program, out);

      if (help_version)
      {
         fputc(' ', out);
         fputs(help_version, out);
      }

      fputc('\n', out);

      if (help_usage)
      {
         fputs("usage: ", out);
         fputs(help_program, out);
         fputc(' ', out);
         write_wrapped(out, width, help_usage, 7, help_program);
      }

      fputc('\n', out);
   }

   if (help_desc_s)
   {
      write_wrapped(out, width, help_desc_s, 0);
      fputc('\n', out);
   }

   option *end = head();
   option *opt = end;

   char const *group = NULL;

   size_t grouplen = 0;

   while ((opt = opt->next) != end)
   {
      size_t baselen;

      char const *desc = opt->descS;

      if (!desc) continue;

      // This gets us past the starting NULL groups and makes checking faster
      // for folded string literals.
      if (opt->group != group && (!opt->group || !group ||
                                  strcmp(opt->group, group) != 0))
      {
         group = opt->group;

         if (group)
         {
            fputc( '\n', out);
            fputs(group, out);
            fputs(":\n", out);
         }

         grouplen = 0;
      }

      if (!grouplen)
      {
         option *it = opt->prev;

         while ((it = it->next) != end &&
                (it->group == group ||
                 (it->group && group && strcmp(it->group, group) == 0)))
         {
            if (!it->nameL) continue;

            size_t len = strlen(it->nameL);

            if (len > grouplen) grouplen = len;
         }

         // If not, no long names were encountered.
         if (grouplen) grouplen += 3;
         grouplen += 7;
      }

      // Print the short name or pad if none.
      if (opt->nameS)
      {
         fputs("  -", out);
         fputc(opt->nameS, out);
         fputc(opt->nameL ? ',' : ' ', out);
      }
      else
         fputs("     ", out);

      baselen = 5;

      // Print the long name, if any.
      if (opt->nameL)
      {
         fputs(" --", out);
         fputs(opt->nameL, out);

         baselen += 3 + strlen(opt->nameL);
      }

      // Pad to the same adding as the rest of the group.
      while (baselen < grouplen)
      {
         fputc(' ', out);
         baselen++;
      }

      // It could happen. No need to deadlock over it.
      if (baselen > width)
      {
         fputc('\n', out);
         continue;
      }

      write_wrapped(out, width, desc, baselen);
   }
}

//
// option::print_vers
//
void option::print_version(FILE *out)
{
   if (help_program)
   {
      fputs(help_program, out);

      if (help_version)
      {
         fputc(' ', out);
         fputs(help_version, out);
      }

      fputc('\n', out);
   }
}

//
// option::process_option_arg
//
int option::process_option_arg(int argc, char const *const *argv, int optf)
{
   int optc = count_args(argc, argv);

   return arg_handler->handle("", optf, optc, argv);
}

//
// option::process_option_long
//
int option::process_option_long(int argc, char const *const *argv, int optf)
{
   int used;
   int optc = count_args(argc-1, argv+1) + 1;
   char const *arg0;
   char const *opts = argv[0]+2;
   option *opt;

   if (opts[0] == 'n' && opts[1] == 'o' && opts[2] == '-')
   {
      opts += 3;
      optf |= OPTF_FALSE;
   }

   if (optc == argc-1)
      optf |= OPTF_FINAL;

   arg0 = strchr(opts, '=');

   if (arg0)
   {
      optc++;
      char const **optv = new char const *[optc];
      optv[0] = arg0+1;
      for (int i = 1; i < optc; ++i)
         optv[i] = argv[i];

      size_t optsLen = arg0 - opts;
      char *optsTemp = new char[optsLen+1];
      memcpy(optsTemp, opts, optsLen);
      optsTemp[optsLen] = '\0';

      try
      {
         opt = find(optsTemp);
         used = opt->handle(optsTemp, optf, optc, optv);
      }
      catch (...)
      {
         delete[] optv;
         delete[] optsTemp;
         throw;
      }

      delete[] optv;
      delete[] optsTemp;

      // An argument was explicitly attached to this option. If it went unused,
      // that's an error.
      if (!used)
         exception::error(optsTemp, optf, "extraneous argument");
   }
   else
   {
      opt = find(opts);
      used = opt->handle(opts, optf, optc, argv+1) + 1;
   }

   return used;
}

//
// option::process_option_short
//
int option::process_option_short(int argc, char const *const *argv, int optf)
{
   int used = 1;
   int optc = count_args(argc-1, argv+1) + 1;
   char const *opts;
   char const **optv;
   option *opt;

   optv = new char const *[optc];
   for (int i = 1; i < optc; ++i)
      optv[i] = argv[i];

   optf |= OPTF_SHORT;

   if (optc == argc)
      optf |= OPTF_FINAL;

   try
   {
      for (opts = argv[0]+1; *opts; ++opts)
      {
         opt = find(*opts);

         if (opts[1])
         {
            optv[0] = opts+1;
            used = opt->handle(opts, optf, optc, optv);

            // used includes the opts in argv, so can return as-is.
            if (used)
               break;
         }
         else
         {
            // Add 1 so that the count includes the opts in argv.
            used = opt->handle(opts, optf, optc-1, argv+1) + 1;
         }
      }
   }
   catch (...)
   {
      delete[] optv;
      throw;
   }

   delete[] optv;

   return used;
}

//
// option::process_options
//
void option::process_options(int argc, char const *const *argv, int optf)
{
   char const *argvName;
   int used;

   while (argc)
   {
      argvName = argv[0];

      if (argvName[0] == '-')
      {
         // -*
         if (argvName[1] == '-')
         {
            // --*
            if (argvName[2] == '\0')
            {
               // --
               argc--; argv++;
               break;
            }
            else
            {
               // --*
               used = process_option_long(argc, argv, optf);
            }
         }
         else if (argvName[1] == '\0')
         {
            // -
            used = process_option_arg(argc, argv, optf);
         }
         else
         {
            // -*
            used = process_option_short(argc, argv, optf);
         }
      }
      else
      {
         // *
         used = process_option_arg(argc, argv, optf);
      }

      argc -= used;
      argv += used;
   }

   // If argc is still nonzero, we encountered an explicit end of options.
   // Process all remaining args as freestanding.
   while (argc)
   {
      used = arg_handler->handle("", optf|OPTF_FINAL, argc, argv);

      argc -= used;
      argv += used;
   }
}

//
// option::wordlen
//
size_t option::wordlen(char const *str)
{
   size_t len = 0;

   while (*str && !isspace(*str++))
      len++;

   return len ? len : 1;
}

//
// option::write_wrapped
//
void option::write_wrapped(FILE *out, size_t width, char const *str,
                           size_t baselen, char const *prefix)
{
   char const *pre;
   size_t linelen = baselen;

   if (prefix)
      linelen += strlen(prefix)+1;

   while (*str)
   {
      // Linewrapping.
      if (*str == '\n' || linelen+wordlen(str) > width)
      {
         fputc('\n', out);

         // Only write line start stuff for the last linefeed.
         if (*str == '\n' && str[1] == '\n')
         {
            str++;
            continue;
         }

         for (linelen = 0; linelen < baselen; ++linelen)
            fputc(' ', out);

         if (prefix)
         {
            // Only reprint prefix for an explicit line break.
            if (*str == '\n')
               for (pre = prefix; *pre; ++pre, ++linelen)
                  fputc(*pre, out);
            else
               for (pre = prefix; *pre; ++pre, ++linelen)
                  fputc(' ', out);

            fputc(' ', out);
            linelen++;
         }

         if (*str == '\n') str++;

         // Skip any leading spaces.
         while (*str == ' ') str++;
      }

      fputc(*str++, out);
      linelen++;
   }

   fputc('\n', out);
}

//=============================================================================
// option::exception                                                          |
//

//
// exception::exception
//
option::exception::exception() : whatmsg(NULL), whatlen(0)
{
}

//
// exception::exception
//
option::exception::exception(exception const &e)
{
   whatlen = e.whatlen;
   whatmsg = new char[whatlen];
   memcpy(whatmsg, e.whatmsg, whatlen);
}

//
// exception::exception
//
option::exception::exception(char const *msg)
{
   whatlen = strlen(msg)+1;
   whatmsg = new char[whatlen];
   memcpy(whatmsg, msg, whatlen);
}

//
// exception::exception
//
option::exception::exception(char const *msg, size_t len)
{
   whatlen = len+1;
   whatmsg = new char[whatlen];
   memcpy(whatmsg, msg, whatlen);
}

//
// exception::~exception
//
option::exception::~exception() throw()
{
   delete[] whatmsg;
}

//
// exception::error
//
void option::exception
::error(char const *opt, int optf, char const *format, ...)
{
   char msg[1024];

   int len;

   if (optf & OPTF_SHORT)
      len = snprintf(msg, 1024, "'-%c': ", *opt);
   else
      len = snprintf(msg, 1024, "'--%s': ", opt);

   if (len > 1024) len = 1024;

   va_list args;
   va_start(args, format);
   len += vsnprintf(msg+len, 1024-len, format, args);
   va_end(args);

   if (len > 1024) len = 1024;

   throw option::exception(msg, len);
}

//=============================================================================
// option::option_args                                                        |
//

//
// option_args::option_args
//
// Only defined in order to make private.
//
option::option_args::option_args()
{
}

//
// option_args::handle
//
// This default handler just appends all of its args to the arg_vector.
//
int option::option_args
::handle(char const *, int optf, int argc, char const *const *argv)
{
   int argi;
   size_t new_count = arg_count + argc;

   if (new_count > arg_alloc)
   {
      size_t new_alloc = new_count;
      if (!(optf & OPTF_FINAL))
         new_alloc *= 2;

      char const **new_vector = new char const*[new_alloc];
      memcpy(new_vector, arg_vector, sizeof(char const *) * arg_count);
      delete[] arg_vector;

      arg_alloc  = new_alloc;
      arg_vector = new_vector;
   }

   for (argi = 0; argi < argc; ++argi)
   {
      if (optf & OPTF_KEEPA)
      {
         arg_vector[arg_count] = argv[argi];
      }
      else
      {
         size_t len = strlen(argv[argi])+1;
         arg_vector[arg_count] = static_cast<char *>(memcpy(new char[len],
                                                            argv[argi], len));
      }

      arg_count++;
   }

   return argc;
}

//=============================================================================
// option::option_call                                                        |
//

//
// option_call::option_call
//
option::option_call
::option_call(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL, handler _h)
              : option(_nameS, _nameL, _group, _descS, _descL), h(_h)
{
}

//
// option_call::handle
//
int option::option_call::handle(char const *opt, int optf, int argc,
                                char const *const *argv)
{
   return h(opt, optf, argc, argv);
}

//=============================================================================
// option::option_copy                                                        |
//

//
// option_copy::option_copy
//
option::option_copy
::option_copy(char _nameS, char const *_nameL, option *_copy)
              : option(_nameS, _nameL, _copy->group, _copy->nameL, NULL),
                copy(_copy)
{
}

int option::option_copy
::handle(char const *opt, int optf, int argc, char const *const *argv)
{
   return copy->handle(opt, optf, argc, argv);
}

//=============================================================================
// option::option_cstr                                                        |
//

//
// option_cstr::option_cstr
//
option::option_cstr
::option_cstr(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL)
              : option(_nameS, _nameL, _group, _descS, _descL), data(NULL)
{
}

//
// option_cstr::option_cstr
//
option::option_cstr
::option_cstr(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL, char const *value)
              : option(_nameS, _nameL, _group, _descS, _descL), data(value)
{
}

//
// option_cstr::handle
//
int option::option_cstr
::handle(char const *opt, int optf, int argc, char const *const *argv)
{
   if (optf & OPTF_FALSE)
   {
      data = NULL;
      return 0;
   }

   if (!argc) exception::error(opt, optf, "requires argument");

   if (optf & OPTF_KEEPA)
   {
      data = argv[0];
   }
   else
   {
      size_t len = strlen(argv[0])+1;
      data = static_cast<char *>(memcpy(new char[len], argv[0], len));
   }

   return 1;
}

//=============================================================================
// option::option_data                                                        |
//

//
// option_data::option_data
//
template<typename T>
option::option_data<T>
::option_data(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL)
              : option_dptr<T>(_nameS, _nameL, _group, _descS, _descL, &data),
                data(0)
{
}
template<>
option::option_data<std::string>
::option_data(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL)
              : option_dptr(_nameS, _nameL, _group, _descS, _descL, &data),
                data()
{
}
template<>
option::option_data<std::vector<std::string> >
::option_data(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL)
              : option_dptr(_nameS, _nameL, _group, _descS, _descL, &data),
                data()
{
}

//
// option_data::option_data
//
template<typename T>
option::option_data<T>
::option_data(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL, T const &value)
              : option_dptr<T>(_nameS, _nameL, _group, _descS, _descL, &data),
                data(value)
{
}
template<>
option::option_data<char const *>
::option_data(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL, char const *const &value)
              : option_dptr(_nameS, _nameL, _group, _descS, _descL, &data),
                data()
{
   size_t len = strlen(value)+1;
   data = static_cast<char *>(memcpy(new char[len], value, len));
}

//
// option_data::~option_data
//
template<typename T>
option::option_data<T>::~option_data()
{
}
template<>
option::option_data<char const *>::~option_data()
{
   delete[] data;
}

template class option::option_data<bool>;
template class option::option_data<int>;
template class option::option_data<char const *>;
template class option::option_data<std::string>;
template class option::option_data<std::vector<std::string> >;

//=============================================================================
// option::option_dptr                                                        |
//

//
// option_dptr::option_dptr
//
template<typename T>
option::option_dptr<T>
::option_dptr(char _nameS, char const *_nameL, char const *_group,
              char const *_descS, char const *_descL, T *_dptr)
              : option(_nameS, _nameL, _group, _descS, _descL), dptr(_dptr)
{
}

//
// option_dptr::handle <bool>
//
template<>
int option::option_dptr<bool>
::handle(char const *, int optf, int, char const *const *)
{
   *dptr = !(optf & OPTF_FALSE);
   return 0;
}

//
// option_dptr::handle <int>
//
template<>
int option::option_dptr<int>
::handle(char const *opt, int optf, int argc, char const *const *argv)
{
   if (optf & OPTF_FALSE)
   {
      *dptr = 0;
      return 0;
   }

   if (!argc) exception::error(opt, optf, "requires argument");

   *dptr = atoi(argv[0]);
   return 1;
}

//
// option_dptr::handle <char const *>
//
template<>
int option::option_dptr<char const *>
::handle(char const *opt, int optf, int argc, char const *const *argv)
{
   if (optf & OPTF_FALSE)
   {
      *dptr = NULL;
      return 0;
   }

   if (!argc) exception::error(opt, optf, "requires argument");

   size_t len = strlen(argv[0])+1;
   *dptr = static_cast<char *>(memcpy(new char[len], argv[0], len));
   return 1;
}

//
// option_dptr::handle <std::string>
//
template<>
int option::option_dptr<std::string>
::handle(char const *opt, int optf, int argc, char const *const *argv)
{
   if (optf & OPTF_FALSE)
   {
      dptr->clear();
      return 0;
   }

   if (!argc) exception::error(opt, optf, "requires argument");

   *dptr = argv[0];
   return 1;
}

//
// option_dptr::handle <std::vector<std::string>>
//
template<>
int option::option_dptr<std::vector<std::string> >
::handle(char const *opt, int optf, int argc, char const *const *argv)
{
   if (optf & OPTF_FALSE)
   {
      dptr->clear();
      return 0;
   }

   if (!argc) exception::error(opt, optf, "requires argument");

   dptr->push_back(argv[0]);
   return 1;
}

template class option::option_dptr<bool>;
template class option::option_dptr<int>;
template class option::option_dptr<char const *>;
template class option::option_dptr<std::string>;
template class option::option_dptr<std::vector<std::string> >;



// EOF

