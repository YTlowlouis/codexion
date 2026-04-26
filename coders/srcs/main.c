#include "codexion.h"


static int check_arguments(int argc, char **argv, int nbr_arg);
static int init_mutexes(t_sim *sim);
static int init_coders(t_sim *sim);
static t_sim *init_sim(char **argv);
int     init_dongles(t_sim *sim);

int main(int argc, char **argv)
{
    t_sim *sim;

    if (!check_arguments(argc, argv, 9))
    {
        fprintf(stderr, "Error: invalid arguments\n");
        return (1); // ← erreur
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
    return (0); // ← succès
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
		pthread_mutex_init(&sim->coders[i].compile_mutex, NULL);
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
		free(sim->coders);
		free(sim);
		return (NULL);
	}
	sim->threads = malloc(sizeof(pthread_t) * sim->nb_coders);
	if (!sim->threads)
	{
		free(sim->dongles);
		free(sim->coders);
		free(sim);
		return NULL;
	}
	sim->start_time = get_time_ms();
	return (sim);
}

int     init_dongles(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        sim->dongles[i].id = i;
        sim->dongles[i].is_available = 1;
        sim->dongles[i].available_at_ms = 0;
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        sim->dongles[i].queue = pqueue_init(sim->nb_coders, sim->scheduler); // ← manque
        if (!sim->dongles[i].queue)
            return (1); // ← gérer l'erreur
        i++;
    }
    return (0);
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

t_pqueue	*pqueue_init(int capacity, char *scheduler)
{
	t_pqueue	*pq;

	pq = malloc(sizeof(t_pqueue));
	if (!pq)
		return (NULL);
	pq->data = calloc(capacity, sizeof(t_request));
	if (!pq->data)
	{
		free(pq);
		return (NULL);
	}
	pq->size = 0;
	pq->capacity = capacity;
	pq->scheduler = scheduler;
	return (pq);
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

static int check_arguments(int argc, char **argv, int nbr_arg)
{
    int x;
    int y;

    x = 1;
    y = 0;
    if (argc != nbr_arg)
        return (0);
    if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
        return (0);
    while (x < nbr_arg - 1) // -1 pour ne pas vérifier scheduler
    {
        while (argv[x][y] && argv[x][y] != '\0')
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
