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


#define X(n) #n ,
#define X2(n,m) m,
static const char* sIdentifierTypeStrings[] = {
	IDENTIFIER_TYPES
	nullptr
};
#undef X2
#undef X



void	forge::tokenizer::end_token( token_type nextType )
{
	if (mCurrToken.mType == carriage_return_token) {
		mCurrToken.mType = newline_token;
	}
	
	if (mCurrToken.mType == string_token) {
		mTokens.push_back(mCurrToken);
	} else if (mCurrToken.mText.length() > 0) {
		if (mCurrToken.mType == identifier_token || mCurrToken.mType == operator_token) {
			for (int x = 0; sIdentifierTypeStrings[x] != nullptr; ++x) {
				if (strcmp(sIdentifierTypeStrings[x], mCurrToken.mText.c_str()) == 0) {
					mCurrToken.mIdentifierType = (identifier_type) x;
					break;
				}
			}
		}
		
		mTokens.push_back(mCurrToken);
	}
	
	mCurrToken.mStartOffset = mCurrToken.mEndOffset;
	mCurrToken.mText.erase();
	mCurrToken.mType = nextType;
	mCurrToken.mIdentifierType = identifier_INVALID;
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
					++mCurrToken.mEndOffset;
					end_token(whitespace_token);
					--mCurrToken.mEndOffset;
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
					end_token(integer_token);
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
					++mCurrToken.mEndOffset;
					end_token(whitespace_token);
					--mCurrToken.mEndOffset;
				}
				break;
				
			default:
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else {
					if (is_operator(currCh)) {
						end_token(operator_token);
						mCurrToken.mText.append(1, currCh);
						++mCurrToken.mEndOffset;
						end_token(whitespace_token);
						--mCurrToken.mEndOffset;
					} else {
						if (mCurrToken.mType != identifier_token) {
							end_token(identifier_token);
						}
						mCurrToken.mText.append(1, currCh);
					}
				}
				break;
		}
		
		++mCurrToken.mEndOffset;
	}
	
	end_token(whitespace_token);
}


void	forge::tokenizer::print( std::ostream &dest )
{
	for (auto currToken : mTokens) {
		dest << "[" << sTokenTypeStrings[currToken.mType] << ": " << currToken.mText;
		if (currToken.mIdentifierType != identifier_INVALID) {
			dest << " {" << sIdentifierTypeStrings[currToken.mIdentifierType] << "}";
		}
		dest << " " << currToken.mStartOffset << "..." << currToken.mEndOffset << "] ";
	}
	dest << std::endl;
}
