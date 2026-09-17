PIO ?= $(shell which pio 2>/dev/null || echo $(HOME)/.platformio/penv/bin/pio)
CC  ?= clang

# STM32 Firmware Targets (PlatformIO)
build:
	$(PIO) run

upload:
	$(PIO) run -t upload

monitor:
	$(PIO) device monitor

# Host Terminal Simulator Targets (Run locally on Mac/Linux)
SIM_BIN = tetris_sim
SIM_SRCS = src/game/tetromino.c src/game/tetris_core.c test/terminal_runner.c
SIM_CFLAGS = -Wall -Wextra -O2 -Isrc/game

build-sim:
	$(CC) $(SIM_CFLAGS) $(SIM_SRCS) -o $(SIM_BIN)

sim: build-sim
	./$(SIM_BIN)

clean:
	$(PIO) run -t clean 2>/dev/null || true
	rm -f $(SIM_BIN)