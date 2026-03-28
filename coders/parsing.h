/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:47:22 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/25 06:24:44 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include <ctype.h>
# include <limits.h>
# include <pthread.h>
# include <stdarg.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef enum s_schedule
{
	SCHED_FIFO,
	SCHED_EDF,
}					t_schedule;

typedef struct s_config
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	long			number_of_compiles_required;
	long			dongle_cooldown;
	t_schedule		scheduler;
}					t_config;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	long			cooldown_until;
}					t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compile_count;
	long			last_compile_time;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	pthread_mutex_t	*log_lock;
	t_config		*config;
}					t_coder;

typedef struct s_env
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_mutex_t	log_lock;
}					t_env;

int					parse_config(t_config *config, int argc, char **argv);
int					parse_config_2(t_config *config, char *str);
int 				init_simulation(t_env *env)

#endif