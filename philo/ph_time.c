/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tponutha <tponutha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:30:08 by tponutha          #+#    #+#             */
/*   Updated: 2025/12/15 23:15:35 by tponutha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ph_timestamp(t_time now, t_time epoch)
{
	long	s;
	long	us;

	s = now.tv_sec - epoch.tv_sec;
	us = now.tv_usec - epoch.tv_usec;
	return (s * 1000 + us / 1000);
}

t_time	ph_time_add(t_time t1, unsigned int delay_ms)
{
	t_time			t2;
	unsigned int	sec;
	unsigned int	usec;
	unsigned int	offset_us;

	sec = delay_ms / 1000;
	usec = (delay_ms % 1000) * 1000;
	t2.tv_sec = t1.tv_sec + sec;
	offset_us = t1.tv_usec + usec;
	if (offset_us >= 1000000)
	{
		t2.tv_sec += offset_us / 1000000;
		offset_us %= 1000000;
	}
	t2.tv_usec = offset_us;
	return (t2);
}

int	ph_is_time_exceed(t_time end)
{
	t_time	now;

	gettimeofday(&now, NULL);
	if (now.tv_sec > end.tv_sec)
		return (1);
	else if (now.tv_sec < end.tv_sec)
		return (0);
	else if (now.tv_usec >= end.tv_usec)
		return (1);
	return (0);
}

void	*ph_free_lock(t_mutex *forks, int n)
{
	int	j;

	j = 0;
	while (j < n)
	{
		pthread_mutex_destroy(&forks[j]);
		j++;
	}
	free(forks);
	return (NULL);
}

t_mutex	*ph_alloc_lock(int n)
{
	t_mutex	*forks;
	int		i;

	i = 0;
	forks = malloc(sizeof(t_mutex) * n);
	if (forks == NULL)
		return (NULL);
	while (i < n)
	{
		if (pthread_mutex_init(&forks[i], NULL) != 0)
			break ;
		i++;
	}
	if (i != n)
		return (ph_free_lock(forks, i));
	return (forks);
}
