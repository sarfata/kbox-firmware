import serial
import time
import argparse


# Returns pair of ts (float, in seconds) and NMEA sentences
def log_read(nmea_file):
    with open(nmea_file) as f:
        for line in f.readlines():
            line = line.strip(" \r\n")
            if line != "" and ":" in line:
                (ts, sentence) = line.split(':', 1)
                ts = float(ts) / 1000
                if ts is not None and sentence is not None:
                    yield (ts, sentence)

def next_generated_sentence():
    while True:
        yield '$GPGGA,211841.000,3750.1297,N,12225.5672,W,2,10,0.90,-13.1,M,-2'
        time.sleep(1)

def next_sentence(nmea_file):
    start = time.time()
    # Timestamps in file are often arbitrary (and in past) so we need to adjust them
    delta = 0
    for (ts, sentence) in log_read(nmea_file):
        if delta == 0:
            delta = ts

        # block until time is reached
        delay = ts - delta - (time.time() - start)

        if delay > 0:
            time.sleep(delay)
        yield sentence

def send_sentence(port, nmea):
    print nmea
    port.write(nmea + "\r\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', help = 'Serial port', default = '/dev/ttyUSB0')
    parser.add_argument('--baud', help = 'Baud rate', default = 4800)
    parser.add_argument('--nmea-file', help = 'Input file to read. Format is <timestamp in ms>:<NMEA Line>')

    args = parser.parse_args()

    port = serial.Serial(port = args.port, baudrate = args.baud, timeout = 1)

    if args.nmea_file:
        for nmea in next_sentence(args.nmea_file):
            send_sentence(port, nmea)
    else:
        for nmea in next_generated_sentence():
            send_sentence(port, nmea)

if __name__ == '__main__':
    main()
