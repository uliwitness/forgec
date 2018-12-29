//
//  forgelib.cpp
//  forgelib
//
//  Created by Uli Kusterer on 11.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include <iostream>
#include <cmath>
#include "forgelib.hpp"
#include "forgelibPriv.hpp"


#pragma mark -


void	forge::value::append( value &src )
{
	std::string result(get_string());
	result.append(src.get_string());
	set_string(result);
}


void	forge::value::prepend( value &src )
{
	std::string result(src.get_string());
	result.append(get_string());
	set_string(result);
}


void		forge::variant_base::set_int64( int64_t inNum )
{
	this->~variant_base();
	new (this) variant_int64(inNum);
}

int64_t		forge::variant_base::get_int64() const
{
	throw std::runtime_error("Expected an integer here, found an empty string.");
}
	
void		forge::variant_base::set_double( double inNum )
{
	this->~variant_base();
	new (this) variant_double(inNum);
}

double		forge::variant_base::get_double() const
{
	throw std::runtime_error("Expected a number here, found an empty string.");
}
	
void		forge::variant_base::set_string( std::string inString )
{
	this->~variant_base();
	new (this) variant_string(inString);
}

std::string	forge::variant_base::get_string() const
{
	return std::string();
}

void		forge::variant_base::set_bool( bool inBool )
{
	this->~variant_base();
	new (this) variant_bool(inBool);
}

bool	forge::variant_base::get_bool() const
{
	throw std::runtime_error("Expected a boolean here, found an empty string.");
}

void	forge::variant_base::set_value_for_key( const value& inValue, const std::string &inKey )
{
	this->~variant_base();
	new (this) variant_map( inValue, inKey );
}

void	forge::variant_base::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::variant_base::copy_to( value &dest ) const
{
	dest.set_string("");
}


#pragma mark -


void		forge::variant_int64::set_int64( int64_t inNum )
{
	mValue.mInteger = inNum;
}

int64_t		forge::variant_int64::get_int64() const
{
	return mValue.mInteger;
}
	
double		forge::variant_int64::get_double() const
{
	return mValue.mInteger;
}
	
std::string	forge::variant_int64::get_string() const
{
	return std::to_string(mValue.mInteger);
}

bool	forge::variant_int64::get_bool() const
{
	throw std::runtime_error("Expected a boolean here, found an integer.");
}

void		forge::variant_int64::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::variant_int64::copy_to( value &dest ) const
{
	dest.set_int64(mValue.mInteger);
}


#pragma mark -


int64_t		forge::variant_double::get_int64() const
{
	if (truncf(mValue.mDouble) != mValue.mDouble) {
		throw std::runtime_error("Expected integer, found a fractional number.");
	}
	return mValue.mDouble;
}
	
void		forge::variant_double::set_double( double inNum )
{
	mValue.mDouble = inNum;
}

double		forge::variant_double::get_double() const
{
	return mValue.mDouble;
}
	
std::string	forge::variant_double::get_string() const
{
	return std::to_string(mValue.mDouble);
}

bool	forge::variant_double::get_bool() const
{
	throw std::runtime_error("Expected a boolean here, found a number.");
}

void		forge::variant_double::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::variant_double::copy_to( value &dest ) const
{
	dest.set_double(mValue.mDouble);
}


#pragma mark -

int64_t		forge::variant_bool::get_int64() const
{
	throw std::runtime_error("Expected integer, found a boolean.");
}

double		forge::variant_bool::get_double() const
{
	throw std::runtime_error("Expected number, found a boolean.");
}


bool	forge::variant_bool::get_bool() const
{
	return mValue.mBool;
}

void	forge::variant_bool::set_bool( bool inBool )
{
	mValue.mBool = inBool;
}

std::string	forge::variant_bool::get_string() const
{
	return std::string(mValue.mBool ? "true" : "false");
}

void		forge::variant_bool::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::variant_bool::copy_to( value &dest ) const
{
	dest.set_bool(mValue.mBool);
}


#pragma mark -


forge::variant_string::variant_string( const variant_string& inOriginal )
{
	mValue.mString = new std::string(*inOriginal.mValue.mString);
}

