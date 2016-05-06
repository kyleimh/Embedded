import cv2
import numpy as np
import sys
import math
import socket
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from mainwindow import Ui_mainWindow

maroon = (26,26,139)
orange = (36,127,255)
green  = (0,255,0)
w = 640
h = 480
pxPerIn = h/22.2
inPerPx = 22.2/h
pxPerCm = h/56.388
#pushOffset = 2.5*pxPerIn
pushOffset = 0
pt1 = (w/2, h/2+75)
pt2 = (w/2, h/2+25)
pt3 = (w/2, h/2-25)
pt4 = (w/2, h/2-75)
hN = (w/2, h/2-140)
hS = (w/2, h/2+140)
hNE = (w/2+60, h/2-140)
hNW = (w/2-60, h/2-140)
hSE = (w/2+60, h/2+140)
hSW = (w/2-60, h/2+140)
hW1 = (w/2-60, h/2+75)
hW2 = (w/2-60, h/2+25)
hW3 = (w/2-60, h/2-25)
hW4 = (w/2-60, h/2-75)
hE1 = (w/2+60, h/2+75)
hE2 = (w/2+60, h/2+25)
hE3 = (w/2+60, h/2-25)
hE4 = (w/2+60, h/2-75)
homePts = [hN,hS,hNE,hNW,hSE,hSW,hW1,hW2,hW3,hW4,hE1,hE2,hE3,hE4]
erodeElem = cv2.getStructuringElement(cv2.MORPH_RECT, (3,3))
dilateElem = cv2.getStructuringElement(cv2.MORPH_RECT, (8,8))

def euclidDist(pt1, pt2):
    a2 = np.square(np.absolute(pt1[0] - pt2[0]))
    b2 = np.square(np.absolute(pt1[1] - pt2[1]))
    return np.sqrt(a2+b2)
    
def getAngle(x1,y1,x2,y2):
    dx = x2 - x1
    dy = y2 - y1
    A = np.array([dx,dy])
    magA = np.sqrt(dx**2+dy**2)
    Ahat = np.multiply(A,1./magA)
    angle = math.atan2(Ahat[1],Ahat[0])
    angle = angle*180./np.pi
    if angle < 0:
        angle = 360 + angle
    return Ahat,magA,angle

def getOffsetPt(objPt, destPt, distance):
    x1,y1 = objPt
    x2,y2 = destPt
    dx = x1-x2
    dy = y1-y2
    xOff = int((distance*dx)/math.hypot(dx,dy))
    yOff = int((distance*dy)/math.hypot(dx,dy))
    return (x1+xOff,y1+yOff)
    
def getObjHomeLocations(argObjectPts, argShelfPts):
    if len(argObjectPts) > len(argShelfPts):
        return -1
    objCount = 0
    matches = []
    for obj in argObjectPts:
        spotCount = 0
        minDist = 1000
        for spot in argShelfPts:
            dist = euclidDist(obj, spot)
            #print dist
            if dist < minDist:
                minDist = dist
                shelfNum = spotCount
            spotCount += 1
        matches.append(objCount)
        matches.append(shelfNum)
        argShelfPts[shelfNum] = (10000,10000)
        #print argShelfPts
        #print matches
        objCount += 1
    return matches
    
def getClosestHomePtIdx(point, homePts):
    minDist = 10000
    count = 0
    for homePt in homePts:
        dist = euclidDist(point, homePt)
        if dist < minDist:
            minDist = dist
            idx = count
        count += 1
    return idx
    
