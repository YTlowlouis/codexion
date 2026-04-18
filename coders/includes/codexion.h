#ifndef codexion_h
# define codexion_h

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

typedef struct s_sim    t_sim;      // forward declaration
typedef struct s_pqueue t_pqueue; 

typedef struct s_dongle
{
    int             id;
    int             is_available;
    long            available_at_ms;   // timestamp cooldown
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    // file d'attente (priority queue) pour FIFO/EDF
    t_pqueue        *queue;
}   t_dongle;

typedef struct s_coder
{
    int             id;
    int             compile_count;
    long            last_compile_start; // pour EDF + burnout
    int             burned_out;
    t_dongle        *left;
    t_dongle        *right;
    t_sim           *sim;              // pointeur vers la simulation
}   t_coder;

typedef struct s_sim
{
    int             nb_coders;
    long            time_to_burnout;
    long            time_to_compile;
    long            time_to_debug;
    long            time_to_refactor;
    int             nb_compiles_required;
    long            dongle_cooldown;
    char            *scheduler;        // "fifo" ou "edf"

    t_coder         *coders;
    t_dongle        *dongles;
    pthread_t       *threads;
    pthread_t       monitor;

    int             simulation_over;   // flag d'arrêt global
    pthread_mutex_t log_mutex;         // protège printf
    pthread_mutex_t state_mutex;       // protège simulation_over
}   t_sim;

#endif // codexion_h
