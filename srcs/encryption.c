/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encryption.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 11:03:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 16:09:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

/**
 * @brief Setup encryption data
 *
 * This function setup encryption data
 * It links C code to ASM code
 *
 * @param encryption
 * @return int
 */
int setup_encryption_data(t_encryption *encryption) {
    if (!encryption) {
        return (display_error("Invalid encryption data"));
    }
    ft_memset(encryption, 0, sizeof(t_encryption));

    /* Generate encryption key */
    if (ARG_IS_KEY() == false) {
        ENCRYPTION.key = generate_encryption_key();
        if (ENCRYPTION.key == 0) {
            return (display_error("generate_encryption_key"));
        }
    } else {
        ENCRYPTION.key = (uint64_t)ARGS.key;
    }
    display_verbose("Encryption key: " COLOR_RED "0x%x" COLOR_RESET "\n", ENCRYPTION.key);

    /* Setup encryption data */
    encryption->original_entry = ELF_HEADER->e_entry;
    encryption->encrypted_code = ELF_TEXT_SECTION()->sh_addr;
    encryption->encrypted_size = ELF_TEXT_SECTION()->sh_size;
    Elf64_Ehdr *tmp = (Elf64_Ehdr *)ELF_PTR;

    Elf64_Phdr *code_segment = ELF_CODE_SEGMENT();
    tmp->e_entry = code_segment->p_vaddr + code_segment->p_memsz;
    encryption->encrypted_entry = tmp->e_entry;

    return (0);
}

uint64_t generate_encryption_key(void) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        display_error("Failed to open /dev/urandom");
        close(fd);
        return 0;
    }
    uint64_t key;
    if (!read(fd, &key, KEY_LEN)) {
        display_error("Failed to read /dev/urandom");
        close(fd);
        return 0;
    }
    close(fd);
    return (key);
}

int encrypt_text_segment(t_file file, const uint64_t key) {

    t_elf_64 *meta_data = file.elf;

    if (!meta_data || !ELF_TEXT_SECTION()) {
        return display_error("Invalid meta data or text segment header");
    }

    void *tmp = ELF_PTR + ELF_TEXT_SECTION()->sh_offset;
    size_t text_section_size = ELF_TEXT_SECTION()->sh_size;

    if (ARG_IS_VERBOSE()) {
        printf(COLOR_RESET BOLD DIM "- Text Segment:\n");
        for (size_t i = 0; i < text_section_size && i < 16; ++i) {
            printf(COLOR_WHITE "0x%02x ", *(unsigned char *)(tmp + i));
        }
        printf("...\n\n");
    }

    encrypt_text_section(text_section_size, tmp, key);
    if (ARG_IS_VERBOSE()) {
        printf(COLOR_RESET BOLD DIM "- Text Segment "COLOR_RESET BOLD COLOR_RED"encrypted"COLOR_RESET":\n");
        for (size_t i = 0; i < text_section_size && i < 16; ++i) {
            printf(COLOR_WHITE "0x%02x ", *(unsigned char *)(tmp + i));
        }
        printf("...\n\n");
    }

    display_pass("Encryption");
    return 0;
}