forge::variant_string::variant_string( std::string inStr )
{
	mValue.mString = new std::string(inStr);
}

forge::variant_string::~variant_string()
{
	if (mValue.mString) {
		delete mValue.mString;
		mValue.mString = nullptr;
	}
}

int64_t		forge::variant_string::get_int64() const
{
	const char * str = mValue.mString->c_str();
	char * endPtr = nullptr;
	int64_t num = strtoll(str, &endPtr, 10);
	if (endPtr != str + mValue.mString->length()) {
		throw std::runtime_error("Expected integer, found a string.");
	}
	return num;
}

double		forge::variant_string::get_double() const
{
	const char * str = mValue.mString->c_str();
	char * endPtr = nullptr;
	double num = strtod(str, &endPtr);
	if (endPtr != str + mValue.mString->length()) {
		throw std::runtime_error("Expected number, found a string.");
	}
	return num;
}

void		forge::variant_string::set_string( std::string inString )
{
	mValue.mString->assign(inString);
}

std::string	forge::variant_string::get_string() const
{
	return *mValue.mString;
}


bool	forge::variant_string::get_bool() const
{
	if (strcasecmp("true", mValue.mString->c_str()) == 0) {
		return true;
	} else if (strcasecmp("false", mValue.mString->c_str()) == 0) {
		return false;
	}
	
	throw std::runtime_error("Expected a boolean here, found a string.");
}

void		forge::variant_string::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::variant_string::copy_to( value &dest ) const
{
	dest.set_string(*mValue.mString);
}


void	forge::variant_string::append( value &src )
{
	mValue.mString->append(src.get_string());
}


void	forge::variant_string::prepend( value &src )
{
	mValue.mString->insert(0, src.get_string());
}


#pragma mark -


forge::static_map::static_map( const static_map& inOriginal )
{
	mMap.insert(inOriginal.mMap.begin(), inOriginal.mMap.end());
}

forge::static_map::static_map( const value& inValue, const std::string& inKey )
{
	set_value_for_key(inValue, inKey);
}

forge::static_map::~static_map()
{
}

int64_t		forge::static_map::get_int64() const
{
	throw std::runtime_error("Expected integer, found a list.");
}

double		forge::static_map::get_double() const
{
	throw std::runtime_error("Expected number, found a list.");
}

std::string	forge::static_map::get_string() const
{
	std::string str;
	bool		isFirst = true;
	
	for (auto currPair : mMap) {
		if (isFirst) {
			isFirst = false;
		} else {
			str.append("\n");
		}
		str.append(currPair.first);
		str.append(":");
		std::string valStr(currPair.second.get_string());
		size_t searchOffs = 0;
		while (searchOffs < valStr.length()) {
			size_t pos = valStr.find_first_of("\n¬", searchOffs + 1);
			if (pos == std::string::npos) {
				break;
			}
			valStr.insert(pos, "¬");
			searchOffs = pos + 2;
		}
		str.append(valStr);
	}
	
	return str;
}

void	forge::static_map::set_bool( bool inBool )
{
	throw std::runtime_error("Expected list, found boolean.");
}

bool	forge::static_map::get_bool() const
{
	throw std::runtime_error("Expected boolean, found list.");
}

void		forge::static_map::set_value_for_key( const value& inValue, const std::string &inKey )
{
	variant &dest = mMap[inKey];
	inValue.copy_to(dest);
}

void		forge::static_map::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	auto foundValue = mMap.find(inKey);
	if (foundValue != mMap.end()) {
		foundValue->second.copy_to(outValue);
	}
}

void	forge::static_map::copy_to( value &dest ) const
{
	for (auto currPair : mMap) {
		dest.set_value_for_key(currPair.second, currPair.first);
	}
}


#pragma mark -


forge::variant_map::variant_map()
{
	mValue.mMap = new std::map<std::string, variant>();
}

forge::variant_map::variant_map( const variant_map& inOriginal )
{
	mValue.mMap = new std::map<std::string, variant>();
	mValue.mMap->insert(inOriginal.mValue.mMap->begin(), inOriginal.mValue.mMap->end());
}

