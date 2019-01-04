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


#define DATA_TYPE_FLAGS			X(int64) \
								X(double) \
								X(string) \
								X(map) \
								X(bool) \
								X(NONE)

#define X(n)	{ forge::value_data_type_ ## n, #n },
static struct { forge::value_data_type flag; const char* flagname; }	flags[] = {
	DATA_TYPE_FLAGS
};
#undef X


forge::value_data_type	forge::syntax_c_parameter::value_data_type( std::string inTypeStr )
{
	for (size_t x = 0; flags[x].flag != value_data_type_NONE; ++x) {
		if (inTypeStr.compare(flags[x].flagname) == 0) {
			return flags[x].flag;
			break;
		}
	}
	
	return value_data_type_NONE;
}

forge::value_data_type	forge::syntax_c_parameter::value_data_type() const
{
	return syntax_c_parameter::value_data_type(mType);
}

std::string forge::syntax_label::flags_string()
{
	return syntax_label::flags_string( mType );
}


std::string forge::syntax_label::flags_string( forge::value_data_type inType )
{
	std::string	result;
	
	for (size_t x = 0; flags[x].flag != value_data_type_NONE; ++x) {
		if (inType & flags[x].flag) {
			result.append(flags[x].flagname);
			result.append(" ");
		}
	}
	
	return result;
}


std::string forge::variable_entry::flags_string()
{
	std::string	result;
	
	for (size_t x = 0; flags[x].flag != value_data_type_NONE; ++x) {
		if (mTypesNeeded & flags[x].flag) {
			result.append(flags[x].flagname);
			result.append(" ");
		}
	}
	
	return result;
}


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
	
	skip_empty_lines(); // Ensure we *always* skip trailing newlines, whether we have params or not, otherwise empty handlers with no params parse their "end" line as a handler call.
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
	} else if (operator1 == identifier_is && operator2 == identifier_not) {
		operator1 = identifier_not_equal_operator;
		return true;
	}
	
	return false;
}


forge::stack_suitable_value	*forge::parser::parse_expression()
{
	stack_suitable_value	*theOperand = parse_one_value();
	int						lastOperatorPrecedence = 0;
	const token 			*operatorToken = nullptr;
	operator_call			*prevCall = nullptr;
	
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

		operator_call *operation = mScript->take_ownership_of(new operator_call);
		operation->mName = token::operator_function(operatorType);

		int		currOperatorPrecedence = operatorToken->operator_precedence();
		if( currOperatorPrecedence > lastOperatorPrecedence && prevCall ) {
			operation->mParameters.push_back( prevCall->mParameters[1] );
			operation->mParameters.push_back( secondOperand );
			prevCall->mParameters[1] = operation;
		} else {
			operation->mParameters.push_back( firstOperand );
			operation->mParameters.push_back( secondOperand );
			theOperand = operation;
		}
		
		lastOperatorPrecedence = currOperatorPrecedence;
		prevCall = operation;
	}
	return theOperand;
}


void	forge::parser::make_variable_for_name( const std::string &varName, const std::string &cppVarName, value_data_type inTypeRequired )
{
	auto foundVariable = mCurrHandler->mVariables.find(varName);
	if (foundVariable != mCurrHandler->mVariables.end()) {
		foundVariable->second.mTypesNeeded |= inTypeRequired;
	} else {
		variable_entry	var = { .mName = varName, .mCppName = cppVarName, .mTypesNeeded = inTypeRequired };
		mCurrHandler->mVariables[varName] = var;
	}
}


const std::string *forge::parser::parse_variable_name( value_data_type inTypeRequired )
{
	const std::string *varName = expect_unquoted_string();
	if (varName) {
		std::string cppVarName("var_");
		cppVarName.append(*varName);
		
		make_variable_for_name(*varName, cppVarName, inTypeRequired);
	}
	
	return varName;
}


