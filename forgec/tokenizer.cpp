//
//  tokenizer.cpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include "tokenizer.hpp"


#define X(n) #n ,
static const char* sTokenTypeStrings[] = {
	TOKEN_TYPES
	nullptr
};
#undef X


void	forge::tokenizer::end_token( token_type nextType )
{
	if (mCurrToken.mType == carriage_return_token) {
		mCurrToken.mType = newline_token;
	}
	
	if (mCurrToken.mType == string_token) {
		mTokens.push_back(mCurrToken);
	} else if (mCurrToken.mText.length() > 0) {
		mTokens.push_back(mCurrToken);
	}
	
	mCurrToken.mStartOffset = mCurrToken.mEndOffset;
	mCurrToken.mText.erase();
	mCurrToken.mType = nextType;
}


bool	forge::tokenizer::is_operator( char currCh )
{
	switch( currCh ) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '<':
		case '>':
		case '=':
		case ',':
		case '.':
		case '?':
		case ':':
		case ';':
		case '\'':
		case '|':
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
		case '&':
		case '^':
		case '%':
		case '$':
		case '#':
		case '@':
		case '!':
		case '~':
			return true;
		default:
			return false;
	}
}


void	forge::tokenizer::add_tokens_from( std::istream &inStream, std::string inFileName )
{
	mCurrToken.mFileName = inFileName;
	mCurrToken.mStartOffset = mCurrToken.mEndOffset = 0;
	
	while (true) {
		int currCh = inStream.get();
		if (inStream.eof()) break;
		
		++mCurrToken.mEndOffset;

		switch (currCh) {
			case ' ':
			case '\t':
				if (mCurrToken.mType == whitespace_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
				} else if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else {
					end_token(whitespace_token);
				}
				break;
				
			case '\r':
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else {
					end_token(carriage_return_token);
					mCurrToken.mText.append(1, currCh);
				}
				break;

			case '\n':
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else {
					if (mCurrToken.mType != carriage_return_token) {
						end_token(newline_token);
					}
					mCurrToken.mText.append(1, currCh);
					end_token(whitespace_token);
				}
				break;
				
			case '\"':
				if (mCurrToken.mType == string_token) {
					end_token(whitespace_token);
				} else if (mCurrToken.mType == whitespace_token) {
					end_token(string_token);
				}
				break;
				
			case '0'...'9':
				if (mCurrToken.mType == whitespace_token) {
					mCurrToken.mType = integer_token;
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == integer_token) {
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == number_token) {
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == identifier_token) {
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				}
				break;
			
			case '.':
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == integer_token) {
					mCurrToken.mType = number_token;
					mCurrToken.mText.append(1, currCh);
				} else {
					end_token(operator_token);
					mCurrToken.mText.append(1, currCh);
					end_token(whitespace_token);
				}
				break;
				
			default:
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else {
					if (is_operator(currCh)) {
						end_token(operator_token);
						mCurrToken.mText.append(1, currCh);
						end_token(whitespace_token);
					} else {
						if (mCurrToken.mType != identifier_token) {
							end_token(identifier_token);
						}
						mCurrToken.mText.append(1, currCh);
					}
				}
				break;
		}
	}
	
	end_token(whitespace_token);
}


void	forge::tokenizer::print( std::ostream &dest )
{
	for (auto currToken : mTokens) {
		dest << "[" << sTokenTypeStrings[currToken.mType] << ": " << currToken.mText << "] ";
	}
	dest << std::endl;
}
