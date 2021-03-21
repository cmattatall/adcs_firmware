#!/usr/bin/python3

# BUS PIRATE SCRIPT TO AUTOMATE UART TRANSTIONS (WE PRETEND TO BE OBC)
# PINOUT: 
#     BUS PIRATE MISO -> MSP430 PIN 3.3
#     BUS PIRATE MOSI -> MSP430 PIN 3.4
#     BUS PIRATE GROUND -> MSP430 GROUND

import serial
import sys
import json
import signal
import time

def signal_handler_SIGINT(sig, frame):
    print("[SIGNAL] You pressed CTRL + C (SIGINT)")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler_SIGINT)


class ObcBusPirate():

    def __init__(self, chardev = "/dev/ttyUSB1", usb_baud = 115200):
        pass
        self.min_delay = 0.1
        self.port = serial.Serial(chardev, baudrate = usb_baud, timeout = 0.1)
        assert(self.port.isOpen())

    def __del__(self):
        pass
        self.__flush()
        if self.port.isOpen():
            self.port.close()
            time.sleep(self.min_delay)
            assert(self.port.isOpen() == False)

    def __flush(self):
        pass
        if self.port is not None:
            if(self.port.isOpen()):
                self.port.flushInput()
                time.sleep(self.min_delay)
                self.port.flushOutput()
                time.sleep(self.min_delay)

    def __write(self, msg):
        pass
        self.port.write(bytes(str(msg) + '\n', 'ascii'))
        time.sleep(self.min_delay)
        
    def write(self, msg):
        pass
        assert(self.port.isOpen())
        self.__write(msg)

    def read(self):
        pass
        assert(self.port.isOpen())
        decoded_lines = list()
        for line in self.port.readlines():
            decoded_lines.append(line.decode('ascii'))
        return decoded_lines

    def reset(self):
        pass
        self.write('#')




class ObcBusPirateUart(ObcBusPirate):

    obc_adcs_uart_message_delim = '!'

    def __init__(self, baudrate = 9600):
        pass
        super().__init__()
        super().reset()
        super().write('m') # enter mode select
        super().write('3') # select uart mode
        super().write('5') # 9600 baudrate
        super().write('1') # parity
        super().write('1') # 1 stop bit
        super().write('1') # "set 1 == idle level"
        super().write('2') # output mode level == normal
        super().write('o') # configure output mode
        super().write('4') # output serialized usb data as ascii representation
    
    def __del__(self):
        pass
        self.stop_stream()
        super().__del__()

    def __transmit_string(self, string):
        pass
        delimited_string = str(string)
        if adcs_uart_message_delim not in delimited_string:
            delimited_string = delimited_string + adcs_uart_message_delim
        payload_string = "\"" + delimited_string + "\""
        self.write(payload_string)


    def __transmit_int(self, int):
        pass
        raise NotImplementedError


    def transmit(self, data):
        pass
        data_type = type(data)
        if data_type == "string":
            self.__transmit_string(data)
        elif data_type == "int":
            self.__transmit_int(data)
        else:
            raise NotImplementedError
        self.receive()

    def write(self, msg):
        pass
        super().write(msg)
        self.read()


    def read(self):
        pass
        decoded_lines = super().read()
        for line in decoded_lines:
            print(line)


    def start_stream(self):
        pass
        self.write("{")


    def stop_stream(self):
        pass
        self.write("}")


    def receive(self):
        pass
        bus_pirate_usb_receive_prefix = "READ: "
        usb_lines = super().read()
        if usb_lines != []:
            received_bytes = ""
            for line in usb_lines:
                prefix = "READ: "
                if prefix in line:
                    tmp = line
                    tmp = tmp.replace(prefix, '')
                    byte = tmp[0]
                    received_bytes = received_bytes + byte
            print("[UART RX] : " + received_bytes)


if __name__ == "__main__":
    uart = ObcBusPirateUart()
    uart.start_stream()
    
    # CTRL + C to cancel (SIGINT)
    while True:
        uart.receive()
        time.sleep(0.1)
        pass


