#include "codexion.h"

int	alloc_sim_arrays(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->dongles)
	{
		free(sim->coders);
		return (1);
	}
	sim->threads = malloc(sizeof(pthread_t) * sim->nb_coders);
	if (!sim->threads)
	{
		free(sim->dongles);
		free(sim->coders);
		return (1);
	}
	return (0);
}

void	init_mutexes(t_sim *sim)
{
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
}

int	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].burned_out = 0;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->nb_coders];
		sim->coders[i].sim = sim;
		pthread_mutex_init(&sim->coders[i].compile_mutex, NULL);
		i++;
	}
	return (0);
}

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].is_available = 1;
		sim->dongles[i].available_at_ms = 0;
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].queue = pqueue_init(sim->nb_coders, sim->scheduler);
		if (!sim->dongles[i].queue)
			return (1);
		i++;
	}
	return (0);
}
