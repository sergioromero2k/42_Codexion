#include <pthread.h>
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
}