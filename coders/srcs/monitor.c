#include "codexion.h"

static void	wake_all_dongles(t_sim *sim);

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_sim_over(sim))
	{
		if (check_burnout(sim))
			break ;
		if (check_all_compiled(sim))
			break ;
		usleep(500);
	}
	wake_all_dongles(sim);
	return (NULL);
}

int	check_burnout(t_sim *sim)
{
	int		i;
	long	now;
	long	deadline;

	now = get_time_ms();
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->coders[i].compile_mutex);
		deadline = sim->coders[i].last_compile_start
			+ sim->time_to_burnout;
		pthread_mutex_unlock(&sim->coders[i].compile_mutex);
		if (now >= deadline)
		{
			log_burnout(sim, sim->coders[i].id);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_all_compiled(t_sim *sim)
{
	int	i;
	int	count;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->coders[i].compile_mutex);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].compile_mutex);
//		printf("DEBUG coder %d : count=%d required=%d\n",
//			i + 1, count, sim->nb_compiles_required); // ← temporaire
		if (count < sim->nb_compiles_required)
			return (0);
		i++;
	}
	pthread_mutex_lock(&sim->state_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->state_mutex);
	return (1);
}

static void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}
