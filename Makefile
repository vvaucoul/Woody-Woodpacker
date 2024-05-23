# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/30 14:25:58 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/23 17:47:46 by mle-faou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

-include colors.mk

NAME			=	woody_woodpacker
LIBFT			=	libft.a

INCLUDES_DIR	=	includes

CC				=	gcc
CFLAGS			= 	-g3 -MMD -MP -I $(INCLUDES_DIR) -Wall -Wextra  -Werror

AS				=	nasm
ASFLAGS			=	-f elf64

SRCS_DIR		=	srcs
SRCS			=	$(shell find $(SRCS_DIR) -name '*.c' ! -path '$(OP_CODES_DIR)/*') \
					$(SRCS_DIR)/opcodes/woody_opcodes.c

SRCS_ASM_DIR	=	$(SRCS_DIR)/asm
SRCS_ASM		=	$(SRCS_ASM_DIR)/asm_encryption.s \
					$(SRCS_ASM_DIR)/asm_decryption.s

OBJS_DIR		=	objs

DEPS_DIR		=	deps
OBJS			=   $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))
DEPS			=	$(patsubst $(SRCS_DIR)/%.c,$(DEPS_DIR)/%.d,$(SRCS))

OBJS_ASM		=	$(patsubst $(SRCS_ASM_DIR)/%.s,$(OBJS_DIR)/%.o,$(SRCS_ASM))
DEPS_ASM		=	$(patsubst $(SRCS_ASM_DIR)/%.s,$(DEPS_DIR)/%.d,$(SRCS_ASM))

OP_CODES_DIR	=	$(SRCS_DIR)/opcodes
OP_CODES		=	$(OP_CODES_DIR)/woody_opcodes.c

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(@D)
	@mkdir -p $(DEPS_DIR)/$(*D)
	@$(CC) -c $(CFLAGS) -MF $(DEPS_DIR)/$*.d -MT $@ $< -o $@
	@printf "$(BCYAN)[WOODY-WOODPACKER]$(BGREEN) $(LIGHT_GREEN)Compiling $(END)$(LIGHT_YELLOW)$<$(END)$(LIGHT_GREEN) to $(END)$(LIGHT_YELLOW)$@$(END)\n"

$(OBJS_DIR)/%.o: $(SRCS_ASM_DIR)/%.s
	@mkdir -p $(@D)
	@mkdir -p $(DEPS_DIR)/$(*D)
	@$(AS) $(ASFLAGS) $< -o $@
	@printf "$(BCYAN)[WOODY-WOODPACKER]$(BGREEN) $(LIGHT_GREEN)Compiling $(END)$(LIGHT_PURPLE)$<$(END)$(LIGHT_GREEN) to $(END)$(LIGHT_PURPLE)$@$(END)\n"

all: $(LIBFT) $(NAME)

$(NAME): ascii $(OP_CODES) $(OBJS) $(OBJS_ASM)
	@printf "\n$(LIGHT_GREEN)Building $(END)$(BOLD)$(NAME)$(END)\n"
	@$(CC) -o $@ $(OBJS) $(OBJS_ASM) $(LIBFT)
	@printf "$(LIGHT_GREEN)Finished building $(END)$(BOLD)$(NAME)$(END)\n"

-include $(DEPS)
-include $(DEPS_ASM)

$(LIBFT):
	@make -C ./libft -s
	@cp ./libft/42_PCC_LIBFT.a ./libft.a

clean:
	@make -C ./libft clean -s
	@make -C ./scripts/woody_opcodes clean -s
	@rm -f $(OBJS)
	@rm -f $(DEPS)
	@rm -rf $(OBJS_DIR)
	@rm -rf $(DEPS_DIR)
	@rm -f $(OP_CODES)
	@rm -rf $(OP_CODES_DIR)

fclean: clean
	@make -C ./libft fclean -s
	@make -C ./scripts/woody_opcodes fclean -s
	@rm -f $(NAME)
	@rm -f $(LIBFT)
	@rm -f woody

re: fclean all $(LIBFT)

fre: clean all

safe-clean:
	@find . -name "*.o" -exec rm -f {} \;
	@printf "$(LIGHT_GREEN)All .o files are removed$(END)\n"

	@find . -name "*.d" -exec rm -f {} \;
	@printf "$(LIGHT_GREEN)All .d files are removed$(END)\n"

get-op-codes: $(OP_CODES)
	@make -C ./scripts/woody_opcodes -s
	@printf "\n$(LIGHT_GREEN)---------------------------------------------------------------$(END)\n"
	@printf "$(LIGHT_GREEN)Woody opcodes: $(END)\n"
	@xxd -l 256 -g 1  -c 16 ./scripts/woody_opcodes/woody
	@printf "$(LIGHT_GREEN)---------------------------------------------------------------$(END)\n"

$(OP_CODES):
	@make re -C ./scripts/woody_opcodes -s

ascii:
	@printf "$(LIGHT_GREEN)░█░░▒█░▄▀▄░▄▀▄░█▀▄░▀▄▀░▒░░█░░▒█░▄▀▄░▄▀▄░█▀▄▒█▀▄▒▄▀▄░▄▀▀░█▄▀▒██▀▒█▀▄\n$(END)"
	@printf "$(LIGHT_GREEN)░▀▄▀▄▀░▀▄▀░▀▄▀▒█▄▀░▒█▒░▀▀░▀▄▀▄▀░▀▄▀░▀▄▀▒█▄▀░█▀▒░█▀█░▀▄▄░█▒█░█▄▄░█▀▄\n$(END)"

run: all
	@./woody_woodpacker tests/helloWorld && chmod 744 woody && ./woody

test: all
	@python3 tests/test_woody.py

.PHONY: all clean fclean re fre safe-clean get-op-codes ascii run test