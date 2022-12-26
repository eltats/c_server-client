.PHONY: format clean fclean re

PROGRAMS = c_server c_client
SERVER_BIN = c_server
CLIENT_BIN = c_client

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

D_H = include/
SERVER_H = server.h
SERVER_H := $(addprefix $(D_H), $(SERVER_H))
CLIENT_H = client.h
CLIENT_H := $(addprefix $(D_H), $(CLIENT_H))

D_SRC = src/
SERVER_SRC = server.c utils.c
SERVER_SRC := $(addprefix $(D_SRC), $(SERVER_SRC))
CLIENT_SRC = client.c utils.c
CLIENT_SRC := $(addprefix $(D_SRC), $(CLIENT_SRC))

D_OBJ = obj/
SERVER_OBJ := $(patsubst $(D_SRC)%.c, $(D_OBJ)%.o, $(SERVER_SRC))
CLIENT_OBJ := $(patsubst $(D_SRC)%.c, $(D_OBJ)%.o, $(CLIENT_SRC))

all: $(SERVER_BIN) $(CLIENT_BIN)

$(D_OBJ):
	@mkdir -p $(D_OBJ)

$(D_OBJ)%.o: $(D_SRC)%.c
	@echo "$< => $@"
	@$(CC) $(CFLAGS) -I$(D_H) -c $< -o $@

$(SERVER_BIN): $(D_OBJ) $(SERVER_OBJ) Makefile
	@$(CC) $(CFLAGS) -o $@ $(SERVER_OBJ)
	@echo $@ compiled

$(CLIENT_BIN): $(D_OBJ) $(CLIENT_OBJ) Makefile
	@$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJ)
	@echo $@ compiled

clean:
	@rm -rf $(D_OBJ)

fclean: clean
	@rm -rf $(PROGRAMS)

re: fclean all

format:
	@clang-format -i $(SERVER_SRC) $(CLIENT_SRC) $(SERVER_H) $(CLIENT_H)
