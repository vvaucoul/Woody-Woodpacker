/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 14:24:21 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 15:47:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

int main(int argc, char **argv) {
    /* Parse arguments */
    if ((parse_arguments(argc, argv)) != 0) {
        return (EXIT_FAILURE);
    }

    /* Read ELF file */
    display_verbose_title("Read ELF file");
    if ((read_elf_binary(&W->file)) != 0) {
        return (EXIT_FAILURE);
    }

    /* Check if Binary has enough space to be infected */
    display_verbose_title("Check SPACE");
    if ((elf_check_space()) != 0) {
        return (display_error("elf_check_space"));
    }

    /* Setup encryption data */
    display_verbose_title("Setup encryption data");
    if ((setup_encryption_data(&ENCRYPTION)) != 0) {
        return (display_error("setup_encryption_data"));
    }

    /* Encrypt text segment */
    display_verbose_title("Encrypt text segment");
    if ((encrypt_text_segment(W->file, ENCRYPTION.key)) != 0) {
        return (display_error("encrypt_text_segment"));
    }

    /* Inject woody */
    display_verbose_title("Inject woody");
    elf_change_load_segment(&ENCRYPTION);

    /* Write new file */
    display_verbose_title("Write new file");
    if ((elf_write_new_file()) != 0) {
        return (display_error("elf_write_new_file"));
    }

    /* Cleanup */
    display_verbose_title("Cleanup");
    close(W->file.fd);
    cleanup_elf(&W->file);

    return (0);
}