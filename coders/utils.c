/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 09:23:30 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/04/03 20:53:29 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Gets the current system time in milliseconds.
 * Multiplies the seconds by 1000 and divides the microseconds by 1000
 * to standarsize the unit to milliseconds.
 *
 * @return long long The total in milliseconds since 1970.
 */
long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (write(2, "gettimeofday error\n", 19), 0);
	return ((long long)(tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/**
 * Calculates how much has passed since the simulation started.
 *
 * @param star_time The time saved at the beginning in env->start_time.
 * @return long long Elapsed milliseconds (0, 100, 2500, etc).
 */
long long	get_timestamp(long long start_time)
{
	return (get_time_in_ms() - start_time);
}

struct timespec	ms_to_timespec(long long ms)
{
	struct timeval	tv;
	struct timespec	ts;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + (ms / 1000);
	ts.tv_nsec = (tv.tv_usec * 1000) + ((ms % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	return (ts);
}

long long	get_priority(t_coder *me)
{
	if (me->config->scheduler == E_SCHED_FIFO)
		return (get_time_in_ms());
	if (me->config->scheduler == E_SCHED_EDF)
		return (me->last_compile_time + me->config->time_to_burnout);
}
