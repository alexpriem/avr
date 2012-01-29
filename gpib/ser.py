import serial

ser = serial.Serial(port='COM2', baudrate=9600, timeout=None)

print ser.read(10)

for i in range(1,10):
        print ser.read(30);


ser.close()
