#include <conio.h>

#include "UDP_Server.h"

int main(int argc, char** argv)
{
	UDPServer server;
	int ch;

	while (1)
	{
		if (_kbhit())
		{
			ch = _getch();

			if (ch == 27) break;
		}
		server.Update();
	}

	return 0;
}