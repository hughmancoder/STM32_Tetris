PIO ?= $(shell which pio 2>/dev/null || echo $(HOME)/.platformio/penv/bin/pio)

build:
	$(PIO) run

build-run:
	$(PIO) run -t upload

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run -t clean