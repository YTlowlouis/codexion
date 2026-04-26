void take_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	while (!dongle->is_available
		|| get_time_ms() < dongle->available_at_ms)
	{
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	dongle->is_available = 0;
	pthread_unlock_mutex(&dongle->mutex);
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

	if (coder->left->id < coder>right->id)
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
