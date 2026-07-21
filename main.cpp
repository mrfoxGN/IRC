#include <cstdio>
#include <cstdlib>
#include <csignal>

#include "src/server/Server.hpp"

int running = 1;

void handle_signal(int){
	running = 0;
}

int main(int ac, char *av[])
{
	if (ac != 3)
	{
		printf("Usage: %s <port> <password>\n", av[0]);
		return 1;
	}
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
	const int port = atoi(av[1]);
	if (port < 1)
	{
		std::cout << "port is not valide" << std::endl;
		return 1;
	}
	const string password = av[2];

	try
	{
		Server server(port, password);
		server.run(running);
	}
	catch (const exception &e)
	{
		printf("Error: %s\n", e.what());
		return 1;
	}
	return 0;
}
