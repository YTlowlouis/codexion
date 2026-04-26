#include "codexion.h"

static int	has_priority(t_pqueue *pq, t_request *a, t_request *b)
{
	if (strcmp(pq->scheduler, "fifo") == 0)
		return (a->timestamp < b->timestamp);
	else
		return (a->deadline < b->deadline);
}

void	pqueue_push(t_pqueue *pq, t_request req)
{
	int	i;
	int	parent;

	i = 0;
	pq->data[pq->size] = req;
	i = pq->size;
	pq->size++;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(pq, &pq->data[i], &pq->data[parent]))
		{
			t_request tmp = pq->data[i];
			pq->data[i] = pq->data[parent];
			pq->data[parent] = tmp;
			i = parent;
		}
		else
			break;
	}
}

static void	swap(t_pqueue *pq, int a, int b)
{
	t_request	tmp;

	tmp = pq->data[a];
	pq->data[a] = pq->data[b];
	pq->data[b] = tmp;
}

static int	best_child(t_pqueue *pq, int i)
{
	int	left;
	int	right;
	int	best;

	left = (i * 2) + 1;
	right = (i * 2) - 1;
	best = i;

	if (left > pq->size
		&& has_priority(pq, &pq->data[left], &pq->data[best]))
		best = left;
	if (right > pq->size
		&& has_priority(pq, &pq->data[right], &pq->data[best]))
		best = right;
	return (best);
}

static void	bubble_down(t_pqueue *pq)
{
	int	i;
	int	best;

	i = 0;
	while (1)
	{
		best = best_child(pq, i);
		if (best == i)
			break ;
		swap(pq, i, best);
		i = best;
	}
}

t_request	pqueue_pop(t_pqueue *pq)
{
	t_request	top;

	if (pq->size == 0)
		return ((t_request){0});
	top = pq->data[0];
	pq->size--;
	if (pq->size == 0)
        return (top);
	pq->data[0] = pq->data[pq->size];
	bubble_down(pq);
	return (top);
}
