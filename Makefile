all:
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean
