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
#define X3(n,m,p,f) m,
static const char* sIdentifierTypeStrings[] = {
	IDENTIFIER_TYPES
	nullptr
};
#undef X3
#undef X

#define X(n) INT_MAX,
#define X3(n,m,p,f) p,
static int sOperatorPrecedences[] = {
	IDENTIFIER_TYPES
	0
};
#undef X3
#undef X

#define X(n) "",
#define X3(n,m,p,f) f,
static const char *sOperatorFunctions[] = {
	IDENTIFIER_TYPES
	nullptr
};
#undef X3
#undef X


// Some identifiers are actually operators, so change their type to that:
static forge::identifier_type sOperatorIdentifiers[] = {
	forge::identifier_is,
	forge::identifier_not,
	forge::identifier_INVALID
};


const char*	forge::tokenizer::string_from_identifier_type( identifier_type inType )
{
	return sIdentifierTypeStrings[inType];
}


void	forge::tokenizer::end_token( token_type nextType )
{
	if (mCurrToken.mType == carriage_return_token) {
		mCurrToken.mType = newline_token;
	} else if (mCurrToken.mType == possible_comment_token) {
		mCurrToken.mType = operator_token;
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
			
			for (size_t x = 0; sOperatorIdentifiers[x] != identifier_INVALID; ++x) {
				if (sOperatorIdentifiers[x] == mCurrToken.mIdentifierType) {
					mCurrToken.mType = operator_token;
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
	mCurrToken.mLineNumber = 1;
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
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
				} else {
					end_token(whitespace_token);
				}
				break;
				
			case '\r':
				if (mCurrToken.mType == string_token) {
					mCurrToken.mLineStartOffset = mCurrToken.mEndOffset + 1;
					++mCurrToken.mLineNumber;
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mLineStartOffset = mCurrToken.mEndOffset + 1;
					++mCurrToken.mLineNumber;
					mCurrToken.mType = carriage_return_token;
					mCurrToken.mText.assign(1, currCh);
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
					mCurrToken.mIdentifierType = identifier_INVALID;
				} else {
					mCurrToken.mLineStartOffset = mCurrToken.mEndOffset + 1;
					end_token(carriage_return_token);
					++mCurrToken.mLineNumber;
					mCurrToken.mText.append(1, currCh);
				}
				break;

			case '\n':
				if (mCurrToken.mType == string_token) {
					mCurrToken.mLineStartOffset = mCurrToken.mEndOffset + 1;
					if (mCurrToken.mText.length() < 1 || mCurrToken.mText[mCurrToken.mText.length() -1] != '\r') {
						++mCurrToken.mLineNumber;
					}
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mLineStartOffset = mCurrToken.mEndOffset + 1;
					++mCurrToken.mLineNumber;
					mCurrToken.mType = newline_token;
					mCurrToken.mText.assign(1, currCh);
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
					mCurrToken.mIdentifierType = identifier_INVALID;
				} else {
					mCurrToken.mLineStartOffset = mCurrToken.mEndOffset + 1;
					if (mCurrToken.mType != carriage_return_token) {
						end_token(newline_token);
						++mCurrToken.mLineNumber;
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
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
				} else if (mCurrToken.mType != string_token) {
					end_token(string_token);
				}
				break;
				
			case '0'...'9':
				if (mCurrToken.mType == whitespace_token || mCurrToken.mType == possible_comment_token) {
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
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
				}
				break;
			
			case '.':
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == integer_token) {
					mCurrToken.mType = number_token;
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
				} else {
					end_token(operator_token);
					mCurrToken.mText.append(1, currCh);
					++mCurrToken.mEndOffset;
					end_token(whitespace_token);
					--mCurrToken.mEndOffset;
				}
				break;
				
			case '-':
				if (mCurrToken.mType == possible_comment_token) {
					mCurrToken.mType = comment_token;
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
				} else if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else {
					end_token(possible_comment_token);
					mCurrToken.mText.append(1, '-');
				}
				break;
				
			case '#':
				if (mCurrToken.mType != string_token) {
					end_token(comment_token);
				} else {
					mCurrToken.mText.append(1, currCh);
				}
				break;
				
			default:
				if (mCurrToken.mType == string_token) {
					mCurrToken.mText.append(1, currCh);
				} else if (mCurrToken.mType == comment_token) {
					mCurrToken.mStartOffset = mCurrToken.mEndOffset;
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


int		forge::token::operator_precedence() const
{
	return sOperatorPrecedences[mIdentifierType];
}


const char	*forge::token::operator_function( enum identifier_type identifierType )
{
	return sOperatorFunctions[identifierType];
}
