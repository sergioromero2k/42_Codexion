/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:47:22 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/21 20:56:25 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# ifndef    PARSING_H
# define    PARSING_H


typedef enum s_schedule {
    SCHED_FIFO,
    SCHED_EDF
} t_schedule;


typedef struct s_config {
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    t_schedule scheduler;
} t_config;

# endif