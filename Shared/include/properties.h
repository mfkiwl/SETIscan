//
//  properties.h
//  dbio
//
//  Created by Simon Gornall on 7/17/20.
//  Copyright © 2020 Simon Gornall. All rights reserved.
//

#ifndef properties_h
#define properties_h

/*****************************************************************************\
|* Getter / Setter macros for properties within classes.
\*****************************************************************************/
#define GETSET(type, var, accessor)			\
	protected:								\
	   type _##var;							\
	public:									\
       inline type& var()					\
	   {									\
		  return _##var;					\
	   }									\
       inline void set##accessor(type val)	\
	   {									\
		  _##var = val;						\
	   }

#define GETSETPUBLIC(type, var, accessor)	\
	public:									\
	   type _##var;							\
       inline type& var()					\
	   {									\
		  return _##var;					\
	   }									\
       inline void set##accessor(type val)	\
	   {									\
		  _##var = val;						\
	   }

#define SET(type, var, accessor)			\
	protected:								\
	   type _##var;							\
	public:									\
       inline void set##accessor(type &val)	\
	   {									\
		  _##var = val;						\
	   }

#define GET(type, var)						\
	protected:								\
	   type _##var;							\
	public:									\
       inline type& var()					\
	   {									\
		  return _##var;					\
	   }

/*****************************************************************************\
|* Make a class non-copiable or moveable
\*****************************************************************************/
#define NON_COPYABLE_NOR_MOVEABLE(T)		\
	  T(T const &) = delete;				\
	  void operator=(T const &t) = delete;	\
	  T(T &&) = delete;


#endif /* properties_h */
