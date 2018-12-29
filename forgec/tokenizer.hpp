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
									X(each) \
									X(times) \
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
									X(import) \
									X(as) \
									X(is) \
									X(not) \
									X(from) \
									X(command) \
									X(function) \
									X3(plus_operator, "+", 1000, "forge::add") \
									X3(minus_operator, "-", 1000, "forge::subtract") \
									X3(multiply_operator, "*", 2000, "forge::multiply") \
									X3(divide_operator, "/", 2000, "forge::divide") \
									X3(dot_operator,".", 4000, "forge::dot_operator") \
									X3(power_operator, "^", 3000, "forge::power") \
									X3(less_than_operator, "<", 900, "forge::less_than") \
									X3(greater_than_operator, ">", 900, "forge::greater_than") \
									X3(less_equal_operator, "<=", 900, "forge::less_than_equal") \
									X3(greater_equal_operator, ">=", 900, "forge::greater_than_equal") \
									X3(not_equal_operator, "<>", 900, "forge::not_equal") \
									X3(equals_operator, "=", 900, "forge::equal") \
									X3(comma_operator, ",", 100, "forge::comma") \
									X3(open_parenthesis_operator, "(", 100, "forge::open_parenthesis") \
									X3(close_parenthesis_operator, ")", 100, "forge::close_parenthesis") \
									X3(ampersand_operator,"&", 600, "forge::concatenate") \
									X3(double_ampersand_operator,"&&", 600, "forge::concatenate_space") \
									X3(apostrophe_operator, "'", 4000, "forge::apostrophe") \
									X3(at_operator, "@", 100, "forge::reference")

	
#define X(n) identifier_ ## n,
#define X3(n,m,p,f) identifier_ ## n,
	enum identifier_type {
		IDENTIFIER_TYPES
		identifier_INVALID
	};
#undef X3
#undef X

	
	class token {
	public:
		bool	is_identifier( enum identifier_type inIdentifier ) const { return (mType == identifier_token || mType == operator_token) && (mIdentifierType == inIdentifier || inIdentifier == identifier_INVALID); }

		int					operator_precedence() const;
		static const char	*operator_function( enum identifier_type identifierType );

		enum token_type			mType = whitespace_token;
		enum identifier_type	mIdentifierType = identifier_INVALID;
		size_t					mStartOffset = 0;
		size_t					mEndOffset = 0;
		size_t					mLineNumber = 0;
		size_t					mLineStartOffset = 0;
		std::string				mText;
		std::string				mFileName;
	};
	
	class tokenizer {
	public:
		tokenizer() {}
		
		void	add_tokens_from( std::istream &inStream, std::string inFileName );

		void	print( std::ostream &dest );
		
		std::vector<token>	mTokens;

		static const char*	string_from_identifier_type( identifier_type inType );

	protected:
		void	end_token( token_type nextType );
		bool	is_operator( char currCh );
		
		token 				mCurrToken;
	};
	
}

#endif /* tokenizer_hpp */
