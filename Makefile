OBJ_DIR := ./obj/
LOGS_DIR:= ./logs/	
SRC_DIR	:= ./
I_FLAG 	:= -I/usr/include/
CC 		:= g++
SFML_FLAGS := -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
CFLAGS     := -Wno-format -g -fsanitize=address
CFLAGS_NO_SANITIZER := -Wno-format -g

DRAW_MODULE := $(OBJ_DIR)alpha.o
ALPHA_EXE   := alpha

SRC    := $(wildcard $(SRC_DIR)*.cpp)
OBJ    := $(patsubst $(SRC_DIR)%.cpp,  $(OBJ_DIR)%.o, $(SRC))

all : $(ALPHA_EXE)

run: $(ALPHA_EXE)
	@./$(ALPHA_EXE)

$(ALPHA_EXE): $(OBJ)
	@$(CC) $(CFLAGS) $(SFML_FLAGS) $(OBJ) -o $(ALPHA_EXE)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@$(CC) $(CFLAGS) $(I_FLAG) -c $< -o $@

mkdir :
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LOGS_DIR)

clean :
	@rm $(ALPHA_EXE) $(OBJ_DIR)*.o 