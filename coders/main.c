/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:33:52 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/03/21 17:40:43 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

enum Estado {
    APAGADO,
    ENCENDIDO
};


struct Dispositivo {
    char nombre[20];
    enum Estado estado;
};

int main(void) {
    struct Dispositivo d1 = {"TV", APAGADO};
    printf("%s está %d\n", d1.nombre, d1.estado);
}