UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
    PORT := /dev/ttyACM0
endif
ifeq ($(UNAME), Darwin)
    PORT := $(shell ls /dev/tty.usbmodem14* )
endif


all: host esp mfg test sktool validation

clean:
	platformio -f -c vim run --target clean

esp:
	platformio run -e esp -t size

esp-upload:
	platformio run -e esp -t upload

esp-uploadfs:
	platformio run -e esp -t uploadfs

host:
	platformio run -e host -t size

host-upload:
	platformio run -e host -t upload

logs:
	sh -c "while [ ! -r $(PORT) ]; do sleep 0.01; done"
	miniterm.py --rts 0 $(PORT) 115200

logs-n2kanalyzer:
	miniterm.py --rts 0 $(PORT) 115200 | grep 'TX: $$PCDIN' |perl -pe 's/.*TX: //' |analyzer -json

reboot:
	tools/kbox.py --port $(PORT) reboot

mfg:
	platformio run -e mfg

runmfg:
	platformio run -e mfg -t upload
	# Wait for the serial port to be available
	sh -c "while [ ! -r $(PORT) ]; do sleep 0.01; done"
	miniterm.py $(PORT) 115200

test:
	platformio run -e test -vv
	find .pioenvs -name '*.gcda'|xargs rm -f
	.pioenvs/test/program

lcov: test
	lcov --directory .pioenvs/test/ --base-directory . --capture -o cov.info
	genhtml cov.info -o lcov-html

sktool:
	platformio run -e sktool

validation: sktool
	src/sktool/sktool-test.py --implementation sktool src/sktool/tests/*json