forge::stack_suitable_value	*forge::parser::parse_one_value()
{
	if (const token *numToken = expect_token_type(integer_token)) {
		static_int64	*theValue = mScript->take_ownership_of(new static_int64);
		theValue->set_string(numToken->mText);
		return theValue;
	} else if (const token *numToken = expect_token_type(number_token)) {
		static_double	*theValue = mScript->take_ownership_of(new static_double);
		theValue->set_string(numToken->mText);
		return theValue;
	} else if (const token *numToken = expect_token_type(string_token)) {
		static_string	*theValue = mScript->take_ownership_of(new static_string);
		theValue->set_string(numToken->mText);
		return theValue;
	} else {
		std::vector<token>::const_iterator savedStartToken = mCurrToken;
		if (expect_identifier(identifier_the) || expect_unquoted_string("", peek)) { // Eat a "the" or just scan ahead if it's an identifier.
			const std::string *propertyName = expect_unquoted_string();
			if (propertyName) {
				if (expect_identifier(identifier_of)) {
					stack_suitable_value *targetValue = parse_one_value();
					handler_call *theValue = mScript->take_ownership_of(new handler_call);
					theValue->mName = ".";
					theValue->mParameters.push_back(targetValue);
					static_string	*propNameValue = mScript->take_ownership_of(new static_string);
					propNameValue->set_string(*propertyName);
					theValue->mParameters.push_back(propNameValue);
					return theValue;
				}
			}
		}
		
		mCurrToken = savedStartToken;
		
		if (const std::string *handlername = expect_unquoted_string()) {
			if (expect_identifier(identifier_open_parenthesis_operator)) {
				handler_call	*newCall = mScript->take_ownership_of(new handler_call);
				newCall->mName = *handlername;
				newCall->mName.insert(0, "fun_");
				
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
				mCurrToken = savedStartToken; // backtrack, that identifier wasn't a function.
			}
		} else {
			throw_parse_error("Expected a value here.");
		}

		handler_call	*newCall = try_to_parse_command( mFunctions );
		if (newCall)
			return newCall;
		
		const std::string *varName = expect_unquoted_string();
		if (!varName) {
			return nullptr;
		}
		std::string cppVarName("var_");
		cppVarName.append(*varName);
		make_variable_for_name( *varName, cppVarName, value_data_type_NONE );
		variable_value	*theValue = mScript->take_ownership_of(new variable_value);
		theValue->set_string(cppVarName);
		return theValue;
	}
}


forge::handler_call *forge::parser::try_to_parse_command( const std::vector<syntax_command> &registeredCommands )
{
	bool foundCommand = false;
	
	handler_call	*newCall = nullptr;
	
	for (auto currCmd : registeredCommands) {
		for (auto currLabel : currCmd.mParameters) {
			bool labelsMatch = true;
			std::vector<token>::const_iterator savedStartToken = mCurrToken;
			for (auto currIdentifier : currLabel.mLabels) {
				if (!expect_unquoted_string_or_operator(currIdentifier)) {
					if (newCall) {
						std::string msg("Expected '");
						msg.append(currIdentifier);
						msg.append("' here.");
						throw_parse_error(msg.c_str());
					} else {
						labelsMatch = false;
						break;
					}
				}
			}
			if (!labelsMatch) {
				mCurrToken = savedStartToken; // Backtrack so we can try any partial matches again.
			} else {
				foundCommand = true;
				if (!newCall) {
					newCall = mScript->take_ownership_of(new handler_call);
					newCall->mName = currCmd.mCName;
					newCall->mParameters.resize(currCmd.mCParameters.size(), nullptr);
				}
				if (currLabel.mType != value_data_type_NONE) {
					forge::stack_suitable_value	*param = parse_one_value();
					syntax_c_parameter &cParameter = currCmd.mCParameters[currLabel.mCParameterName];
					size_t paramIndex = cParameter.mParameterIndex;
					newCall->mParameters[paramIndex] = param;
				}
			}
		}
		if (foundCommand) {
			skip_empty_lines();
			return newCall;
		}
	}

	return nullptr;
}


