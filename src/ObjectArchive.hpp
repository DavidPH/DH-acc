//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Object-format I/O.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectArchive_
#define HPP_ObjectArchive_

#include "bignum.hpp"

#include <istream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectLoad
//
class ObjectLoad
{
public:
   explicit ObjectLoad(std::istream &load_) : load{load_} {}

   ObjectLoad &operator >> (bool &data)                   {return loadBool(data);}
   ObjectLoad &operator >> (char &data)                   {return loadChar(data);}
   ObjectLoad &operator >> (float &data)                  {return loadReal(data);}
   ObjectLoad &operator >> (double &data)                 {return loadReal(data);}
   ObjectLoad &operator >> (long double &data)            {return loadReal(data);}
   ObjectLoad &operator >> (signed char &data)            {return loadSInt(data);}
   ObjectLoad &operator >> (signed short int &data)       {return loadSInt(data);}
   ObjectLoad &operator >> (signed int &data)             {return loadSInt(data);}
   ObjectLoad &operator >> (signed long int &data)        {return loadSInt(data);}
   ObjectLoad &operator >> (signed long long int &data)   {return loadSInt(data);}
   ObjectLoad &operator >> (unsigned char &data)          {return loadUInt(data);}
   ObjectLoad &operator >> (unsigned short int &data)     {return loadUInt(data);}
   ObjectLoad &operator >> (unsigned int &data)           {return loadUInt(data);}
   ObjectLoad &operator >> (unsigned long int &data)      {return loadUInt(data);}
   ObjectLoad &operator >> (unsigned long long int &data) {return loadUInt(data);}

   template<typename T> T           loadBits() {T data; *this >> data; return data;}
   template<typename T> T           loadBits(T const &) {T data; *this >> data; return data;}
   template<typename T> ObjectLoad &loadBool(T &data);
   template<typename T> ObjectLoad &loadChar(T &data);
   template<typename T> ObjectLoad &loadEnum(T &data, T max) {return loadEnum(data, max, max);}
   template<typename T> ObjectLoad &loadEnum(T &data, T max, T bad);
                        ObjectLoad &loadNull() {load.get(); return *this;}
   template<typename T> ObjectLoad &loadRange(T begin, T end);
   template<typename T> ObjectLoad &loadReal(T &data);
   template<typename T> ObjectLoad &loadSInt(T &data);
   template<typename T> ObjectLoad &loadSkip() {T data; return *this >> data;}
   template<typename T> ObjectLoad &loadSkip(T const &) {T data; return *this >> data;}
   template<typename T> ObjectLoad &loadUInt(T &data);

private:
   bool    loadPrimBool();
   char    loadPrimChar();
   bigreal loadPrimReal();
   bigsint loadPrimSInt();
   biguint loadPrimUInt();

   std::istream &load;
};

//
// ObjectSave
//
class ObjectSave
{
public:
   explicit ObjectSave(std::ostream &save_) : save{save_} {}

   ObjectSave &operator << (bool const &data)                   {return saveBool(data);}
   ObjectSave &operator << (char const &data)                   {return saveChar(data);}
   ObjectSave &operator << (float const &data)                  {return saveReal(data);}
   ObjectSave &operator << (double const &data)                 {return saveReal(data);}
   ObjectSave &operator << (long double const &data)            {return saveReal(data);}
   ObjectSave &operator << (signed char const &data)            {return saveSInt(data);}
   ObjectSave &operator << (signed short int const &data)       {return saveSInt(data);}
   ObjectSave &operator << (signed int const &data)             {return saveSInt(data);}
   ObjectSave &operator << (signed long int const &data)        {return saveSInt(data);}
   ObjectSave &operator << (signed long long int const &data)   {return saveSInt(data);}
   ObjectSave &operator << (unsigned char const &data)          {return saveUInt(data);}
   ObjectSave &operator << (unsigned short int const &data)     {return saveUInt(data);}
   ObjectSave &operator << (unsigned int const &data)           {return saveUInt(data);}
   ObjectSave &operator << (unsigned long int const &data)      {return saveUInt(data);}
   ObjectSave &operator << (unsigned long long int const &data) {return saveUInt(data);}

   template<typename T> ObjectSave &saveBool(T const &data);
   template<typename T> ObjectSave &saveChar(T const &data);
   template<typename T> ObjectSave &saveEnum(T const &data);
                        ObjectSave &saveNull() {save.put(0); return *this;}
   template<typename T> ObjectSave &saveRange(T begin, T end);
   template<typename T> ObjectSave &saveReal(T const &data);
   template<typename T> ObjectSave &saveSInt(T const &data);
   template<typename T> ObjectSave &saveUInt(T const &data);

private:
   void savePrimBool(bool    data);
   void savePrimChar(char    data);
   void savePrimReal(bigreal data);
   void savePrimSInt(bigsint data);
   void savePrimUInt(biguint data);

