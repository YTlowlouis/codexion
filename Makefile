NAME		= codexion

CC			= cc
# On sépare les flags de base des flags de debug
CFLAGS		= -Wall -Wextra -Werror -pthread
DEBUG_FLAGS	= -g3 -fsanitize=address

SRCS_DIR	= coders/srcs
INCS_DIR	= coders/includes

SRCS		= $(SRCS_DIR)/main.c \
			  $(SRCS_DIR)/coder.c \
			  $(SRCS_DIR)/monitor.c \
			  $(SRCS_DIR)/dongle.c \
			  $(SRCS_DIR)/scheduler.c \
			  $(SRCS_DIR)/log.c \
			  $(SRCS_DIR)/utils.c \
			  $(SRCS_DIR)/cleanup.c

OBJS		= $(SRCS:.c=.o)
INCS		= -I$(INCS_DIR)

# ─── Colors ───────────────────────────────────────────────────────────────────

GREEN		= \033[0;32m
YELLOW		= \033[0;33m
RED			= \033[0;31m
MAGENTA		= \033[0;35m
RESET		= \033[0m

# ─── Rules ────────────────────────────────────────────────────────────────────

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully$(RESET)"

# Règle de debug : elle ajoute les flags et relance la compilation
debug: CFLAGS += $(DEBUG_FLAGS)
debug: re
	@echo "$(MAGENTA)🔧 Debug mode enabled (AddressSanitizer + g3)$(RESET)"

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

.PHONY: all clean fclean re debug
