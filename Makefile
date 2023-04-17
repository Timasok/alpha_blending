CC 		:= g++
O_FLAGS := -O3

OBJ_DIR := ./obj/
LOGS_DIR:= ./logs/	
SRC_DIR	:= ./
I_FLAG 	:= -I/usr/include/
SFML_FLAGS := -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
CFLAGS     := -Wno-format -Wno-unused-result -g -fsanitize=address -mavx
CFLAGS_NO_SANITIZER := -Wno-format -g

DRAW_MODULE := $(OBJ_DIR)alpha.o
ALPHA_EXE   := alpha

SRC    := $(wildcard $(SRC_DIR)*.cpp)
OBJ    := $(patsubst $(SRC_DIR)%.cpp,  $(OBJ_DIR)%.o, $(SRC))

all : $(ALPHA_EXE)

run: $(ALPHA_EXE)
	@./$(ALPHA_EXE)

$(ALPHA_EXE): $(OBJ)
	@$(CC) $(CFLAGS) $(O_FLAGS) $(SFML_FLAGS) $(OBJ) -o $(ALPHA_EXE)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@$(CC) $(CFLAGS) $(O_FLAGS) $(I_FLAG) -c $< -o $@

mkdir :
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LOGS_DIR)

clean :
	@rm $(ALPHA_EXE) $(OBJ_DIR)*.o 