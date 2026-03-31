/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 20:04:59 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/31 02:30:57 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Coder attempts to pick up both adjacent dongles.
 * The coder locks the right dongle first, then the left one.
 *
 * @param me Pointer to the coder's individual structure.
 */
void	take_dongles(t_coder *me)
{
	pthread_mutex_lock(&me->right_dongle->mutex);
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d has taken a dongle\n", get_timestamp(me->start_time),
		me->id);
	pthread_mutex_unlock(me->log_lock);
	pthread_mutex_lock(&me->left_dongle->mutex);
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d has taken a dongle\n", get_timestamp(me->start_time),
		me->id);
	pthread_mutex_unlock(me->log_lock);
}

/**
 * Releases both dongles held by the coder.
 *
 * @param me Pointer to th e coder's individual structure.
 */
void	drop_dongles(t_coder *me)
{
	pthread_mutex_unlock(&me->right_dongle->mutex);
	pthread_mutex_unlock(&me->left_dongle->mutex);
}

/**
 * Simulates the compilation process.
 * Logs the "is eating" status and updates the last_compile_time.
 *
 * @param me Pointer to the coder's individual structure.
 */
void	coder_compile(t_coder *me)
{
	pthread_mutex_lock(&me->state_lock);
	me->last_compile_time = get_timestamp(me->start_time);
	pthread_mutex_unlock(&me->state_lock);
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d is compiling\n", get_timestamp(me->start_time), me->id);
	pthread_mutex_unlock(me->log_lock);
	usleep(me->config->time_to_compile * 1000);
	pthread_mutex_lock(&me->state_lock);
	me->compile_count++;
	pthread_mutex_unlock(&me->state_lock);
}

void	coder_debug(t_coder *me)
{
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d is debugging\n", get_timestamp(me->start_time), me->id);
	pthread_mutex_unlock(me->log_lock);
	usleep(me->config->time_to_debug * 1000);
}

/**
 * Simulates the debugging and refactoring phases (sleep and thimk).
 *
 * @param me Pointer to the coder's individual structure.
 */
void	coder_refactor(t_coder *me)
{
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d is refactoring\n", get_timestamp(me->start_time), me->id);
	pthread_mutex_unlock(me->log_lock);
	usleep(me->config->time_to_refactor * 1000);
}