forge::variant_map::variant_map( const value& inValue, const std::string& inKey )
{
	mValue.mMap = new std::map<std::string, variant>();
	set_value_for_key(inValue, inKey);
}

forge::variant_map::~variant_map()
{
	if (mValue.mMap) {
		delete mValue.mMap;
		mValue.mMap = nullptr;
	}
}

int64_t		forge::variant_map::get_int64() const
{
	throw std::runtime_error("Expected integer, found a list.");
}

double		forge::variant_map::get_double() const
{
	throw std::runtime_error("Expected number, found a list.");
}

std::string	forge::variant_map::get_string() const
{
	std::string str;
	bool		isFirst = true;
	
	for (auto currPair : *mValue.mMap) {
		if (isFirst) {
			isFirst = false;
		} else {
			str.append("\n");
		}
		str.append(currPair.first);
		str.append(":");
		std::string valStr(currPair.second.get_string());
		size_t searchOffs = 0;
		while (searchOffs < valStr.length()) {
			size_t pos = valStr.find_first_of("\n¬", searchOffs + 1);
			if (pos == std::string::npos) {
				break;
			}
			valStr.insert(pos, "¬");
			searchOffs = pos + 2;
		}
		str.append(valStr);
	}
	
	return str;
}

bool	forge::variant_map::get_bool() const
{
	throw std::runtime_error("Expected a boolean here, found a list.");
}

void		forge::variant_map::set_value_for_key( const value& inValue, const std::string &inKey )
{
	variant &dest = (*mValue.mMap)[inKey];
	inValue.copy_to(dest);
}

void		forge::variant_map::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	(*mValue.mMap)[inKey].copy_to(outValue);
}

void	forge::variant_map::copy_to( value &dest ) const
{
	for (auto currPair : *mValue.mMap) {
		dest.set_value_for_key(currPair.second, currPair.first);
	}
}


#pragma mark -


forge::static_string::static_string( std::string inStr )
{
	mString = inStr;
}


void		forge::static_string::set_int64( int64_t inNum )
{
	mString.assign(std::to_string(inNum));
}

int64_t		forge::static_string::get_int64() const
{
	const char * str = mString.c_str();
	char * endPtr = nullptr;
	int64_t num = strtoll(str, &endPtr, 10);
	if (endPtr != str + mString.length()) {
		throw std::runtime_error("Expected integer, found a string.");
	}
	return num;
}

void		forge::static_string::set_double( double inNum )
{
	mString.assign(std::to_string(inNum));
}

double		forge::static_string::get_double() const
{
	const char * str = mString.c_str();
	char * endPtr = nullptr;
	double num = strtod(str, &endPtr);
	if (endPtr != str + mString.length()) {
		throw std::runtime_error("Expected number, found a string.");
	}
	return num;
}

void		forge::static_string::set_string( std::string inString )
{
	mString.assign(inString);
}

std::string	forge::static_string::get_string() const
{
	return mString;
}

void		forge::static_string::set_value_for_key( const value& inValue, const std::string &inKey )
{
	throw std::runtime_error("Expected string, found a list.");
}

void		forge::static_string::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::static_string::copy_to( value &dest ) const
{
	dest.set_string(mString);
}


void	forge::static_string::append( value &src )
{
	mString.append(src.get_string());
}


void	forge::static_string::prepend( value &src )
{
	mString.insert(0, src.get_string());
}


void	forge::static_string::set_bool( bool inBool )
{
	set_string(std::string(inBool ? "true" : "false"));
}


bool	forge::static_string::get_bool() const
{
	if (strcasecmp("true", mString.c_str()) == 0) {
		return true;
	} else if (strcasecmp("false", mString.c_str()) == 0) {
		return false;
	} else {
		throw std::runtime_error("Expected boolean, found a string.");
	}
	
	return false;
}


#pragma mark -


forge::static_int64::static_int64( int64_t inNum )
{
	mInteger = inNum;
}

void		forge::static_int64::set_int64( int64_t inNum )
{
	mInteger = inNum;
}

