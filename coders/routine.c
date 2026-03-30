/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 14:07:21 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/31 00:43:43 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Main routine for each coder thread.
 *
 * @param arg Pointer to the specific t_coder structure.
 * @return void* Returns NULL upon thread completion.
 */
void	*coder_routine(void *arg)
{
	t_coder *me;

    me  = (t_coder *) arg;
	if( me->id % 2 == 0) 
		usleep(1500);
	
	while(1) {
		take_dongles(me);
		coder_compile(me);
		drop_dongles(me);
		coder_sleep_and_think(me);
	}
	return NULL;
}