void	forge::parser::parse_one_line( std::vector<handler_call *> &outCommands )
{
	skip_empty_lines();
	
	if (expect_identifier(identifier_put)) {
		handler_call	*newCall = mScript->take_ownership_of(new handler_call);
		newCall->mParameters.push_back(parse_expression());
		if(expect_identifier(identifier_into)) {
			newCall->mParameters.push_back(parse_expression());
			newCall->mName = "forge::assign_to";
		} else if(expect_identifier(identifier_before)) {
			newCall->mParameters.push_back(parse_expression());
			newCall->mName = "forge::prefix_to";
		} else if(expect_identifier(identifier_after)) {
			newCall->mParameters.push_back(parse_expression());
			newCall->mName = "forge::append_to";
		} else {
			newCall->mName = "cmd_put";

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
			const std::string *varName = parse_variable_name(value_data_type_int64);
			if (varName == nullptr) {
				throw_parse_error("Expected repeated variable name here.");
			}
			if (!expect_identifier(identifier_equals_operator)
				&& !expect_identifier(identifier_is)
				&& !expect_identifier(identifier_from)) {
				throw_parse_error("Expected 'from' after variable name here.");
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
			std::string cppVarName("var_");
			cppVarName.append(*varName);
			variable_value *counterVarName = mScript->take_ownership_of(new variable_value(cppVarName));
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
			if (expect_identifier(identifier_end) && expect_identifier(identifier_repeat)) {
				break;
			} else {
				mCurrToken = saveToken;
			}
			
			parse_one_line(newCall->mCommands);
		}
		
		outCommands.push_back(newCall);
	} else if (expect_identifier(identifier_end)) {
		if (const std::string *handlerName = expect_unquoted_string()) {
			std::stringstream msg;
			msg << "Unbalanced \"end " << *handlerName << "\" found.";
			throw_parse_error(msg.str().c_str());
		} else {
			throw_parse_error("Unbalanced \"end\" found.");
		}
	} else {
		handler_call *newCall = try_to_parse_command( mCommands );
		if (newCall) {
			outCommands.push_back(newCall);
			return;
		}
		
		if (const std::string *handlerName = expect_unquoted_string()) {
			newCall = mScript->take_ownership_of(new handler_call);
			newCall->mName = *handlerName;
			newCall->mName.insert(0, "cmd_");
			
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
	}
	
	skip_empty_lines();
}


void	forge::parser::parse_handler( identifier_type inType, handler_definition &outHandler )
{
	if (const std::string *handlerName = expect_unquoted_string()) {
		outHandler.mCName = *handlerName;
		outHandler.mCName.insert(0, (inType == identifier_on) ? "cmd_" : "fun_");
		outHandler.mName = *handlerName;
		parse_parameter_declaration(outHandler.mParameters);
		
		for (auto &currParameter : outHandler.mParameters) {
			variable_entry	var;
			var.mName = currParameter.mName;
			var.mTypesNeeded = value_data_type_ALL;
			var.mIsParameter = true;
			outHandler.mVariables[currParameter.mName] = var;
		}

		while (mCurrToken != mTokens->end()) {
			auto saveToken = mCurrToken;
			if (expect_identifier(identifier_end) ) {
				if (expect_unquoted_string(*handlerName)) {
					break;
				}
				mCurrToken = saveToken;
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


const std::string	*forge::parser::expect_unquoted_string( const std::string inStr, skip_type inSkip )
{
	if (mCurrToken != mTokens->end() && mCurrToken->mType == identifier_token
		&& (inStr.length() == 0 || inStr.compare(mCurrToken->mText) == 0)) {
		const std::string *str = &mCurrToken->mText;
		if (inSkip == skip) {
			++mCurrToken;
		}
		return str;
	}
	
	return nullptr;
}


const std::string	*forge::parser::expect_unquoted_string_or_operator( const std::string inStr )
{
	if (mCurrToken != mTokens->end() && (mCurrToken->mType == identifier_token || mCurrToken->mType == operator_token)
		&& (inStr.length() == 0 || inStr.compare(mCurrToken->mText) == 0)) {
		const std::string *str = &mCurrToken->mText;
		++mCurrToken;
		return str;
	}
	
	return nullptr;
}


void	forge::parser::parse_import_statement()
{
	syntax_command	cmd;
	bool	isCommand = false;
	if (expect_identifier(identifier_function)) {
		isCommand = false;
		
		const std::string *returnType = expect_unquoted_string();
		if (!returnType) {
			throw_parse_error("Expected return type after 'import function'.");
		}
		
		cmd.mReturnType = syntax_c_parameter::value_data_type( *returnType );
	} else if(expect_identifier(identifier_command)) {
		isCommand = true;
	} else {
		throw_parse_error("Expected 'command' or 'function' after 'import'.");
	}
	
	const std::string *cName = expect_unquoted_string_or_operator();
	if (!cName) {
		throw_parse_error("Expected C function name (identifier) after 'import command/function'.");
	}
	cmd.mCName = *cName;
	
	if (!expect_identifier(identifier_open_parenthesis_operator)) {
		throw_parse_error("Expected '(' and parameter list after C function name in import.");
	}
	
	size_t	parameterIndex = 0;
	
	while (!expect_token_type(newline_token, peek) && !expect_identifier(identifier_close_parenthesis_operator, peek)) {
		const std::string *paramType = expect_unquoted_string();
		if (!paramType) {
			throw_parse_error("Expected parameter type here.");
		}
		const std::string *paramName = expect_unquoted_string();
		if (!paramName) {
			throw_parse_error("Expected parameter name here.");
		}
		
		cmd.mCParameters[*paramName] = syntax_c_parameter{ .mName = *paramName, .mType = *paramType, .mParameterIndex = parameterIndex };
		++parameterIndex;
		
		if (!expect_identifier(identifier_comma_operator)) {
			break;
		}
	}
	
	if (!expect_identifier(identifier_close_parenthesis_operator)) {
		throw_parse_error("Expected ')' at end of C function parameter list in import.");
	}
	
	if (!expect_identifier(identifier_as)) {
		throw_parse_error("Expected 'as' after C function in import.");
	}
	
	while (!expect_token_type(newline_token, peek)) {
		syntax_label	label;
		while (!expect_identifier(identifier_less_than_operator, peek) && !expect_token_type(newline_token, peek)) {
			const std::string *labelWord = expect_unquoted_string_or_operator();
			if (!labelWord) {
				throw_parse_error("Expected parameter label or '<' introducing parameter here.");
			}
			label.mLabels.push_back(*labelWord);
		}
		
		if (expect_identifier(identifier_less_than_operator)) {
			const std::string *paramName = expect_unquoted_string();
			if (!paramName) {
				throw_parse_error("Expected a <parameter type> here.");
			}
			
			syntax_c_parameter &cParameter = cmd.mCParameters[*paramName];
			label.mType = cParameter.value_data_type();
			label.mCParameterName = *paramName;
			
			if (!expect_identifier(identifier_greater_than_operator)) {
				throw_parse_error("Expected '>' after parameter.");
			}
		}
		cmd.mParameters.push_back(label);
	}
	if (isCommand) {
		mCommands.push_back(cmd);
	} else {
		mFunctions.push_back(cmd);
	}
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
			mCurrHandler = &theHandler;
			parse_handler(identifier_on, theHandler);
			mCurrHandler = nullptr;
			outScript.mHandlers.push_back(theHandler);
			if (mFirstHandlerName.length() == 0) {
				mFirstHandlerName = theHandler.mName;
				mFirstHandlerType = identifier_on;
			}
		} else if (expect_identifier(identifier_function)) {
			handler_definition	theHandler;
			mCurrHandler = &theHandler;
			parse_handler(identifier_function, theHandler);
			mCurrHandler = nullptr;
			outScript.mHandlers.push_back(theHandler);
			if (mFirstHandlerName.length() == 0) {
				mFirstHandlerName = theHandler.mName;
				mFirstHandlerType = identifier_function;
			}
		} else if (expect_identifier(identifier_import)) {
			parse_import_statement();
		}
	}
	
	mCurrToken = mTokens->end();
	mScript = nullptr;
	mTokens = nullptr;
}


void	forge::parser::print(std::ostream &dest) {
	for (auto currCmd : mCommands) {
		currCmd.print(dest);
		dest << std::endl;
	}
	for (auto currCmd : mFunctions) {
		currCmd.print(dest);
		dest << std::endl;
	}
}


void	forge::script::generate_code( forge::codegen& inCodegen )
{
	inCodegen.start_encoding_script(*this);
	
	for (auto &currHandler : mHandlers) {
		currHandler.generate_code(inCodegen);
	}
	
	inCodegen.end_encoding_script(*this);
}


void	forge::handler_definition::generate_code( forge::codegen& inCodegen )
{
	inCodegen.start_encoding_handler(*this);
	
	for (auto &currParameter : mParameters) {
		currParameter.generate_code(inCodegen);
	}
	for (auto &currVariable : mVariables) {
		currVariable.second.generate_code(inCodegen);
	}
	
	for (auto currCommand : mCommands) {
		inCodegen.start_encoding_command(*this, *currCommand);
		currCommand->generate_code(inCodegen);
		inCodegen.end_encoding_command(*this, *currCommand);
	}

	inCodegen.end_encoding_handler(*this);
}


void	forge::parameter_declaration::generate_code( forge::codegen& inCodegen )
{
	
}


void	forge::variable_entry::generate_code( forge::codegen& inCodegen )
{
	
}


void	forge::handler_call::generate_code( forge::codegen& inCodegen )
{
	inCodegen.start_encoding_handler_call(*this);
	bool isFirst = true;
	for (auto currParam : mParameters) {
		inCodegen.start_encoding_handler_call_parameter(*this, currParam, isFirst);
		forge::generate_code(currParam, inCodegen);
		inCodegen.end_encoding_handler_call_parameter(*this, currParam, isFirst);
		isFirst = false;
	}
	inCodegen.end_encoding_handler_call(*this);
}


void	forge::handler_call::set_int64( int64_t inNum )
{
	
}


int64_t	forge::handler_call::get_int64() const
{
	return 0;
}



void	forge::handler_call::set_double( double inNum )
{
	
}


double	forge::handler_call::get_double() const
{
	return 0.0;
}


void	forge::handler_call::set_bool( bool inBool )
{
	
}


bool	forge::handler_call::get_bool() const
{
	return false;
}


void	forge::handler_call::set_string( std::string inString )
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
	outValue.set_string(std::string());
}


void	forge::handler_call::copy_to( value &dest ) const
{
	dest.set_string(std::string());
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


void	forge::codegen::start_encoding_script( const forge::script &inScript )
{
	mCode << "#include \"forgelib.hpp\"" << std::endl
		<< "#include <vector>" << std::endl << std::endl;
	
	for (auto &currHandler : inScript.mHandlers) {
		mCode << "forge::variant " << currHandler.mCName << "(std::vector<forge::variant> params);" << std::endl;
	}
	mCode << std::endl;

	mCode << "int main(int argc, const char * argv[]) {" << std::endl
	<< "\tforge::Process::currentProcess().set_args(argc, argv);" << std::endl
	<< "\tcmd_startUp(forge::Process::currentProcess().parameters);" << std::endl
	<< "\treturn 0;" << std::endl
	<< "}" << std::endl << std::endl;
}


void	forge::codegen::start_encoding_handler( const forge::handler_definition &inHandler )
{
	mCode << "forge::variant " << inHandler.mCName << "(std::vector<forge::variant> params) {" << std::endl;

	size_t paramNum = 0;
	for (auto &currParam : inHandler.mParameters) {
		mCode << "\tforge::variant var_"; // TODO: Look up type in mVariables and narrow it down as necessary.
		mCode << currParam.mName << "(params[" << paramNum << "]);" << std::endl;
		++paramNum;
	}

	for (auto &currVariable : inHandler.mVariables) {
		if (currVariable.second.mIsParameter) {
			continue;
		}
		
		mCode << "\tforge::variant "; // TODO: Narrow down type.
		mCode << currVariable.second.mCppName;
		mCode << ";" << std::endl;
		mCode << "\t" << currVariable.second.mCppName << ".set_string(\"" << currVariable.second.mName << "\");" << std::endl;
	}
	mCode << std::endl;
}


void	forge::codegen::end_encoding_handler( const forge::handler_definition &inHandler )
{
	mCode << "\treturn forge::variant();" << std::endl;
	mCode << "}" << std::endl << std::endl;
}


void	forge::codegen::start_encoding_command( const forge::handler_definition &inHandler, const forge::handler_call &inCall )
{
	mCode << "\t";
}


void	forge::codegen::end_encoding_command( const forge::handler_definition &inHandler, const forge::handler_call &inCall )
{
	mCode << ";" << std::endl;
}


void	forge::codegen::start_encoding_handler_call( const forge::handler_call &inHandler )
{
	mCode << inHandler.mName << "(";
}


void	forge::codegen::start_encoding_handler_call_parameter( const forge::handler_call &inHandler, stack_suitable_value *inValue, bool isFirst )
{
	if (!isFirst) {
		mCode << ", ";
	}
}


void	forge::codegen::end_encoding_handler_call_parameter( const forge::handler_call &inHandler, stack_suitable_value *inValue, bool isFirst )
{

}


void	forge::codegen::end_encoding_handler_call( const forge::handler_call &inHandler )
{
	mCode << ")";
}


void	forge::codegen::end_encoding_script( const forge::script &inScript )
{
	mCode << std::endl; // Make sure file is a nice Unix citizen and ends on a newline.
}


void	forge::codegen::encode_value( stack_suitable_value* inValue )
{
	if (inValue->data_type() & value_data_type_int64) {
		mCode << "forge::static_int64(" << inValue->get_string() << ")";
	} else if (inValue->data_type() & value_data_type_double) {
		mCode << "forge::static_double(" << inValue->get_string() << ")";
	} else if (inValue->data_type() & value_data_type_bool) {
		mCode << "forge::static_bool(" << inValue->get_string() << ")";
	} else if (inValue->data_type() & value_data_type_string) {
		mCode << "forge::static_string(\"" << inValue->get_string() << "\")";
	} else {
		mCode << inValue->get_string();
	}
}


void forge::generate_code( stack_suitable_value *inValue, forge::codegen& inCodegen )
{
	handler_call *call = dynamic_cast<handler_call *>(inValue);
	if (call) {
		call->generate_code(inCodegen);
	} else {
		inCodegen.encode_value( inValue );
	}
}
