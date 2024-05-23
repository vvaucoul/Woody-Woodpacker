/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 11:08:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/08 17:50:49 by mle-faou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "libft.h"

int ft_strcmp(const char *str, const char *str2)
{
    unsigned int i;

    i = 0;
    while (str[i] && str[i] == str2[i])
        ++i;
    return ((unsigned char)str[i] - (unsigned char)str2[i]);
}
