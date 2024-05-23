# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/09 16:08:44 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/08 16:35:26 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

-include ../../colors.mk

AS = nasm
ASFLAGS = -f bin

NAME = woody

SRC = woody.s

all: $(NAME)

$(NAME):
	@$(AS) $(ASFLAGS) $(SRC)
	@echo "\n$(BPURPLE)[OPCODES]$(BWHITE) Binary created$(RESET)"
	@xxd -i -c8 woody > woody_opcodes.c
	@echo "$(BPURPLE)[OPCODES] $(BWHITE)OpCodes created$(RESET)"
	@mkdir -p ../../srcs/opcodes
	@mv woody_opcodes.c ../../srcs/opcodes/woody_opcodes.c
	@echo "$(BPURPLE)[OPCODES] $(BWHITE)OpCodes moved to srcs$(RESET)\n"

clean:
	@rm -f woody_opcodes.txt
	@rm -f woody_opcodes_clean.txt

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re