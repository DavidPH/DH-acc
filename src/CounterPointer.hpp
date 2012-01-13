/* Copyright (C) 2010, 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* CounterPointer.hpp
**
** Defines the CounterPointer class.
*/

#ifndef HPP_CounterPointer_
#define HPP_CounterPointer_

#include <cstddef>
#include <ostream>



/// Simple reference counting pointer.
template<typename T> class CounterPointer
{
public:
	/// Default constructor. Initializes to NULL.
	CounterPointer();
	/// Copy constructor.
	CounterPointer(CounterPointer<T> const & p);
	/// Pointer-cast constructor.
	template<typename T2> CounterPointer(CounterPointer<T2> const & p);
	/// Pointer-cast constructor.
	template<typename T2> CounterPointer(T2 * const p);
	/// Initializes to p.
	CounterPointer(T * const p);
	/// Destructor.
	~CounterPointer();

	/// Returns the number of references to the pointed to object.
	/**
	*** Returns 0 if NULL.
	**/
	size_t count() const;

	operator T * () const;

	CounterPointer<T> & operator = (CounterPointer<T> const & p);
	CounterPointer<T> & operator = (T * const p);

	bool operator == (CounterPointer<T> const & p) const;
	bool operator == (T const * const p) const;
	bool operator != (CounterPointer<T> const & p) const;
	bool operator != (T const * const p) const;

	T * operator -> () const;
	T & operator *  () const;

private:
	T * _p;
};

#define MAKE_ABSTRACT_COUNTER_CLASS(CLASS)\
public:\
virtual CLASS * clone() const = 0;\
virtual char const * getName() const {return #CLASS;};\
typedef CounterPointer<CLASS> Pointer;\
typedef CounterPointer<CLASS const> ConstPointer;\
friend class CounterPointer<CLASS>;\
friend class CounterPointer<CLASS const>

#define MAKE_ABSTRACT_COUNTER_CLASS_BASE(CLASS,BASE)\
MAKE_ABSTRACT_COUNTER_CLASS(CLASS);\
typedef BASE Super

#define MAKE_COUNTER_CLASS(CLASS)\
public:\
virtual CLASS * clone() const {return new CLASS(*this);}\
virtual char const * getName() const {return #CLASS;};\
typedef CounterPointer<CLASS> Pointer;\
typedef CounterPointer<CLASS const> ConstPointer;\
friend class CounterPointer<CLASS>;\
friend class CounterPointer<CLASS const>

#define MAKE_COUNTER_CLASS_BASE(CLASS,BASE)\
MAKE_COUNTER_CLASS(CLASS);\
typedef BASE Super

/// Convenient base for reference-counted classes.
/**
*** Not only does this class remove the tedious redundancy needed to set up a
*** class for reference counting, it also allows potential virtual inheritance
*** of reference counting.
**/
class Counter
{
	MAKE_COUNTER_CLASS(Counter);

public:
	virtual ~Counter();

	Counter & operator = (Counter const & c);

protected:
	Counter();
	Counter(Counter const & c);

	/// Number of references to this.
	/**
	*** Made protected so that inherited classes can use CounterPointer.
	*** Made mutable so that pointer-to-const can be passed and still be
	*** reference counted.
	**/
	mutable size_t _referenceCount;
};

#include "CounterPointer.inline.hpp"

#endif /* HPP_CounterPointer_ */

