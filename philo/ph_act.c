/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_act.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tponutha <tponutha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/08 16:26:11 by tponutha          #+#    #+#             */
/*   Updated: 2025/12/15 23:11:16 by tponutha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// /*
// 1.First check fork (use once)
// 2.Think & check fork
// 3.Eat & reset lift_t
// 4.Sleep
// - All must check if someone really die
// */

int	ph_delay(t_philo *phi, unsigned int delay_ms)
{
	t_time			start_ms;
	t_time			end_ms;

	gettimeofday(&start_ms, NULL);
	end_ms = ph_time_add(start_ms, delay_ms);
	while (ph_is_time_exceed(end_ms) != 1)
	{
		if (ph_check_die(phi))
			return (-1);
		usleep(DELAY_US);
	}
	return (1);
}

int	ph_get_fork(t_philo *phi)
{
	t_time	now;
	int		get_fork;
	int		i;
	int		j;

	gettimeofday(&now, NULL);
	if (ph_check_die(phi))
		return (-1);
	i = phi->i;
	j = (i + 1) % phi->info->philo_n;
	if (phi->info->philo_n % 2 == 1)
		if (!ft_offset(phi) || phi->i == phi->odd_stop[0])
			return (0);
	pthread_mutex_lock(phi->lock);
	pthread_mutex_lock(phi->lock2);
	get_fork = (phi->table[i] == 1 && phi->table[j] == 1 && i != j);
	if (get_fork && phi->i != phi->odd_stop[0])
	{
		phi->table[i] = 0;
		phi->table[j] = 0;
		ph_print_philo(phi, now, _take);
	}
	pthread_mutex_unlock(phi->lock2);
	pthread_mutex_unlock(phi->lock);
	return (get_fork && phi->i != phi->odd_stop[0]);
}

// // it will stop only when it got fork or die
/*
0ms 1 has taken a fork
0ms 1 is eating
0ms 2 is thinking
201ms 1 is sleeping
202ms 2 has taken a fork
202ms 2 is eating
403ms 2 is sleeping
407ms 1 die
*/

int	ph_think(t_philo *phi)
{
	int		state;
	t_time	now;

	state = 0;
	gettimeofday(&now, NULL);
	if (ph_check_die(phi))
		return (-1);
	while (state == 0)
	{
		if (ph_delay(phi, 1) == -1)
			return (-1);
		state = ph_get_fork(phi);
	}
	return (state);
}

int	ph_eat(t_philo *phi)
{
	t_time	now;
	int		j;

	gettimeofday(&now, NULL);
	j = (phi->i + 1) % phi->info->philo_n;
	if (ph_check_die(phi))
		return (-1);
	pthread_mutex_lock(&phi->locker->age);
	phi->life_ms = 0;
	phi->eat_n += 1;
	pthread_mutex_unlock(&phi->locker->age);
	ph_print_philo(phi, now, _eat);
	if (ph_delay(phi, phi->info->eat_ms) == -1)
		return (-1);
	if (phi->i == ((phi->odd_stop[0] + 1) % phi->info->philo_n))
		ph_odd_change(phi);
	pthread_mutex_lock(phi->lock);
	pthread_mutex_lock(phi->lock2);
	phi->table[phi->i] = 1;
	phi->table[j] = 1;
	pthread_mutex_unlock(phi->lock2);
	pthread_mutex_unlock(phi->lock);
	return (1);
}

int	ph_sleep(t_philo *phi)
{
	int		state;
	t_time	now;

	gettimeofday(&now, NULL);
	if (ph_check_die(phi))
		return (-1);
	ph_print_philo(phi, now, _sleep);
	state = ph_delay(phi, phi->info->sleep_ms);
	gettimeofday(&now, NULL);
	if (state != -1)
		ph_print_philo(phi, now, _think);
	return (state);
}
