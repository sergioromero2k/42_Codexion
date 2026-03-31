/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 14:07:27 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/31 01:55:10 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Checks if a specific coder has exceeded the burnout time.
 * We use log_lock to ensure we read a stable value of last_compile_time.
 *
 * @param coder Pointer to the coder structure being checked.
 * @return Time in ms since last compilation.
 */
long long	check_health(t_coder *coder)
{
	long long	elapsed;

	pthread_mutex_lock(&coder->state_lock);
	elapsed = get_timestamp(coder->start_time) - coder->last_compile_time;
	pthread_mutex_unlock(&coder->state_lock);
	return (elapsed);
}

int	check_all_compiled(t_env *env)
{
	int	i;
	int	compile;

	i = 0;
	while (i < (env->config.number_of_coders))
	{
		pthread_mutex_lock(&env->coders[i].state_lock);
		compile = env->coders[i].compile_count;
		pthread_mutex_unlock(&env->coders[i].state_lock);
		if (compile < (env->config.number_of_compiles_required))
			return (0);
		i++;
	}
	return (1);
}
void	set_sim_over(t_env *env)
{
	pthread_mutex_lock(&env->state_lock);
	env->simulation_over = 1;
	pthread_mutex_unlock(&env->state_lock);
}

static void	handle_burnout(t_env *env, int i)
{
	pthread_mutex_lock(&env->log_lock);
	printf("%lld %d burned out\n", get_timestamp(env->start_time),
		env->coders[i].id);
	pthread_mutex_unlock(&env->log_lock);
	set_sim_over(env);
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
				return (handle_burnout(env, i), NULL);
			i++;
		}
		if (check_all_compiled(env))
		{
			set_sim_over(env);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}

