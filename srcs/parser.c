/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 18:25:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 16:20:49 by mle-faou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

/**
 * @brief Parse arguments
 * 
 * Parse arguments of the program
 * 
 * @param argc
 * @param argv
 * @return int
 */
int parse_arguments(int argc, char **argv) {
    if (argc < 2) {
        return (display_help(argv[0]));
    }

    for (int i = 1; i < argc; i++) {
        if (ft_strcmp(argv[i], "-h") == 0 || ft_strcmp(argv[i], "--help") == 0) {
            if (ARG_IS_HELP())
                return (display_error("Multiple help flags"));
            ARGS.args |= ARG_HELP;
            display_help(argv[0]);
        } else if (ft_strcmp(argv[i], "-v") == 0 || ft_strcmp(argv[i], "--verbose") == 0) {
            if (ARG_IS_VERBOSE())
                return (display_error("Multiple verbose flags"));
            ARGS.args |= ARG_VERBOSE;
        } else if (ft_strncmp(argv[i], "-k=", 3) == 0) {
            if (ARG_IS_KEY())
                return (display_error("Multiple key flags"));
            if (ft_strlen(argv[i]) > KEY_LEN + 3)
                return (display_error("Invalid key size"));
            ARGS.args |= ARG_KEY;
            ARGS.key = ft_atol(argv[i] + 3);
        } else if (ft_strncmp(argv[i], "--key=", 6) == 0) {
            if (ARG_IS_KEY())
                return (display_error("Multiple key flags"));
            if (ft_strlen(argv[i]) > KEY_LEN + 6)
                return (display_error("Invalid key size"));
            ARGS.key = ft_atol(argv[i] + 6);
            ARGS.args |= ARG_KEY;
        } else {
            if (ARG_IS_FILE())
                return (display_error("Multiple file flags"));
            ARGS.args |= ARG_FILE;

            /* Setup file names */
            W->file.name = argv[i];
        }
    }

    return (0);
}