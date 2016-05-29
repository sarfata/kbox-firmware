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

host:
	platformio run -e host -t size

runlogs: host
	platformio run -e host -t upload
	sleep 1
	miniterm.py --rts 0 $(PORT) 115200

logs:
	miniterm.py --rts 0 $(PORT) 115200

mfg:
	platformio run -e mfg

runmfg:
	platformio run -e mfg -t upload
	# Wait for the serial port to be available
	sh -c "while [ ! -r $(PORT) ]; do sleep 0.01; done"
	miniterm.py $(PORT) 115200

test:
	cd lib/List/test && make test
	cd lib/KBox/test && make test
