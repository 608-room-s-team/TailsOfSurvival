#include <iostream>
#include <exception>
#include "server.h"

int main()
{
	using namespace server::tcp;

	try
	{
		TCPServer x;
		x.run();
	}
	catch (const std::runtime_error& err)
	{
		std::cout << err.what();
	}

	return 0;
}