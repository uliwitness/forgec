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
	X(newline_token) \
	X(possible_comment_token) \
	X(comment_token)

	
#define X(n) n,
	enum token_type {
		TOKEN_TYPES
	};
#undef X
	
#define IDENTIFIER_TYPES			X(on) \
									X(end) \
									X(of) \
									X(to) \
									X(repeat) \
									X(with) \
									X(for) \
									X(down) \
									X(while) \
									X(until) \
									X(if) \
									X(then) \
									X(else) \
									X(put) \
									X(into) \
									X(before) \
									X(after) \
									X(the) \
									X(s) \
									X2(plus_operator, "+", 1000) \
									X2(minus_operator, "-", 1000) \
									X2(multiply_operator, "*", 2000) \
									X2(divide_operator, "/", 2000) \
									X2(dot_operator,".", 4000) \
									X2(power_operator, "^", 3000) \
									X2(less_than_operator, "<", 900) \
									X2(greater_than_operator, ">", 900) \
									X2(equals_operator, "=", 900) \
									X2(comma_operator, ",", 100) \
									X2(open_parenthesis_operator, "(", 100 ) \
									X2(close_parenthesis_operator, ")", 100 ) \
									X2(ampersand_operator,"&", 600) \
									X2(apostrophe_operator, "'", 4000) \
									X2(at_operator, "@", 100)

	
#define X(n) identifier_ ## n,
#define X2(n,m,p) identifier_ ## n,
	enum identifier_type {
		IDENTIFIER_TYPES
		identifier_INVALID
	};
#undef X2
#undef X

	
	class token {
	public:
		bool	is_identifier( enum identifier_type inIdentifier ) const { return (mType == identifier_token || mType == operator_token) && (mIdentifierType == inIdentifier || inIdentifier == identifier_INVALID); }

		int		operator_precedence() const;

		enum token_type			mType = whitespace_token;
		enum identifier_type	mIdentifierType = identifier_INVALID;
		size_t					mStartOffset = 0;
		size_t					mEndOffset = 0;
		std::string				mText;
		std::string				mFileName;
	};
	
	class tokenizer {
	public:
		tokenizer() {}
		
		void	add_tokens_from( std::istream &inStream, std::string inFileName );

		void	print( std::ostream &dest );
		
		std::vector<token>	mTokens;

	protected:
		void	end_token( token_type nextType );
		bool	is_operator( char currCh );
		
		token 				mCurrToken;
	};
	
}

#endif /* tokenizer_hpp */
