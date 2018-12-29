//
//  forgelibPriv.hpp
//  forgelib
//
//  Created by Uli Kusterer on 11.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include "forgelib.hpp"

/* The classes below are not exported */
#pragma GCC visibility push(hidden)


namespace forge {

// Internal base class used by 'variant' to store integer values:
//	Do not use! Use variant instead!
class variant_int64 : public variant_base {
	public:
	virtual void		set_int64( int64_t inNum );
	virtual int64_t		get_int64() const;
	
	virtual double		get_double() const;
	
	virtual std::string	get_string() const;
	
	virtual bool		get_bool() const;

	virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
	
	virtual void		copy_to( value &dest ) const;
	
	virtual value_data_type	data_type() const { return value_data_type_int64; }
	
	protected:
	variant_int64( int64_t inNum ) : variant_base() { mValue.mInteger = inNum; }
	
	friend class variant_base;
};
static_assert(sizeof(variant_int64) == sizeof(variant_base), "subclasses of variant_base must be the same size.");

// Internal base class used by 'variant' to store floating point values:
//	Do not use! Use variant instead!
class variant_double : public variant_base {
	public:
	virtual int64_t		get_int64() const;
	
	virtual void		set_double( double inNum );
	virtual double		get_double() const;
	
	virtual std::string	get_string() const;
	
	virtual bool		get_bool() const;

	virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
	
	virtual void		copy_to( value &dest ) const;
	
	virtual value_data_type	data_type() const { return (trunc(mValue.mDouble) != mValue.mDouble) ? value_data_type_double : value_data_type_int64; }
	
	protected:
	variant_double( double inNum ) : variant_base() { mValue.mDouble = inNum; }
	
	friend class variant_base;
};
static_assert(sizeof(variant_double) == sizeof(variant_base), "subclasses of variant_base must be the same size.");


// Internal base class used by 'variant' to store string values:
//	Do not use! Use variant instead!
class variant_string : public variant_base {
	public:
	variant_string( const variant_string& inOriginal );
	
	virtual int64_t		get_int64() const;
	
	virtual double		get_double() const;
	
	virtual void		set_string( std::string inString );
	virtual std::string	get_string() const;
	
	virtual bool		get_bool() const;

	virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
	
	virtual void		copy_to( value &dest ) const;
	
	virtual value_data_type	data_type() const { return value_data_type_string; }
	
	protected:
	variant_string( std::string inStr );
	~variant_string();
	
	friend class variant_base;
};
static_assert(sizeof(variant_string) == sizeof(variant_base), "subclasses of variant_base must be the same size.");


// Internal base class used by 'variant' to store dictionary values:
//	Do not use! Use variant instead!
class variant_map : public variant_base {
	public:
	variant_map( const variant_map& inOriginal );
	
	virtual int64_t		get_int64() const;
	
	virtual double		get_double() const;
	
	virtual std::string	get_string() const;
	
	virtual bool		get_bool() const;

	virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
	virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
	
	virtual void		copy_to( value &dest ) const;
	
	virtual value_data_type	data_type() const { return value_data_type_map; }
	
	protected:
	variant_map();
	variant_map( const value& inValue, const std::string& inKey );
	~variant_map();
	
	friend class variant_base;
};
static_assert(sizeof(variant_int64) == sizeof(variant_base), "subclasses of variant_base must be the same size.");

// Internal base class used by 'variant' to store dictionary values:
//	Do not use! Use variant instead!
class variant_bool : public variant_base {
public:
	variant_bool( const variant_bool& inOriginal ) { mValue.mBool = inOriginal.mValue.mBool; }
	
	virtual int64_t		get_int64() const;
	
	virtual double		get_double() const;
	
	virtual std::string	get_string() const;
	
	virtual bool		get_bool() const;
	virtual void		set_bool( bool inBool );

	virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
	
	virtual void		copy_to( value &dest ) const;
	
	virtual value_data_type	data_type() const { return value_data_type_bool; }
	
protected:
	variant_bool() {}
	variant_bool( bool inBool ) { mValue.mBool = inBool; }
	
	friend class variant_base;
};
static_assert(sizeof(variant_bool) == sizeof(variant_base), "subclasses of variant_base must be the same size.");
}
	
#pragma GCC visibility pop
