//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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
// Low-level processing of source code.
//
//-----------------------------------------------------------------------------

#include "SourceStream.hpp"

#include "option.hpp"
#include "SourceException.hpp"
#include "SourcePosition.hpp"

#include <fstream>
#include <sstream>
#include <vector>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static int AddIncludeDirUser(char const *opt, int optf, int argc, char const *const *argv);
static int AddIncludeDirSys(char const *opt, int optf, int argc, char const *const *argv);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_call option_include_dir
('i', "include-dir", "input",
 "Specifies a directory to search for includes in.", NULL,
 AddIncludeDirUser);

static option::option_call option_system_include_dir
('I', "system-include-dir", "input",
 "Specifies a directory to search for system includes in.", NULL,
 AddIncludeDirSys);

static option::option_data<int> option_tab_columns
('\0', "tab-columns", "input",
 "How many columns a tab counts for in error reporting.", NULL, 1);

// Used to push and pop directories due to inclusion.
static std::vector<std::string> autoIncludes;

// Used to track user-specified directories.
static std::vector<std::string> userIncludes;

// Used to track system directories.
static std::vector<std::string> sysIncludes;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// AddIncludeDirUser
//
static int AddIncludeDirUser(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   SourceStream::AddIncludeDirUser(argv[0]);

   return 1;
}

//
// AddIncludeDirSys
//
static int AddIncludeDirSys(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   SourceStream::AddIncludeDirSys(argv[0]);

   return 1;
}

//
// GetPathSep
//
// Determines what path separator is in use for a given path.
// For POSIX (read: sane) systems, this is a constant.
//
static char GetPathSep(std::string const &path)
{
   #if defined (__WIN32__)
   for(std::string::const_iterator itr = path.begin(), end = path.end(); itr != end; ++itr)
      if(*itr == '/' || *itr == '\\')
         return *itr;

   return '\\';
   #else
   (void)path;
   return '/';
   #endif
}

//
// NormalizePath
//
static void NormalizePath(std::string &path)
{
   #if defined(__WIN32__)
   char const pathSep = GetPathSep(path);

   for(std::string::iterator itr = path.begin(), end = path.end(); itr != end; ++itr)
      if(*itr == '/' || *itr == '\\')
         *itr = pathSep;
   #else
   // POSIX paths are inherently "normal", having only one representation.
   (void)path;
   #endif
}

//
// DirectoryPath
//
static void DirectoryPath(std::string &path)
{
   char const pathSep = GetPathSep(path);
   std::string::size_type indexSep = path.find_last_of(pathSep);
   if(indexSep != std::string::npos)
      path.resize(indexSep);
}

//
// TerminatePath
//
static void TerminatePath(std::string &path)
{
   char const pathSep = GetPathSep(path);

   if(*path.rbegin() != pathSep)
      path += pathSep;
}

//
// AppendPath
//
static void AppendPath(std::vector<std::string> &includes, std::string const &dir)
{
   // Don't try to append an empty name.
   if(dir.empty()) return;

   includes.push_back(dir);
   NormalizePath(includes.back());
   TerminatePath(includes.back());
}

