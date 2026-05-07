#include "codexion.h"

static void	compile(t_coder *coder);
static void	debug(t_coder *coder);
static void	refactor(t_coder *coder);
static int	coder_cycle(t_coder *coder);

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	coder->last_compile_start = get_time_ms();
	if (coder->sim->nb_coders == 1)
	{
		usleep(coder->sim->time_to_burnout * 1000);
		log_burnout(coder->sim, coder->id);
		return (NULL);
	}
	while (!is_sim_over(coder->sim))
	{
		if (coder_cycle(coder))
			break ;
	}
	return (NULL);
}

static int	coder_cycle(t_coder *coder)
{
	take_two_dongle(coder);
	if (is_sim_over(coder->sim))
	{
		release_dongle(coder->left, coder->sim);
		release_dongle(coder->right, coder->sim);
		return (1);
	}
	compile(coder);
	release_dongle(coder->left, coder->sim);
	release_dongle(coder->right, coder->sim);
	debug(coder);
	refactor(coder);
	return (0);
}

static void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->compile_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->compile_mutex);
	log_state(coder->sim, coder->id, "is compiling");
	usleep(coder->sim->time_to_compile * 1000);
	pthread_mutex_lock(&coder->compile_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->compile_mutex);
}

static void	debug(t_coder *coder)
{
	if (is_sim_over(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is debugging");
	usleep(coder->sim->time_to_debug * 1000);
}

static void	refactor(t_coder *coder)
{
	if (is_sim_over(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is refactoring");
	usleep(coder->sim->time_to_refactor * 1000);
}
