#include "client_server.h"

void	init_sockfd(int *sockfd)
{
	int enable;

	enable = 1;
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd < 0)
		print_error("ERROR opening socket");
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
		print_error("setsockopt(SO_REUSEADDR) failed");
}

void	init_address(struct sockaddr_in *address)
{
	struct hostent *hostinfo;

	hostinfo = gethostbyname("localhost");
	if (!hostinfo)
		print_error("no host: localhost");
	address->sin_family = AF_INET;
	address->sin_port = htons(6500);
	address->sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;
}

int		parser(char *str, uint64_t arg[3], char *cmd)
{
	char	*tmp;
	char	*set;
	int		i;

	i = 0;
	tmp = strtok(str, "\r\t\v\f ");
	while (tmp)
	{
		set = (i == 0 && (!strcmp(tmp, "seq1") || !strcmp(tmp, "seq2") || !strcmp(tmp, "seq3"))) ? tmp + 3 : tmp;
		if (!(arg[i] = strtol(set, NULL, 0)) || errno == ERANGE || arg[i] < 0)
			return (0);
		if (i == 0)
		{
			if ((*cmd >> (arg[i] - 1)) & 1)
			{
				printf("the command exists\n");
				return (0);
			}
		}
		tmp = strtok(NULL, "\r\t ");
		i++;
	}
	if (i == 3)
	{
		((*cmd ^= (1 << (arg[0] -1))));
		return (1);
	}
	return (0);
}

void	rider(int sockfd)
{
	int			ret;
	char		str[128];
	uint64_t	arg[3];
	char		cmd;

	cmd = 0;
	printf("enter a command or \033[%dm%s\033[%dm%s\033[%dm%s\033[%dm\n", 31, "q ", 39, "to ", 31, "EXIT", 39);
	while ((ret = read(0, str, 127)) > 0)
	{
		str[ret] = 0;
		if (!strcmp(str, "q\n"))
			close_socket(sockfd);
		if (!strcmp(str, "export seq\n"))
		{
			if (cmd)
				return;
			printf("Zenter a command or \033[%dm%s\033[%dm%s\033[%dm%s\033[%dm\n", 31, "q ", 39, "to ", 31, "EXIT", 39);
		}
		else
		{
			if (parser(str, arg, &cmd))
				for (int i = 0; i < 3; i++)
				{
					uint64_t z = reverse_bit(arg[i]);
					write(sockfd, &z,8);
				}
			else
				printf("mistake\n");
		}
	}
}

int		main(int argc, char *argv[])
{
	int					sockfd;
	int					len;
	int					result;
	struct sockaddr_in	address;
	uint64_t			i;
	char				buff[2];
	struct pollfd		fds;

	fds.fd = STDIN_FILENO;
	fds.events = POLLIN;
	init_sockfd(&sockfd);
	init_address(&address);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *)&address, len);
	if (result == -1)
		print_error("connect error");
	rider(sockfd);
	i = 4;
	write(sockfd, &i, 8);
	set_keypress();
	printf("press \033[%dm%s\033[%dm%s\033[%dm%s\033[%dm\n", 31, "q ", 39, "to ", 31, "EXIT", 39);
	while (1)
	{
		client_poll(&fds, buff, sockfd);
		result = read(sockfd, &i, 8);
		if (result)
		{
			i = reverse_bit(i);
			usleep(25000);
			printf("\033[1A\033[2Knum: %lu\n", i);
			printf("press \033[%dm%s\033[%dm%s\033[%dm%s\033[%dm\n", 31, "q ", 39, "to ", 31, "EXIT", 39);
		}
	}
	return (0);
}
