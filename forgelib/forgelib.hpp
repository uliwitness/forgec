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
#include <cmath>


/* The classes below are exported */
#pragma GCC visibility push(default)

namespace forge {
	
	class variant;
	
	enum value_data_type_ {
		value_data_type_NONE = 0,
		value_data_type_int64 = (1 << 0),
		value_data_type_double = (1 << 1),
		value_data_type_string = (1 << 2),
		value_data_type_map = (1 << 3),
		value_data_type_bool = (1 << 4),
	};
	typedef uint32_t value_data_type;
	
	// Base class for our variables etc., so scripts can access them
	//	without having to know what type they are.
	class value {
	public:
		virtual void		set_int64( int64_t inNum ) = 0;
		virtual int64_t		get_int64() const = 0;
		
		virtual void		set_double( double inNum ) = 0;
		virtual double		get_double() const = 0;
		
		virtual void		set_string( std::string inString ) = 0;
		virtual std::string	get_string() const = 0;

		virtual void		set_bool( bool inBool ) = 0;
		virtual bool		get_bool() const = 0;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey ) = 0;
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const = 0;
		
		virtual void		copy_to( value &dest ) const = 0;
		
		virtual value_data_type	data_type() const { return value_data_type_NONE; }

	protected:
		virtual ~value() {}
	};
	
	
	class stack_suitable_value : public value {
	public:
		virtual ~stack_suitable_value() {}
	};
	
	
	// Internal base class used by 'variant' to store all values:
	//	Do not use! Use variant instead!
	class variant_base : public value {
	public:
		virtual void		set_int64( int64_t inNum );
		virtual int64_t		get_int64() const;

		virtual void		set_double( double inNum );
		virtual double		get_double() const;

		virtual void		set_string( std::string inString );
		virtual std::string	get_string() const;

		virtual void		set_bool( bool inBool );
		virtual bool		get_bool() const;

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

	protected:
		union {
			int64_t							mInteger;
			double							mDouble;
			std::string						*mString;
			std::map<std::string, variant>	*mMap;
			bool							mBool;
		} mValue;
	};
	
	
	// Class used for variables. A variable may change its type depending on
	//	what value you assign to it. To achieve this, we employ a mean hack:
	//	we store the actual values in an untyped buffer, and use placement new
	//	to actually allocate the appropriat subclass inside that.
	class variant : public stack_suitable_value {
	public:
		variant( const variant &inOriginal ) 				{ new (mValue) variant_base(); inOriginal.val().copy_to(*val()); }
		variant() 											{ new (mValue) variant_base(); }
		~variant()											{ val()->~variant_base();  }
		
		virtual void		set_int64( int64_t inNum ) 		{ val()->set_int64(inNum); }
		virtual int64_t		get_int64() const				{ return val().get_int64(); }
		
		virtual void		set_double( double inNum )	 	{ val()->set_double(inNum); }
		virtual double		get_double() const				{ return val().get_double(); }
		
		virtual void		set_string( std::string inString ) 	{ val()->set_string(inString); }
		virtual std::string	get_string() const					{ return val().get_string(); }
		
		virtual void		set_bool( bool inBool ) 			{ val()->set_bool(inBool); }
		virtual bool		get_bool() const					{ return val().get_bool(); }
		
		virtual void		set_value_for_key( const value& inValue, const std::string &inKey )	{ val()->set_value_for_key(inValue, inKey); }
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const{ val().get_value_for_key( outValue, inKey ); }
		
		virtual void		copy_to( value &dest ) const	{ val().copy_to(dest); }

		virtual value_data_type	data_type() const { return val().data_type(); }

	protected:
		variant_base*		val()							{ return (variant_base *) mValue; }
		const variant_base&	val() const						{ return *(variant_base *) mValue; }

		uint8_t	mValue[sizeof(variant_base)];
	};

	
	// Class used for storing a string in a way that lets one access its contents
	//	as any other type as long as the string can be converted to that:
	class static_string : public stack_suitable_value {
	public:
		explicit static_string( std::string inStr = std::string() );
		virtual ~static_string() {}

		virtual void		set_int64( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set_double( double inNum );
		virtual double		get_double() const;
		
		virtual void		set_string( std::string inString );
		virtual std::string	get_string() const;

		virtual bool		get_bool() const;
		virtual void		set_bool( bool inBool );

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;
		
		virtual value_data_type	data_type() const { return value_data_type_string; }

	protected:
		std::string	mString;
	};

	// Class used for storing an integer in a way that lets one access its contents
	//	as any other type as long as the string can be converted to that:
	class static_int64 : public stack_suitable_value {
	public:
		explicit static_int64( int64_t inNum = 0LL );
		virtual ~static_int64() {}

		virtual void		set_int64( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set_double( double inNum );
		virtual double		get_double() const;
		
		virtual void		set_string( std::string inString );
		virtual std::string	get_string() const;

		virtual bool		get_bool() const;
		virtual void		set_bool( bool inBool );

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;
		
		virtual value_data_type	data_type() const { return value_data_type_int64; }

	protected:
		int64_t mInteger;
	};
	
	// Class used for storing an floating point number in a way that lets one
	//	access its contents as any other type as long as the string can be
	//	converted to that:
	class static_double : public stack_suitable_value {
	public:
		explicit static_double( double inDouble = 0.0 );
		virtual ~static_double() {}

		virtual void		set_int64( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set_double( double inNum );
		virtual double		get_double() const;
		
		virtual void		set_string( std::string inString );
		virtual std::string	get_string() const;

		virtual bool		get_bool() const;
		virtual void		set_bool( bool inBool );

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

		virtual value_data_type	data_type() const { return (trunc(mDouble) != mDouble) ? value_data_type_double : value_data_type_int64; }

	protected:
		double mDouble;
	};

	class static_map : public stack_suitable_value {
	public:
		static_map();
		static_map( const static_map& inOriginal );
		static_map( const value& inValue, const std::string& inKey );
		~static_map();

		virtual int64_t		get_int64() const;
		
		virtual double		get_double() const;
		
		virtual std::string	get_string() const;

		virtual bool		get_bool() const;
		virtual void		set_bool( bool inBool );

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;
		
		virtual value_data_type	data_type() const { return value_data_type_map; }
	
	public:
		std::map<std::string,variant>	mMap;
	};

	class static_bool : public stack_suitable_value {
	public:
		static_bool() : mBool(false) {}
		static_bool( const static_bool& inOriginal ) { mBool = inOriginal.mBool; }
		explicit static_bool( bool inBool )	{ mBool = inBool; }
		
		virtual int64_t		get_int64() const;
		virtual void		set_int64( int64_t inNum );

		virtual double		get_double() const;
		virtual void		set_double( double inNum );

		virtual std::string	get_string() const;
		virtual void		set_string( std::string inStr );

		virtual bool		get_bool() const;
		virtual void		set_bool( bool inBool );

		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;

		virtual void		copy_to( value &dest ) const;
		
		virtual value_data_type	data_type() const { return value_data_type_bool; }
		
	public:
		bool mBool;
	};

	variant concatenate( variant a, variant b );
	variant concatenate_space( variant a, variant b );
	variant add( variant a, variant b );
	variant subtract( variant a, variant b );
	variant multiply( variant a, variant b );
	variant divide( variant a, variant b );
	variant power( variant a, variant b );
	variant equal( forge::variant a, forge::variant b );
	variant not_equal( forge::variant a, forge::variant b );
	variant less_than( forge::variant a, forge::variant b );
	variant less_than_equal( forge::variant a, forge::variant b );
	variant greater_than( forge::variant a, forge::variant b );
	variant greater_than_equal( forge::variant a, forge::variant b );
}

#pragma GCC visibility pop
#endif
