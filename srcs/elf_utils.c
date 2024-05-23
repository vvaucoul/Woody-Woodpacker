/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 19:34:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 16:37:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

void cleanup_elf(t_file *file) {
    if (file->elf) {
        if (file->elf->ptr) {
            munmap(file->elf->ptr, file->size);
        }
        free(file->elf);
    }
}

__attribute__((unused)) void display_sections(void) {

    t_file *file = &W->file;

    Elf64_Ehdr *header = file->elf->ehdr;
    Elf64_Shdr *section_header = (Elf64_Shdr *)(file->elf->ptr + header->e_shoff);
    Elf64_Shdr *section_string_table = &section_header[header->e_shstrndx];
    const char *section_string_table_data = (const char *)(file->elf->ptr + section_string_table->sh_offset);

    printf("Section Headers:\n");
    printf("  [Nr] Name                      Addr             Offset           Size             Type\n");

    for (int i = 0; i < header->e_shnum; i++) {
        printf("  [%2d] %-18s 0x%016lx 0x%016lx 0x%016lx 0x%08x\n", i, &section_string_table_data[section_header[i].sh_name],
               section_header[i].sh_addr, section_header[i].sh_offset, section_header[i].sh_size, section_header[i].sh_type);
    }
}

Elf64_Phdr *elf_get_segment(woody_t *woody, uint8_t segment) {
    if (woody->file.elf->ehdr->e_phoff == 0 || woody->file.elf->ehdr->e_phnum == 0) {
        display_error("get_elf_segment: e_phoff or e_phnum is NULL");
        return (NULL);
    } else {
        for (uint32_t i = 0; i < ELF_HEADER->e_phnum; i++) {
            if ((woody->file.elf->phdr[i].p_type == segment) &&
                (woody->file.elf->phdr[i].p_vaddr <= woody->file.elf->ehdr->e_entry) &&
                (woody->file.elf->phdr[i].p_vaddr + woody->file.elf->phdr[i].p_memsz > woody->file.elf->ehdr->e_entry) &&
                (woody->file.elf->phdr[i].p_vaddr == woody->file.elf->phdr[i].p_paddr) &&
                (woody->file.elf->phdr[i].p_memsz == woody->file.elf->phdr[i].p_filesz)) {
                return (&woody->file.elf->phdr[i]);
            }
        }
        return (NULL);
    }
}

Elf64_Shdr *elf_get_section(__attribute__((unused)) woody_t *woody, const unsigned char *section, uint8_t sh_type, Elf64_Xword flags) {
    if (!ELF_HEADER->e_shoff || !ELF_HEADER->e_shnum ||
        (ELF_HEADER->e_shstrndx >= ELF_HEADER->e_shnum)) {
        return (NULL);
    }
    const unsigned char *sections = ELF_PTR + ELF_SECTION[ELF_HEADER->e_shstrndx].sh_offset;
    size_t section_len = ft_strlen((const char *)section);

    for (int i = 0; i < ELF_HEADER->e_shnum; ++i) {
        if (ft_strncmp((char *)section, (char *)(sections + ELF_SECTION[i].sh_name), section_len) == 0 &&
            (ELF_SECTION[i].sh_type == sh_type) &&
            (ELF_SECTION[i].sh_flags & flags) == flags &&
            (section_len == ft_strlen((const char *)section)) &&
            (ELF_SECTION[i].sh_size > 0)) {
            return (&ELF_SECTION[i]);
        }
    }
    return (NULL);
}

static int64_t elf_get_empty_space(__attribute__((unused)) woody_t *woody, Elf64_Phdr *segment, uint64_t file_size, uint8_t decryptor_length) {
    void *tmp = ELF_PTR + segment->p_offset + segment->p_filesz;
    void *pos = tmp;
    while ((pos < (ELF_PTR + file_size)) &&
           (*(unsigned char *)pos == 0)) {
        ++pos;
    }
    if ((pos - tmp) < decryptor_length) {
        display_error("elf_check_empty_space: not enough space for decryptor");
        return (-1);
    }
    return (pos - tmp);
}

int elf_check_space(void) {
    /* Get empty space size */
    int64_t empty_space_size = 0;

    if ((empty_space_size = elf_get_empty_space(W, ELF_CODE_SEGMENT(), W->file.size, get_op_code_size())) <= 0) {
        return (display_error("elf_get_empty_space"));
    } else {
        display_verbose("Empty space size: " COLOR_WHITE "[%lu]" COLOR_RESET "\n", empty_space_size);
        display_pass("elf_get_empty_space: OK");
    }
    return (0);
}

void elf_change_load_segment(t_encryption *encryption) {

    void *tmp_ptr = ELF_PTR + ELF_CODE_SEGMENT()->p_offset + ELF_CODE_SEGMENT()->p_filesz;

    /* Change load segment size */
    ELF_CODE_SEGMENT()->p_memsz += woody_len;
    ELF_CODE_SEGMENT()->p_filesz += woody_len;

    /* PF_W: Writeable segment */
    ELF_CODE_SEGMENT()->p_flags = ELF_CODE_SEGMENT()->p_flags | PF_W;
    int64_t len = woody_len - sizeof(t_encryption);
    ft_memmove(tmp_ptr, woody, len);
    ft_memmove(tmp_ptr + len, encryption, sizeof(t_encryption));

    display_verbose("Encryption size: "COLOR_WHITE"%ld\n" COLOR_RESET, sizeof(t_encryption));
    display_verbose("Text segment addr: "COLOR_WHITE"%p\n" COLOR_RESET, tmp_ptr);
    display_verbose("Text segment size: "COLOR_WHITE"%ld\n" COLOR_RESET, len);
    display_verbose("Encryption data size: "COLOR_WHITE"%ld\n" COLOR_RESET, sizeof(t_encryption));
    display_verbose("Stub size: "COLOR_WHITE"%d\n" COLOR_RESET, woody_len);
    display_verbose("Stub offset: "COLOR_WHITE"%ld\n" COLOR_RESET, woody_len - sizeof(t_encryption));

    display_pass("elf_change_load_segment");
}

ssize_t wright_data(int fd, ssize_t size, ssize_t chunk, void *storage) {
    ssize_t i = 0;
    void *tmp_ptr = storage;

    display_verbose("file addr: "COLOR_WHITE"%p\n" COLOR_RESET, tmp_ptr);
    display_verbose("file size: "COLOR_WHITE"%ld\n" COLOR_RESET, size);
    display_verbose("file chunk: "COLOR_WHITE"%ld\n" COLOR_RESET, chunk);

    while (i < size) {
        tmp_ptr = storage + i;
        if (size - i >= chunk) {
            i += write(fd, tmp_ptr, chunk);
        } else {
            i += write(fd, tmp_ptr, size % chunk);
        }
    }
    display_verbose("Data wrote: "COLOR_WHITE"%ld\n"COLOR_RESET, i);
    return i;
}

int elf_write_new_file(void) {
    int fd = open(ELF_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    display_verbose("fd: %d\n", fd);

    if (fd > 0) {    
        int64_t i = wright_data(fd, W->file.size, BUFFER_SIZE, ELF_PTR);

        close(fd);
        display_verbose("Create: "COLOR_WHITE"woody\n" COLOR_RESET);
        display_verbose("Check with original file size "COLOR_WHITE"%ld == "COLOR_WHITE"%d = "COLOR_WHITE"%d\n" COLOR_RESET, i, W->file.size, (i == W->file.size));
        display_pass("elf_write_new_file");
        return (i == W->file.size ? 0 : 1);
    } else {
        return (display_error("elf_write_new_file"));
    }
    return (0);
}