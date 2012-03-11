//-----------------------------------------------------------------------------
//
// Copyright(C) 2010, 2011, 2012 David Hill
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
// Reference counting.
//
//-----------------------------------------------------------------------------

#ifndef COUNTER_HPP__
#define COUNTER_HPP__


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define MAKE_COMMON_COUNTER_CLASS(CLASS)                  \
public:                                                   \
virtual char const *getClassName() const {return #CLASS;} \
typedef CounterPointer<CLASS> Pointer;                    \
typedef CounterPointer<CLASS const> ConstPointer;         \
friend class CounterPointer<CLASS>;                       \
friend class CounterPointer<CLASS const>;                 \
typedef CounterReference<CLASS> Reference;                \
typedef CounterReference<CLASS const> ConstReference;     \
friend class CounterReference<CLASS>;                     \
friend class CounterReference<CLASS const>

//
// MAKE_ABSTRACT_COUNTER_CLASS
//
// For use in defining abstract reference-counted classes.
//
#define MAKE_ABSTRACT_COUNTER_CLASS(CLASS)               \
private:                                                 \
virtual CLASS *cloneRaw() const = 0;                     \
public:                                                  \
CounterPointer<CLASS> clone() const {return cloneRaw();} \
MAKE_COMMON_COUNTER_CLASS(CLASS)

//
// MAKE_ABSTRACT_COUNTER_CLASS_BASE
//
#define MAKE_ABSTRACT_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_ABSTRACT_COUNTER_CLASS(CLASS);                  \
typedef BASE Super

//
// MAKE_NOCLONE_COUNTER_CLASS
//
// For use in definining un-copy-able reference-counted classes.
//
#define MAKE_NOCLONE_COUNTER_CLASS(CLASS)                \
private:                                                 \
virtual CLASS *cloneRaw() const {throw 1;}               \
public:                                                  \
CounterPointer<CLASS> clone() const {return cloneRaw();} \
MAKE_COMMON_COUNTER_CLASS(CLASS)

//
// MAKE_NOCLONE_COUNTER_CLASS_BASE
//
#define MAKE_NOCLONE_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_NOCLONE_COUNTER_CLASS(CLASS);                  \
typedef BASE Super

//
// MAKE_COUNTER_CLASS
//
#define MAKE_COUNTER_CLASS(CLASS)                          \
private:                                                   \
virtual CLASS *cloneRaw() const {return new CLASS(*this);} \
public:                                                    \
CounterPointer<CLASS> clone() const {return cloneRaw();}   \
MAKE_COMMON_COUNTER_CLASS(CLASS)

//
// MAKE_COUNTER_CLASS_BASE
//
#define MAKE_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_COUNTER_CLASS(CLASS);                  \
typedef BASE Super


//----------------------------------------------------------------------------|
// Types                                                                      |
//

template<typename T>
class CounterReference;

//
// CounterPointer
//
// Reference-counting pointer.
//
template<typename T>
class CounterPointer
{
public:
   //
   // ::CounterPointer
   //
   CounterPointer()
                  : p(0)
   {
   }

   //
   // ::CounterPointer
   //
   CounterPointer(T *_p)
                  : p(_p)
   {
      if (p) ++p->refCount;
   }

   //
   // ::CounterPointer
   //
   template<typename T2>
   CounterPointer(T2 *_p)
                  : p(static_cast<T*>(_p))
   {
      if (p) ++p->refCount;
   }

   //
   // ::CounterPointer
   //
   CounterPointer(CounterPointer<T> const &_p)
                  : p(_p.p)
   {
      if (p) ++p->refCount;
   }

   //
   // ::CounterPointer
   //
   template<typename T2>
   CounterPointer(CounterPointer<T2> const &_p)
                  : p(static_cast<T*>(_p))
   {
      if (p) ++p->refCount;
   }

   //
   // ::CounterPointer
   //
   CounterPointer(CounterReference<T> const &_p)
                    : p(static_cast<T*>(_p))
   {
      ++p->refCount;
   }

   //
   // ::CounterPointer
   //
   template<typename T2>
   CounterPointer(CounterReference<T2> const &_p)
                    : p(static_cast<T*>(_p))
   {
      ++p->refCount;
   }

   //
   // ::~CounterPointer
   //
   ~CounterPointer()
   {
      if (p && !--p->refCount)
         delete p;
   }

   //
   // ::operator T *
   //
   operator T * () const
   {
      return p;
   }

   //
   // ::operator =
   //
   CounterPointer<T> &operator = (T *_p)
   {
      T *old = p;

      p = _p;

      if (p) ++p->refCount;

      if (old && !--old->refCount)
         delete old;

      return *this;
   }

   //
   // ::operator =
   //
   template<typename T2>
   CounterPointer<T> &operator = (T2 *_p)
   {
      return *this = static_cast<T *>(_p);
   }

   //
   // ::operator =
   //
   CounterPointer<T> &operator = (CounterPointer<T> const &_p)
   {
      return *this = static_cast<T *>(_p);
   }

   //
   // ::operator =
   //
   template<typename T2>
   CounterPointer<T> &operator = (CounterPointer<T2> const &_p)
   {
      return *this = static_cast<T *>(_p);
   }

   //
   // ::operator =
   //
   CounterPointer<T> &operator = (CounterReference<T> const &_p)
   {
      T *old = p;

      p = static_cast<T *>(_p);

      ++p->refCount;

      if (old && !--old->refCount)
         delete old;

      return *this;
   }

   //
   // ::operator =
   //
   template<typename T2>
   CounterPointer<T> &operator = (CounterReference<T2> const &_p)
   {
      T *old = p;

      p = static_cast<T *>(_p);

      ++p->refCount;

      if (old && !--old->refCount)
         delete old;

      return *this;
   }

   //
   // ::operator ->
   //
   T *operator -> () const
   {
      return p;
   }

   //
   // ::operator *
   //
   T &operator * () const
   {
      return *p;
   }

   //
   // ::raw
   //
   T *raw() const
   {
      return p;
   }

   //
   // ::ref
   //
   CounterReference<T> ref() const
   {
      return CounterReference<T>(p);
   }

   //
   // ::refCount
   //
   unsigned refCount() const
   {
      return p ? p->refCount : 0;
   }

private:
   T *p;
};

//
// CounterReference
//
// Reference-counting pointer that is never null.
//
template<typename T>
class CounterReference
{
public:
   //
   // ::CounterReference
   //
   explicit CounterReference(T *_p)
                             : p(_p)
   {
      ++p->refCount;
   }

   //
   // ::CounterReference
   //
   template<typename T2>
   explicit CounterReference(T2 *_p)
                             : p(static_cast<T*>(_p))
   {
      ++p->refCount;
   }

   //
   // ::CounterReference
   //
   explicit CounterReference(CounterPointer<T> const &_p)
                             : p(static_cast<T*>(_p))
   {
      ++p->refCount;
   }

   //
   // ::CounterReference
   //
   template<typename T2>
   explicit CounterReference(CounterPointer<T2> const &_p)
                             : p(static_cast<T*>(_p))
   {
      ++p->refCount;
   }

   //
   // ::CounterReference
   //
   CounterReference(CounterReference<T> const &_p)
                    : p(_p.p)
   {
      ++p->refCount;
   }

   //
   // ::CounterReference
   //
   template<typename T2>
   CounterReference(CounterReference<T2> const &_p)
                    : p(static_cast<T*>(_p))
   {
      ++p->refCount;
   }

   //
   // ::~CounterReference
   //
   ~CounterReference()
   {
      if (!--p->refCount)
         delete p;
   }

   //
   // ::operator T *
   //
   operator T * () const
   {
      return p;
   }

   //
   // ::operator =
   //
   CounterReference<T> &operator = (CounterReference<T> const &_p)
   {
      T *old = p;

      p = _p.p;

      ++p->refCount;

      if (!--old->refCount)
         delete old;

      return *this;
   }

   //
   // ::operator =
   //
   template<typename T2>
   CounterReference<T> &operator = (CounterReference<T2> const &_p)
   {
      return *this = CounterReference<T>(_p);
   }

   //
   // ::operator ->
   //
   T *operator -> () const
   {
      return p;
   }

   //
   // ::operator *
   //
   T &operator * () const
   {
      return *p;
   }

   //
   // ::refCount
   //
   unsigned refCount() const
   {
      return p->refCount;
   }

private:
   T *p;
};

//
// Counter
//
// Reference-counted base class.
//
class Counter
{
   MAKE_COUNTER_CLASS(Counter);

public:
   //
   // ::~Counter
   //
   virtual ~Counter()
   {
   }

   //
   // ::operator =
   //
   Counter &operator = (Counter const &)
   {
      return *this;
   }

protected:
   //
   // ::Counter
   //
   Counter()
           : refCount(0)
   {
   }

   //
   // ::Counter
   //
   Counter(Counter const &)
           : refCount(0)
   {
   }

   // Protected so that derived classes can use CounterPointer.
   // Mutable for CounterPointer-to-const.
   mutable unsigned refCount;
};

#endif//COUNTER_HPP__

