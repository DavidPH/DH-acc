//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Keyword/name table.
//
//-----------------------------------------------------------------------------

#ifndef Keyword_H__
#define Keyword_H__

#include <functional>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// KeywordIndex
//
enum KeywordIndex
{
   KWRD,
};

//
// Keyword
//
class Keyword
{
public:
   constexpr Keyword(KeywordIndex kwrd_ = KWRD) : kwrd{kwrd_} {}

   explicit constexpr operator bool () {return kwrd;}

   std::string const &getString() const;

   std::size_t kwrd;
};

//
// ContextKey
//
class ContextKey
{
public:
   constexpr ContextKey(KeywordIndex kwrd = KWRD) : ctxk{kwrd} {}

   explicit constexpr operator bool () {return ctxk;}

   ContextKey getBase() const;
   Keyword getKeyword() const;

   std::size_t ctxk;
};

//
// std::hash<ContextKey>
//
template<> struct std::hash<ContextKey>
{
   constexpr size_t operator () (ContextKey const &ctxk) {return ctxk.ctxk;}
};

//
// std::hash<Keyword>
//
template<> struct std::hash<Keyword>
{
   constexpr size_t operator () (Keyword const &kwrd) {return kwrd.kwrd;}
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// operator ContextKey == ContextKey
//
constexpr bool operator == (ContextKey const &l, ContextKey const &r)
{
   return l.ctxk == r.ctxk;
}

//
// operator Keyword == Keyword
//
constexpr bool operator == (Keyword const &l, Keyword const &r)
{
   return l.kwrd == r.kwrd;
}

#endif//Keyword_H__

