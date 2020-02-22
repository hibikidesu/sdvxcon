from serial import Serial
from struct import unpack, calcsize

struct_fmt = ">i"
struct_size = calcsize(struct_fmt)

def run():
    com = Serial("COM2", 57600)
    packets = []
    while True:
        packets.append(com.read())
        if len(packets) == struct_size:
            print(unpack(struct_fmt, b"".join(packets)))
            packets.clear()

if __name__ == "__main__":
    run()
