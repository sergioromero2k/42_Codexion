/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 19:31:35 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/04/03 19:45:12 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_my_turn(t_dongle *dongle, int coder_id)
{
	t_node	nodo;

	if (dongle->waiters.size == 0)
		return (0);
	nodo = pqueue_peek(&dongle->waiters);
	if (nodo.coder_id == coder_id)
		return (1);
	return (0);
}
int	cooldown_active(t_dongle *dongle)
{
	long long	currently_time;

	currently_time = get_time_in_ms();
	if (currently_time < dongle->cooldown_until)
		return (1);
	return (0);
}
