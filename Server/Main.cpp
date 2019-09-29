#include "System/SystemCore/SystemCore.hpp"


int main(int argc, char** argv, char** envp)
{
	SystemCore sys;
	errno_t err = sys.Start(1337);
	return (err);
}
