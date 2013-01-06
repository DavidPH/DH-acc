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
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectArchive
//
class ObjectArchive
{
public:
   explicit ObjectArchive(std::istream *_in) : in(_in), out(NULL) {}
   explicit ObjectArchive(std::ostream *_out) : in(NULL), out(_out) {}

   //
   // archiveBool
   //
   template<typename T> ObjectArchive &archiveBool(T &data)
   {
      if(isSaving())
         writeBool(static_cast<bool>(data));
      else
         data = static_cast<T>(readBool());

      return *this;
   }

   //
   // archiveChar
   //
   template<typename T> ObjectArchive &archiveChar(T &data)
   {
      if(isSaving())
         writeChar(static_cast<char>(data));
      else
         data = static_cast<T>(readChar());

      return *this;
   }

   //
   // archiveEnum
   //
   // Shortcut for the below version.
   //
   template<typename T> ObjectArchive &archiveEnum(T &data, T max)
   {
      return archiveEnum(data, max, max);
   }

   //
   // archiveEnum
   //
   template<typename T> ObjectArchive &archiveEnum(T &data, T max, T bad)
   {
      archiveUInt(data);

      if(isLoading())
      {
         if(data > max)
            data = bad;
      }

      return *this;
   }

   //
   // archiveNull
   //
   // Used to terminate strings that are preceded by a length.
   //
   ObjectArchive &archiveNull()
   {
      if(isSaving())
         out->put(0);
      else
         in->get();

      return *this;
   }

   //
   // archiveRange
   //
   template<typename Iter> ObjectArchive &archiveRange(Iter itr, Iter end)
   {
      while(itr != end) *this << *itr++;

      return *this;
   }

   //
   // archiveRange
   //
   template<typename Iter, typename T> ObjectArchive &archiveRange(Iter itr, Iter end, T size)
   {
      *this << size;

      return archiveRange(itr, end);
   }

   //
   // archiveReal
   //
   template<typename T> ObjectArchive &archiveReal(T &data)
   {
      if(isSaving())
         writeReal(static_cast<bigreal>(data));
      else
         data = static_cast<T>(readReal());

      return *this;
   }

   //
   // archiveSInt
   //
   template<typename T> ObjectArchive &archiveSInt(T &data)
   {
      if(isSaving())
         writeSInt(static_cast<bigsint>(data));
      else
         data = static_cast<T>(readSInt());

      return *this;
   }

   //
   // archiveUInt
   //
   template<typename T> ObjectArchive &archiveUInt(T &data)
   {
      if(isSaving())
         writeUInt(static_cast<biguint>(data));
      else
         data = static_cast<T>(readUInt());

      return *this;
   }

   bool isLoading() const {return in;}
   bool isSaving() const {return out;}

   ObjectArchive &operator << (bool &data)                   {return archiveBool(data);}
   ObjectArchive &operator << (char &data)                   {return archiveChar(data);}
   ObjectArchive &operator << (float &data)                  {return archiveReal(data);}
   ObjectArchive &operator << (double &data)                 {return archiveReal(data);}
   ObjectArchive &operator << (long double &data)            {return archiveReal(data);}
   ObjectArchive &operator << (signed char &data)            {return archiveSInt(data);}
   ObjectArchive &operator << (signed short int &data)       {return archiveSInt(data);}
   ObjectArchive &operator << (signed int &data)             {return archiveSInt(data);}
   ObjectArchive &operator << (signed long int &data)        {return archiveSInt(data);}
   ObjectArchive &operator << (signed long long int &data)   {return archiveSInt(data);}
   ObjectArchive &operator << (unsigned char &data)          {return archiveUInt(data);}
   ObjectArchive &operator << (unsigned short int &data)     {return archiveUInt(data);}
   ObjectArchive &operator << (unsigned int &data)           {return archiveUInt(data);}
   ObjectArchive &operator << (unsigned long int &data)      {return archiveUInt(data);}
   ObjectArchive &operator << (unsigned long long int &data) {return archiveUInt(data);}

   bool    readBool();
   char    readChar();
   bigreal readReal();
   bigsint readSInt();
   biguint readUInt();

   void writeBool(bool    data);
   void writeChar(char    data);
   void writeReal(bigreal data);
   void writeSInt(bigsint data);
   void writeUInt(biguint data);

private:
   std::istream *const in;
   std::ostream *const out;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// operator ObjectArchive << std::basic_string
//
template<typename T>
ObjectArchive &operator << (ObjectArchive &arc, std::basic_string<T> &data)
{
   typename std::basic_string<T>::size_type size;

   if(arc.isSaving())
   {
      arc.archiveRange(data.begin(), data.end(), data.size());
   }
   else
   {
      arc << size;

      data.resize(size);

      arc.archiveRange(data.begin(), data.end());
   }

   return arc.archiveNull();
}

//
// operator ObjectArchive << std::map
//
template<typename Tk, typename Tv>
ObjectArchive &operator << (ObjectArchive &arc, std::map<Tk, Tv> &data)
{
   typename std::map<Tk, Tv>::iterator  itr, end;
   typename std::map<Tk, Tv>::size_type size;

   if(arc.isSaving())
   {
      arc.archiveRange(data.begin(), data.end(), data.size());
   }
   else
   {
      arc << size;

      while(size--)
      {
         std::pair<Tk, Tv> o;
         arc << o;

         if((itr = data.find(o.first)) == data.end())
            data.insert(o);
         else
            OA_Override(itr->second, o.second);
      }
   }

   return arc;
}

//
// operator ObjectArchive << std::pair
//
template<typename Tk, typename Tv>
ObjectArchive &operator << (ObjectArchive &arc, std::pair<Tk, Tv> &data)
{
   return arc << data.first << data.second;
}

template<typename Tk, typename Tv>
ObjectArchive &operator << (ObjectArchive &arc, std::pair<Tk const, Tv> &data)
{
   if(arc.isLoading())
      throw __FILE__ ": loading pair with const";

   return arc << const_cast<Tk &>(data.first) << data.second;
}

//
// operator ObjectArchive << std::set
//
template<typename T>
ObjectArchive &operator << (ObjectArchive &arc, std::set<T> &data)
{
   typename std::set<T>::size_type size;

   if(arc.isSaving())
   {
      arc.writeUInt(data.size());

      for(T const &itr : data)
         arc << const_cast<T &>(itr);
   }
   else
   {
      arc << size;

      while(size--)
      {
         T o;
         arc << o;
         data.insert(o);
      }
   }

   return arc;
}

//
// operator ObjectArchive << std::vector
//
template<typename T>
ObjectArchive &operator << (ObjectArchive &arc, std::vector<T> &data)
{
   typename std::vector<T>::size_type size, sizeRead;

   if(arc.isSaving())
   {
      arc.archiveRange(data.begin(), data.end(), data.size());
   }
   else
   {
      size = data.size();

      arc << sizeRead;

      data.resize(size + sizeRead);

      arc.archiveRange(data.begin() + size, data.end());
   }

   return arc;
}

#endif//HPP_ObjectArchive_