//
// TryOpenFile
//
static std::istream *TryOpenFile(std::string const &filename)
{
   std::istream *in = new std::ifstream(filename.c_str());

   if(!*in)
   {
      delete in;
      return NULL;
   }
   else
      return in;
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceStream::SourceStream
//
SourceStream::SourceStream(std::string const &_filename, unsigned type)
 : oldC(-2), curC(-2), newC(-2),
   in(NULL),
   filename(_filename),
   pathname(),

   countColumn(0),
   countLine(1),

   depthComment(0),

   inComment(false),

   inEOF(false),

   inQuoteDouble(false),
   inQuoteSingle(false),

   doInclude(false),

   doPadEOF(true)
{
   switch(type & ST_MASK)
   {
   case ST_ASMPLX:
      doCommentASM = true;
      doCommentC   = false;
      doCommentCPP = false;

      doQuoteDouble = false;
      doQuoteSingle = false;
      break;

   case ST_C:
      doCommentASM = false;
      doCommentC   = true;
      doCommentCPP = true;

      doQuoteDouble = true;
      doQuoteSingle = true;
      break;
   }

   // String stream.
   if(type & STF_STRING)
   {
      doPadEOF = false;
      in = new std::istringstream(filename);
      filename = "string";

      return;
   }

   std::vector<std::string>::iterator dir, end;

   // Try the string as-is.
   if(!in)
      in = TryOpenFile(pathname = filename);

   // Automatic include directories.
   if(!in && !(type & STF_NOUSER))
   {
      // Search auto includes backwards.
      for(dir = autoIncludes.end(), end = autoIncludes.begin(); dir-- != end;)
         if((in = TryOpenFile(pathname = *dir + filename))) break;
   }

   // User-specified include directories.
   if(!in && !(type & STF_NOUSER))
   {
      for(dir = userIncludes.begin(), end = userIncludes.end(); dir != end; ++dir)
         if((in = TryOpenFile(pathname = *dir + filename))) break;
   }

   // System include directories.
   if(!in)
   {
      for(dir = sysIncludes.begin(), end = sysIncludes.end(); dir != end; ++dir)
         if((in = TryOpenFile(pathname = *dir + filename))) break;
   }

   if(!in) throw std::exception();

   NormalizePath(pathname);
   DirectoryPath(pathname);

   if(!pathname.empty())
   {
      doInclude = true;
      AddIncludeDir(pathname);
   }
}

//
// SourceStream::~SourceStream
//
SourceStream::~SourceStream()
{
   if(doInclude)
      PopIncludeDir();

   delete in;
}

//
// SourceStream::AddIncludeDir
//
void SourceStream::AddIncludeDir(std::string const &dir)
{
   AppendPath(autoIncludes, dir);
}

//
// SourceStream::AddIncludeDirSys
//
void SourceStream::AddIncludeDirSys(std::string const &dir)
{
   AppendPath(sysIncludes, dir);
}

//
// SourceStream::AddIncludeDirUser
//
void SourceStream::AddIncludeDirUser(std::string const &dir)
{
   AppendPath(userIncludes, dir);
}

//
// SourceStream::get
//
char SourceStream::get()
{
   if (!ungetStack.empty())
   {
      char c = ungetStack.top();
      ungetStack.pop();
      return c;
   }

   while (true)
   {
      oldC = curC;
      curC = newC != -2 ? newC : in->get();
      newC = in->get();

      if(!*in && curC < 0)
      {
         if(!inEOF && doPadEOF) {inEOF = true; return '\n';}
         throw EndOfStream();
      }


      // \t has special counting
      if (curC == '\t')
         countColumn += option_tab_columns.data;
      else
         ++countColumn;

      // \n end of line
      if (curC == '\n')
      {
         if(isInQuote())
            Error(SourcePosition(filename, countLine, countColumn), "unterminated string");

         inComment = false;
         countColumn = 0;
         ++countLine;
      }

      // ; line comment start
      if (curC == ';' && doCommentASM && !isInComment() && !isInQuote())
      {
         inComment = true;
      }

      // // line comment start?
      if (curC == '/' && newC == '/' && doCommentCPP && !isInComment() && !isInQuote())
      {
         inComment = true;
      }

      // /* comment start?
      if (curC == '/' && newC == '*' && doCommentC && !isInComment() && !isInQuote())
      {
         ++depthComment;
      }

      // */ commend end?
      if (curC == '*' && newC == '/' && doCommentC && !inComment && depthComment && !isInQuote())
      {
         --depthComment;

         // Convert multi-line comments into a space.
         curC = ' ';
         newC = -2;
      }

      // " double quote
      if (curC == '"' && doQuoteDouble && !isInComment() && !inQuoteSingle)
      {
         inQuoteDouble = !inQuoteDouble;
      }

      // " single quote
      if (curC == '\'' && doQuoteSingle && !isInComment() && !inQuoteDouble)
      {
         inQuoteSingle = !inQuoteSingle;
      }

      // \\n escaped newline
      if (curC == '\\' && newC == '\n')
      {
         countColumn = 0;
         ++countLine;

         curC = -2;
         newC = -2;
         continue;
      }


      // Comments are stripped.
      if (isInComment())
         continue;


      // Quoted string escape sequences.
      if (isInQuote() && curC == '\\')
      {
         int _newC = newC;
         newC = -2;
         ++countColumn;

         switch (_newC)
         {
         case 'a': curC = '\a'; break;
         case 'b': curC = '\b'; break;
         case 'c': curC = 0x1C; break;
         case 'f': curC = '\f'; break;
         case 'r': curC = '\r'; break;
         case 'n': curC = '\n'; break;
         case 't': curC = '\t'; break;
         case 'v': curC = '\v'; break;

         case '\n':
            countColumn = 0;
            ++countLine;
            curC = _newC;
            break;

         case '\t':
            countColumn += option_tab_columns.data-1;
         case '\\':
         case '\'':
         case '"':
         case ' ':
         case '?':
            curC = _newC;
            break;

            #define IORDIGIT() switch (in->get()) {  \
            case '0': curC <<= 3; curC |= 00; break; \
            case '1': curC <<= 3; curC |= 01; break; \
            case '2': curC <<= 3; curC |= 02; break; \
            case '3': curC <<= 3; curC |= 03; break; \
            case '4': curC <<= 3; curC |= 04; break; \
            case '5': curC <<= 3; curC |= 05; break; \
            case '6': curC <<= 3; curC |= 06; break; \
            case '7': curC <<= 3; curC |= 07; break; \
            default: in->unget();             break; }

         case '0': case '1': case '2': case '3':
         case '4': case '5': case '6': case '7':
            curC = 0; in->unget();
            IORDIGIT();
            IORDIGIT();
            IORDIGIT();
            break;

            #undef IORDIGIT

            #define IORDIGIT() switch (in->get()) {   \
            case '0': curC <<= 4; curC |= 0x0; break; \
            case '1': curC <<= 4; curC |= 0x1; break; \
            case '2': curC <<= 4; curC |= 0x2; break; \
            case '3': curC <<= 4; curC |= 0x3; break; \
            case '4': curC <<= 4; curC |= 0x4; break; \
            case '5': curC <<= 4; curC |= 0x5; break; \
            case '6': curC <<= 4; curC |= 0x6; break; \
            case '7': curC <<= 4; curC |= 0x7; break; \
            case '8': curC <<= 4; curC |= 0x8; break; \
            case '9': curC <<= 4; curC |= 0x9; break; \
            case 'A': curC <<= 4; curC |= 0xA; break; \
            case 'B': curC <<= 4; curC |= 0xB; break; \
            case 'C': curC <<= 4; curC |= 0xC; break; \
            case 'D': curC <<= 4; curC |= 0xD; break; \
            case 'E': curC <<= 4; curC |= 0xE; break; \
            case 'F': curC <<= 4; curC |= 0xF; break; \
            case 'a': curC <<= 4; curC |= 0xa; break; \
            case 'b': curC <<= 4; curC |= 0xb; break; \
            case 'c': curC <<= 4; curC |= 0xc; break; \
            case 'd': curC <<= 4; curC |= 0xd; break; \
            case 'e': curC <<= 4; curC |= 0xe; break; \
            case 'f': curC <<= 4; curC |= 0xf; break; \
            default: in->unget();              break; }

         case 'x':
            curC = 0;
            IORDIGIT();
            IORDIGIT();
            break;

            #undef IORDIGIT

         default:
            Error(SourcePosition(filename, countLine, countColumn),
                  "unknown escape character '\\%c'", _newC);
         }
      }


      return (char)curC;
   }
}

