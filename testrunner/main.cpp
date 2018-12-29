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

int on_main() {
	std::cout << "Main!" << std::endl;
	
	return 0;
}

int main(int argc, const char * argv[]) {
	forge::Process::currentProcess().set_args(argc, argv);
	std::cout << forge::Process::currentProcess().name.get_string() << std::endl;
	std::cout << forge::Process::currentProcess().parameters.get_string() << std::endl;

	forge::variant		firstParam;

	firstParam.set_int64(777LL);
	try { std::cout << "Integer: " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_double(1234.5);
	try { std::cout << "Number:  " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_double(1234.00);
	try { std::cout << "Number:  " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_string("Hello you!");
	try { std::cout << "String:  " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_bool(true);
	try { std::cout << "Boolean: " << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_string("true");
	try { std::cout << "Bool-String:" << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_string("false");
	try { std::cout << "Bool-String:" << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_string("123.4");
	try { std::cout << "Double-String:" << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_string("777");
	try { std::cout << "Int-String:" << firstParam.get_int64() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_double() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_string() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }
	try { std::cout << "         " << firstParam.get_bool() << std::endl; } catch(std::exception& err) { std::cerr << err.what() << std::endl; }

	firstParam.set_value_for_key(forge::static_string("list\n\nitem"), "1");
	forge::static_string	returnedString;
	firstParam.get_value_for_key(returnedString, "1");
	std::cout << returnedString.get_string() << std::endl;
	std::cout << firstParam.get_string() << std::endl;

	return on_main();
}
