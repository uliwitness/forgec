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
	};
	
	class handler_definition {
	public:
		std::vector<parameter_declaration>	mParameters;
	};
	
	class script {
	public:
		std::vector<handler_definition>	mHandlers;
	};
	
	class parser {
	public:
		void	parse( std::vector<token>& inTokens, script &outScript );
		
	protected:
		void	skip_empty_lines();
		void	skip_rest_of_line();
		
		void	parse_handler( identifier_type inType, handler_definition &outHandler );
		void	parse_parameter_declaration( std::vector<parameter_declaration> &outParameters );
		void	parse_one_line();
		
		void	throw_parse_error( const char *msg );
		
		bool				expect_token_type( token_type inType );
		bool				expect_identifier( identifier_type inType, skip_type inSkip = skip_type::skip );
		const std::string	*expect_unquoted_string( const std::string inStr = std::string() );
		const std::string	*expect_string();

		std::vector<token> 					*mTokens;
		std::vector<token>::const_iterator	mCurrToken;
	};
	
}

#endif /* parser_hpp */