int64_t		forge::static_int64::get_int64() const
{
	return mInteger;
}

void		forge::static_int64::set_double( double inNum )
{
	if (truncf(inNum) != inNum) {
		throw std::runtime_error("Expected integer, found a fractional number.");
	}
	mInteger = inNum;
}

double		forge::static_int64::get_double() const
{
	return mInteger;
}

void		forge::static_int64::set_string( std::string inString )
{
	const char * str = inString.c_str();
	char * endPtr = nullptr;
	int64_t num = strtoll(str, &endPtr, 10);
	if (endPtr != str + inString.length()) {
		throw std::runtime_error("Expected string, found an integer.");
	}
	mInteger = num;
}

std::string	forge::static_int64::get_string() const
{
	return std::to_string(mInteger);
}

void		forge::static_int64::set_value_for_key( const value& inValue, const std::string &inKey )
{
	throw std::runtime_error("Expected number, found a list.");
}

void		forge::static_int64::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::static_int64::copy_to( value &dest ) const
{
	dest.set_int64(mInteger);
}


void	forge::static_int64::set_bool( bool inBool )
{
	throw std::runtime_error("Expected integer, found a boolean.");
}


bool	forge::static_int64::get_bool() const
{
	throw std::runtime_error("Expected boolean, found an integer.");
	
	return false;
}


#pragma mark -


forge::static_double::static_double( double inNum )
{
	mDouble = inNum;
}

void		forge::static_double::set_int64( int64_t inNum )
{
	mDouble = inNum;
}

int64_t		forge::static_double::get_int64() const
{
	if (truncf(mDouble) != mDouble) {
		throw std::runtime_error("Expected integer, found a fractional number.");
	}
	return mDouble;
}

void		forge::static_double::set_double( double inNum )
{
	mDouble = inNum;
}

double		forge::static_double::get_double() const
{
	return mDouble;
}

void		forge::static_double::set_string( std::string inString )
{
	const char * str = inString.c_str();
	char * endPtr = nullptr;
	double num = strtod(str, &endPtr);
	if (endPtr != (str + inString.length())) {
		throw std::runtime_error("Expected string, found a number.");
	}
	mDouble = num;
}

std::string	forge::static_double::get_string() const
{
	return std::to_string(mDouble);
}

void		forge::static_double::set_value_for_key( const value& inValue, const std::string &inKey )
{
	throw std::runtime_error("Expected number, found a list.");
}

void		forge::static_double::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::static_double::copy_to( value &dest ) const
{
	dest.set_double(mDouble);
}


void	forge::static_double::set_bool( bool inBool )
{
	throw std::runtime_error("Expected number, found a boolean.");
}


bool	forge::static_double::get_bool() const
{
	throw std::runtime_error("Expected boolean, found a number.");
	
	return false;
}


#pragma mark -


void		forge::static_bool::set_int64( int64_t inNum )
{
	throw std::runtime_error("Expected boolean, found integer.");
}

int64_t		forge::static_bool::get_int64() const
{
	throw std::runtime_error("Expected integer, found boolean.");
}

void		forge::static_bool::set_double( double inNum )
{
	throw std::runtime_error("Expected boolean, found number.");
}

double		forge::static_bool::get_double() const
{
	throw std::runtime_error("Expected number, found boolean.");
}

void	forge::static_bool::set_string( std::string inString )
{
	if (strcasecmp("true", inString.c_str()) == 0) {
		mBool = true;
	} else if (strcasecmp("false", inString.c_str()) == 0) {
		mBool = false;
	} else {
		throw std::runtime_error("Expected boolean, found a string.");
	}
}

std::string	forge::static_bool::get_string() const
{
	return (mBool ? "true" : "false");
}

void		forge::static_bool::set_value_for_key( const value& inValue, const std::string &inKey )
{
	throw std::runtime_error("Expected boolean, found a list.");
}

void		forge::static_bool::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set_string("");
}

void	forge::static_bool::copy_to( value &dest ) const
{
	dest.set_bool(mBool);
}


