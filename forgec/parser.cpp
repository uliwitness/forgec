//
//  parser.cpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include "parser.hpp"
#include <iostream>
#include <string>
#include <stdexcept>


void	forge::parser::throw_parse_error( const char *msg )
{
	std::string	combinedMsg;
	token currToken = (mCurrToken != mTokens->end()) ? *mCurrToken : mTokens->back();
	if (mCurrToken == mTokens->end()) {
		currToken.mStartOffset = currToken.mEndOffset; // Make sure we indicate past *end* of last token.
	}
	
	throw forge::parse_error(msg, currToken.mFileName, currToken.mStartOffset, currToken.mLineNumber, currToken.mStartOffset - currToken.mLineStartOffset);
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
			throw_parse_error("Expected parameter name here.");
		}
		
		outParameters.push_back(newParam);
		
		if (!expect_identifier(identifier_comma_operator)) {
			break;
		}
	}
}


bool	forge::parser::combine_binary_operator_tokens_if_appropriate( identifier_type &operator1, identifier_type operator2 ) {
	if (operator1 == identifier_ampersand_operator && operator2 == identifier_ampersand_operator) {
		operator1 = identifier_double_ampersand_operator;
		return true;
	} else if (operator1 == identifier_less_than_operator && operator2 == identifier_equals_operator) {
		operator1 = identifier_less_equal_operator;
		return true;
	} else if (operator1 == identifier_greater_than_operator && operator2 == identifier_equals_operator) {
		operator1 = identifier_greater_equal_operator;
		return true;
	} else if (operator1 == identifier_less_than_operator && operator2 == identifier_greater_than_operator) {
		operator1 = identifier_not_equal_operator;
		return true;
	}
	
	return false;
}


forge::stack_suitable_value	*forge::parser::parse_expression()
{
	stack_suitable_value	*theOperand = parse_one_value();
	int						lastOperatorPrecedence = 0;
	const token *operatorToken = nullptr;
	handler_call* prevCall = nullptr;
	
	while (!expect_identifier(identifier_comma_operator, peek)
		   && !expect_identifier(identifier_close_parenthesis_operator, peek)
		   && (operatorToken = expect_token_type(operator_token))) {
		stack_suitable_value	*firstOperand = theOperand;
		identifier_type			operatorType = operatorToken->mIdentifierType;
		
		const token *operatorToken2 = expect_token_type(operator_token, peek);
		if (operatorToken2) {
			if (combine_binary_operator_tokens_if_appropriate(operatorType, operatorToken2->mIdentifierType)) {
				++mCurrToken;
			}
		}
		
		stack_suitable_value	*secondOperand = parse_one_value();

		handler_call *operation = mScript->take_ownership_of(new handler_call);
		operation->mName = tokenizer::string_from_identifier_type(operatorType);

		if( operatorToken->operator_precedence() > lastOperatorPrecedence
		   && (prevCall = dynamic_cast<handler_call*>(theOperand)) ) {
			
			operation->mParameters.push_back( prevCall->mParameters[1] );
			operation->mParameters.push_back( secondOperand );
			prevCall->mParameters[1] = operation;
		} else {
			operation->mParameters.push_back( firstOperand );
			operation->mParameters.push_back( secondOperand );
			theOperand = operation;
		}
	}
	return theOperand;
}

forge::stack_suitable_value	*forge::parser::parse_one_value()
{
	if (const token *numToken = expect_token_type(integer_token)) {
		static_int64	*theValue = mScript->take_ownership_of(new static_int64);
		theValue->set(numToken->mText);
		return theValue;
	} else if (const token *numToken = expect_token_type(number_token)) {
		static_double	*theValue = mScript->take_ownership_of(new static_double);
		theValue->set(numToken->mText);
		return theValue;
	} else if (const token *numToken = expect_token_type(string_token)) {
		static_string	*theValue = mScript->take_ownership_of(new static_string);
		theValue->set(numToken->mText);
		return theValue;
	} else if (const std::string *handlername = expect_unquoted_string()) {
		if (expect_identifier(identifier_open_parenthesis_operator)) {
			handler_call	*newCall = mScript->take_ownership_of(new handler_call);
			newCall->mName = *handlername;

			while (!expect_identifier(identifier_close_parenthesis_operator, peek)) {
				newCall->mParameters.push_back(parse_expression());
				
				if (!expect_identifier(identifier_comma_operator)) {
					break;
				}
			}
			
			if (!expect_identifier(identifier_close_parenthesis_operator)) {
				throw_parse_error("Expected closing bracket here.");
			}
			return newCall;
		} else {
			static_string	*theValue = mScript->take_ownership_of(new static_string);
			theValue->set(*handlername);
			return theValue;
		}
	}
	
	throw_parse_error("Expected a value here.");
}


