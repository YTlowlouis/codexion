#include "codexion.h"


static int check_arguments(int argc, char **argv, int nbr_arg);
static int init_mutexes(t_sim *sim);
static int init_coders(t_sim *sim);
static t_sim *init_sim(char **argv);

int main(int argc, char **argv)
{
	t_sim	*sim;

	if (check_arguments(argc, argv, 9))
		printf("good");
	else
	{
		printf("bad");
		return 0;
	}
	sim = init_sim(argv);
	if (!sim)
		return 0;
	init_mutexes(sim);
	init_coders(sim);
	return 1;
}

static int init_mutexes(t_sim *sim)
{
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
	return(0);
}

static int init_coders(t_sim *sim)
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
		i++;
	}
	return (0);
}

static t_sim *init_sim(char **argv)
{
	t_sim	*sim;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	sim->nb_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->nb_compiles_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	sim->scheduler = argv[8];
	sim->simulation_over = 0;
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
	{
		free(sim);
		return(NULL);
	}
	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->dongles)
	{
		free(sim);
		return (NULL);
	}
	sim->threads = malloc(sizeof(pthread_t) * sim->nb_coders);
	if (!sim->threads)
	{
		free(sim);
		return NULL;
	}
	return (sim);
}

static int check_arguments(int argc, char **argv, int nbr_arg)
{
	int x;
	int	y;

	x = 1;
	y = 0;
	if (argc != nbr_arg)
		return (0);
	while (x < nbr_arg)
	{
		while (argv[x][y] && argv[x][y] != '\0')
		{
			if (argv[x][y] < '0' || argv[x][y] > '9')
				return 0;
			y++;
		}
		y = 0;
		x++;
	}
	return (1);
}
