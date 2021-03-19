#!/usr/bin/python3
import serial

BUSPIRATE_PORT = '/dev/ttyUSB0' #customize this! Find it in device manager.

def send_to_pirate(port, msg):
    assert(port.isOpen())
    port.write(bytes(str(msg + '\n'), 'ascii'))
    for line in port.readlines():
        print(line.decode('utf-8').rstrip())
    print('\n')


if __name__ == "__main__":
    ser = serial.Serial(BUSPIRATE_PORT, 115200, timeout = 1)
    assert(ser.isOpen())
    
    send_to_pirate(ser, '?')

