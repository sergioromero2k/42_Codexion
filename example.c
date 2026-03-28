/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   example.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 09:34:10 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/28 09:38:46 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/time.h>

int main() {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    printf("Segundos: %ld\n", tv.tv_sec);
    printf("Microsegundos: %ld\n", tv.tv_usec);

    return 0;
}


/* #include <pthread.h>
#include <stdio.h>

int				contador = 0;
pthread_mutex_t	mutex;

void	*incrementar(void *args)
{
	int	i;

	for (i = 0; i < 100000; i++)
	{
		pthread_mutex_lock(&mutex);
		contador++;
		pthread_mutex_unlock(&mutex);
	}
	return (NULL);
}

int	main(void)
{
	pthread_t t1, t2;
	pthread_mutex_init(&mutex, NULL);

	pthread_create(&t1, NULL, incrementar, NULL);
	pthread_create(&t2, NULL, incrementar, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("Contador final: %d\n", contador);
	pthread_mutex_destroy(&mutex);
	return (0);
} */