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
	
#define TOKEN_TYPES			X(whitespace_token) \
	X(identifier_token) \
	X(number_token) \
	X(integer_token) \
	X(operator_token) \
	X(string_token) \
	X(carriage_return_token) \
	X(newline_token)

	
#define X(n) n,
	enum token_type {
		TOKEN_TYPES
	};
#undef X
	
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
