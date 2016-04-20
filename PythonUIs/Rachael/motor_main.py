from PyQt4 import QtGui		#import Python Qt
import sys # We need sys so that we can pass argv to QApplication
import socket
from PyQt4.QtCore import QThread, pyqtSlot, pyqtSignal
import binascii
import struct
import design		#Holds our MainWindow and all design realated things

deg = 0
dis = 0

class ExampleApp(QtGui.QMainWindow, design.Ui_MotorGUI):

	def __init__(self):
		super(self.__class__, self).__init__()
		self.setupUi(self)

		#Setup Button Clicks
		self.bttn_turnRight.clicked.connect(self.turnRight)
		self.bttn_turnLeft.clicked.connect(self.turnLeft)
		self.bttn_forward.clicked.connect(self.forward)
		self.bttn_backward.clicked.connect(self.backward)
		self.bttn_stop.clicked.connect(self.stop)

		self.myThread = Communication()
		self.myThread.rEncoderSend.connect(self.rEncoder)
		self.myThread.lEncoderSend.connect(self.lEncoder)
		self.myThread.getDone.connect(self.sendDone)
		self.myThread.start()

	def turnRight(self):
		deg = int(self.txt_distance.text()) + 5  #is this right function
		deg = chr(deg)
		conn.sendall(bytearray(['\x02', '\x31', '\x0D',   deg , '\x00', '\x03']))             
	def turnLeft(self):
		deg = int(self.txt_distance.text()) + 5  #is this right function
		deg = chr(deg)
		conn.sendall(bytearray(['\x02', '\x31', '\x0E',   deg , '\x00', '\x03']))
	def forward(self):	
		dis = int(self.txt_distance.text()) + 5  #is this right function
		dis = chr(dis)
		conn.sendall(bytearray(['\x02', '\x31', '\x0B',   dis , '\x00', '\x03']))
	def backward(self):
		dis = int(self.txt_distance.text()) + 5  
		dis = chr(dis)
		conn.sendall(bytearray(['\x02', '\x31', '\x0C',   dis , '\x00', '\x03']))
	def stop(self):
		conn.sendall(messge_stop)

        @pyqtSlot(int)
        def rEncoder(self, data):
                print ("Right: " + str(self.data))
	
        @pyqtSlot(int)
        def lEncoder(self, data):
                print ("Right: " + str(self.data))	

        @pyqtSlot(int)
        def sendDone(self, data):
                print "DONE"

class Communication(QThread):
        rEncoderSend = pyqtSignal(int);
        lEncoderSend = pyqtSignal(int);        
        getDone = pyqtSignal(int);

        def __init__(self):
                QThread.__init__(self)
        def __del__(self):
                self.wait()
        def run(self):
                while 1:
                        try:
                                response = conn.recv(4);
                                print int(binascii.hexlify(response[1]),16);
                                if int(binascii.hexlify(response[1]), 16) == 40:
                                        self.lEncoderSend.emit(int(binascii.hexlify(response[2]),16))                               
                                elif int(binascii.hexlify(response[1]), 16) == 41:
                                        self.rEncoderSend.emit(int(binascii.hexlify(response[3]),16))                             
                                elif int(binascii.hexlify(response[1]), 16) == 100:
                                        self.getDone.emit(int(binascii.hexlify(response[2]),16))
                        except KeyboardInterrupt:
                                print "Keyboard Interrupt"                
                s.shutdown(socket.SHUT_RDWR)
                s.close()
    

def main():
	app = QtGui.QApplication(sys.argv)
	form = ExampleApp()
	form.show()
	app.exec_()

if __name__ == '__main__':              # if we're running file directly and not importing it
    HOST = '192.168.42.1'   
    PORT = 2000
    s = socket.socket()
    s.settimeout(3)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    greeting = conn.recv(8)
    print 'Connected by', addr
    print greeting
    
    val_stop  = ['\x02', '\x31', '\x00', '\x00', '\x00', '\x03'] # Stop        
    messge_stop 	= bytearray(val_stop)
    
    main()                              # run the main function

if __name__ == '__main__':
	main()
