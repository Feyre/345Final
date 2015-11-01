import serial
#Set up serial port
ser = serial.Serial(
    port="/dev/ttyAMC0",\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

print("connected to: " + ser.portstr)

#Variable to store data from serial port
line = []

#Print data from serial port to screen
while True:
    for c in ser.read():
        line.append(c)
        if c == '\n':
            print(line)
            line = []
            break

ser.close()