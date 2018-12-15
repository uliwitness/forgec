//
//  parser.hpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include <vector>
#include "tokenizer.hpp"
#include "forgelib.hpp"


namespace forge {
	
	enum skip_type {
		skip,
		peek
	};
	
	class parse_error : public std::exception {
	public:
		parse_error( const std::string &str, const std::string& fileName, size_t offsetInFile, size_t lineNumber, size_t offsetInLine ) : mMessage(str), mFileName(fileName), mOffsetInFile(offsetInFile), mLineNumber(lineNumber), mOffsetInLine(offsetInLine) {}
		
		virtual const char* what() { std::string msg(mFileName); msg.append(":"); msg.append(std::to_string(mLineNumber)); msg.append(":"); msg.append(std::to_string(mOffsetInLine)); msg.append(": error: "); msg.append(mMessage); return msg.c_str(); }
		
		std::string	mMessage;
		std::string	mFileName;
		size_t		mOffsetInFile;
		size_t		mLineNumber;
		size_t		mOffsetInLine;
	};
	
	class variable_value : public static_string {
	public:
	};
	
	class parameter_declaration {
	public:
		std::string	mName;
		bool		mIsReference = false;
		void	print( std::ostream &dest ) {
			if (mIsReference) {
				dest << "@";
			}
			dest << mName;
		}
	};
	
	class handler_call : public stack_suitable_value {
	public:
		std::string							mName;
		std::vector<stack_suitable_value *>	mParameters;

		virtual void		set( int64_t inNum );
		virtual int64_t		get_int64() const;
		
		virtual void		set( double inNum );
		virtual double		get_double() const;
		
		virtual void		set( std::string inString );
		virtual std::string	get_string() const;
		
		virtual void		set_value_for_key( const value& inValue, const std::string &inKey );
		virtual void		get_value_for_key( value& outValue, const std::string &inKey ) const;
		
		virtual void		copy_to( value &dest ) const;

		void	print( std::ostream &dest ) {
			dest << get_string() << std::endl;
		}
	};
	
	class loop_call: public handler_call {
	public:
		std::vector<handler_call *>	mCommands;

		virtual std::string	get_string() const;
	};
	
	class variable_entry {
	public:
		std::string		mName;
		value_data_type	mTypesNeeded = value_data_type_NONE;

		std::string 	flags_string();
	};
	
	class handler_definition {
	public:
		std::string								mName;
		std::vector<parameter_declaration>		mParameters;
		std::vector<handler_call *>				mCommands;
		std::map<std::string,variable_entry>	mVariables;
		
		void	print( std::ostream &dest ) {
			dest << mName << "(";
			bool isFirst = true;
			for(auto p : mParameters) {
				if (isFirst) {
					isFirst = false;
				} else {
					dest << ", ";
				}
				p.print(dest);
			}
			dest << ") {" << std::endl;
			
			for (auto v : mVariables) {
				dest << "\tvar\t";
				dest << v.second.mName << ": " << v.second.flags_string() << std::endl;
			}

			for (auto c : mCommands) {
				dest << "\t";
				c->print(dest);
			}
			dest << "}" << std::endl;
		}
	};
	
	class script {
	public:
		std::vector<handler_definition>		mHandlers;
		
		template<class T>
		T *	take_ownership_of(T *inValue) { mValuePool.push_back(std::unique_ptr<stack_suitable_value>(inValue)); return inValue; }

		void	print( std::ostream &dest ) { for (auto h : mHandlers) { h.print(dest); } }
		
	protected:
		std::vector<std::unique_ptr<stack_suitable_value>> mValuePool;
	};
	
	class parser {
	public:
		void	parse( std::vector<token>& inTokens, script &outScript );
		
	protected:
		void	skip_empty_lines();
		void	skip_rest_of_line();
		
		void	parse_handler( identifier_type inType, handler_definition &outHandler );
		void	parse_parameter_declaration( std::vector<parameter_declaration> &outParameters );
		bool	combine_binary_operator_tokens_if_appropriate( identifier_type &operator1, identifier_type operator2 );
		void					make_variable_for_name( const std::string &varName, value_data_type inTypeRequired );
		const std::string 		*parse_variable_name( value_data_type inTypeRequired );
		stack_suitable_value	*parse_expression();
		stack_suitable_value	*parse_one_value();
		void					parse_one_line(std::vector<handler_call *> &outCommands);

		void	throw_parse_error( const char *msg ) __attribute__((noreturn));
		
		const token			*expect_token_type( token_type inType, skip_type inSkip = skip_type::skip );
		bool				expect_identifier( identifier_type inType, skip_type inSkip = skip_type::skip );
		const std::string	*expect_unquoted_string( const std::string inStr = std::string() );
		const std::string	*expect_string();

		std::vector<token> 					*mTokens = nullptr;
		std::vector<token>::const_iterator	mCurrToken;
		script								*mScript = nullptr;
		handler_definition					*mCurrHandler = nullptr;
	};
	
}

#endif /* parser_hpp */
