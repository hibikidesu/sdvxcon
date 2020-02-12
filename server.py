from serial import Serial
from struct import unpack, calcsize
import pyvjoy
from time import sleep
from enum import IntFlag
import keyboard


BUTTON_LAYOUT = ["a", "s", "d", "f", "g", "h", "j", "k", "l"]
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


def keyboard_input(buttons: int):
    for i, b in enumerate(Button):
        if b.value & buttons:
            keyboard.press_and_release(BUTTON_LAYOUT[i])


def run():
    com = Serial("COM2", 57600)
    data = b""
    controller = pyvjoy.VJoyDevice(1)
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
                    #controller.data.lButtons = response[0]
                    keyboard_input(response[0])
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
