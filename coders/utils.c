/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 09:23:30 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/28 19:58:07 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

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
	return ((long long)(tv.tv_sec * 1000) + (tv.tv_sec / 1000));
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
