/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:47:22 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/22 19:29:16 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

typedef enum s_schedule
{
	SCHED_FIFO,
	SCHED_EDF
}				t_schedule;

typedef struct s_config
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	long		number_of_compiles_required;
	long		dongle_cooldown;
	t_schedule	scheduler;
}				t_config;

int				parse_config(t_config *config, int argc, char **argv);

#endif