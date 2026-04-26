#include "codexion.h"

long get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000))
}

int	is_sim_over(t_sim *sim)
{
	int	result;

	pthread_mutex_lock(&sim->state_mutex);
	result = sim->simulation_over;
	pthread_mutex_unlock(&sim->state_mutex);
	return result;
}
