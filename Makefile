CC = g++
DEL = rm -f

SYS = $(shell uname)

DBG_FLAGS = -g

ifeq ($(SYS), Linux)
	CMP_FLAGS = -I"/usr/include/SDL2" $(DBG_FLAGS)
	LNK_FLAGS = -lSDL2main -lSDL2
	EXEC_EXT = 
else
ifeq ($(findstring MINGW32, $(SYS)), MINGW32)
	CMP_FLAGS = -I"C:\MinGW\include\SDL2" $(DBG_FLAGS)
	LNK_FLAGS = -L"C:\MinGW\lib" -lmingw32 -lSDL2main -lSDL2 -lwinmm
	EXEC_EXT = .exe
else
	$(info Unsupported system $(SYS))
endif
endif

## Utils

HEADS_utils/utils := utils/utils

## Game objects

# Data

HEADS_game/data/game_objects := game/data/game_objects utils/serialization

# Logic

HEADS_game/logic/game := game/logic/game game/interface/game_view game/data/game_objects utils/serialization game/logic/maze
HEADS_game/logic/maze := game/logic/maze game/data/game_objects utils/utils

## GUI

HEADS_gui/gui := gui/gui gui/utils/clock

# Utils

HEADS_gui/utils/utils := gui/utils/utils
HEADS_gui/utils/colors := gui/utils/colors
HEADS_gui/utils/clock := gui/utils/clock

# Game

HEADS_gui/game/game_drawer := gui/game/game_drawer gui/utils/utils gui/utils/colors game/interface/game_view game/data/game_objects
HEADS_gui/game/game_gui := gui/game/game_gui gui/gui gui/game/game_drawer gui/utils/utils gui/utils/colors game/interface/game_view game/data/game_objects

## Executables

HEADS_client_main := gui/game/game_gui gui/gui gui/game/game_drawer gui/utils/utils gui/utils/colors game/logic/game game/interface/game_view game/data/game_objects

CLIENT_OBJECTS := client_main gui/gui gui/game/game_gui gui/game/game_drawer gui/utils/utils gui/utils/clock gui/utils/colors
SERVER_OBJECTS := 
COMMON_OBJECTS := game/data/game_objects utils/utils game/logic/game game/logic/maze

CLIENT_EXEC := tank_trouble
SERVER_EXEC := server

OBJECTS_$(CLIENT_EXEC) := $(COMMON_OBJECTS) $(CLIENT_OBJECTS)

OBJECTS_$(SERVER_EXEC) := $(COMMON_OBJECTS) $(SERVER_OBJECTS)

# Rules
OBJECTS = $(COMMON_OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS)

OBJECTS := $(addprefix build/,$(addsuffix .o,$(OBJECTS)))
SERVER_EXEC := $(addprefix build/,$(addsuffix $(EXEC_EXT),$(SERVER_EXEC)))
CLIENT_EXEC := $(addprefix build/,$(addsuffix $(EXEC_EXT),$(CLIENT_EXEC)))
EXECUTABLES := $(CLIENT_EXEC) $(SERVER_EXEC)

all: client server

.PHONY: client server

client: $(CLIENT_EXEC)

server: $(SERVER_EXEC)

clear:
	$(DEL) $(OBJECTS)

clear_all: clear
	$(DEL) $(EXECUTABLES)

.SECONDEXPANSION:
$(EXECUTABLES): build/%$(EXEC_EXT): $$(addprefix build/,$$(addsuffix .o,$$(OBJECTS_$$*)))
	mkdir -p $(dir $@)
	$(CC) $(CMP_FLAGS) $^ -o $@ $(LNK_FLAGS)
	
$(OBJECTS): build/%.o: src/%.cpp $$(addprefix src/,$$(addsuffix .h,$$(HEADS_$$*)))
	mkdir -p $(dir $@)
	$(CC) $(CMP_FLAGS) -c $< -o $@
