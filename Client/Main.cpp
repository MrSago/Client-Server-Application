#include "./ConsoleClient/ConsoleClient.hpp"


int main(int argc, char** argv, char** envp)
{
	ConsoleClient client;
	errno_t err = client.Start();
	return (err);
}
