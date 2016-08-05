#ifndef FUNCTION_TRAITS_H_
#define FUNCTION_TRAITS_H_

// Example program
#include <iostream>
#include <string>
#include <tuple>
#include <functional>
#include <typeinfo>
using namespace std;

namespace arphomod {

template<typename... Ts>
class __args_type {

};


template <class T>
class function_traits
	: public function_traits<decltype(&T::operator())>
{
public:
	function_traits() = default;
	~function_traits() = default;
};
// For generic types, directly use the result of the signature of its 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
class function_traits<ReturnType(ClassType::*)(Args...) const>
	// we specialize for pointers to member function
{
public:
	enum { arity = sizeof...(Args) };
	// arity is the number of arguments.

	using result_type = ReturnType;
	using function_type = function<ReturnType(Args...)>;
	using args_type = __args_type<Args...>;

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		// the i-th argument is equivalent to the i-th tuple element of a tuple
		// composed of those arguments.
	};
};

template <typename... Args>
void foo(Args&&... args) {

}
/*
int main() {

	function<int(int)> f = [](int a) {return a + 10; };
	auto ff = [](int b, int c) {return b + 20; };
	cout << typeid(decltype(f)).name() << endl;
	cout << typeid(ff).name() << endl;

	function_traits<decltype(ff)> a;
	cout << typeid(function_traits<decltype(ff)>::function_type).name() << endl;
	cout << typeid(function_traits<decltype(ff)>::result_type).name() << endl;
	cout << typeid(function_traits<decltype(ff)>::args_type).name() << endl;
	return 0;
}*/

}

#endif