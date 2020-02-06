#include "client_server.h"

void	remove_client(t_clients **clients, int fd)
{
	t_clients	*tmp;
	t_clients	*prev;

	tmp = *clients;
	prev = NULL;
	while (tmp && tmp->fd != fd)
	{
		prev = tmp;
		tmp = tmp->next;
	}
	if (tmp == NULL)
		return;
	if (prev)
		prev->next = tmp->next;
	else
		(*clients) = (*clients)->next;
	free(tmp);
}

void	generator(t_clients *client, int fd)
{
	uint64_t	tmp;
	t_clients	*tmp1;

	tmp1 = client;
	if (!tmp1)
		print_error("no client");
	if (!(tmp1->arr[0] || tmp1->arr[4] || tmp1->arr[8]))
		print_error("do not set the sequence");
	for (int i = 0; i < 3; i++)
		if (tmp1->arr[i * 4])
		{
			uint64_t z = reverse_bit(tmp1->arr[i * 4 + 3]);
			if (write(client->fd, (const void *)&z, 8) < 1)
			{
				close(client->fd);
				printf("client disconnected, deleting a client in a child process on fd %d\n", client->fd);
				exit(1);
			}
			tmp = tmp1->arr[i * 4 + 3] + tmp1->arr[i * 4 + 2];
			tmp1->arr[i * 4 + 3] = (tmp > tmp1->arr[i * 4 + 1] && tmp > tmp1->arr[i * 4 + 2]) ? tmp : tmp1->arr[i * 4 + 1];
		}
}

t_clients *create_client(int fd)
{
	t_clients *tmp;

	if (!(tmp = malloc(sizeof(t_clients))))
		exit(1);
	tmp->fd = fd;
	for (int i = 0; i < 12; i++)
		tmp->arr[i] = 0;
	tmp->next = NULL;
	return tmp;
}

void	set_sequence(int fd, t_clients **clients, int cmd)
{
	t_clients *tmp;

	tmp = *clients;
	while (tmp && tmp->fd != fd)
		tmp = tmp->next;
	if (tmp == NULL)
	{
		tmp = create_client(fd);
		tmp->next = *clients;
		*clients = tmp;
	}
	if ((!(cmd^1) || !(cmd^2) || !(cmd^3)) && !tmp->arr[(cmd - 1) * 4])
	{
		read(fd, (void *)&(tmp->arr[(cmd - 1) * 4 + 1]), 8);
		read(fd, (void *)&(tmp->arr[(cmd - 1) * 4 + 2]), 8);
		tmp->arr[(cmd - 1) * 4] = 1;
		tmp->arr[(cmd - 1) * 4 + 3] = tmp->arr[(cmd - 1) * 4 + 1];
	}
	else
		print_error("the sequence is not set");
}

void	get_client(t_clients **clients, int fd, t_clients *cl)
{
	t_clients	*tmp;
	t_clients	*prev;

	tmp = *clients;
	prev = NULL;
	while (tmp && tmp->fd != fd)
	{
		prev = tmp;
		tmp = tmp->next;
	}
	if (tmp == NULL)
		print_error("get_client");
	if (prev)
		prev->next = tmp->next;
	else
		(*clients) = (*clients)->next;
	for (int i = 0; i < 12; i++)
		cl->arr[i] = tmp->arr[i];
	cl->fd = tmp->fd;
	cl->next = NULL;
	free(tmp);
}

void	init_server_sockfd(struct sockaddr_in *server_address, socklen_t *server_len)
{
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd < 0)
		print_error("ERROR opening socket");
	int enable = 1;
	if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
		print_error("setsockopt(SO_REUSEADDR) failed");
	server_address->sin_family = AF_INET;
	server_address->sin_addr.s_addr = htonl(INADDR_ANY);
	server_address->sin_port = htons(6500);
	*server_len = sizeof(*server_address);
	bind(server_sockfd, (struct sockaddr *)server_address, *server_len);
}

void	handler_client(int fd,fd_set *readfds, t_clients **clients)
{
	int			nread;
	uint64_t	cmd;

	ioctl(fd, FIONREAD, &nread);
	if (nread == 0) {
		remove_client(clients, fd);
		close(fd);
		FD_CLR(fd, readfds);
		printf("removing client on fd %d\n", fd);
	}
	else {
		if (read(fd, &cmd, 8) < 1)
			print_error("error command");
		cmd = reverse_bit(cmd);
		if (cmd == 4)
		{
			t_clients client;
			get_client(clients, fd, &client);
			if (fork() == 0)
				while (1)
					generator(&client, fd);
			else
			{
				printf("fd close = %d\n", fd);
				close(fd);
				FD_CLR(fd, readfds);
			}
		}
		else
			set_sequence(fd, clients, cmd);
	}
}

void	listener(fd_set *readfds)
{
	int						client_sockfd;
	socklen_t				client_len;
	struct sockaddr_in		client_address;
	int						result;
	fd_set					testfds;
	int						fd;
	t_clients				*clients;

	clients = NULL;
	while(1) {
		printf("server waiting\n");
		testfds = *readfds;
		result = select(FD_SETSIZE, &testfds, (fd_set *)0,(fd_set *)0, (struct timeval *)0);
		if (result < 1) {
			perror("server error");
			exit(1);
		}
		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				if (fd == server_sockfd) {
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
					FD_SET(client_sockfd, readfds);
					printf("adding client on fd %d\n", client_sockfd);
				}
				else
					handler_client(fd, readfds, &clients);
			}
		}
	}
}

int main() {
	socklen_t				server_len;
	struct sockaddr_in		server_address;
	fd_set					readfds;

	pthread_mutex_init(&mutex, NULL);
	signal(SIGINT, sigint);
	init_server_sockfd(&server_address, &server_len);
	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);
	listen(server_sockfd, 5);
	signal(SIGCHLD, SIG_IGN);
	listener(&readfds);
	return (0);
}
