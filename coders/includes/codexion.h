#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

typedef struct s_sim	t_sim;
typedef struct s_pqueue	t_pqueue;

typedef struct s_dongle
{
	int				id;
	int				is_available;
	long			available_at_ms;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_pqueue		*queue;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compile_count;
	long			last_compile_start;
	int				burned_out;
	t_dongle		*left;
	t_dongle		*right;
	t_sim			*sim;
	pthread_mutex_t	compile_mutex;
}	t_coder;

typedef struct s_sim
{
	int				nb_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				nb_compiles_required;
	long			dongle_cooldown;
	char			*scheduler;
	long			start_time;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		*threads;
	pthread_t		monitor;
	int				simulation_over;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	state_mutex;
}	t_sim;

typedef struct s_request
{
	t_coder	*coder;
	long	timestamp;
	long	deadline;
}	t_request;

typedef struct s_pqueue
{
	t_request	*data;
	int			size;
	int			capacity;
	char		*scheduler;
}	t_pqueue;

/* utils.c */
long		get_time_ms(void);
int			is_sim_over(t_sim *sim);

/* log.c */
void		log_state(t_sim *sim, int id, char *msg);
void		log_burnout(t_sim *sim, int id);

/* dongle.c */
void		release_dongle(t_dongle *dongle, t_sim *sim);
void		take_dongle(t_dongle *dongle, t_coder *coder);
void		take_two_dongle(t_coder *coder);

/* coder.c */
void		*coder_routine(void *arg);

/* main.c */
int			launch_threads(t_sim *sim);
int			join_threads(t_sim *sim);

/* init.c */
int			alloc_sim_arrays(t_sim *sim);
void		init_mutexes(t_sim *sim);
int			init_coders(t_sim *sim);
int			init_dongles(t_sim *sim);

/* monitor.c */
void		*monitor_routine(void *arg);
int			check_burnout(t_sim *sim);
int			check_all_compiled(t_sim *sim);

/* scheduler.c */
t_pqueue	*pqueue_init(int capacity, char *scheduler);
void		pqueue_push(t_pqueue *pq, t_request req);
t_request	pqueue_pop(t_pqueue *pq);

/* cleanup.c */
void		cleanup(t_sim *sim);

#endif
