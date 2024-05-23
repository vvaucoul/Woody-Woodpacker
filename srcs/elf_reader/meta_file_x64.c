/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meta_file_x64.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 16:37:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 16:35:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

static int get_binary_instructions() {
    ELF_CODE_SEGMENT() = ELF_GET_SEGMENT_CODE();
    if (!ELF_CODE_SEGMENT()) {
        return display_error("get_binary_instructions: ELF_CODE_SEGMENT is NULL");
    } else {
        display_verbose("Code Segment " COLOR_GREEN "[✓] " COLOR_RESET "\n");
    }

    ELF_TEXT_SECTION() = ELF_GET_SECTION_TEXT();
    if (!ELF_TEXT_SECTION()) {
        return display_error("get_binary_instructions: ELF_TEXT_SECTION is NULL");
    } else {
        display_verbose("Text Section " COLOR_GREEN "[✓] " COLOR_RESET "\n");
    }
    return (0);
}

static int get_segments(t_elf_64 *meta_data) {
    /* Get .ehdr: Elf64_Ehdr (Elf Header) */
    if (!meta_data || !meta_data->ptr) {
        return display_error("get_segments: meta_data or meta_data->ptr is NULL");
    } else {
        meta_data->ehdr = (Elf64_Ehdr *)meta_data->ptr;
        display_verbose("e_shoff " COLOR_WHITE "%lu " COLOR_RESET "\n", meta_data->ehdr->e_shoff);
    }

    /* Get .phdr: Elf64_Phdr (Program Header) */
    if (meta_data->ehdr->e_shoff == 0) {
        return display_error("get_segments: e_shoff is NULL");
    } else {
        meta_data->shdr = (Elf64_Shdr *)(meta_data->ptr + meta_data->ehdr->e_shoff);
        display_verbose("e_shnum " COLOR_WHITE "%d " COLOR_RESET "\n", meta_data->ehdr->e_shnum);
    }

    /* Get .shstrtab: char * (Section Header String Table) */
    if (IS_SECTION_INDEX_VALID(meta_data->ehdr, meta_data->ehdr->e_shstrndx) == false) {
        return display_error("get_segments: e_shstrndx is invalid");
    } else {
        meta_data->strtab = (char *)(meta_data->ptr + meta_data->shdr[meta_data->ehdr->e_shstrndx].sh_offset);
        display_verbose("e_shstrndx " COLOR_WHITE "%d " COLOR_RESET "\n", meta_data->ehdr->e_shstrndx);
    }

    /* Get .phdr: Elf64_Phdr (Program Header) */
    if (meta_data->ehdr->e_phoff == 0) {
        return display_error("get_segments: e_phoff is NULL");
    } else {
        meta_data->phdr = (Elf64_Phdr *)(meta_data->ptr + meta_data->ehdr->e_phoff);
        display_verbose("e_phnum " COLOR_WHITE "%d " COLOR_RESET "\n", meta_data->ehdr->e_phnum);
    }
    return (0);
}

static int get_shdr_values(t_elf_64 *meta_data) {
    if (!meta_data || !meta_data->ehdr || !meta_data->shdr) {
        return display_error("get_shdr_values: meta_data, meta_data->ehdr or meta_data->shdr is NULL");
    }

    /* Get .symtab && sym_count */
    Elf64_Shdr *shdr_sym = elf_get_section(W, ELF_SECTION_SYMTAB, SHT_SYMTAB, 0);

    if (shdr_sym == NULL) {
        display_warning("Invalid symbtab (process without it).");
        meta_data->symtab = NULL;
        meta_data->sym_count = 0;
        return (0);
    }

    meta_data->symtab = (Elf64_Sym *)(meta_data->ptr + shdr_sym->sh_offset);
    meta_data->sym_count = shdr_sym->sh_size / shdr_sym->sh_entsize;
    return (0);
}

/**
 * Setup meta data for x64 architecture
 * @param file
 * @return 0 on success, 1 on error
 */
int setup_x64_meta_data(t_file *file) {
    if (!file || !file->elf) {
        return display_error("setup_x64_meta_data: file or file->elf is NULL");
    }

    /* Get values && Get shdr values */
    if ((get_segments(file->elf)) != 0) {
        return display_error("Failed to get segments");
    }
    if ((get_shdr_values(file->elf)) != 0) {
        return display_error("Failed to get shdr values");
    }
    if ((get_binary_instructions()) != 0) {
        return display_error("Failed to get binary instructions");
    }
    return (0);
}