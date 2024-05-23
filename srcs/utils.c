/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 10:52:51 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 16:35:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

/**
 * @brief Get the woody object
 *
 * This function return the woody object
 *
 * @return woody_t*
 */
woody_t *get_woody(void) {
    static woody_t woody;
    static bool init = false;

    if (init == false) {
        ft_memset(&woody, 0, sizeof(woody_t));
        init = true;
    }

    return (&woody);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                DISPLAY FUNCTIONS                               ||
// ! ||--------------------------------------------------------------------------------||

int __display_pass(const char *file, const char *fun, const char *header) {
    if (ARG_IS_VERBOSE() == false)
        return (0);
    printf(DIM "[%s] - " COLOR_RESET, file);
    printf(BOLD COLOR_CYAN "[%s] " COLOR_RESET, fun);
    printf(BOLD COLOR_GREEN);
    printf(COLOR_YELLOW BOLD "[%s]" RESET_COLOR COLOR_GREEN BOLD " [✔]\n" RESET_COLOR, header);
    return (0);
}

int display_perror(const char *str) {
    printf(BOLD COLOR_RED);
    perror(str);
    printf(COLOR_RESET);
    return (1);
}

int display_error(const char *str) {
    fprintf(stderr, BOLD COLOR_RED "Error: " COLOR_WHITE "%s\n", str);
    return (1);
}

int display_warning(const char *str) {
    if  (ARG_IS_VERBOSE() == false)
        return (0);
    fprintf(stderr, BOLD COLOR_YELLOW "Warning: " COLOR_WHITE "%s\n", str);
    return (1);
}

int display_help(const char *binary) {
    printf("Usage: %s [flags] <ELF file>\n\n", binary);
    printf("  Description:\n");
    printf("    Woody-Woodpacker is a tool to crypt and pack a binary file.\n");
    printf("    It will create an other file with named \'woody\' crypted by an algorithm.\n");
    printf("    When this binary will be executed, it will print  \'....WOODY....\' and the original binary will be executed.\n");
    printf("\n  Informations:\n");
    printf("    - ELF file: The binary file to crypt and pack. (ELF x64 only)\n");
    printf("\n  Flags:\n");
    printf("    -h, --help: Display this help.\n");
    printf("    -v, --verbose: Display verbose informations.\n");
    printf("    -k, --key: Set the key to crypt the binary file.\n");
    exit(0);
}

int __display_verbose_title(const char *title) {
    if (ARG_IS_VERBOSE()) {
        const uint32_t count = 80 - ft_strlen(title);

        printf(BOLD COLOR_YELLOW);
        printf("\n");
        for (uint32_t i = 0; i < count / 2; ++i) {
            printf("-");
        }
        printf(" ");
        printf("%s", title);
        printf(" ");
        for (uint32_t i = 0; i < count / 2; ++i) {
            printf("-");
        }
        printf("\n\n" COLOR_RESET);
    }
    return (0);
}

int __display_verbose(const char *file, const char *fun, const char *str, ...) {
    if (ARG_IS_VERBOSE()) {
        printf(DIM "[%s] - " COLOR_RESET, file);
        printf(BOLD COLOR_CYAN "[%s] " COLOR_RESET, fun);
        printf(BOLD COLOR_GREEN);

        va_list args;
        va_start(args, str);
        vprintf(str, args);
        va_end(args);

        printf(RESET_COLOR);
    }
    return (0);
}