/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_mutex.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tponutha <tponutha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 00:20:35 by tponutha          #+#    #+#             */
/*   Updated: 2025/12/15 23:15:19 by tponutha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ph_check_die(t_philo *phi)
{
	int	res;

	pthread_mutex_lock(&phi->locker->grim);
	res = phi->who_die[0] != -1;
	pthread_mutex_unlock(&phi->locker->grim);
	return (res);
}

/*
◦ timestamp_in_ms X has taken a fork
◦ timestamp_in_ms X is eating
◦ timestamp_in_ms X is sleeping
◦ timestamp_in_ms X is thinking
◦ timestamp_in_ms X died
*/

void	ph_print_philo(t_philo *phi, t_time now, t_stat stat)
{
	char	*str;
	long	ms;
	int		i;

	str = NULL;
	i = phi->i + 1;
	if (stat == _take)
		str = "\033[0;32mhas taken a fork\n\033[0m";
	else if (stat == _think)
		str = "\033[0;33mis thinking\n\033[0m";
	else if (stat == _eat)
		str = "\033[0;34mis eating\n\033[0m";
	else if (stat == _sleep)
		str = "\033[0;35mis sleeping\n\033[0m";
	else if (stat == _die)
		str = "\033[0;36mdie\n\033[0m";
	ms = ph_timestamp(now, phi->info->epoch);
	pthread_mutex_lock(&phi->locker->print);
	printf("\033[0;31m%ldms \033[0m%d %s", ms, i, str);
	pthread_mutex_unlock(&phi->locker->print);
}

// Check if philo die yet?

static int	sb_aging_process(t_philo *philo, int n)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&philo->locker->age);
	while (i < n)
	{
		philo[i].life_ms += 1;
		if (philo[i].life_ms >= philo[i].info->die_ms)
			break ;
		i++;
	}
	pthread_mutex_unlock(&philo->locker->age);
	if (i != n)
	{
		pthread_mutex_lock(&philo->locker->grim);
		philo[i].who_die[0] = i;
		pthread_mutex_unlock(&philo->locker->grim);
	}
	return (i == n);
}

static int	sb_aging_limit(t_philo *philo, int n)
{
	int	i;
	int	complete;

	i = 0;
	complete = 0;
	pthread_mutex_lock(&philo->locker->age);
	while (i < n)
	{
		philo[i].life_ms += 1;
		complete += philo[i].eat_n >= philo->info->end_n;
		if (philo[i].life_ms >= philo[i].info->die_ms)
			break ;
		i++;
	}
	pthread_mutex_unlock(&philo->locker->age);
	if (i != n || complete == philo->info->philo_n)
	{
		pthread_mutex_lock(&philo->locker->grim);
		philo->who_die[0] = i;
		if (complete == philo->info->philo_n)
			philo->who_die[0] = -2;
		pthread_mutex_unlock(&philo->locker->grim);
	}
	return (i == n && complete != philo->info->philo_n);
}

// TODO : fix death delay

void	ph_aging(t_philo *philo)
{
	int		state;
	t_time	start;
	t_time	end;

	state = 1;
	gettimeofday(&start, NULL);
	end = ph_time_add(start, 1);
	while (state)
	{
		while (ph_is_time_exceed(end) != 1)
			continue ;
		gettimeofday(&start, NULL);
		end = ph_time_add(start, 1);
		if (philo->info->end_n == -1)
			state = sb_aging_process(philo, philo->info->philo_n);
		else
			state = sb_aging_limit(philo, philo->info->philo_n);
	}
}
