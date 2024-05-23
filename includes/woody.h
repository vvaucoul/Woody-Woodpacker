/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 14:23:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/23 16:33:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
#define WOODY_H

#include <elf.h>
#include <fcntl.h>
#include <linux/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "../libft/includes/libft.h"

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     COLORS                                     ||
// ! ||--------------------------------------------------------------------------------||

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"
#define COLOR_RESET "\x1b[0m"

#define BOLD "\x1b[1m"
#define UNDERLINE "\x1b[4m"
#define REVERSE "\x1b[7m"

#define DIM "\x1b[2m"
#define BLINK "\x1b[5m"
#define HIDDEN "\x1b[8m"
#define STRIKE "\x1b[9m"

#define RESET_COLOR "\x1b[0m"

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   STRUCTURES                                   ||
// ! ||--------------------------------------------------------------------------------||

#define ELF_FILE_NAME "woody"
#define ELF_TEMP_FILE_NAME ".woody_tmp"

#define KEY_LEN 8 // 64 bits (8 bytes -> uint64_t)

#define BUFFER_SIZE 4096

typedef struct s_elf_64 {
    /* ELF File */
    void *ptr; // ptr to the file

    /* ELF Headers */
    Elf64_Ehdr *ehdr; // ELF headers
    Elf64_Phdr *phdr; // ELF Segment headers
    Elf64_Shdr *shdr; // ELF Section headers

#define ELF_PTR W->file.elf->ptr
#define ELF_HEADER W->file.elf->ehdr
#define ELF_SEGMENT W->file.elf->phdr
#define ELF_SECTION W->file.elf->shdr

    /* ELF Sections */
    char *strtab; // String table

#define ELF_GET_SECTION_TEXT() elf_get_section(W, ELF_SECTION_TEXT, SHT_PROGBITS, SHF_EXECINSTR)
#define ELF_GET_SECTION_FINI() elf_get_section(W, ELF_SECTION_FINI, SHT_PROGBITS, SHF_EXECINSTR)

#define ELF_GET_SEGMENT_CODE() elf_get_segment(W, PT_LOAD)
    /* ELF Symbols */
    Elf64_Sym *symtab; // Symbol table
    size_t sym_count;  // Number of symbols

#define ELF_CODE_SEGMENT() W->file.elf->code_segment
#define ELF_TEXT_SECTION() W->file.elf->text_section

    Elf64_Phdr *code_segment; // Code segment
    Elf64_Shdr *text_section; // Text section

} t_elf_64;

typedef struct s_file {
    /* File Info */
    char *name;
    int fd;
    int size;

    /* ELF */
    t_elf_64 *elf;
} t_file;

/**
 * Encryption
 */
#define ROR(val, shift) ((val >> shift) | (val << (sizeof(val) * 8 - shift)))
#define ROL(x, n) (((x) << (n)) | ((x) >> (sizeof(x) * 8 - (n))))

#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)
#define INV_R(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))

#define ROUNDS 32

/**
 * Order is important !
 * To be compliant with woody stub ASM code
 */
typedef struct s_encryption {
    /* Entrypoints */
    uint64_t original_entry;
    uint64_t encrypted_code;

    uint64_t encrypted_size;

    /* Key */
    uint64_t key;
    uint64_t encrypted_entry;

    /* Code (text segment) */

} t_encryption;

/**
 * Arguments
 * -h / --help: Display help
 * -v / --verbose: Display verbose
 * -k / --key: Encryption key
 */
typedef struct s_parser {
    uint32_t args : 8; // Arguments
    int64_t key;       // Encryption key
} __attribute__((packed)) t_parser;

#define ARGS W->args

#define ARG_HELP 1 << 0
#define ARG_VERBOSE 1 << 1
#define ARG_KEY 1 << 2
#define ARG_FILE 1 << 3

#define ARG_IS_HELP() (ARGS.args & ARG_HELP)
#define ARG_IS_VERBOSE() (ARGS.args & ARG_VERBOSE)
#define ARG_IS_KEY() (ARGS.args & ARG_KEY)
#define ARG_IS_FILE() (ARGS.args & ARG_FILE)

