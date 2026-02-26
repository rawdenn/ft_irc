#include "../includes/ft_irc.h"

int	g_running = 1;

static void	ctrl_c(int sig)
{
	(void)sig;
	g_running = 0;
}

void	main_signal(void)
{
	signal(SIGINT, ctrl_c);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}
