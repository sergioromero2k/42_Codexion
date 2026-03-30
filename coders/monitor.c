/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 14:07:27 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/30 14:33:28 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

/**
 * Checks if a specific coder has exceeded the burnout time.
 * We use log_lock to ensure we read a stable value of last_compile_time.
 * 
 * @param coder Pointer to the coder structure being checked.
 * @return Time in ms since last compilation.
 */
long long	check_health(t_coder *coder)
{
	long long	current_time;
	long long	time_since_last_meal;

	pthread_mutex_lock(coder->log_lock);
	current_time = get_timestamp(coder->start_time);
	time_since_last_meal = current_time - coder->last_compile_time;
    pthread_mutex_unlock(coder->log_lock);
	return (time_since_last_meal);
}

/**
 * Main monitor loop that patrols all coders for death conditions.
 * It iterates through all coders and checks if any has exceeded the burnout.
 * 
 * @param arg Pointer to the global environment (t_env).
 * @return NULL when the simulation ends.
 */
void	*monitor_routine(void *arg)
{
	t_env	*env;
	int		i;

	env = (t_env *)arg;
	while (1)
	{
		i = 0;
		while (i < env->config.number_of_coders)
		{
			if (check_health(&env->coders[i]) >= env->config.time_to_burnout)
			{
				pthread_mutex_lock(&env->log_lock);
				printf("%lld %d died\n", get_timestamp(env->start_time),
					env->coders[i].id);
				// AQUÍ LEVANTARÍAMOS LA BANDERA DE PARADA (Día 4.2)
				// env->stop_sim = 1;
				return (NULL);
			}
			i++;
		}
		usleep(500);
	}
	return (NULL);
}
