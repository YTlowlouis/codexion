NAME		= codexion

CC		= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRCS_DIR	= coders/srcs
INCS_DIR	= coders/includes

SRCS		= $(SRCS_DIR)/main.c \
		  $(SRCS_DIR)/coder.c \
		  $(SRCS_DIR)/monitor.c \
		  $(SRCS_DIR)/dongle.c \
		  $(SRCS_DIR)/scheduler.c \
		  $(SRCS_DIR)/log.c \
		  $(SRCS_DIR)/utils.c

OBJS		= $(SRCS:.c=.o)

INCS		= -I$(INCS_DIR)

# ─── Colors ───────────────────────────────────────────────────────────────────

GREEN		= \033[0;32m
YELLOW		= \033[0;33m
RED			= \033[0;31m
RESET		= \033[0m

# ─── Rules ────────────────────────────────────────────────────────────────────

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully$(RESET)"

$(SRCS_DIR)/%.o: $(SRCS_DIR)/%.c
	@$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	@echo "$(YELLOW)  compiling $<...$(RESET)"

clean:
	@rm -f $(OBJS)
	@echo "$(RED)✗ objects removed$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✗ $(NAME) removed$(RESET)"

re: fclean all

.PHONY: all clean fclean re
