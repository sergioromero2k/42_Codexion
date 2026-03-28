/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 06:22:41 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/28 09:08:42 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

/**
 * Destroys initialized mutex and frees allocated memory.
 *
 * @param env Pointer to the main enviroment structure.
 * @param i Number of succesfully initialized dongle mutexes to destroy.
 */
void	cleanup(t_env *env, int i)
{
	while (--i >= 0)
		pthread_mutex_destroy(&env->dongles[i].mutex);
	pthread_mutex_destroy(&env->log_lock);
	free(env->coders);
	free(env->dongles);
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
	pthread_mutex_lock(me->log_lock);
	printf("Coder %d: ¡Estoy en la mesa!\n", me->id);
	pthread_mutex_unlock(me->log_lock);
	return (NULL);
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
	int	i;

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
 * Sets up the simulation by initializing mutexes and linking resources.
 * Handles the "Circular Table" logic: assigning right and left dongles.
 *
 * @param env Pointer to the main environment structure.
 * @param n Total number of coders participating.
 * @return int 0 on success, 1 if any mutex initialization fails.
 */
int	init_simulation(t_env *env, int n)
{
	int	i;
	int	check;

	if (inizialite(env, n) != 0)
		return (1);
	pthread_mutex_init(&env->log_lock, NULL);
	i = 0;
	while (++i < n)
	{
		if (pthread_mutex_init(&env->dongles[i].mutex, NULL) != 0)
			return (cleanup(env, i), 1);
		env->coders[i].id = i + 1;
		env->coders[i].log_lock = &env->log_lock;
		env->coders[i].config = &env->config;
		env->coders[i].right_dongle = &env->dongles[i];
		env->coders[i].left_dongle = &env->dongles[(i + 1) % n];
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
	i = 0;
	while (i < n)
	{
		pthread_join(env->coders[i].thread, NULL);
		i++;
	}
}
