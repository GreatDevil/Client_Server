#include "client_server.h"

void	print_error(char *str)
{
	printf("%s\n", str);
	exit(0);
}

void	set_keypress(void)
{
	struct termios new_settings;

	tcgetattr(0, &stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= (~ICANON);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &new_settings);
}

void	reset_keypress(void)
{
	tcsetattr(0, TCSANOW, &stored_settings);
}

void	client_poll(struct pollfd *fds, char buff[2], int sockfd)
{
	if (poll(fds, 1, 0) > 0)
	{
		if (fds->revents & POLLIN)
			fds->revents = 0;
		read(0, buff, 1);
		buff[1] = 0;
		if (!strcmp(buff, "q"))
		{
			printf("\nCLOSE\n");
			reset_keypress();
			close(sockfd);
			exit(1);
		}
	}
}

void	close_socket(int socket)
{
	if (socket > 0)
		close(socket);
	exit(0);
}

void	sigint()
{
	if(server_sockfd != 0)
		close(server_sockfd);
	exit(0);
}

uint64_t reverse_bit(uint64_t in) {
	const int i = 1;
	char *chr;
	uint64_t out;

	if ((*(char*)&i) == 0)
	{
		chr = (char*)&out;
		chr[0] = (in >> 56) & 0xff;
		chr[1] = (in >> 48) & 0xff;
		chr[2] = (in >> 40) & 0xff;
		chr[3] = (in >> 32) & 0xff;
		chr[4] = (in >> 24) & 0xff;
		chr[5] = (in >> 16) & 0xff;
		chr[6] = (in >> 8) & 0xff;
		chr[7] = (in >> 0) & 0xff;
		return out;
	}
	return in;
}