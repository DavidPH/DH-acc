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

#include "Keyword.hpp"

#include "ObjectArchive.hpp"

#include <cstring>
#include <unordered_map>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ContextKeyData
//
struct ContextKeyData
{
   ContextKeyData(std::size_t base_, std::size_t kwrd_, std::size_t num_) :
      base{base_}, kwrd{kwrd_}, num{num_} {}

   std::size_t const base;
   std::size_t const kwrd;
   std::size_t const num;
};

//
// KeywordData
//
struct KeywordData
{
   KeywordData(std::string const &str_, std::size_t num_) : str{str_}, num{num_} {}

   std::string const str;
   std::size_t const num;
};


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static std::vector<ContextKeyData> ContextKeyVec =
{
   {0, 0, 0},
};

static std::vector<KeywordData> KeywordVec =
{
   {"", KWRD},
   #define KWRD(NAME) {#NAME, KWRD_##NAME},
   #include "KWRD.hpp"
};

static std::unordered_map<std::string, std::size_t> KeywordMap =
{
   #define KWRD(NAME) {#NAME, KWRD_##NAME},
   #include "KWRD.hpp"
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ContextKey::getBase
//
ContextKey ContextKey::getBase() const
{
   return ContextKey(ContextKeyVec[ctxk].base);
}

//
// ContextKey::getKeyword
//
Keyword ContextKey::getKeyword() const
{
   return Keyword(ContextKeyVec[ctxk].kwrd);
}

//
// ContextKey::Get
//
ContextKey ContextKey::Get(ContextKey base, Keyword kwrd)
{
   for(auto const &ctxk : ContextKeyVec)
      if(ctxk.base == base.ctxk && ctxk.kwrd == kwrd.kwrd)
         return ContextKey(&ctxk - ContextKeyVec.data());

   std::size_t num = ContextKeyVec.size();
   ContextKeyVec.emplace_back(base.ctxk, kwrd.kwrd, num);
   return ContextKey(num);
}

//
// ContextKey::Get
//
ContextKey ContextKey::Get(Keyword kwrd)
{
   for(auto const &ctxk : ContextKeyVec)
      if(ctxk.base == 0 && ctxk.kwrd == kwrd.kwrd)
         return ContextKey(&ctxk - ContextKeyVec.data());

   std::size_t num = ContextKeyVec.size();
   ContextKeyVec.emplace_back(0, kwrd.kwrd, num);
   return ContextKey(num);
}

//
// Keyword::getString
//
std::string const &Keyword::getString() const
{
   return KeywordVec[kwrd].str;
}

//
// Keyword::Get
//
Keyword Keyword::Get(std::string const &str)
{
   auto kwrd = KeywordMap.find(str);
   if(kwrd != KeywordMap.end()) return Keyword(kwrd->second);

   std::size_t num = KeywordVec.size();
   KeywordVec.emplace_back(str, num);
   KeywordMap.emplace     (str, num);
   return Keyword(num);
}

//
// operator ObjectSave << ContextKey
//
ObjectSave &operator << (ObjectSave &save, ContextKey const &data)
{
   if(auto ctxk = data.getBase())
      save << "ctxk" << ctxk;
   else
      save << "kwrd";

   return save << data.getKeyword();
}

//
// operator ObjectSave << Keyword
//
ObjectSave &operator << (ObjectSave &save, Keyword const &data)
{
   return save << KeywordVec[data.kwrd].str;
}

//
// operator ObjectSave << KeywordIndex
//
ObjectSave &operator << (ObjectSave &save, KeywordIndex const &data)
{
   return save << KeywordVec[data].str;
}

//
// operator ObjectLoad >> ContextKey
//
ObjectLoad &operator >> (ObjectLoad &load, ContextKey &data)
{
   char type[5];

   load >> type;

   if(!std::memcmp(type, "ctxk", 5))
   {
      ContextKey ctxk;
      Keyword kwrd;
      load >> ctxk >> kwrd;
      data = ContextKey::Get(ctxk, kwrd);
   }
   else
   {
      Keyword kwrd;
      load >> kwrd;
      data = ContextKey::Get(kwrd);
   }

   return load;
}

//
// operator ObjectLoad >> Keyword
//
ObjectLoad &operator >> (ObjectLoad &load, Keyword &data)
{
   std::string kwrd;
   load >> kwrd;
   data = Keyword::Get(kwrd);

   return load;
}

// EOF

