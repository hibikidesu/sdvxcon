from serial import Serial
from struct import unpack, calcsize
import pyvjoy
from time import sleep
from enum import IntFlag

need_config = False


class Button(IntFlag):
    A = 1
    B = 2
    C = 4
    D = 8
    FX_L = 16
    FX_R = 32
    START = 64
    TEST = 128
    SERVICE = 256


def press_button(con, button: int):
    con.set_button(button, 1)
    sleep(0.05)
    con.set_button(button, 0)


def setup_con(con):
    for x in range(9):
        print("Pressing button {} in 4s".format(x + 1))
        sleep(4)
        press_button(con, x+1)
    print("Controller Setup")


def run():
    com = Serial("COM2", 57600)
    data = b""
    controller = pyvjoy.VJoyDevice(1)
    if need_config == True:
        setup_con(controller)
        com.close()
        exit(0)
    while True:
        try:
            incoming_b = com.read()
            if incoming_b == b"\xAA":
                data = b""
            else:
                data += incoming_b
            if len(data) == 9:
                try:
                    response = unpack(">bii", data)  # buttons, axis l, axis r
                    controller.data.lButtons = response[0]
                    controller.data.wAxisXRot = int(response[1] * 32)
                    controller.data.wAxisYRot = int(response[2] * 32)
                    controller.update()
                except Exception as e:
                    print("Failed to capture data!, {}".format(e))
        except KeyboardInterrupt:
            break
    com.close()


if __name__ == "__main__":
    run()
