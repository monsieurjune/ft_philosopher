/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_sim.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tponutha <tponutha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/08 15:47:33 by tponutha          #+#    #+#             */
/*   Updated: 2023/08/03 07:24:59 by tponutha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* EVEN
1.) odd-th philo start eat first
	- odd-th philo call mutex on table
	- even-th philo start thinking & idle
	- odd-th philo start sleeping & return fork
2.) even-th philo start eating after odd-th philo sleep
	- even-th philo call mutex on table
	- even-th philo start sleeping & return fork
	- odd-th philo wake up & thinking ~1ms
		- if there is enough fork on table then eat again
		- else idle until there is fork
*/

void	ph_odd_change(t_philo *phi)
{
	pthread_mutex_lock(&phi->locker->lock);
	phi->odd_stop[0] += 1;
	phi->odd_stop[0] %= phi->info->philo_n;
	pthread_mutex_unlock(&phi->locker->lock);
}

void	*ph_run_odd(void *arg)
{
	t_philo	*phi;
	int		success;

	phi = (t_philo *)arg;
	success = 0;
	if (ft_offset(phi) && phi->i != phi->odd_stop[0])
		success = ph_get_fork(phi);
	while (success != -1)
	{
		if (success)
		{
			if (ph_eat(phi) == -1)
				break ;
			if (ph_sleep(phi) == -1)
				break ;
		}
		success = ph_think(phi);
	}
	return (NULL);
}

void	*ph_run_even(void *arg)
{
	t_philo	*phi;
	int		success;

	phi = (t_philo *)arg;
	success = 0;
	if (phi->i % 2 == 0)
		success = ph_get_fork(phi);
	while (success != -1)
	{
		if (success)
		{
			if (ph_eat(phi) == -1)
				break ;
			if (ph_sleep(phi) == -1)
				break ;
		}
		success = ph_think(phi);
	}
	return (NULL);
}

void	ph_sim(t_philo *philo, int is_even)
{
	static int		i = 0;
	static int		j = 0;

	while (i < philo->info->philo_n && is_even)
	{
		if (pthread_create(&philo->id, NULL, ph_run_even, &philo[i]) != 0)
			break ;
		i++;
	}
	while (i < philo->info->philo_n && !is_even)
	{
		if (pthread_create(&philo->id, NULL, ph_run_odd, &philo[i]) != 0)
			break ;
		i++;
	}
	ph_aging(philo);
	ph_print_die(philo);
	while (j < i)
	{
		pthread_join(philo->id, NULL);
		j++;
	}
}
