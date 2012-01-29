import serial


s=serial.Serial(1,9600)
s.write (chr(27)+chr(0))    # scherm wissen
s.write ('test\n')
s.write (chr(27)+chr(1))    # cursor home
s.write ('xxt\n')
s.write (chr(27)+chr(2)+chr(10)+chr(1))    # gotoxy(10,1)
s.write ('1234\n')
