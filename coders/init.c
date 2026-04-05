/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:22:41 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/04/05 04:00:58 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Releases all synchronized resources and deadlocates dynamic
 * memory to prevent leaks.
 *
 * @param env Pointer to the main enviroment structure.
 * @param i Number of succesfully initialized dongle mutexes to destroy.
 */
void	cleanup(t_env *env, int i)
{
	while (--i >= 0)
	{
		pthread_mutex_destroy(&env->dongles[i].mutex);
		pthread_mutex_destroy(&env->coders[i].state_lock);
		pthread_cond_destroy(&env->dongles[i].cond);
		pqueue_free(&env->dongles[i].waiters);
	}
	pthread_mutex_destroy(&env->log_lock);
	free(env->coders);
	free(env->dongles);
}

/**
 * Allocates dynamic memory for coders and dongles arrays.
 *
 * @param Pointer to the main environment structure.
 * @param Number of coders/dongles to allocate.
 * @return int 0 on success, 1 on allocation failure.
 */
int	inizialite(t_env *env, int n)
{
	env->coders = malloc(sizeof(t_coder) * n);
	if (!env->coders)
		return (write(2, "Error: Coder malloc\n", 20), 1);
	env->dongles = malloc(sizeof(t_dongle) * n);
	if (!env->dongles)
	{
		free(env->coders);
		return (write(2, "Error: Dongle malloc\n", 21), 1);
	}
	memset(env->coders, 0, sizeof(t_coder) * n);
	memset(env->dongles, 0, sizeof(t_dongle) * n);
	return (0);
}

/**
 * Initializes mutexes, conditions,
	and internal variables for a specific coder and its associated dongle.
 *
 * @param env Pointer to the main environment structure.
 * @param i Current index of the coder/dongle being initialized.
 * @param n Total number of coders in the simulation.
 * @return 0 on success, or 1 if a mutex initialization fails.
 */
static int	init_coder(t_env *env, int i, int n)
{
	if (pthread_mutex_init(&env->dongles[i].mutex, NULL) != 0)
		return (cleanup(env, i), 1);
	if (pthread_mutex_init(&env->coders[i].state_lock, NULL) != 0)
		return (cleanup(env, i), 1);
	pthread_cond_init(&env->dongles[i].cond, NULL);
	env->dongles[i].in_use = 0;
	env->dongles[i].cooldown_until = 0;
	pqueue_init(&env->dongles[i].waiters, n);
	env->coders[i].id = i + 1;
	env->coders[i].log_lock = &env->log_lock;
	env->coders[i].config = &env->config;
	env->coders[i].start_time = env->start_time;
	env->coders[i].right_dongle = &env->dongles[i];
	env->coders[i].left_dongle = &env->dongles[(i + 1) % n];
	env->coders[i].env = env;
	return (0);
}

/**
 * Setps up the global simulation state,
	including memory allocation and resource initialization for all participants.
 *
 * @param env Pointer to the main environment structure.
 * @param n Total number of coders/dongles to participate in the simulation.
 * @return 0 on success, or 1 if any initialization step fails.
 */
int	init_simulation(t_env *env, int n)
{
	int	i;

	if (inizialite(env, n) != 0)
		return (1);
	pthread_mutex_init(&env->log_lock, NULL);
	pthread_mutex_init(&env->state_lock, NULL);
	env->simulation_over = 0;
	env->start_time = get_time_in_ms();
	i = 0;
	while (i < n)
	{
		if (init_coder(env, i, n) != 0)
			return (1);
		i++;
	}
	return (0);
}

/**
 * Spawns threads for all coders and waits for their completion.
 *
 * @param env Pointer to the main environment structure.
 * @param n Total number of coder threads to create.
 */
void	start_simulation(t_env *env, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_create(&env->coders[i].thread, NULL, coder_routine,
			&env->coders[i]);
		i++;
	}
	pthread_create(&env->monitor, NULL, monitor_routine, env);
	i = 0;
	while (i < n)
	{
		pthread_join(env->coders[i].thread, NULL);
		i++;
	}
	pthread_join(env->monitor, NULL);
}
