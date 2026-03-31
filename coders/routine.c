/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 14:07:21 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/31 02:28:08 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_is_over(t_env *env)
{
	int	simulation;

	pthread_mutex_lock(&env->state_lock);
	simulation = env->simulation_over;
	pthread_mutex_unlock(&env->state_lock);
	return (simulation);
}

/**
 * Main routine for each coder thread.
 *
 * @param arg Pointer to the specific t_coder structure.
 * @return void* Returns NULL upon thread completion.
 */
void	*coder_routine(void *arg)
{
	t_coder	*me;

	me = (t_coder *)arg;
	if (me->id % 2 == 0)
		usleep(1500);
	while (!(sim_is_over(me->env)))
	{
		take_dongles(me);
		if (sim_is_over(me->env))
		{
			drop_dongles(me);
			break ;
		}
		coder_compile(me);
		drop_dongles(me);
		if (sim_is_over(me->env))
			break ;
		coder_debug(me);
		if (sim_is_over(me->env))
			break ;
		coder_refactor(me);
	}
	return (NULL);
}