long SourceStream::getColumn() const
{
   return countColumn;
}

std::string const &SourceStream::getFilename() const
{
   return filename;
}

long SourceStream::getLineCount() const
{
   return countLine;
}

//
// SourceStream::Init
//
void SourceStream::Init(char const *arg0)
{
   std::string exepath = arg0;
   NormalizePath(exepath);
   DirectoryPath(exepath);

   if(!exepath.empty())
      TerminatePath(exepath);

   char pathSep[4] = "..";
   pathSep[2] = GetPathSep(exepath);

   for(int i = 4; i--; exepath += pathSep)
      AddIncludeDirSys(exepath + "inc");
}

bool SourceStream::is_HWS(char c)
{
   return c == ' ' || c == '\t';
}

bool SourceStream::isInComment() const
{
   return inComment || depthComment;
}

bool SourceStream::isInQuote() const
{
   return inQuoteDouble || inQuoteSingle;
}

//
// SourceStream::PopIncludeDir
//
void SourceStream::PopIncludeDir()
{
   autoIncludes.pop_back();
}

bool SourceStream::skipHWS()
{
   bool found(false);
   char c;

   while (is_HWS(c = get())) found = true;
   unget(c);

   return found;
}

void SourceStream::unget(char const c)
{
   ungetStack.push(c);
   oldC = -2;
   curC = -2;
}