#            0  1   2   3   4   5   6   7   8   9  10  11  12  13
#homePts = [hN,hS,hNE,hNW,hSE,hSW,hW1,hW2,hW3,hW4,hE1,hE2,hE3,hE4]
def getPath(objPt, destPt, returning):
    p = []
    shelfPt = destPt
    endPt = getOffsetPt(objPt, destPt, pushOffset)
    homePtIdx = getClosestHomePtIdx(endPt, homePts)
    if homePtIdx != 1:
        if homePtIdx == 0:
            p = [hS,hSE,hNE,hN]
        if homePtIdx == 2:
            p = [hS,hSE,hNE]
        if homePtIdx == 3:
            p = [hS,hSW,hNW]
        if homePtIdx == 4:
            p = [hS,hSE]
        if homePtIdx == 5:
            p = [hS,hSW]
        if homePtIdx == 6:
            p = [hS,hSW,hW1]
        if homePtIdx == 7:
            p = [hS,hSW,hW2]
        if homePtIdx == 8:
            p = [hS,hSW,hW3]
        if homePtIdx == 9:
            p = [hS,hSW,hW4]
        if homePtIdx == 10:
            p = [hS,hSE,hE1]
        if homePtIdx == 11:
            p = [hS,hSE,hE2]
        if homePtIdx == 12:
            p = [hS,hSE,hE3]
        if homePtIdx == 13:
            p = [hS,hSE,hE4]
    else:
        p = [hS]    
    if returning == 1:
        p = p[::-1] #flip the list
        p.insert(0,objPt)
        print p
        return p
    if homePts[homePtIdx][1] > endPt[1]:
        endTmp = (endPt[0], endPt[1]+50)
    else:
        endTmp = (endPt[0], endPt[1]-50)
    if shelfPt == pt1:
        if endPt[0] > w/2:
            p = p+[endTmp, endPt, hE1, shelfPt]
        else:
            p = p+[endTmp, endPt, hW1, shelfPt]
    if shelfPt == pt2:
        if endPt[0] > w/2:
            p = p+[endTmp, endPt, hE2, shelfPt]
        else:
            p = p+[endTmp, endPt, hW2, shelfPt]
    if shelfPt == pt3:
        if endPt[0] > w/2:
            p = p+[endTmp, endPt, hE3, shelfPt]
        else:
            p = p+[endTmp, endPt, hW3, shelfPt]
    if shelfPt == pt4:
        if endPt[0] > w/2:
            p = p+[endTmp, endPt, hE4, shelfPt]
        else:
            p = p+[endTmp, endPt, hW4, shelfPt]
    return p
    
def drawPath(argPath, argFrame):
    countPath = 0
    while countPath < len(argPath)-1:
        F = cv2.line(argFrame, argPath[countPath], argPath[countPath+1], green, 2)
        countPath += 1
    return F
    
def drawBase(argFrame,argToggleHomePts):
    F = cv2.rectangle(argFrame.copy(),(w/2-30, h/2-110),(w/2+30, h/2+110),maroon, 2)
    F = cv2.circle(F.copy(), pt1, 18, maroon, 2)
    F = cv2.circle(F.copy(), pt2, 18, maroon, 2)
    F = cv2.circle(F.copy(), pt3, 18, maroon, 2)
    F = cv2.circle(F.copy(), pt4, 18, maroon, 2)
    if argToggleHomePts == 1:
        F = cv2.circle(F.copy(), hN, 4, green, -1)
        F = cv2.circle(F.copy(), hS, 4, green, -1)
        F = cv2.circle(F.copy(), hNE, 4, green, -1)
        F = cv2.circle(F.copy(), hNW, 4, green, -1)
        F = cv2.circle(F.copy(), hSE, 4, green, -1)
        F = cv2.circle(F.copy(), hSW, 4, green, -1)
        F = cv2.circle(F.copy(), hW1, 4, green, -1)
        F = cv2.circle(F.copy(), hW2, 4, green, -1)
        F = cv2.circle(F.copy(), hW3, 4, green, -1)
        F = cv2.circle(F.copy(), hW4, 4, green, -1)
        F = cv2.circle(F.copy(), hE1, 4, green, -1)
        F = cv2.circle(F.copy(), hE2, 4, green, -1)
        F = cv2.circle(F.copy(), hE3, 4, green, -1)
        F = cv2.circle(F.copy(), hE4, 4, green, -1)
    return F
    
