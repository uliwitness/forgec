//
//  main.cpp
//  testrunner
//
//  Created by Uli Kusterer on 11.12.18.
//  Copyright © 2018 Uli Kusterer. All rights reserved.
//

#include <iostream>
#include <string>
#include "forgelib.hpp"

int main(int argc, const char * argv[]) {
	forge::variant		firstParam;
	
	firstParam.set(777LL);
	try { std::cout << "Integer: " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	
	firstParam.set(1234.5);
	try { std::cout << "Number:  " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	
	firstParam.set(1234.00);
	try { std::cout << "Number:  " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	
	firstParam.set("Hello you!");
	try { std::cout << "String:  " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_value_for_key(forge::static_string("list item"), "1");
	forge::static_string	returnedString;
	firstParam.get_value_for_key(returnedString, "1");
	std::cout << returnedString.get_string() << std::endl;
	
	return 0;
}
