#include "codexion.h"

static int		check_arguments(int argc, char **argv, int nbr_arg);
static t_sim	*init_sim(char **argv);

int	main(int argc, char **argv)
{
	t_sim	*sim;

	if (!check_arguments(argc, argv, 9))
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	sim = init_sim(argv);
	if (!sim)
		return (1);
	init_mutexes(sim);
	init_coders(sim);
	init_dongles(sim);
	launch_threads(sim);
	join_threads(sim);
	cleanup(sim);
	return (0);
}

static t_sim	*init_sim(char **argv)
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
	if (alloc_sim_arrays(sim))
	{
		free(sim);
		return (NULL);
	}
	sim->start_time = get_time_ms();
	return (sim);
}

int	launch_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		pthread_create(&sim->threads[i], NULL,
			coder_routine, &sim->coders[i]);
		i++;
	}
	pthread_create(&sim->monitor, NULL, monitor_routine, sim);
	return (0);
}

int	join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_join(sim->threads[i], NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
	return (0);
}

static int	check_arguments(int argc, char **argv, int nbr_arg)
{
	int	x;
	int	y;

	x = 1;
	y = 0;
	if (argc != nbr_arg)
		return (0);
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (0);
	while (x < nbr_arg - 1)
	{
		while (argv[x][y])
		{
			if (argv[x][y] < '0' || argv[x][y] > '9')
				return (0);
			y++;
		}
		y = 0;
		x++;
	}
	return (1);
}
