import serial
# to print to file: usbSerialFile.py >> log.txt

#Set up serial port
ser = serial.Serial(
    port="/dev/tty.usbmodem12341",\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

print("connected to: " + ser.portstr)

#Variable to store data from serial port
line = []

#Open file to save data in
# f = open('myfile','w')

#Print data from serial port to screen
while True:
    for c in ser.read():
        line.append(c)
        if c == '\n':
            print(line)
            # f.write(line + os.linesep)   # python will convert \n to os.linesep
            line = []
            break
# f.close()   # you can omit in most cases as the destructor will call it
ser.close()