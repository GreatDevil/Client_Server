SERVER =		server
CLIENT =		client
NAME =			$(SERVER) $(CLIENT)

COMP =			gcc -I include -c -o

OBJ_DIR =		obj/
S_SRC_DIR =		shared/
P_SRC_DIR =		server_dir/
C_SRC_DIR =		client_dir/

S_SRC =			functions.c

P_SRC =			server.c

C_SRC =			client.c

S_OBJ =			$(S_SRC:%.c=%.o)
P_OBJ =			$(P_SRC:%.c=%.o)
C_OBJ =			$(C_SRC:%.c=%.o)

OBJ =			$(S_OBJ) $(P_OBJ) $(C_OBJ)

S_SRC_PATH =   	$(S_SRC:%=$(S_SRC_DIR)%)
P_SRC_PATH =   	$(P_SRC:%=$(P_SRC_DIR)%)
C_SRC_PATH =   	$(C_SRC:%=$(C_SRC_DIR)%)
SRC_PATH =		$(S_SRC_PATH) $(P_SRC_PATH) $(C_SRC_PATH)

S_OBJ_PATH =	$(addprefix $(OBJ_DIR), $(S_OBJ))
P_OBJ_PATH =	$(addprefix $(OBJ_DIR), $(P_OBJ))
C_OBJ_PATH =	$(addprefix $(OBJ_DIR), $(C_OBJ))
OBJ_PATH =		$(S_OBJ_PATH) $(P_OBJ_PATH) $(C_OBJ_PATH)

all:			$(OBJ_DIR) $(NAME)
				@echo "\\n"DONE"\\n"

$(OBJ_DIR):
				@mkdir -p $(OBJ_DIR)
				@echo Create: Object directory

$(NAME):		$(OBJ_PATH)
				@gcc $(S_OBJ_PATH) $(P_OBJ_PATH) -o server -I include -pthread
				@gcc $(S_OBJ_PATH) $(C_OBJ_PATH) -o client -I include

$(S_OBJ_PATH):	$(S_SRC_PATH)
				@$(MAKE) $(S_OBJ)

$(P_OBJ_PATH):	$(P_SRC_PATH)
				@$(MAKE) $(P_OBJ)

$(C_OBJ_PATH):	$(C_SRC_PATH)
				@$(MAKE) $(C_OBJ)

$(S_OBJ):

				@$(COMP) $(OBJ_DIR)$@ $(S_SRC_DIR)$(@:%.o=%.c)
$(C_OBJ):

				@$(COMP) $(OBJ_DIR)$@ $(C_SRC_DIR)$(@:%.o=%.c)
$(P_OBJ):

				@$(COMP) $(OBJ_DIR)$@ $(P_SRC_DIR)$(@:%.o=%.c)

clean:
				@/bin/rm -rf $(OBJ_DIR)

fclean:			clean
				@/bin/rm -f $(SERVER) $(CLIENT)
				@echo Cleaned $(NAME)

re: 			fclean all

.PHONY:			all clean flcean re
