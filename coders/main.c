/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:33:52 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/31 02:53:03 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_env	env;

	memset(&env, 0, sizeof(t_env));
	if (parse_config(&env.config, argc, argv))
		return (1);
	if (init_simulation(&env, env.config.number_of_coders))
		return (1);
	start_simulation(&env, env.config.number_of_coders);
	cleanup(&env, env.config.number_of_coders);
	return (0);
}
