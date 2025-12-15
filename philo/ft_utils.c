/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tponutha <tponutha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:30:08 by tponutha          #+#    #+#             */
/*   Updated: 2023/08/03 07:25:32 by tponutha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	ft_strclen(const char *str, char c)
{
	size_t	len;

	len = 0;
	if (!str)
		return (len);
	while (str[len] != 0 && str[len] != c)
		len++;
	return (len);
}

static int	sb_convert_number(const char *str)
{
	int	neg;
	int	sum;

	if (!str)
		return (0);
	sum = 0;
	neg = 1;
	while ((*str >= '\t' && *str <= '\r') || *str == ' ')
		str++;
	if (*str == '-')
		neg = -1;
	str += (*str == '+' || *str == '-');
	while (*str >= '0' && *str <= '9')
	{
		sum = (sum * 10) + (*str - '0');
		if (sum < 0)
			return (-1);
		str++;
	}
	return (neg * sum);
}

int	ft_philo_atoi(const char *str)
{
	int	i;
	int	res;

	i = ft_strclen(str, 0);
	if (i > 10)
		return (0);
	i = 0;
	while (str[i] != 0)
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	res = sb_convert_number(str);
	if (res <= 0)
		return (0);
	return (res);
}

int	ft_offset(t_philo *phi)
{
	int	offset;

	offset = phi->odd_stop[0] + phi->i;
	if (phi->i < phi->odd_stop[0])
		offset++;
	return (offset % 2 == 1);
}

void	ph_print_die(t_philo *philo)
{
	t_time	now;

	if (philo->who_die[0] < 0)
		return ;
	pthread_mutex_lock(&philo->locker->grim);
	gettimeofday(&now, NULL);
	usleep(100);
	ph_print_philo(&philo[philo->who_die[0]], now, _die);
	pthread_mutex_unlock(&philo->locker->grim);
}
