#!/usr/bin/python3

# BUS PIRATE SCRIPT TO AUTOMATE UART TRANSTIONS (WE PRETEND TO BE OBC)
# PINOUT: 
#     BUS PIRATE MISO -> MSP430 PIN 3.3
#     BUS PIRATE MOSI -> MSP430 PIN 3.4
#     BUS PIRATE GROUND -> MSP430 GROUND

import serial
import time

BUSPIRATE_PORT = '/dev/ttyUSB0' #customize this! Find it in device manager.

def write_to_pirate(port, msg):
    assert(port.isOpen())
    port.write(bytes(str(msg + '\n'), 'ascii'))

def read_from_pirate(port):
    assert(port.isOpen())
    string = ""
    for line in port.readlines():
        #byte = line.decode('utf-8').rstrip().replace("READ:", '')
        #print(byte)
        print(line.decode('utf-8').rstrip())

    #print(string)

def pirate_exchange(port, msg):
    read_from_pirate(port)
    time.sleep(0.1)
    write_to_pirate(port, msg)
    time.sleep(0.1)

def pirate_transmit_string(port, msg):
    string = "\"" + str(msg) + "\""
    pirate_exchange(port, string)


if __name__ == "__main__":
    ser = serial.Serial(BUSPIRATE_PORT, 115200, timeout = 1)
    assert(ser.isOpen())
    pirate_exchange(ser, '?')
    pirate_exchange(ser, 'm')
    pirate_exchange(ser, '3') # uart mode
    pirate_exchange(ser, '5') # 9600 baudrate
    pirate_exchange(ser, '1') # parity
    pirate_exchange(ser, '1') # 1 stop bit
    pirate_exchange(ser, '1') # "set 1 == idle level"
    pirate_exchange(ser, '2') # output mode level == normal
    pirate_exchange(ser, '{')

    while True:
        time.sleep(0.1)
        for line in ser.readlines():
            #byte = line.decode('utf-8').rstrip().replace("READ:", '')
            #print(byte)
            print(line.decode('utf-8').rstrip())