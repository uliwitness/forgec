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


void		forge::variant_base::set( int64_t inNum )
{
	this->~variant_base();
	new (this) variant_int64(inNum);
}

int64_t		forge::variant_base::get_int64() const
{
	throw std::runtime_error("Expected an integer here, found an empty string.");
}
	
void		forge::variant_base::set( double inNum )
{
	this->~variant_base();
	new (this) variant_double(inNum);
}

double		forge::variant_base::get_double() const
{
	throw std::runtime_error("Expected a number here, found an empty string.");
}
	
void		forge::variant_base::set( std::string inString )
{
	this->~variant_base();
	new (this) variant_string(inString);
}

std::string	forge::variant_base::get_string() const
{
	return std::string();
}

void	forge::variant_base::set_value_for_key( const value& inValue, const std::string &inKey )
{
	this->~variant_base();
	new (this) variant_map( inValue, inKey );
}

void	forge::variant_base::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set("");
}

void	forge::variant_base::copy_to( value &dest ) const
{
	dest.set("");
}


#pragma mark -


void		forge::variant_int64::set( int64_t inNum )
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

void		forge::variant_int64::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set("");
}

void	forge::variant_int64::copy_to( value &dest ) const
{
	dest.set(mValue.mInteger);
}


#pragma mark -


int64_t		forge::variant_double::get_int64() const
{
	if (truncf(mValue.mDouble) != mValue.mDouble) {
		throw std::runtime_error("Expected integer, found a fractional number.");
	}
	return mValue.mDouble;
}
	
void		forge::variant_double::set( double inNum )
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

void		forge::variant_double::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set("");
}

void	forge::variant_double::copy_to( value &dest ) const
{
	dest.set(mValue.mDouble);
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

void		forge::variant_string::set( std::string inString )
{
	mValue.mString->assign(inString);
}

std::string	forge::variant_string::get_string() const
{
	return *mValue.mString;
}

void		forge::variant_string::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set("");
}

void	forge::variant_string::copy_to( value &dest ) const
{
	dest.set(*mValue.mString);
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


void		forge::static_string::set( int64_t inNum )
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

void		forge::static_string::set( double inNum )
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

void		forge::static_string::set( std::string inString )
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
	outValue.set("");
}

void	forge::static_string::copy_to( value &dest ) const
{
	dest.set(mString);
}


#pragma mark -


forge::static_int64::static_int64( int64_t inNum )
{
	mInteger = inNum;
}

void		forge::static_int64::set( int64_t inNum )
{
	mInteger = inNum;
}

int64_t		forge::static_int64::get_int64() const
{
	return mInteger;
}

void		forge::static_int64::set( double inNum )
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

void		forge::static_int64::set( std::string inString )
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
	outValue.set("");
}

void	forge::static_int64::copy_to( value &dest ) const
{
	dest.set(mInteger);
}
