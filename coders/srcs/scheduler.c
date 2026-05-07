#include "codexion.h"

static int	has_priority(t_pqueue *pq, t_request *a, t_request *b);
static void	bubble_down(t_pqueue *pq);

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

static int	has_priority(t_pqueue *pq, t_request *a, t_request *b)
{
	if (strcmp(pq->scheduler, "fifo") == 0)
		return (a->timestamp < b->timestamp);
	return (a->deadline < b->deadline);
}

void	pqueue_push(t_pqueue *pq, t_request req)
{
	int			i;
	int			parent;
	t_request	tmp;

	pq->data[pq->size] = req;
	i = pq->size;
	pq->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(pq, &pq->data[i], &pq->data[parent]))
		{
			tmp = pq->data[i];
			pq->data[i] = pq->data[parent];
			pq->data[parent] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

static void	bubble_down(t_pqueue *pq)
{
	int			i;
	int			left;
	int			right;
	int			best;
	t_request	tmp;

	i = 0;
	while (1)
	{
		left = (i * 2) + 1;
		right = (i * 2) + 2;
		best = i;
		if (left < pq->size
			&& has_priority(pq, &pq->data[left], &pq->data[best]))
			best = left;
		if (right < pq->size
			&& has_priority(pq, &pq->data[right], &pq->data[best]))
			best = right;
		if (best == i)
			break ;
		tmp = pq->data[i];
		pq->data[i] = pq->data[best];
		pq->data[best] = tmp;
		i = best;
	}
}

t_request	pqueue_pop(t_pqueue *pq)
{
	t_request	top;

	memset(&top, 0, sizeof(top));
	if (pq->size == 0)
		return (top);
	top = pq->data[0];
	pq->size--;
	if (pq->size == 0)
		return (top);
	pq->data[0] = pq->data[pq->size];
	bubble_down(pq);
	return (top);
}
