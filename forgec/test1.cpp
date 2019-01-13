#include "forgelib.hpp"
#include <vector>

forge::variant cmd_startUp(std::vector<forge::variant> params);
forge::variant fun_doTest(std::vector<forge::variant> params);

int main(int argc, const char * argv[]) {
	forge::Process::currentProcess().set_args(argc, argv);
	cmd_startUp(forge::Process::currentProcess().parameters);
	return 0;
}

forge::variant cmd_startUp(std::vector<forge::variant> params) {
	forge::variant var_down;
	var_down.set_string("down");
	forge::variant var_myVar;
	var_myVar.set_string("myVar");
	forge::variant var_x;
	var_x.set_string("x");

	cmd_put((std::vector<forge::variant>){ forge::subtract(forge::add(forge::static_int64(4), forge::multiply(forge::static_int64(5), forge::static_int64(7))), forge::static_int64(16))});
	cmd_put((std::vector<forge::variant>){ forge::static_int64(1), forge::static_string("hello"), forge::static_double(123.450000)});
	forge::assign_to((forge::Process::currentProcess()).name , var_myVar);
	cmd_put((std::vector<forge::variant>){ (forge::subtract(forge::concatenate_space(fun_myFunc((std::vector<forge::variant>){ forge::static_string("This could be really useful!"), forge::static_int64(77)}), forge::add(forge::static_int64(4), forge::multiply(forge::static_int64(5), forge::static_int64(7)))), forge::power(forge::static_int64(16), (forge::static_double(2.500000)).1.100000 ))).1 });
	for( 1.copy_to(var_x); forge::is_less_than(var_x,50); var_x )var_x, forge::static_int64(1), forge::static_int64(50), forge::static_int64(1)) {
	forge::append_to(forge::concatenate_space(forge::concatenate(forge::concatenate(forge::static_string("Hello"), forge::multiply(forge::static_int64(4), forge::static_int64(5))), forge::static_string("World")), var_x), var_myVar);
	};
	while(forge::not_equal(Button(), var_down)) {
	cmd_put((std::vector<forge::variant>){ forge::static_string("Click!")});
	};
	doSomething(forge::static_int64(50), forge::static_string("Greenroot"));
	return forge::variant();
}

forge::variant fun_doTest(std::vector<forge::variant> params) {
	forge::variant var_foo(params[0]);
	forge::variant var_bar(params[1]);
	forge::variant var_baz(params[2]);

	return forge::variant();
}


