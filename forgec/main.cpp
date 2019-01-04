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
#include "parser.hpp"


using namespace forge;


int main(int argc, const char * argv[]) {
	try {
		std::ifstream fileStream(argv[1]);
		//	std::string str((std::istreambuf_iterator<char>(fileStream)),
		//					std::istreambuf_iterator<char>());
		//
		//	std::cout << str << std::endl;
		
		tokenizer	t;
		t.add_tokens_from(fileStream, argv[1]);
//		t.print( std::cout );
		
		parser		p;
		script		s;
		try {
			p.parse(t.mTokens, s);
		} catch( ... ) {
			p.print( std::cout );
			s.print( std::cout );

			throw;
		}
		
//		p.print( std::cout );
//		s.print( std::cout );
		
		codegen	codeGen;
		s.generate_code(codeGen);
		
		std::string path(argv[1]);
		size_t suffixPos = path.find_last_of(".");
		if (suffixPos != std::string::npos) {
			path = path.substr(0, suffixPos);
		}
		path.append(".cpp");
		std::ofstream	outputFile(path);
		codeGen.print( outputFile );
	} catch( forge::parse_error& err ) {
		std::cerr << err.what() << std::endl;
	} catch( std::exception& err ) {
		std::cerr << "error: " << err.what() << std::endl;
	} catch( ... ) {
		std::cerr << "error: unknown exception caught." << std::endl;
	}
	
	return 0;
}
