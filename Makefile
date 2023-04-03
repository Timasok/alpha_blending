OBJ_DIR := ./obj/
LOGS_DIR:= ./logs/	
SRC_DIR	:= ./
I_FLAG 	:=
CC 		:= g++
CFLAGS  := -Wno-format -g -fsanitize=address
CFLAGS_NO_SANITIZER := -Wno-format -g

ALPHA_EXE  := alpha

SRC    := $(wildcard $(SRC_DIR)*.cpp)
OBJ    := $(patsubst $(SRC_DIR)%.cpp,  $(OBJ_DIR)%.o, $(SRC))

all : $(ALPHA_EXE)

$(ALPHA_EXE): $(OBJ) 
	@$(CC) $(CFLAGS_NO_SANITIZER) $(OBJ) -o $(ALPHA_EXE)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@$(CC) $(I_FLAG) -c $< -o $@

mkdir :
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LOGS_DIR)

clean :
	@rm $(ALPHA_EXE) $(OBJ_DIR)*.o 