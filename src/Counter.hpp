//-----------------------------------------------------------------------------
//
// Copyright(C) 2010-2012 David Hill
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

#ifndef HPP_Counter_
#define HPP_Counter_


//----------------------------------------------------------------------------|
// Macros                                                                     |
//
#define MAKE_COMMON_COUNTER_CLASS_TYPES(CLASS)        \
public:                                               \
typedef CounterPointer<CLASS> Pointer;                \
typedef CounterPointer<CLASS const> ConstPointer;     \
friend class CounterPointer<CLASS>;                   \
friend class CounterPointer<CLASS const>;             \
typedef CounterReference<CLASS> Reference;            \
typedef CounterReference<CLASS const> ConstReference; \
friend class CounterReference<CLASS>;                 \
friend class CounterReference<CLASS const>

#define MAKE_COMMON_COUNTER_CLASS(CLASS)                  \
public:                                                   \
CounterPointer<CLASS> clone() const {return cloneRaw();}  \
virtual char const *getClassName() const {return #CLASS;} \
MAKE_COMMON_COUNTER_CLASS_TYPES(CLASS)

//
// MAKE_ABSTRACT_COUNTER_CLASS
//
// For use in defining abstract reference-counted classes.
//
#define MAKE_ABSTRACT_COUNTER_CLASS(CLASS)    \
private: virtual CLASS *cloneRaw() const = 0; \
MAKE_COMMON_COUNTER_CLASS(CLASS)

//
// MAKE_ABSTRACT_COUNTER_CLASS_BASE
//
#define MAKE_ABSTRACT_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_ABSTRACT_COUNTER_CLASS(CLASS); typedef BASE Super

//
// MAKE_NOCLONE_COUNTER_CLASS
//
// For use in defining reference-counted classes that cannot be copied.
//
#define MAKE_NOCLONE_COUNTER_CLASS(CLASS) private: \
virtual CLASS *cloneRaw() const {throw 1;}         \
MAKE_COMMON_COUNTER_CLASS(CLASS)

//
// MAKE_NOCLONE_COUNTER_CLASS_BASE
//
#define MAKE_NOCLONE_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_NOCLONE_COUNTER_CLASS(CLASS); typedef BASE Super

//
// MAKE_NOVIRTUAL_COUNTER_CLASS
//
// For use in defining reference-counted classes without virtuals.
//
#define MAKE_NOVIRTUAL_COUNTER_CLASS(CLASS)       \
char const *getClassName() const {return #CLASS;} \
MAKE_COMMON_COUNTER_CLASS_TYPES(CLASS)

//
// MAKE_NOVIRTUAL_COUNTER_CLASS_BASE
//
#define MAKE_NOVIRTUAL_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_NOVIRTUAL_COUNTER_CLASS(CLASS); typedef BASE Super

//
// MAKE_COUNTER_CLASS
//
#define MAKE_COUNTER_CLASS(CLASS) private:                 \
virtual CLASS *cloneRaw() const {return new CLASS(*this);} \
MAKE_COMMON_COUNTER_CLASS(CLASS)

//
// MAKE_COUNTER_CLASS_BASE
//
#define MAKE_COUNTER_CLASS_BASE(CLASS,BASE) \
MAKE_COUNTER_CLASS(CLASS); typedef BASE Super


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
   CounterPointer() : p(0) {}

   CounterPointer(T *_p) : p(_p) {if (p) ++p->refCount;}

   template<typename T2>
   CounterPointer(T2 *_p) : p(static_cast<T*>(_p)) {if (p) ++p->refCount;}

   CounterPointer(CounterPointer<T> const &_p) : p(_p.p) {if (p) ++p->refCount;}

   template<typename T2>
   CounterPointer(CounterPointer<T2> const &_p) : p(static_cast<T*>(_p))
   {if (p) ++p->refCount;}

   CounterPointer(CounterReference<T> const &_p) : p(static_cast<T*>(_p))
   {++p->refCount;}

   template<typename T2>
   CounterPointer(CounterReference<T2> const &_p) : p(static_cast<T*>(_p))
   {++p->refCount;}

   ~CounterPointer() {if (p && !--p->refCount) delete p;}

   operator T * () const {return p;}

   CounterPointer<T> &operator = (T *_p)
   {
      T *old = p; if ((p = _p)) ++p->refCount;
      if (old && !--old->refCount) delete old;
      return *this;
   }

   template<typename T2>
   CounterPointer<T> &operator = (T2 *_p)
   {
      T *old = p; if ((p = _p)) ++p->refCount;
      if (old && !--old->refCount) delete old;
      return *this;
   }

   CounterPointer<T> &operator = (CounterPointer<T> const &_p)
   {return *this = static_cast<T *>(_p);}

   template<typename T2>
   CounterPointer<T> &operator = (CounterPointer<T2> const &_p)
   {return *this = static_cast<T2 *>(_p);}

   CounterPointer<T> &operator = (CounterReference<T> const &_p)
   {
      T *old = p; ++(p = static_cast<T *>(_p))->refCount;
      if (old && !--old->refCount) delete old;
      return *this;
   }

   template<typename T2>
   CounterPointer<T> &operator = (CounterReference<T2> const &_p)
   {
      T *old = p; ++(p = static_cast<T2 *>(_p))->refCount;
      if (old && !--old->refCount) delete old;
      return *this;
   }

   T *operator -> () const {return p;}

   T &operator * () const {return *p;}

   unsigned refCount() const {return p ? p->refCount : 0;}

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
   explicit CounterReference(T *_p) : p(_p) {++p->refCount;}

   template<typename T2>
   explicit CounterReference(T2 *_p) : p(static_cast<T*>(_p)) {++p->refCount;}

   explicit CounterReference(CounterPointer<T> const &_p)
    : p(static_cast<T*>(_p)) {++p->refCount;}

   template<typename T2>
   explicit CounterReference(CounterPointer<T2> const &_p)
    : p(static_cast<T*>(_p)) {++p->refCount;}

   CounterReference(CounterReference<T> const &_p) : p(_p.p) {++p->refCount;}

   template<typename T2>
   CounterReference(CounterReference<T2> const &_p) : p(static_cast<T*>(_p))
   {++p->refCount;}

   ~CounterReference() {if (!--p->refCount) delete p;}

   operator T * () const {return p;}

   CounterReference<T> &operator = (CounterReference<T> const &_p)
   {
      T *old = p; ++(p = static_cast<T *>(_p))->refCount;
      if (!--old->refCount) delete old;
      return *this;
   }

   template<typename T2>
   CounterReference<T> &operator = (CounterReference<T2> const &_p)
   {
      T *old = p; ++(p = static_cast<T2 *>(_p))->refCount;
      if (!--old->refCount) delete old;
      return *this;
   }

   T *operator -> () const {return p;}

   T &operator * () const {return *p;}

   unsigned refCount() const {return p->refCount;}

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
   virtual ~Counter() {}

protected:
   Counter() : refCount(0) {}

   Counter(Counter const &) : refCount(0) {}

   Counter &operator = (Counter const &) {return *this;}

   // Protected so that derived classes can use CounterPointer.
   // Mutable for CounterPointer-to-const.
   mutable unsigned refCount;
};
//
// PlainCounter
//
// Reference-counted base class with no virtuals.
//
class PlainCounter
{
   MAKE_NOVIRTUAL_COUNTER_CLASS(PlainCounter);

protected:
   PlainCounter() : refCount(0) {}

   PlainCounter(PlainCounter const &) : refCount(0) {}

   PlainCounter &operator = (PlainCounter const &) {return *this;}

   // Protected so that derived classes can use CounterPointer.
   // Mutable for CounterPointer-to-const.
   mutable unsigned refCount;
};

#endif//HPP_Counter_

