PORT=/dev/ttyACM0

all:
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean

wifi:
	platformio run -e esp
	platformio run -e program-esp -t upload
	sleep 1
	../esptool/esptool.py --port $(PORT) write_flash 0x0 .pioenvs/esp/firmware.bin

runlogs:
	platformio run -e host
	sleep 1
	miniterm.py --rts 0 $(PORT) 115200

logs:
	miniterm.py --rts 0 $(PORT) 115200
