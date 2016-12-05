UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
PORT=/dev/ttyACM0
endif
ifeq ($(UNAME), Darwin)
PORT=/dev/tty.usbmodem1461
endif


all:
	platformio -f -c vim run --target upload

clean:
	platformio -f -c vim run --target clean

wifi:
	platformio run -e esp -t upload

host:
	platformio run -e host -t size

runlogs: host
	platformio run -e host -t upload
	# Wait for the serial port to be available
	sh -c "while [ ! -r $(PORT) ]; do sleep 0.01; done"
	miniterm.py --rts 0 $(PORT) 115200

logs:
	sh -c "while [ ! -r $(PORT) ]; do sleep 0.01; done"
	miniterm.py --rts 0 $(PORT) 115200

logs-n2kanalyzer:
	miniterm.py --rts 0 $(PORT) 115200 | grep 'TX: $$PCDIN' |perl -pe 's/.*TX: //' |analyzer -json

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
