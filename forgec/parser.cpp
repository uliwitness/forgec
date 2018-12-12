//
//  parser.cpp
//  forgec
//
//  Created by Uli Kusterer on 12.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include "parser.hpp"


void	forge::parser::parse( const std::vector<token>& inTokens )
{
	std::vector<token>::const_iterator	currToken = inTokens.begin();
	
	while (currToken != inTokens.end()) {
		while (currToken != inTokens.end() && currToken->mType == newline_token) {
			++currToken;
		}
		
		if (currToken->is_identifier(identifier_on)) {
			
		}
	}
}
