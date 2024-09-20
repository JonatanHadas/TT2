CC = g++
DEL = rm -f

SYS = $(shell uname)

DBG_FLAGS = -g

ifeq ($(SYS), Linux)
	CMP_FLAGS = -I"/usr/include/SDL2" -std=c++17 $(DBG_FLAGS)
	LNK_FLAGS = -lSDL2main -lSDL2 -lSDL2_image
	EXEC_EXT = 
else
ifeq ($(findstring MINGW32, $(SYS)), MINGW32)
	CMP_FLAGS = -I"C:\MinGW\include\SDL2" -std=c++17 $(DBG_FLAGS)
	LNK_FLAGS = -L"C:\MinGW\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lwinmm
	EXEC_EXT = .exe
else
	$(info Unsupported system $(SYS))
endif
endif

## Utils

HEADS_utils/utils := utils/utils
HEADS_utils/numbers := utils/numbers	
HEADS_utils/serialization := utils/serialization

## Game objects

# Data

HEADS_game/data/game_objects := game/data/game_objects utils/serialization utils/numbers
HEADS_game/data/game_settings := game/data/game_settings utils/serialization

# Logic

HEADS_game/logic/geometry := game/logic/geometry utils/numbers
HEADS_game/logic/logic := game/logic/logic game/logic/geometry game/data/game_objects utils/serialization
HEADS_game/logic/game := game/logic/game game/interface/game_view game/interface/game_advancer game/interface/player_interface game/data/game_objects utils/serialization utils/numbers game/logic/maze game/logic/logic
HEADS_game/logic/maze := game/logic/maze game/data/game_objects utils/numbers utils/utils

## GUI

HEADS_gui/gui := gui/gui gui/utils/clock

# Utils

HEADS_gui/utils/utils := gui/utils/utils
HEADS_gui/utils/colors := gui/utils/colors
HEADS_gui/utils/clock := gui/utils/clock
HEADS_gui/utils/images := gui/utils/images gui/utils/utils

# Controlls

HEAD_gui/controls/keyset := gui/controls/keyset gui/controls/controller game/data/game_objects

# Game

HEADS_gui/game/game_drawer := gui/game/game_drawer gui/utils/utils gui/utils/colors game/interface/game_view game/data/game_objects utils/numbers game/data/game_settings gui/utils/images
HEADS_gui/game/game_gui := gui/game/game_gui gui/gui gui/game/game_drawer gui/utils/utils gui/utils/colors game/interface/game_view game/interface/game_advancer game/interface/player_interface game/data/game_objects utils/numbers game/data/game_settings gui/controls/keyset gui/controls/controller

## Executables

HEADS_client_main := gui/game/game_gui gui/gui gui/game/game_drawer gui/utils/utils gui/utils/colors game/logic/game game/interface/game_view game/interface/game_advancer game/interface/player_interface game/data/game_objects utils/numbers gui/controls/keyset gui/controls/controller

CLIENT_OBJECTS := client_main gui/gui gui/game/game_gui gui/game/game_drawer gui/utils/utils gui/utils/clock gui/utils/colors gui/controls/keyset gui/utils/images
SERVER_OBJECTS := 
COMMON_OBJECTS := game/data/game_objects utils/utils game/logic/game game/logic/geometry game/logic/maze utils/numbers game/data/game_settings game/logic/logic utils/serialization

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