class MainApp(QMainWindow):
    def __init__(self, parent=None):
        QWidget.__init__(self,parent)
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
        
        self.ui.exitButton.clicked.connect(self.exit)
        self.ui.actionExit.triggered.connect(self.exit)
        self.ui.goHomeButton.clicked.connect(self.returnHome)
        self.ui.sendNextButton.clicked.connect(self.sendNextInstr)
        self.ui.stopSendingButton.clicked.connect(self.clearInstr)
        self.ui.refreshViewButton.clicked.connect(self.refreshSingleFrame)
        self.ui.showObjectDetectionCheckbox.stateChanged.connect(self.toggleObjDetection)
        self.ui.showObjectPathCheckbox.stateChanged.connect(self.togglePathShow)
        self.ui.toggleHomePtsCheckbox.stateChanged.connect(self.enableHomePts)
        self.ui.toggleHomePathCheckbox.stateChanged.connect(self.enableHomePath)
        
        self.capTimer = QTimer()
        self.capTimer.timeout.connect(self.capture)
        self.capTimer.start(5)
        
        self.frame = cv2.imread('./init.jpg')
        self.sFrame = self.frame
        self.cap = cv2.VideoCapture(0)
        #self.cap.set(3, 960)
        #self.cap.set(4, 720)
        self.capWidth = int(self.cap.get(3))
        self.capHeight = int(self.cap.get(4))
        print 'Video dimensions:', self.capWidth, 'by', self.capHeight
        self.detectedObjs = []
        self.roverPos = (0,0)
        self.roverHeading = 0
        self.path1 = []
        self.path2 = []
        self.path3 = []
        self.path4 = []
        self.path1Adj = []
        self.path2Adj = []
        self.path3Adj = []
        self.path4Adj = []
        self.pathToHome = []
        self.pathToHomeAdj = []
        self.pathInstr = []
        self.pathInstrCounter = 0
        self.pathOnToggle = 0
        self.objDetectionOnToggle = 0
        self.toggleHomePts = 0
        self.homePathOnToggle = 0
        
    def exit(self):
        QApplication.quit()
        # When everything done, release the capture
        self.cap.release()
        cv2.destroyAllWindows()
        
    def capture(self):
        ret, self.frame = self.cap.read()
        ret = self.detectRover(self.frame.copy())
        if ret == -1:
            self.ui.roverPositionValueLabel.setText(QString("N/A"))
            self.ui.roverHeadingValueLabel.setText(QString("N/A"))
        else:
            text1 = "(" + str(self.roverPos[0]) + ", " + str(self.roverPos[1]) + ")"
            text2 = "%.2f"%self.roverHeading + '\xb0'
            self.ui.roverPositionValueLabel.setText(text1)
            self.ui.roverHeadingValueLabel.setText(text2)
            #self.frame = cv2.circle(self.frame.copy(), ret[0], 4, orange, -1)
            #self.frame = cv2.circle(self.frame.copy(), ret[1], 4, orange, -1)
            #self.frame = cv2.line(self.frame.copy(), ret[0], ret[1], orange, 2)
        self.frame = drawBase(self.frame.copy(), self.toggleHomePts)
        if self.homePathOnToggle == 1:
            if len(self.pathToHome) != 0:
                self.frame = drawPath(self.pathToHome, self.frame.copy())
            else:
                print 'ERROR: calculate home path first'
                self.ui.toggleHomePathCheckbox.setChecked(False)
        if self.pathOnToggle == 1:
            if len(self.path1) == 0:
                print 'ERROR: No paths loaded, refresh'
                self.ui.showObjectPathCheckbox.setChecked(False)
            else:
                if self.ui.pathSelectSpinBox.value() == 1:
                    self.frame = drawPath(self.path1, self.frame.copy())
                if self.ui.pathSelectSpinBox.value() == 2:
                    self.frame = drawPath(self.path2, self.frame.copy())
                if self.ui.pathSelectSpinBox.value() == 3:
                    self.frame = drawPath(self.path3, self.frame.copy())
                if self.ui.pathSelectSpinBox.value() == 4:
                    self.frame = drawPath(self.path4, self.frame.copy())
        if self.objDetectionOnToggle == 1:
            self.frame = self.detectObjects(self.frame, 0)
        image = QImage(self.frame.tostring(),self.capWidth,self.capHeight,QImage.Format_RGB888).rgbSwapped()
        pixmap = QPixmap.fromImage(image)
        self.ui.videoViewLabel.setPixmap(pixmap)
        if len(self.pathInstr) == 0:
            self.ui.sendNextButton.setEnabled(False)
            self.ui.stopSendingButton.setEnabled(False)
        else:
            self.ui.sendNextButton.setEnabled(True)
            self.ui.stopSendingButton.setEnabled(True)
        
    def detectRover(self, argFrame):
        frame    = self.frame
        hsvFrame = self.frame
        thresh   = self.frame[:,:,0]
        rGreen = (38,67,155,198,0,255)
        rPink = (165,182,155,192,0,255)
        hsvFrame  = cv2.cvtColor(self.frame.copy(), cv2.COLOR_BGR2HSV)
        thresh = cv2.inRange(hsvFrame.copy(),np.array([rGreen[0],rGreen[2],rGreen[4]]),np.array([rGreen[1],rGreen[3],rGreen[5]]))
        thresh = cv2.medianBlur(thresh.copy(),5)
        thresh = cv2.erode(thresh.copy(), erodeElem)
        #thresh = cv2.erode(thresh.copy(), erodeElem)
        thresh = cv2.dilate(thresh.copy(), dilateElem)
        thresh = cv2.dilate(thresh.copy(), dilateElem)
        _, contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) != 1:
            return -1
        (x,y,w,h) = cv2.boundingRect(contours[0])
        greenPt = (int((x+x+w)/2),int((y+y+h)/2))
        thresh = cv2.inRange(hsvFrame.copy(),np.array([rPink[0],rPink[2],rPink[4]]),np.array([rPink[1],rPink[3],rPink[5]]))
        thresh = cv2.medianBlur(thresh.copy(),5)
        thresh = cv2.erode(thresh.copy(), erodeElem)
        #thresh = cv2.erode(thresh.copy(), erodeElem)
        thresh = cv2.dilate(thresh.copy(), dilateElem)
        thresh = cv2.dilate(thresh.copy(), dilateElem)
        _, contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) != 1:
            return -1
        (x,y,w,h) = cv2.boundingRect(contours[0])
        pinkPt = (int((x+x+w)/2),int((y+y+h)/2))
        self.roverPos = (int((greenPt[0]+pinkPt[0])/2),int((greenPt[1]+pinkPt[1])/2))
        angle = getAngle(pinkPt[0],pinkPt[1],greenPt[0],greenPt[1])
        self.roverHeading = 360+angle[2]*-1
        return greenPt, pinkPt
        
        
    def detectObjects(self, argFrame, updatePaths):
        frame    = self.frame
        hsvFrame = self.frame
        thresh   = self.frame[:,:,0]
        shelfPts = [pt1, pt2, pt3, pt4]
        objCenters = []
        linePts = []
        distList = []
        distOut = []
        orng = (4,16,179,255,0,255)
        hsvFrame  = cv2.cvtColor(self.frame.copy(), cv2.COLOR_BGR2HSV)
        thresh = cv2.inRange(hsvFrame.copy(),np.array([orng[0],orng[2],orng[4]]),np.array([orng[1],orng[3],orng[5]]))
        thresh = cv2.medianBlur(thresh.copy(),5)
        thresh = cv2.erode(thresh.copy(), erodeElem)
        #thresh = cv2.erode(thresh.copy(), erodeElem)
        thresh = cv2.dilate(thresh.copy(), dilateElem)
        thresh = cv2.dilate(thresh.copy(), dilateElem)
        _, contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if len(contours) < 5:
            for cont in contours:
                #if cv2.contourArea(cont) < 20:
                    #continue
                (x,y,w,h) = cv2.boundingRect(cont)
                centroid = (int((x+x+w)/2),int((y+y+h)/2))
                objCenters.append(centroid)
                #frame = cv2.rectangle(frame.copy(), (x,y), (x+w,y+h), maroon, 3)
                frame = cv2.putText(frame.copy(),str(centroid),(x+w,y+h+8),cv2.FONT_HERSHEY_PLAIN,1,maroon,1,cv2.LINE_AA)
                frame = cv2.circle(frame.copy(), centroid, 1, orange, 2)
            self.detectedObjs = objCenters
            if len(objCenters) == 4:
                for obj in objCenters:
                    distList = []
                    for pt in shelfPts:
                        dist = distList.append(euclidDist(obj,pt))
                    idx = distList.index(min(distList))
                    distOut.append(str(float("{0:.2f}".format(min(distList)/pxPerIn)))+"\"")
                    linePts.append(obj)
                    linePts.append(shelfPts[idx])
                    shelfPts.remove(shelfPts[idx])
                i = 0
                j = 0
                while i < len(objCenters)*2:
                    frame = cv2.line(frame.copy(), linePts[i], linePts[i+1], orange, 2)
                    frame = cv2.putText(frame.copy(), distOut[j],(int(objCenters[j][0]),int(objCenters[j][1])-8),cv2.FONT_HERSHEY_PLAIN,1,maroon,1,cv2.LINE_AA)
                    i += 2
                    j += 1
        if updatePaths == 1:
            return objCenters
        else:
            return frame
            
    def returnHome(self):
        self.pathToHome = getPath(self.roverPos, (0,0), 1)
        instructions = self.getPathInstructions(self.pathToHome, 1)
        print instructions
        self.pathInstr = instructions
        #sendInstrList(instructions)
            
    def getPathInstructions(self, path, returning):
        i=0
        instructions = []
        pathBLOrigin = []
        for p1 in path:
            pathBLOrigin.append((p1[0],h-p1[1]))
        path = pathBLOrigin
        if returning == 1:
            curHeading = self.roverHeading
        else:
            curHeading = 90
        while i<len(path)-1:
            print '-----\nCurrent heading:', curHeading
            a1 = getAngle(path[i][0],path[i][1],path[i+1][0],path[i+1][1])
            #if curHeading > 180 and a1[2] == 0:
            #    a1 = [a1[0],a1[1],360]
            #dTheta1 = curHeading + 360-a1[2]
            #dTheta2 = curHeading - a1[2]
            #thetas = [dTheta1, dTheta2]
            #absThetas = [abs(dTheta1),abs(dTheta2)]
            #dTheta = thetas[absThetas.index(min(absThetas))]
            if curHeading > 180:
                curHeading = curHeading - 360
            if a1[2] > 180:
                a1 = [a1[0],a1[1],a1[2]-360]
            dTheta = curHeading - a1[2]
            instructions.append(dTheta)
            instructions.append(a1[1]/pxPerCm)
            print 'Angle between pts:', a1[2]
            print 'Angle changing by:', dTheta
            print 'Move forward:', a1[1], 'px or', a1[1]/pxPerCm, 'cm\n'
            #if returning == 1:
                #self.pathToHomeAdj.append([(path[i][0],h-path[i][1],a1[2]])
            #print a1[0][0],a1[0][1],'\t',instructions[len(instructions)-2],instructions[len(instructions)-1]
            curHeading = a1[2]
            i += 1
        if returning == 1:
            if abs(instructions[len(instructions)-2]) == 90:
                instructions.append(instructions[len(instructions)-2])
                idx = 0
                pIdx = 0
                for inst in instructions:
                    if idx==0:
                        self.pathToHomeAdj.append([(path[pIdx][0],h-path[pIdx][1]),inst])
                        pIdx += 1
                    else:
                        if idx%2 == 0:
                            self.pathToHomeAdj.append([(path[pIdx][0],h-path[pIdx][1]),inst])
                            pIdx += 1
                            print 'incd'
                        else:
                            self.pathToHomeAdj.append([(path[idx][0],h-path[pIdx][1]),inst])
                    print self.pathToHomeAdj
            else:
                pass
        return instructions
        
    def sendNextInstr(self):
        if self.pathInstrCounter%2 == 0:
            tmpI = int(round(self.pathInstr[self.pathInstrCounter]))
            print tmpI, 'degrees'
            if tmpI<0:
                turnLeft(tmpI*-1)
            else:
                turnRight(tmpI)
        else:
            tmpI = round(self.pathInstr[self.pathInstrCounter])
            print tmpI, 'cm'
            forward(tmpI)
        self.pathInstrCounter += 1
    
    def clearInstr(self):
        self.pathInstrCounter = 0
        self.pathInstr = []
        
    def toggleObjDetection(self):
        if self.objDetectionOnToggle == 0:
            self.objDetectionOnToggle = 1
        else:
            self.objDetectionOnToggle = 0
            
    def togglePathShow(self):
        if self.pathOnToggle == 0:
            self.pathOnToggle = 1
        else:
            self.pathOnToggle = 0
            
    def enableHomePts(self):
        if self.toggleHomePts == 0:
            self.toggleHomePts = 1
        else:
            self.toggleHomePts = 0
    
    def enableHomePath(self):
        if self.homePathOnToggle == 0:
            self.homePathOnToggle = 1
        else:
            self.homePathOnToggle = 0
            
    def refreshSingleFrame(self):
        shelfPts = [pt1, pt2, pt3, pt4]
        matches = getObjHomeLocations(self.detectedObjs,shelfPts)
        if len(self.detectedObjs) != 4:
            print 'ERROR: Not 4 objects in view, refresh'
            self.path1 = []
            self.path2 = []
            self.path3 = []
            self.path4 = []
        else:
            shelfPts = [pt1, pt2, pt3, pt4]
            self.path1 = getPath(self.detectedObjs[matches[0]], shelfPts[matches[1]], 0)
            self.path2 = getPath(self.detectedObjs[matches[2]], shelfPts[matches[3]], 0)
            self.path3 = getPath(self.detectedObjs[matches[4]], shelfPts[matches[5]], 0)
            self.path4 = getPath(self.detectedObjs[matches[6]], shelfPts[matches[7]], 0)
        print self.path1
        print self.path2
        print self.path3
        print self.path4
        
