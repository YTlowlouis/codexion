#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	coder->last_compile_start = get_time_ms();
	if (coder->sim->nb_coders == 1)
	{
		usleep(coder->sim->time_to_burnout);
		log_burnout(coder->sim, coder->id);
		return (NULL);
	}

	while (!is_sim_over(coder->sim))
	{
		take_two_dongle(coder);
		if (is_sim_over(coder->sim))
		{
			release_dongle(coder->left, coder->sim);
			release_dongle(coder->right, coder->sim);
			break ;
		}
		compile(coder);
		release_dongle(coder->left, coder->sim);
		release_dongle(coder->right, coder->sim);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}

static void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->compile_mutex)
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->compile_mutex)
	log_state(coder->sim, coder->id, "is compiling");
	usleep(coder->sim->time_to_compile * 1000);
	coder->compile_count++;
}

static void	debug(t_coder *coder)
{
	if (is_sim_over(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is debugging");
	usleep(coder->sim->time_to_debug  * 1000);
}

static void	refactor(t_coder *coder)
{
	if (is_sim_over(coder->sim))
		return ;
	log_state(coder->sim, coder->id, "is refactoring");
	usleep(coder->sim->time_to_refactor * 1000);
}
