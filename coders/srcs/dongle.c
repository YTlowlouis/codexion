/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loboehm <loboehm@student.42nice.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 14:20:56 by loboehm           #+#    #+#             */
/*   Updated: 2026/04/30 15:18:06 by loboehm          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void fill_timespec(struct timespec *ts, long long target_time_ms)
{
    ts->tv_sec = target_time_ms / 1000;
    ts->tv_nsec = (target_time_ms % 1000) * 1000000;
}

void take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_request		req;
	struct timespec	ts;

	req.coder = coder;
	req.timestamp = get_time_ms();
	req.deadline = coder->last_compile_start
		+ coder->sim->time_to_burnout;
	pthread_mutex_lock(&dongle->mutex);
	pqueue_push(dongle->queue, req);
	while (dongle->queue->data[0].coder != coder
		|| !dongle->is_available
		|| get_time_ms() < dongle->available_at_ms)
	{
		if (is_sim_over(coder->sim))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
		if (dongle->is_available && get_time_ms() < dongle->available_at_ms)
		{
			fill_timespec(&ts, dongle->available_at_ms); // Fonction helper à créer
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	pqueue_pop(dongle->queue);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	log_state(coder->sim, coder->id, "has taken a dongle");
}

void release_dongle(t_dongle *dongle, t_sim *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	dongle->available_at_ms = get_time_ms() + sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void take_two_dongle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left->id < coder->right->id)
	{
		first = coder->left;
		second = coder->right;
	}
	else
	{
		first = coder->right;
		second = coder->left;
	}
	take_dongle(first, coder);
	take_dongle(second, coder);
}