def turnRight(argDeg):
    val = int(argDeg) + 5
    val = chr(val)
    conn.sendall(bytearray(['\x02', '\x21', '\x0D',   val , '\x00', '\x03']))             
def turnLeft(argDeg):
    val = int(argDeg) + 5
    val = chr(val)
    conn.sendall(bytearray(['\x02', '\x21', '\x0E',   val , '\x00', '\x03']))
def forward(argDist):	
    val = int(argDist) + 5
    val = chr(val)
    conn.sendall(bytearray(['\x02', '\x21', '\x0B',   val , '\x00', '\x03']))
def backward(argDist):
    val = int(argDist) + 5
    val = chr(val)
    conn.sendall(bytearray(['\x02', '\x21', '\x0C',   val , '\x00', '\x03']))
def stopRover():
    conn.sendall(bytearray(['\x02', '\x21', '\x00', '\x00', '\x00', '\x03']))
def sendInstrList(instr):
    print instr
    try:
        count = 0
        for i in instr:
            mode = int(raw_input('0 for send next, 1 for break out: '))
            if mode == 0:
                if count%2 == 0:
                    tmpI = int(round(i))
                    print tmpI, 'degrees'
                    if tmpI<0:
                        turnLeft(tmpI*-1)
                    else:
                        turnRight(tmpI)
                else:
                    tmpI = round(i)
                    print tmpI, 'cm'
                    forward(tmpI)
            else:
                break
            count += 1
    except:
        print 'ERROR: In sendInstrList'

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
            
#if __name__ == '__main_GUI__':
app = QApplication(sys.argv)
myapp = MainApp()
#myapp.resize(950,600)
myapp.move(0,0)
myapp.show()
sys.exit(app.exec_())
