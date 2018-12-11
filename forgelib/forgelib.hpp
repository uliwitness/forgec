//
//  forgelib.hpp
//  forgelib
//
//  Created by Uli Kusterer on 11.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#ifndef forgelib_
#define forgelib_

#include <string>
#include <map>
#include <cstdint>


/* The classes below are exported */
#pragma GCC visibility push(default)

namespace forge {
	
	class variant;
	
	// Base class for our variables etc., so scripts can access them
	//	without having to know what type they are.
	class value {
	public:
		virtual void		set( int64_t inNum ) = 0;
		virtual int64_t		get_int64() const = 0;
		
		virtual void		set( double inNum ) = 0;
		virtual double		get_double() const = 0;
		
		virtual void		set( std::string inString ) = 0;
		virtual std::string	get_string() const = 0;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey ) = 0;
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const = 0;
		
		virtual void		copy_to( value &dest ) const = 0;

	protected:
		virtual ~value() {}
	};
	
	
	// Internal base class used by 'variant' to store all values:
	//	Do not use! Use variant instead!
	class variant_base : public value {
	public:
		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;

		virtual void		set( double inNum );
		virtual double		get_double() const;

		virtual void		set( std::string inString );
		virtual std::string	get_string() const;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		union {
			int64_t							mInteger;
			double							mDouble;
			std::string						*mString;
			std::map<std::string, variant>	*mMap;
		} mValue;
	};
	

	// Internal base class used by 'variant' to store integer values:
	//	Do not use! Use variant instead!
	class variant_int64 : public variant_base {
	public:
		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual double		get_double() const;
		
		virtual std::string	get_string() const;

		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		variant_int64( int64_t inNum ) : variant_base() { mValue.mInteger = inNum; }
		
		friend class variant_base;
	};


	// Internal base class used by 'variant' to store floating point values:
	//	Do not use! Use variant instead!
	class variant_double : public variant_base {
	public:
		virtual int64_t		get_int64() const;
		
		virtual void		set( double inNum );
		virtual double		get_double() const;
		
		virtual std::string	get_string() const;

		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		variant_double( double inNum ) : variant_base() { mValue.mDouble = inNum; }

		friend class variant_base;
	};


	// Internal base class used by 'variant' to store string values:
	//	Do not use! Use variant instead!
	class variant_string : public variant_base {
	public:
		variant_string( const variant_string& inOriginal );

		virtual int64_t		get_int64() const;
		
		virtual double		get_double() const;
		
		virtual void		set( std::string inString );
		virtual std::string	get_string() const;

		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		variant_string( std::string inStr );
		~variant_string();

		friend class variant_base;
	};

	
	// Internal base class used by 'variant' to store dictionary values:
	//	Do not use! Use variant instead!
	class variant_map : public variant_base {
	public:
		variant_map( const variant_map& inOriginal );
		
		virtual int64_t		get_int64() const;
		
		virtual double		get_double() const;
		
		virtual std::string	get_string() const;
		
		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		variant_map();
		variant_map( const value& inValue, const std::string& inKey );
		~variant_map();
		
		friend class variant_base;
	};

	
	// Class used for variables. A variable may change its type depending on
	//	what value you assign to it. To achieve this, we employ a mean hack:
	//	we store the actual values in an untyped buffer, and use placement new
	//	to actually allocate the appropriat subclass inside that.
	class variant : public value {
	public:
		variant( const variant &inOriginal ) 				{ new (mValue) variant_base(); inOriginal.val().copy_to(*val()); }
		variant() 											{ new (mValue) variant_base(); }
		~variant()											{ val()->~variant_base();  }
		
		virtual void		set( int64_t inNum ) 			{ val()->set(inNum); }
		virtual int64_t		get_int64() const				{ return val().get_int64(); }
		
		virtual void		set( double inNum )	 			{ val()->set(inNum); }
		virtual double		get_double() const				{ return val().get_double(); }
		
		virtual void		set( std::string inString ) 	{ val()->set(inString); }
		virtual std::string	get_string() const				{ return val().get_string(); }

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey )	{ val()->set_value_for_key(inValue, inKey); }
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const{ val().get_value_for_key( outValue, inKey ); }
		
		virtual void		copy_to( value &dest ) const	{ val().copy_to(dest); }

	protected:
		variant_base*		val()							{ return (variant_base *) mValue; }
		const variant_base&	val() const						{ return *(variant_base *) mValue; }

		uint8_t	mValue[sizeof(variant_base)];
	};

	
	// Class used for storing a string in a way that lets one access its contents
	//	as any other type as long as the string can be converted to that:
	class static_string : public value {
	public:
		static_string( std::string inStr = std::string() );
		
		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set( double inNum );
		virtual double		get_double() const;
		
		virtual void		set( std::string inString );
		virtual std::string	get_string() const;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		std::string	mString;
	};

	// Class used for storing an integer in a way that lets one access its contents
	//	as any other type as long as the string can be converted to that:
	class static_int64 : public value {
	public:
		static_int64( int64_t inNum = 0LL );
		
		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set( double inNum );
		virtual double		get_double() const;
		
		virtual void		set( std::string inString );
		virtual std::string	get_string() const;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		int64_t mInteger;
	};
	
	// Class used for storing an floating point number in a way that lets one
	//	access its contents as any other type as long as the string can be
	//	converted to that:
	class static_double : public value {
	public:
		static_double( double inDouble = 0.0 );
		
		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set( double inNum );
		virtual double		get_double() const;
		
		virtual void		set( std::string inString );
		virtual std::string	get_string() const;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		int64_t mDouble;
	};
}

#pragma GCC visibility pop
#endif
