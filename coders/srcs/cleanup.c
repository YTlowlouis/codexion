#include "codexion.h"

void	cleanup(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->coders[i].compile_mutex);
		if (sim->dongles[i].queue)
		{
			free(sim->dongles[i].queue->data);
			free(sim->dongles[i].queue);
		}
		i++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	free(sim->coders);
	free(sim->dongles);
	free(sim->threads);
	free(sim);
}
