/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_elf_binary.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 11:03:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 18:09:19 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <woody.h>

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    ELF UTILS                                   ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Get the size of the file.
 */
static int get_file_size(int fd) {
    off_t len = lseek(fd, 0, SEEK_END);
    if (len == -1) {
        return (display_perror("lseek"));
    } else if (len < (long)(sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) + sizeof(Elf64_Shdr))) {
        return (1);
    }

    /* Reset file descriptor position */
    lseek(fd, 0, SEEK_SET);
    return (len);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   ELF CHECKER                                  ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Checks if the file is an ELF file.
 */
static bool is_elf(const Elf64_Ehdr *elf) {
    return elf &&
           elf->e_ident[EI_MAG0] == ELFMAG0 &&
           elf->e_ident[EI_MAG1] == ELFMAG1 &&
           elf->e_ident[EI_MAG2] == ELFMAG2 &&
           elf->e_ident[EI_MAG3] == ELFMAG3;
}

/**
 * Checks if the file is already infected.
 */
static bool is_infected_binary(void) {
    void *tmp = W->file.elf->ptr + W->file.elf->code_segment->p_offset + W->file.elf->code_segment->p_filesz - woody_len;

    if (ft_memcmp(tmp, woody, (uint32_t)(woody_len - sizeof(t_encryption))) == 0) {
        return (true);
    }
    return (false);
}

/**
 * Checks if the file is an ELF 64 file.
 */
static bool is_elf_64(const Elf64_Ehdr *elf) {
    return elf && elf->e_ident[EI_CLASS] == ELFCLASS64;
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  READ ELF FILE                                 ||
// ! ||--------------------------------------------------------------------------------||

/**
 * Reads an ELF binary file and returns a pointer to the ELF header.
 *
 * @param filename The name of the ELF binary file to read.
 * @return 0 on success, 1 on error.
 */
int read_elf_binary(t_file *file) {
    if (!file->name) {
        return display_error("No file name");
    }
    if ((W->file.fd = open(W->file.name, O_RDONLY)) < 0)
        return (display_perror("open"));

    file->size = get_file_size(file->fd);
    if (file->size < EI_NIDENT) {
        return display_error("File size too small to be an ELF file");
    }

    /* Allocate memory for the ELF file */
    if (!(file->elf = malloc(sizeof(t_elf_64)))) {
        cleanup_elf(file);
        return display_perror("malloc");
    }

    /* Map the ELF file into memory:
    - PROT_READ: Pages may be read.
    - PROT_WRITE: Pages may be written.
    - MAP_PRIVATE: Create a private copy-on-write mapping.
    */
    if ((file->elf->ptr = mmap(NULL, file->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, file->fd, 0)) == MAP_FAILED) {
        cleanup_elf(file);
        return display_perror("mmap");
    } else {
        ft_memset(ELF_PTR, 0, file->size);
        if (read(file->fd, ELF_PTR, file->size) != file->size) {
            cleanup_elf(file);
            return display_perror("read");
        }
    }

    /* Check if the file is an ELF file (with valid architecture) */
    if (!is_elf((Elf64_Ehdr *)file->elf->ptr) || !is_elf_64((Elf64_Ehdr *)file->elf->ptr)) {
        cleanup_elf(file);
        return display_error("Invalid ELF file");
    }

    /* Set the program table data */
    if (setup_x64_meta_data(file) != 0) {
        cleanup_elf(file);
        return display_error("Failed to setup meta data");
    }

    /* Check if the file is a valid ELF file */
    if (IS_FILE_SIZE_LESS_THAN_HEADERS(file)) {
        cleanup_elf(file);
        return display_error("Invalid ELF file");
    }

    /* Check if the file is already infected */
    if (is_infected_binary() == true) {
        cleanup_elf(file);
        return display_error("File is already infected");
    }

    display_pass("ELF Binary");
    return (0);
}