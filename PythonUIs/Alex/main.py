from PyQt4 import QtGui # Import the PyQt4 module we'll need
from PyQt4.QtCore import QThread, pyqtSlot, pyqtSignal
import sys # We need sys so that we can pass argv to QApplication
import socket
import binascii
import struct
import sensor_debug # This file holds our MainWindow and all design related things
              # it also keeps events etc that we defined in Qt Designer
              
class Sensor_App(QtGui.QMainWindow, sensor_debug.Ui_MainWindow):
    dataFormat = True
    stat = 'Running'
            
    @pyqtSlot(int)
    def recvADC(self, data):
        if self.dataFormat:
            self.distance.setText(str(data) + ' cm')
        else:
            self.distance.setText(str(data))
            
    @pyqtSlot(int)
    def recvLine(self, data):
        self.line_value.setText('{0:08b}'.format(data))
            
    @pyqtSlot(str)
    def recvSafe(self, data):
        self.proximity_alert.setText(data)
    
    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)
        
        # Setup Button Clicks
        self.push_button.clicked.connect(self.push)
        self.wait_button.clicked.connect(self.wait)
        self.raw_button.clicked.connect(self.raw)
        self.cm_button.clicked.connect(self.cm)
        self.clear_debug.clicked.connect(self.clearDebug)
        self.debug_adc.clicked.connect(self.adcDebug)
        self.debug_line.clicked.connect(self.lineDebug)
        
        # Start Communication thread
        self.myThread = Communication()
        self.myThread.sendADC.connect(self.recvADC)
        self.myThread.sendSafe.connect(self.recvSafe)
        self.myThread.sendLine.connect(self.recvLine)
        self.myThread.start()

    # Button Functions
    def push(self):
        if self.stat == 'Running':
            self.stat = 'Pushing'
            self.status.setText('Pushing')
            conn.sendall(values3)
        elif self.stat == 'Pushing':
            self.stat = 'Running'
            self.status.setText('Running')
            conn.sendall(values3)
        
    def raw(self):
        self.dataFormat = False
        conn.sendall(values1)
        
    def cm(self):
        self.dataFormat = True
        conn.sendall(values2)
        
    def wait(self):
        if self.stat == 'Running':
            self.stat = 'Waiting'
            self.status.setText('Waiting')
            conn.sendall(values4)
        elif self.stat == 'Waiting':
            self.stat = 'Running'
            self.status.setText('Running')
            conn.sendall(values4)
        
    def adcDebug(self):
        conn.sendall(values5)
    
    def lineDebug(self):
        conn.sendall(values6)
        
    def clearDebug(self):
        conn.sendall(values7)
        
class Communication(QThread):

    sendADC = pyqtSignal(int)
    sendLine = pyqtSignal(int)
    sendSafe = pyqtSignal(str)
    
    def __init__(self):      
        QThread.__init__(self)
       
    def __del__(self):
        self.wait()
            
    def run(self):
        while 1:
            try:
                response = conn.recv(4)
                if int(binascii.hexlify(response[1]), 16) == 32:
                    self.sendSafe.emit('Not Safe')
                    print 'Not Safe'
                elif int(binascii.hexlify(response[1]), 16) == 33:
                    self.sendSafe.emit('Safe')
                    print 'Safe'
                elif int(binascii.hexlify(response[1]), 16) == 34:
                    self.sendLine.emit(int(binascii.hexlify(response[2]), 16))                
                elif int(binascii.hexlify(response[1]), 16) == 35:
                    self.sendADC.emit(int(binascii.hexlify(response[2]), 16) + 256*int(binascii.hexlify(response[3]), 16))
                elif int(binascii.hexlify(response[1]), 16) == 36:
                    print 'Intersection'
                
            except ValueError:
                print "Not a number"
            except socket.error, e:
                print e
            except KeyboardInterrupt:
                print "\n\nYou pressed Ctrl+C. Killing socket...\n"
                s.shutdown(socket.SHUT_RDWR)
                s.close() 
                sys.exit(0)
        
        s.shutdown(socket.SHUT_RDWR)
        s.close()
    
def main():
    app = QtGui.QApplication(sys.argv)  # A new instance of QApplication
    form = Sensor_App()                 # We set the form to be our ExampleApp (design)
    form.show()                         # Show the form
    app.exec_()                         # and execute the app


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
    
    
    elements1 = ['\x02', '\x53', '\x15', '\x00', '\x00', '\x03'] # Raw Data
    elements2 = ['\x02', '\x53', '\x16', '\x00', '\x00', '\x03'] # CM Data
    elements3 = ['\x02', '\x53', '\x17', '\x00', '\x00', '\x03'] # Push
    elements4 = ['\x02', '\x53', '\x18', '\x00', '\x00', '\x03'] # Wait
    elements5 = ['\x02', '\x58', '\x41', '\x00', '\x00', '\x03'] # Enable adc debugging
    elements6 = ['\x02', '\x58', '\x42', '\x00', '\x00', '\x03'] # Enable line sensor degbugging
    elements7 = ['\x02', '\x58', '\x40', '\x00', '\x00', '\x03'] # Clear Debugging
    values1 = bytearray(elements1)
    values2 = bytearray(elements2)
    values3 = bytearray(elements3)
    values4 = bytearray(elements4)
    values5 = bytearray(elements5)
    values6 = bytearray(elements6)
    values7 = bytearray(elements7)
    
    main()                              # run the main function