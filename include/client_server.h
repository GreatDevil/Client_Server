#ifndef SOCK_CLIENT_SERVER_H
#define SOCK_CLIENT_SERVER_H
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/poll.h>
#include <termios.h>
#include <pthread.h>

static int					server_sockfd = 0;
static struct termios		stored_settings;
pthread_mutex_t				mutex;

typedef struct				s_clients
{
	int					fd;
	uint64_t			arr[12];
	struct s_clients	*next;
	uint64_t 				cmd;
	int 					flag;
}							t_clients;

void						print_error(char *str);
void						set_keypress(void);
void						reset_keypress(void);
void						client_poll(struct pollfd *fds, char buff[2], int sockfd);
void						close_socket(int socket);
void						sigint();
uint64_t					reverse_bit(uint64_t in);
#endif
