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

SourceStream::SourceStream(std::string const &_filename, unsigned type)
 :
oldC(-2), curC(-2), newC(-2),
in(NULL),
filename(_filename),

countColumn(0),
countLine(1),

depthComment(0),

inComment(false),

inQuoteDouble(false),
inQuoteSingle(false)
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


      // \t has special counting
      if (curC == '\t')
         countColumn += option_tab_columns.data;
      else
         ++countColumn;

      // \n end of line
      if (curC == '\n')
      {
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



      // Comments are stripped.
      if (isInComment())
         continue;

      // End of multi-line comments are stripped, unless quoted.
      if (((curC == '*' && newC == '/') || (oldC == '*' && curC == '/')) && !isInQuote())
         continue;



      // Quoted string escape sequences.
      if (isInQuote() && curC == '\\')
      {
         int _newC = newC;
         newC = -2;
         ++countColumn;

         switch (_newC)
         {
         case 'r': curC = '\r'; break;
         case 'n': curC = '\n'; break;
         case 't': curC = '\t'; break;

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
            curC = _newC;
            break;

         // TODO: \xXX

         default:
            newC = _newC;
            break;
         }
      }


      if (!*in) throw EndOfStream();
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