void	forge::static_bool::set_bool( bool inBool )
{
	mBool = inBool;
}


bool	forge::static_bool::get_bool() const
{
	return mBool;
}


#pragma mark -


forge::variant forge::concatenate( forge::variant a, forge::variant b )
{
	std::string result(a.get_string());
	result.append(b.get_string());
	variant v;
	v.set_string(result);
	return v;
}


forge::variant forge::concatenate_space( forge::variant a, forge::variant b )
{
	std::string result(a.get_string());
	result.append(" ");
	result.append(b.get_string());
	variant v;
	v.set_string(result);
	return v;
}


forge::variant forge::add( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_int64(a.get_int64() + b.get_int64());
	} else {
		v.set_double(a.get_double() + b.get_double());
	}
	return v;
}


forge::variant forge::subtract( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_int64(a.get_int64() - b.get_int64());
	} else {
		v.set_double(a.get_double() - b.get_double());
	}
	return v;
}


forge::variant forge::multiply( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_int64(a.get_int64() * b.get_int64());
	} else {
		v.set_double(a.get_double() * b.get_double());
	}
	return v;
}


forge::variant forge::divide( forge::variant a, forge::variant b )
{
	variant v;
	v.set_double(a.get_double() / b.get_double());
	return v;
}


forge::variant forge::power( forge::variant a, forge::variant b )
{
	variant v;
	v.set_double(pow(a.get_double(), b.get_double()));
	return v;
}


forge::variant forge::equal( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_bool(a.get_int64() == b.get_int64());
	} else if (a.data_type() & value_data_type_double && b.data_type() & value_data_type_double) {
		v.set_bool(a.get_double() == b.get_double());
	} else {
		v.set_bool( strcasecmp(a.get_string().c_str(), b.get_string().c_str()) == 0 );
	}

	return v;
}


forge::variant forge::not_equal( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_bool(a.get_int64() != b.get_int64());
	} else if (a.data_type() & value_data_type_double && b.data_type() & value_data_type_double) {
		v.set_bool(a.get_double() != b.get_double());
	} else {
		v.set_bool( strcasecmp(a.get_string().c_str(), b.get_string().c_str()) != 0 );
	}
	
	return v;
}


forge::variant forge::less_than( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_bool(a.get_int64() < b.get_int64());
	} else if (a.data_type() & value_data_type_double && b.data_type() & value_data_type_double) {
		v.set_bool(a.get_double() < b.get_double());
	} else {
		v.set_bool( strcasecmp(a.get_string().c_str(), b.get_string().c_str()) < 0 );
	}
	
	return v;
}


forge::variant forge::less_than_equal( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_bool(a.get_int64() <= b.get_int64());
	} else if (a.data_type() & value_data_type_double && b.data_type() & value_data_type_double) {
		v.set_bool(a.get_double() <= b.get_double());
	} else {
		v.set_bool( strcasecmp(a.get_string().c_str(), b.get_string().c_str()) <= 0 );
	}
	
	return v;
}


forge::variant forge::greater_than( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_bool(a.get_int64() > b.get_int64());
	} else if (a.data_type() & value_data_type_double && b.data_type() & value_data_type_double) {
		v.set_bool(a.get_double() > b.get_double());
	} else {
		v.set_bool( strcasecmp(a.get_string().c_str(), b.get_string().c_str()) > 0 );
	}
	
	return v;
}


forge::variant forge::greater_than_equal( forge::variant a, forge::variant b )
{
	variant v;
	if (a.data_type() & value_data_type_int64 && b.data_type() & value_data_type_int64) {
		v.set_bool(a.get_int64() >= b.get_int64());
	} else if (a.data_type() & value_data_type_double && b.data_type() & value_data_type_double) {
		v.set_bool(a.get_double() >= b.get_double());
	} else {
		v.set_bool( strcasecmp(a.get_string().c_str(), b.get_string().c_str()) >= 0 );
	}
	
	return v;
}


void forge::append_to( forge::variant a, forge::variant &b )
{
	b.append(a);
}


void forge::prefix_to( forge::variant a, forge::variant &b )
{
	b.prepend(a);
}
