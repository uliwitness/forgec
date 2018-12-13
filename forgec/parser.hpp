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


namespace forge {
	
	enum skip_type {
		skip,
		peek
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
	
	class value {
	public:
		~value() {}
		
		std::string	mValue;
		
		void	print( std::ostream &dest ) {
			dest << mValue;
		}
	};
	
	class handler_call {
	public:
		std::string			mName;
		std::vector<value>	mParameters;

		void	print( std::ostream &dest ) {
			dest << mName;
			for (auto p : mParameters) {
				dest << " ";
				p.print(dest);
			}
		}
	};
	
	class handler_definition {
	public:
		std::string							mName;
		std::vector<parameter_declaration>	mParameters;
		std::vector<handler_call>			mCommands;
		
		void	print( std::ostream &dest ) {
			dest << "on " << mName;
			for (auto p : mParameters) {
				dest << " ";
				p.print(dest);
			}
			dest << std::endl;
			
			for (auto c : mCommands) {
				dest << "\t";
				c.print(dest);
				dest << std::endl;
			}
			dest << "end " << mName << std::endl;
		}
	};
	
	class script {
	public:
		std::vector<handler_definition>	mHandlers;
		
		void	print( std::ostream &dest ) { for (auto h : mHandlers) { h.print(dest); } }
	};
	
	class parser {
	public:
		void	parse( std::vector<token>& inTokens, script &outScript );
		
	protected:
		void	skip_empty_lines();
		void	skip_rest_of_line();
		
		void	parse_handler( identifier_type inType, handler_definition &outHandler );
		void	parse_parameter_declaration( std::vector<parameter_declaration> &outParameters );
		value	parse_one_value();
		void	parse_one_line( handler_definition &outHandler );
		
		void	throw_parse_error( const char *msg ) __attribute__((noreturn));
		
		const token			*expect_token_type( token_type inType, skip_type inSkip = skip_type::skip );
		bool				expect_identifier( identifier_type inType, skip_type inSkip = skip_type::skip );
		const std::string	*expect_unquoted_string( const std::string inStr = std::string() );
		const std::string	*expect_string();

		std::vector<token> 					*mTokens;
		std::vector<token>::const_iterator	mCurrToken;
	};
	
}

#endif /* parser_hpp */
