#include "codexion.h"

void	log_state(t_sim *sim, int id, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	timestamp = get_time_ms()
	printf("%ld %d %s\n", timestamp, id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

void log_burnout(t_sim *sim, int id)
{
	long	timestamp;

	pthread_mutex_lock(sim->log_mutex);
	pthread_mutex_lock(sim->state_mutex);
	if (!sim->simulation_over)
	{
		sim->simulation_over = 1;
		printf("%ld %d burned out\n", get_time_ms() - sim->start_time, id);
	}
	pthread_mutex_unlock(sim->log_mutex);
	pthread_mutex_unlock(sim->state_mutex);
}