#define ARG_SET_HELP() (ARGS.args |= ARG_HELP)
#define ARG_SET_VERBOSE() (ARGS.args |= ARG_VERBOSE)
#define ARG_SET_KEY() (ARGS.args |= ARG_KEY)

#define ARG_UNSET_HELP() (ARGS.args &= ~ARG_HELP)
#define ARG_UNSET_VERBOSE() (ARGS.args &= ~ARG_VERBOSE)
#define ARG_UNSET_KEY() (ARGS.args &= ~ARG_KEY)

#define ARG_SET_ALL (ARGS.args |= ARG_HELP | ARG_VERBOSE | ARG_KEY)

typedef struct s_woody {
    /* Parser */
    t_parser args;

    /* File */
    t_file file;

    /* Encryption */
    t_encryption encryption;
#define ENCRYPTION W->encryption
} woody_t;

#define File t_file;

#define ELF_SECTION_TEXT ((const unsigned char *)".text")
#define ELF_SECTION_FINI ((const unsigned char *)".fini")
#define ELF_SECTION_DATA ((const unsigned char *)".data")
#define ELF_SECTION_BSS ((const unsigned char *)".bss")
#define ELF_SECTION_SYMTAB ((const unsigned char *)".symtab")

#define IS_SECTION_INDEX_VALID(ehdr, index) ((index) < (ehdr)->e_shnum)
#define IS_FILE_SIZE_LESS_THAN_HEADERS(file) ((size_t)file->size < (size_t)file->elf->ehdr->e_shoff + (file->elf->ehdr->e_shnum * file->elf->ehdr->e_shentsize))

#define SHDR_NAME(meta_data, i) (char *)(meta_data->strtab + meta_data->shdr[i].sh_name)
#define SYM_NAME(meta_data, i) (char *)(meta_data->start + meta_data->sym[i].st_name)

// ! ||--------------------------------------------------------------------------------||
// ! ||                                CORE - FUNCTIONS                                ||
// ! ||--------------------------------------------------------------------------------||

extern int read_elf_binary(t_file *file);
extern int setup_x64_meta_data(t_file *file);
extern uint64_t generate_encryption_key(void);

extern int setup_encryption_data(t_encryption *encryption);
extern int encrypt_text_segment(t_file file, const uint64_t key);

extern int elf_write_new_file(void);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     PARSER                                     ||
// ! ||--------------------------------------------------------------------------------||

extern int parse_arguments(int argc, char **argv);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   ELF - UTILS                                  ||
// ! ||--------------------------------------------------------------------------------||

extern void cleanup_elf(t_file *file);
extern Elf64_Phdr *elf_get_segment(woody_t *woody, uint8_t segment);
extern Elf64_Shdr *elf_get_section(woody_t *woody, const unsigned char *section, uint8_t sh_type, Elf64_Xword flags);
extern void elf_change_load_segment(t_encryption *encryption);
extern int elf_check_space(void);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   ENCRYPTION                                   ||
// ! ||--------------------------------------------------------------------------------||

extern void encrypt_text_section(size_t size, void *data, uint64_t key);
extern void decrypt_text_section(size_t size, uint64_t *data, uint64_t key);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     OP CODE                                    ||
// ! ||--------------------------------------------------------------------------------||

extern unsigned char woody[];
extern unsigned int woody_len;

extern unsigned char *get_op_code();
extern uint32_t get_op_code_size();

// ! ||--------------------------------------------------------------------------------||
// ! ||                                      UTILS                                     ||
// ! ||--------------------------------------------------------------------------------||

extern woody_t *get_woody(void);
#define W get_woody()

extern int display_help(const char *binary);
extern int display_perror(const char *str);
extern int display_error(const char *str);
extern int display_warning(const char *str);

#define display_pass(str) (__display_pass(__FILE__, __func__, str))
extern int __display_pass(const char *file, const char *fun, const char *header);

#define display_verbose(str, ...) (__display_verbose(__FILE__, __func__, str, ##__VA_ARGS__))
extern int __display_verbose(const char *file, const char *fun, const char *str, ...);
#define display_verbose_title(title) (__display_verbose_title(title))
extern int __display_verbose_title(const char *title);

extern void display_sections(void);

#endif /* !WOODY_H */