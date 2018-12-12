//
//  main.cpp
//  forgec
//
//  Created by Uli Kusterer on 11.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "tokenizer.hpp"


using namespace forge;


int main(int argc, const char * argv[]) {
	std::ifstream fileStream(argv[1]);
//	std::string str((std::istreambuf_iterator<char>(fileStream)),
//					std::istreambuf_iterator<char>());
//
//	std::cout << str << std::endl;
	
	tokenizer	t;
	t.add_tokens_from(fileStream, argv[1]);
	t.print( std::cout );
	
	return 0;
}
