/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 03:40:19 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/04/02 05:29:08 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	pqueue_init(t_pqueue *pq, int capacity)
{
	pq->heap = malloc(capacity * sizeof(t_node));
	if (!(pq->heap))
		return (1);
	pq->size = 0;
	pq->capacity = capacity;
	return (0);
}

void	pqueue_push(t_pqueue *pq, int coder_id, long long priority)
{
	if (pq->size >= pq->capacity)
		return ;
	pq->heap[pq->size].coder_id = coder_id;
	pq->heap[pq->size].priority = priority;
	pq->size++;
	sift_up(pq, pq->size - 1);
}
t_node	pqueue_pop(t_pqueue *pq)
{
	t_node	min;
	t_node	empty;

	if (pq->size == 0)
	{
		empty.coder_id = -1;
		empty.priority = -1;
		return (empty);
	}
	min = pq->heap[0];
	pq->heap[0] = pq->heap[pq->size - 1];
	pq->size--;
	sift_down(pq, 0);
	return (min);
}
t_node	pqueue_peek(t_pqueue *pq)
{
	t_node	empty;

	if (pq->size == 0)
	{
		empty.coder_id = -1;
		empty.priority = -1;
		return (empty);
	}
	return (pq->heap[0]);
}
void	pqueue_free(t_pqueue *pq)
{
	free(pq->heap);
	pq->heap = NULL;
	pq->size = 0;
	pq->capacity = 0;
}