void	forge::parser::parse_one_line( std::vector<handler_call *> &outCommands )
{
	skip_empty_lines();
	
	if (expect_identifier(identifier_put)) {
		handler_call	*newCall = mScript->take_ownership_of(new handler_call);
		newCall->mParameters.push_back(parse_expression());
		if(expect_identifier(identifier_into)) {
			newCall->mParameters.push_back(parse_expression());
			newCall->mName = "assign_to";
		} else if(expect_identifier(identifier_before)) {
			newCall->mParameters.push_back(parse_expression());
			newCall->mName = "prefix_to";
		} else if(expect_identifier(identifier_after)) {
			newCall->mParameters.push_back(parse_expression());
			newCall->mName = "append_to";
		} else {
			newCall->mName = "put";

			if (expect_identifier(identifier_comma_operator)) {
				while (!expect_token_type(newline_token, peek)) {
					newCall->mParameters.push_back(parse_expression());
					
					if (!expect_identifier(identifier_comma_operator)) {
						break;
					}
				}
			}
		}
		
		if (expect_token_type(newline_token) == nullptr) {
			throw_parse_error("Expected end of line here.");
		}
		
		outCommands.push_back(newCall);
	} else if (expect_identifier(identifier_repeat)) {
		loop_call	*newCall = mScript->take_ownership_of(new loop_call);
		if (expect_identifier(identifier_while)) {
			newCall->mName = "while";
			newCall->mParameters.push_back(parse_expression());
		} else if (expect_identifier(identifier_with)) {
			const std::string *varName = expect_unquoted_string();
			if (varName == nullptr) {
				throw_parse_error("Expected repeated variable name here.");
			}
			if (!expect_identifier(identifier_equals_operator)) {
				throw_parse_error("Expected = after variable name here.");
			}
			stack_suitable_value *startNum = parse_expression();
			int stepSize = 1;
			if (expect_identifier(identifier_down)) {
				stepSize = -1;
			}
			if (!expect_identifier(identifier_to)) {
				throw_parse_error("Expected 'to' after start number here.");
			}
			stack_suitable_value *endNum = parse_expression();
			newCall->mName = "for";
			static_string *counterVarName = mScript->take_ownership_of(new static_string(*varName));
			static_int64 *stepSizeValue = mScript->take_ownership_of(new static_int64(stepSize));
			newCall->mParameters.push_back(counterVarName);
			newCall->mParameters.push_back(startNum);
			newCall->mParameters.push_back(endNum);
			newCall->mParameters.push_back(stepSizeValue);
		} else if (expect_identifier(identifier_for)) {
			newCall->mName = "for";
			static_string *counterVarName = mScript->take_ownership_of(new static_string(""));
			static_int64 *startNum = mScript->take_ownership_of(new static_int64(1));
			stack_suitable_value *endNum = parse_expression();
			static_int64 *stepSizeValue = mScript->take_ownership_of(new static_int64(1));
			newCall->mParameters.push_back(counterVarName);
			newCall->mParameters.push_back(startNum);
			newCall->mParameters.push_back(endNum);
			newCall->mParameters.push_back(stepSizeValue);

			if (expect_identifier(identifier_times)) {
				// Skip optional "times".
			}
		} else if (expect_identifier(identifier_until)) {
			newCall->mName = "until";
			newCall->mParameters.push_back(parse_expression());
		} else {
			newCall->mParameters.push_back(parse_expression());
			
			if (expect_identifier(identifier_times)) {
				// Skip optional "times".
			}
		}
		
		while (mCurrToken != mTokens->end()) {
			auto saveToken = mCurrToken;
			if (expect_identifier(identifier_end) && expect_unquoted_string("repeat")) {
				break;
			} else {
				mCurrToken = saveToken;
			}
			
			parse_one_line(newCall->mCommands);
		}
		
		outCommands.push_back(newCall);
	} else if (const std::string *handlerName = expect_unquoted_string()) {
		handler_call	*newCall = mScript->take_ownership_of(new handler_call);
		newCall->mName = *handlerName;
		
		while (!expect_token_type(newline_token, peek)) {
			newCall->mParameters.push_back(parse_expression());
			
			if (!expect_identifier(identifier_comma_operator)) {
				break;
			}
		}
		
		if (expect_token_type(newline_token) == nullptr) {
			throw_parse_error("Expected end of line here.");
		}
		
		outCommands.push_back(newCall);
	} else {
		throw_parse_error("Expected handler name here.");
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
			
			parse_one_line(outHandler.mCommands);
		}
	} else {
		throw_parse_error("Expected handler name here.");
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
	mScript = &outScript;
	
	while (mCurrToken != mTokens->end()) {
		skip_empty_lines();
		
		if (expect_identifier(identifier_on)) {
			handler_definition	theHandler;
			parse_handler(identifier_on, theHandler);
			outScript.mHandlers.push_back(theHandler);
		}
	}
}


void	forge::handler_call::set( int64_t inNum )
{
	
}


int64_t	forge::handler_call::get_int64() const
{
	return 0;
}



void	forge::handler_call::set( double inNum )
{
	
}


double	forge::handler_call::get_double() const
{
	return 0.0;
}



void	forge::handler_call::set( std::string inString )
{
	
}


std::string	forge::handler_call::get_string() const
{
	std::string msg(mName);
	msg.append("(");
	
	bool isFirst = true;
	for(auto p : mParameters) {
		if (isFirst) {
			isFirst = false;
		} else {
			msg.append(", ");
		}
		msg.append(p->get_string());
	}

	msg.append(")");
	return msg;
}


void	forge::handler_call::set_value_for_key( const value& inValue, const std::string &inKey )
{
	
}


void	forge::handler_call::get_value_for_key( value& outValue, const std::string &inKey ) const
{
	outValue.set(std::string());
}


void	forge::handler_call::copy_to( value &dest ) const
{
	dest.set(std::string());
}


std::string	forge::loop_call::get_string() const
{
	std::string msg(mName);
	msg.append("(");
	
	bool isFirst = true;
	for(auto p : mParameters) {
		if (isFirst) {
			isFirst = false;
		} else {
			msg.append(", ");
		}
		msg.append(p->get_string());
	}
	
	msg.append(") {\n");
	
	for (auto c : mCommands) {
		msg.append("\t\t");
		msg.append(c->get_string());
	}
	msg.append("\n\t}");

	return msg;
}