   std::ostream &save;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

template<typename T, std::size_t N>
ObjectSave &operator << (ObjectSave &arc, T const (&data)[N]);

template<typename T>
ObjectSave &operator << (ObjectSave &arc, std::basic_string<T> const &data);

template<typename Tk, typename Tv>
ObjectSave &operator << (ObjectSave &arc, std::map<Tk, Tv> const &data);

template<typename Tk, typename Tv>
ObjectSave &operator << (ObjectSave &arc, std::pair<Tk, Tv> const &data);

template<typename T>
ObjectSave &operator << (ObjectSave &arc, std::set<T> const &data);

template<typename Tk, typename Tv>
ObjectSave &operator << (ObjectSave &arc, std::unordered_map<Tk, Tv> const &data);

template<typename T>
ObjectSave &operator << (ObjectSave &arc, std::vector<T> const &data);

template<typename T, std::size_t N>
ObjectLoad &operator >> (ObjectLoad &arc, T (&data)[N]);

template<typename T>
ObjectLoad &operator >> (ObjectLoad &arc, std::basic_string<T> &data);

template<typename Tk, typename Tv>
ObjectLoad &operator >> (ObjectLoad &arc, std::map<Tk, Tv> &data);

template<typename Tk, typename Tv>
ObjectLoad &operator >> (ObjectLoad &arc, std::pair<Tk, Tv> &data);

template<typename T>
ObjectLoad &operator >> (ObjectLoad &arc, std::set<T> &data);

template<typename Tk, typename Tv>
ObjectLoad &operator >> (ObjectLoad &arc, std::unordered_map<Tk, Tv> &data);

template<typename T>
ObjectLoad &operator >> (ObjectLoad &arc, std::vector<T> &data);

template<typename T>
ObjectLoad &OA_LoadEmplace(ObjectLoad &load, std::vector<T> &data);

void OA_Override(bigsint &out, bigsint const &in);

//
// ObjectLoad::loadBool
//
template<typename T>
ObjectLoad &ObjectLoad::loadBool(T &data)
{
   data = static_cast<T>(loadPrimBool());
   return *this;
}

//
// ObjectLoad::loadChar
//
template<typename T>
ObjectLoad &ObjectLoad::loadChar(T &data)
{
   data = static_cast<T>(loadPrimChar());
   return *this;
}

//
// ObjectLoad::loadEnum
//
template<typename T>
ObjectLoad &ObjectLoad::loadEnum(T &data, T max, T bad)
{
   loadUInt(data);

   if(data > max)
      data = bad;

   return *this;
}

//
// ObjectLoad::loadRange
//
template<typename Iter>
ObjectLoad &ObjectLoad::loadRange(Iter itr, Iter end)
{
   for(; itr != end; ++itr)
      *this >> *itr;

   return *this;
}

//
// ObjectLoad::loadReal
//
template<typename T>
ObjectLoad &ObjectLoad::loadReal(T &data)
{
   data = static_cast<T>(loadPrimReal());
   return *this;
}

//
// ObjectLoad::loadSInt
//
template<typename T>
ObjectLoad &ObjectLoad::loadSInt(T &data)
{
   data = static_cast<T>(loadPrimSInt());
   return *this;
}

//
// ObjectLoad::loadUInt
//
template<typename T>
ObjectLoad &ObjectLoad::loadUInt(T &data)
{
   data = static_cast<T>(loadPrimUInt());
   return *this;
}

//
// ObjectSave::saveBool
//
template<typename T>
ObjectSave &ObjectSave::saveBool(T const &data)
{
   savePrimBool(static_cast<bool>(data));
   return *this;
}

//
// ObjectSave::saveChar
//
template<typename T>
ObjectSave &ObjectSave::saveChar(T const &data)
{
   savePrimChar(static_cast<char>(data));
   return *this;
}

//
// ObjectSave::saveEvum
//
template<typename T>
ObjectSave &ObjectSave::saveEnum(T const &data)
{
   return saveUInt(data);
}

//
// ObjectSave::saveRange
//
template<typename Iter>
ObjectSave &ObjectSave::saveRange(Iter itr, Iter end)
{
   for(; itr != end; ++itr)
      *this << *itr;

   return *this;
}

//
// ObjectSave::saveReal
//
template<typename T>
ObjectSave &ObjectSave::saveReal(T const &data)
{
   savePrimReal(static_cast<bigreal>(data));
   return *this;
}

//
// ObjectSave::saveSInt
//
template<typename T>
ObjectSave &ObjectSave::saveSInt(T const &data)
{
   savePrimSInt(static_cast<bigsint>(data));
   return *this;
}

//
// ObjectSave::saveUInt
//
template<typename T>
ObjectSave &ObjectSave::saveUInt(T const &data)
{
   savePrimUInt(static_cast<biguint>(data));
   return *this;
}

//
// operator ObjectSave << T[N]
//
template<typename T, std::size_t N>
ObjectSave &operator << (ObjectSave &arc, T const (&data)[N])
{
   for(T const &d : data)
      arc << d;

   return arc;
}

//
// operator ObjectSave << std::basic_string
//
template<typename T>
ObjectSave &operator << (ObjectSave &arc, std::basic_string<T> const &data)
{
   arc << data.size();
   arc.saveRange(data.begin(), data.end());
   arc.saveNull();

   return arc;
}

//
// operator ObjectSave << std::map
//
template<typename Tk, typename Tv>
ObjectSave &operator << (ObjectSave &arc, std::map<Tk, Tv> const &data)
{
   arc << data.size();
   arc.saveRange(data.begin(), data.end());

   return arc;
}

//
// operator ObjectSave << std::pair
//
template<typename Tk, typename Tv>
ObjectSave &operator << (ObjectSave &arc, std::pair<Tk, Tv> const &data)
{
   return arc << data.first << data.second;
}

//
// operator ObjectSave << std::set
//
template<typename T>
ObjectSave &operator << (ObjectSave &arc, std::set<T> const &data)
{
   arc << data.size();
   arc.saveRange(data.begin(), data.end());

   return arc;
}

//
// operator ObjectSave << std::unordered_map
//
template<typename Tk, typename Tv>
ObjectSave &operator << (ObjectSave &arc, std::unordered_map<Tk, Tv> const &data)
{
   arc << data.size();
   arc.saveRange(data.begin(), data.end());

   return arc;
}

//
// operator ObjectSave << std::vector
//
template<typename T>
ObjectSave &operator << (ObjectSave &arc, std::vector<T> const &data)
{
   arc << data.size();
   arc.saveRange(data.begin(), data.end());

   return arc;
}

//
// operator ObjectLoad >> T[N]
//
template<typename T, std::size_t N>
ObjectLoad &operator >> (ObjectLoad &arc, T (&data)[N])
{
   for(T &d : data)
      arc >> d;

   return arc;
}

//
// operator ObjectLoad >> std::basic_string
//
template<typename T>
ObjectLoad &operator >> (ObjectLoad &arc, std::basic_string<T> &data)
{
   typename std::basic_string<T>::size_type size;

   arc >> size;
   data.resize(size);
   arc.loadRange(data.begin(), data.end());
   arc.loadNull();

   return arc;
}

//
// operator ObjectLoad >> std::map
//
template<typename Tk, typename Tv>
ObjectLoad &operator >> (ObjectLoad &arc, std::map<Tk, Tv> &data)
{
   typename std::map<Tk, Tv>::iterator  itr;
   typename std::map<Tk, Tv>::size_type size;

   arc >> size;

   while(size--)
   {
      std::pair<Tk, Tv> o;
      arc >> o;

      if((itr = data.find(o.first)) == data.end())
         data.insert(std::move(o));
      else
         OA_Override(itr->second, o.second);
   }

   return arc;
}

//
// operator ObjectLoad >> std::pair
//
template<typename Tk, typename Tv>
ObjectLoad &operator >> (ObjectLoad &arc, std::pair<Tk, Tv> &data)
{
   return arc >> data.first >> data.second;
}

//
// operator ObjectLoad >> std::set
//
template<typename T>
ObjectLoad &operator >> (ObjectLoad &arc, std::set<T> &data)
{
   typename std::set<T>::size_type size;

   arc >> size;

   while(size--)
   {
      T o;
      arc >> o;
      data.insert(std::move(o));
   }

   return arc;
}

//
// operator ObjectLoad >> std::unordered_map
//
template<typename Tk, typename Tv>
ObjectLoad &operator >> (ObjectLoad &arc, std::unordered_map<Tk, Tv> &data)
{
   typename std::unordered_map<Tk, Tv>::iterator  itr;
   typename std::unordered_map<Tk, Tv>::size_type size;

   arc >> size;

   while(size--)
   {
      std::pair<Tk, Tv> o;
      arc >> o;

      if((itr = data.find(o.first)) == data.end())
         data.insert(std::move(o));
      else
         OA_Override(itr->second, o.second);
   }

   return arc;
}

//
// operator ObjectLoad >> std::vector
//
template<typename T>
ObjectLoad &operator >> (ObjectLoad &arc, std::vector<T> &data)
{
   typename std::vector<T>::size_type sizeBase, sizeLoad;

   sizeBase = data.size();
   arc >> sizeLoad;
   data.resize(sizeBase + sizeLoad);
   arc.loadRange(data.begin() + sizeBase, data.end());

   return arc;
}

//
// OA_LoadEmplace std::vector
//
template<typename T>
ObjectLoad &OA_LoadEmplace(ObjectLoad &load, std::vector<T> &data)
{
   typename std::vector<T>::size_type sizeBase, sizeLoad;

   sizeBase = data.size();
   load >> sizeLoad;
   data.reserve(sizeBase + sizeLoad);
   while(sizeLoad--) data.emplace_back(load);

   return load;
}

#endif//HPP_ObjectArchive_

