//
//  main.cpp
//  forgec
//
//  Created by Uli Kusterer on 11.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include <iostream>
#include <fstream>

int main(int argc, const char * argv[]) {
	std::ifstream fileStream(argv[1]);
	std::string str((std::istreambuf_iterator<char>(fileStream)),
					std::istreambuf_iterator<char>());

	std::cout << str << std::endl;
	
	return 0;
}
