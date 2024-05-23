/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 20:12:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/08 20:12:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int skip_useless_char(const char *str)
{
    int i;

    i = 0;
    while (str[i] == '\t' || str[i] == '\n' ||
           str[i] == '\v' || str[i] == '\r' ||
           str[i] == '\f' || str[i] == ' ')
        ++i;
    return (i);
}

long ft_atol(const char *str)
{
    long result = 0;
    bool isneg = false;
    size_t i;

    i = skip_useless_char(str);
    if (str[i] == '-' || str[i] == '+')
    {
        isneg = str[i] == '-' ? true : false;
        ++i;
    }
    while (str[i] && ft_isdigit(str[i]))
    {
        result = result * 10 + str[i] - 48;
        ++i;
    }
    result *= (isneg == 1 ? -1 : 1);
    return (result);
}
