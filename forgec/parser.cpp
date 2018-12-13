//
//  parser.cpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include "parser.hpp"
#include <iostream>


void	forge::parser::throw_parse_error( const char *msg )
{
	std::string	combinedMsg(msg);
	
	combinedMsg.append(" at ");
	if (mCurrToken != mTokens->end()) {
		combinedMsg.append(std::to_string(mCurrToken->mStartOffset));
	} else {
		combinedMsg.append("end of file");
	}
	combinedMsg.append(".");
	
	throw std::runtime_error(combinedMsg);
}


void	forge::parser::parse_parameter_declaration( std::vector<parameter_declaration> &outParameters )
{
	while(mCurrToken != mTokens->end()) {
		if (expect_token_type(newline_token)) {
			break;
		}
		
		parameter_declaration	newParam;
		if (expect_identifier(identifier_at_operator)) {
			newParam.mIsReference = true;
		}
		if (const std::string *paramName = expect_unquoted_string()) {
			newParam.mName = *paramName;
		}
		
		if (newParam.mName.length() == 0) {
			throw_parse_error("Expected parameter name ");
		}
		
		outParameters.push_back(newParam);
		
		if (!expect_identifier(identifier_comma_operator)) {
			break;
		}
	}
}


forge::value	forge::parser::parse_one_value()
{
	if (const token *numToken = expect_token_type(integer_token)) {
		value	theValue;
		theValue.mValue = numToken->mText;
		return theValue;
	} else if (const token *numToken = expect_token_type(number_token)) {
		value	theValue;
		theValue.mValue = numToken->mText;
		return theValue;
	} else if (const token *numToken = expect_token_type(string_token)) {
		value	theValue;
		theValue.mValue = numToken->mText;
		return theValue;
	}
	
	throw_parse_error("Expected a value");
}


void	forge::parser::parse_one_line( handler_definition &outHandler )
{
	skip_empty_lines();
	
	if (const std::string *handlerName = expect_unquoted_string()) {
		handler_call	newCall;
		newCall.mName = *handlerName;
		
		while (!expect_token_type(newline_token, peek)) {
			newCall.mParameters.push_back(parse_one_value());
			
			if (!expect_identifier(identifier_comma_operator)) {
				break;
			}
		}
		
		if (expect_token_type(newline_token) == nullptr) {
			throw_parse_error("Expected end of line");
		}
		
		outHandler.mCommands.push_back(newCall);
	} else {
		throw_parse_error("Expected handler name");
	}
	
	skip_empty_lines();
}


void	forge::parser::parse_handler( identifier_type inType, handler_definition &outHandler )
{
	if (const std::string *handlerName = expect_unquoted_string()) {
		outHandler.mName = *handlerName;
		parse_parameter_declaration(outHandler.mParameters);

		while (mCurrToken != mTokens->end()) {
			auto saveToken = mCurrToken;
			if (expect_identifier(identifier_end) && expect_unquoted_string(*handlerName)) {
				break;
			} else {
				mCurrToken = saveToken;
			}
			
			parse_one_line(outHandler);
		}
	} else {
		throw_parse_error("Expected handler name");
	}
}


void	forge::parser::skip_rest_of_line()
{
	while (!expect_token_type(newline_token) && mCurrToken != mTokens->end()) {
		std::cout << mCurrToken->mText;
		++mCurrToken;
	}
}


void	forge::parser::skip_empty_lines()
{
	while (expect_token_type(newline_token)) {
		// already skipped it
	}
}


const forge::token*	forge::parser::expect_token_type( token_type inType, skip_type inSkip )
{
	if (mCurrToken != mTokens->end() && mCurrToken->mType == inType) {
		const token* theToken = &(*mCurrToken);
		if (inSkip == skip_type::skip) {
			++mCurrToken;
		}
		return theToken;
	}
	
	return nullptr;
}


bool	forge::parser::expect_identifier( identifier_type inType, skip_type inSkip )
{
	if (mCurrToken != mTokens->end() && mCurrToken->is_identifier(inType)) {
		if (inSkip == skip_type::skip) {
			++mCurrToken;
		}
		return true;
	}
	
	return false;
}


const std::string	*forge::parser::expect_string()
{
	if (mCurrToken != mTokens->end() && mCurrToken->mType == string_token) {
		const std::string *str = &mCurrToken->mText;
		++mCurrToken;
		return str;
	}
	
	return nullptr;
}


const std::string	*forge::parser::expect_unquoted_string( const std::string inStr )
{
	if (mCurrToken != mTokens->end() && mCurrToken->mType == identifier_token
		&& (inStr.length() == 0 || inStr.compare(mCurrToken->mText) == 0)) {
		const std::string *str = &mCurrToken->mText;
		++mCurrToken;
		return str;
	}
	
	return nullptr;
}


void	forge::parser::parse( std::vector<token>& inTokens, script &outScript )
{
	mTokens = &inTokens;
	mCurrToken = inTokens.begin();
	
	while (mCurrToken != mTokens->end()) {
		skip_empty_lines();
		
		if (expect_identifier(identifier_on)) {
			handler_definition	theHandler;
			parse_handler(identifier_on, theHandler);
			outScript.mHandlers.push_back(theHandler);
		}
	}
}
