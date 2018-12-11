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

int64_t		forge::variant_base::get_int64()
{
	throw std::runtime_error("Expected an integer here, found an empty string.");
}
	
void		forge::variant_base::set( double inNum )
{
	this->~variant_base();
	new (this) variant_double(inNum);
}

double		forge::variant_base::get_double()
{
	throw std::runtime_error("Expected a number here, found an empty string.");
}
	
void		forge::variant_base::set( std::string inString )
{
	this->~variant_base();
	new (this) variant_string(inString);
}

std::string	forge::variant_base::get_string()
{
	return std::string();
}


#pragma mark -


void		forge::variant_int64::set( int64_t inNum )
{
	mValue.mInteger = inNum;
}

int64_t		forge::variant_int64::get_int64()
{
	return mValue.mInteger;
}
	
double		forge::variant_int64::get_double()
{
	return mValue.mInteger;
}
	
std::string	forge::variant_int64::get_string()
{
	return std::to_string(mValue.mInteger);
}


#pragma mark -


int64_t		forge::variant_double::get_int64()
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

double		forge::variant_double::get_double()
{
	return mValue.mDouble;
}
	
std::string	forge::variant_double::get_string()
{
	return std::to_string(mValue.mDouble);
}


#pragma mark -

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

int64_t		forge::variant_string::get_int64()
{
	const char * str = mValue.mString->c_str();
	char * endPtr = nullptr;
	int64_t num = strtoll(str, &endPtr, 10);
	if (endPtr != str + mValue.mString->length()) {
		throw std::runtime_error("Expected integer, found a string.");
	}
	return num;
}

double		forge::variant_string::get_double()
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

std::string	forge::variant_string::get_string()
{
	return *mValue.mString;
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

int64_t		forge::static_string::get_int64()
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

double		forge::static_string::get_double()
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

std::string	forge::static_string::get_string()
{
	return mString;
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

int64_t		forge::static_int64::get_int64()
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

double		forge::static_int64::get_double()
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

std::string	forge::static_int64::get_string()
{
	return std::to_string(mInteger);
}
