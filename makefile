PIO ?= $(shell which pio 2>/dev/null || echo $(HOME)/.platformio/penv/bin/pio)

build:
	$(PIO) run

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run -t clean

upload:
	$(PIO) run -t upload