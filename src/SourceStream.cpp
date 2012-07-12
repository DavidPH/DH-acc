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
// Static Variables                                                           |
//

static option::option_data<std::vector<std::string> > option_include_dir
('i', "include-dir", "input",
 "Specifies a directory to search for includes in.", NULL,
 std::vector<std::string>(1));

static option::option_data<int> option_tab_columns
('\0', "tab-columns", "input",
 "How many columns a tab counts for in error reporting.", NULL, 1);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceStream::SourceStream
//
SourceStream::SourceStream(std::string const &_filename, unsigned type)
 :
oldC(-2), curC(-2), newC(-2),
in(NULL),
filename(_filename),

countColumn(0),
countLine(1),

depthComment(0),

inComment(false),

inEOF(false),

inQuoteDouble(false),
inQuoteSingle(false),

doPadEOF(true)
{
   switch (type & ST_MASK)
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

   if (type & STF_STRING)
   {
      doPadEOF = false;
      in = new std::istringstream(filename);
      filename = "string";
   }

   if (!in)
   {
      std::vector<std::string>::iterator dir;
      for (dir  = option_include_dir.data.begin();
           dir != option_include_dir.data.end() && !in; ++dir)
      {
         in = new std::ifstream((*dir + filename).c_str());

         if (!*in)
         {
            delete in;
            in = NULL;
         }
      }
   }

   if (!in) throw std::exception();
}
SourceStream::~SourceStream()
{
   delete in;
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
            ERROR(SourcePosition(filename, countLine, countColumn), "unterminated string");

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
            case '0': curC <<= 4; curC |= 00; break; \
            case '1': curC <<= 4; curC |= 01; break; \
            case '2': curC <<= 4; curC |= 02; break; \
            case '3': curC <<= 4; curC |= 03; break; \
            case '4': curC <<= 4; curC |= 04; break; \
            case '5': curC <<= 4; curC |= 05; break; \
            case '6': curC <<= 4; curC |= 06; break; \
            case '7': curC <<= 4; curC |= 07; break; \
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
            ERROR(SourcePosition(filename, countLine, countColumn),
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


