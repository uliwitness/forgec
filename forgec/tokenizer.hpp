//
//  tokenizer.hpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <istream>
#include <vector>

namespace forge {
	
	enum token_type {
		whitespace_token,
		identifier_token,
		number_token,
		integer_token,
		operator_token,
		string_token,
		carriage_return_token, // temp token type when we hit a CR so we can detect CRLF as one break only.
		newline_token,
	};
	
	class token {
	public:
		enum token_type	mType = whitespace_token;
		size_t			mStartOffset = 0;
		size_t			mEndOffset = 0;
		std::string		mText;
		std::string		mFileName;
	};
	
	class tokenizer {
	public:
		tokenizer() {}
		
		void	add_tokens_from( std::istream &inStream, std::string inFileName );
		
		void	print( std::ostream &dest );
		
	protected:
		void	end_token( token_type nextType );
		bool	is_operator( char currCh );
		
		std::vector<token>	mTokens;
		token 				mCurrToken;
		
	};
	
}

#endif /* tokenizer_hpp */
