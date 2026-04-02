/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 05:07:49 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/04/02 05:23:14 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap(t_pqueue *pq, int index, int padre)
{
	t_node	temp;

	temp = pq->heap[index];
	pq->heap[index] = pq->heap[padre];
	pq->heap[padre] = temp;
}

void	sift_up(t_pqueue *pq, int index)
{
	int	padre;

	while (index > 0)
	{
		padre = (index - 1) / 2;
		if (pq->heap[index].priority < pq->heap[padre].priority)
		{
			swap(pq, index, padre);
			index = padre;
		}
		else
			index = 0;
	}
}

void	sift_down(t_pqueue *pq, int index)
{
	int left_child;
	int right_child;
	int smallest;

	while (2 * index + 1 < pq->size)
	{
		left_child = 2 * index + 1;
		right_child = 2 * index + 2;
		smallest = left_child;
		if ((right_child < pq->size)
			&& (pq->heap[right_child].priority < pq->heap[left_child].priority))
			smallest = right_child;
		if (pq->heap[index].priority > pq->heap[smallest].priority)
		{
			swap(pq, index, smallest);
			index = smallest;
		}
		else
			index = 0;
	}
}
