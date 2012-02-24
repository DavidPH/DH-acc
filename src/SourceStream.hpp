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

#ifndef HPP_SourceStream_
#define HPP_SourceStream_

#include <istream>
#include <stack>
#include <stdexcept>
#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceStream
//
class SourceStream
{
public:
   //
   // ::EndOfStream
   //
   // Thrown when attempting to read past the end of the stream.
   //
   class EndOfStream : public std::exception
   {
   };

   enum SourceType
   {
      ST_ASMPLX,
      ST_C,

      ST_MASK = 0xFF,

      STF_STRING = 0x100
   };


   SourceStream();
   SourceStream(std::string const &filename, unsigned type);
   ~SourceStream();

   char get();

   long getColumn() const;
   std::string const &getFilename() const;
   long getLineCount() const;

   bool isInComment() const;
   bool isInQuote() const;

   // Skips horizontal whitespace, returning true if any found.
   bool skipHWS();

   void unget(char const c);


   static bool is_HWS(char c);

private:
   int oldC, curC, newC;
   std::istream *in;
   std::string filename;
   std::stack<char> ungetStack;

   long countColumn;
   long countLine;

   long depthComment;

   bool inComment : 1; // Single-line comment.

   bool inQuoteDouble : 1; // "
   bool inQuoteSingle : 1; // '

   bool doCommentASM : 1; // ;
   bool doCommentC   : 1; // /* */
   bool doCommentCPP : 1; // //

   bool doQuoteDouble : 1; // "
   bool doQuoteSingle : 1; // '
};



#endif /* HPP_SourceStream_ */


