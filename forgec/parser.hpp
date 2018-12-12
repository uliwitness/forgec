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
	
	class parser {
	public:
		void	parse( const std::vector<token>& inTokens );
	};
	
}

#endif /* parser_hpp */
