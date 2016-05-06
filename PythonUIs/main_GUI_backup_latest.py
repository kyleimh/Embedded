import cv2
import numpy as np
import sys
import math
import socket
import binascii
import struct
import time
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
pxPerCm = w/75.08875
#pushOffset = 2.5*pxPerIn
pushOffset = 0
pushUp = -40
pt1 = (w/2, h/2+75+pushUp)
pt2 = (w/2, h/2+25+pushUp)
pt3 = (w/2, h/2-25+pushUp)
pt4 = (w/2, h/2-75+pushUp)
hN = (w/2, h/2-190)
hS = (w/2, h/2+190)
hNE = (w/2+100, h/2-190)
hNW = (w/2-130, h/2-190)
hSE = (w/2+130, h/2+190)
hSW = (w/2-130, h/2+190)
hW1 = (w/2-130, h/2+75+pushUp)
hW2 = (w/2-130, h/2+25+pushUp)
hW3 = (w/2-130, h/2-25+pushUp)
hW4 = (w/2-130, h/2-75+pushUp)
hE1 = (w/2+130, h/2+75+pushUp)
hE2 = (w/2+130, h/2+25+pushUp)
hE3 = (w/2+130, h/2-25+pushUp)
hE4 = (w/2+130, h/2-75+pushUp)
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
        endTmp = (endPt[0], endPt[1]+80)
    else:
        endTmp = (endPt[0], endPt[1]-80)
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
    F = cv2.rectangle(argFrame.copy(),(w/2-30, h/2-110+pushUp),(w/2+30, h/2+110+pushUp),maroon, 2)
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
    
    dataFormat = True
    stat = 'Running'
            
    @pyqtSlot(int)
    def recvADC(self, data):
        if self.dataFormat:
            self.ui.distance.setText(str(data) + ' cm')
        else:
            self.ui.distance.setText(str(data))
            
    @pyqtSlot(int)
    def recvLine(self, data):
        self.ui.line_value.setText('{0:08b}'.format(data))
            
    @pyqtSlot(str)
    def recvSafe(self, data):
        self.ui.proximity_alert.setText(data)
        
    @pyqtSlot()
    def recvDone(self):
        print 'DONE Recvd'
        #if(self.moveDone > 0):
        #    self.adjTimerCallback()
        #else:
        #    self.adjTimer.start(7000)
        
    @pyqtSlot(str)
    def endcoderVal(self, data):
        encode = data.split('/')
        myString = "Left: " + encode[0] + " Right: " + encode[1]
        self.ui.label_encoders.setText(myString)
        
    
    def __init__(self, parent=None):
        QWidget.__init__(self,parent)
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
                
        # Setup Button Clicks
        self.ui.push_button.clicked.connect(self.push)
        self.ui.wait_button.clicked.connect(self.wait)
        self.ui.raw_button.clicked.connect(self.raw)
        self.ui.cm_button.clicked.connect(self.cm)
        self.ui.clear_debug.clicked.connect(self.clearDebug)
        self.ui.debug_adc.clicked.connect(self.adcDebug)
        self.ui.debug_line.clicked.connect(self.lineDebug)
        
        self.ui.btn_deliver.clicked.connect(self.deliveryLogic)
        
        # Start Communication thread
        self.cleanupThread = CleanupComm()
        self.cleanupThread.sendADC.connect(self.recvADC)
        self.cleanupThread.sendSafe.connect(self.recvSafe)
        self.cleanupThread.sendDone.connect(self.recvDone)
        self.cleanupThread.encoderSend.connect(self.endcoderVal)
        self.cleanupThread.start()
        
        self.deliveryThread = DeliveryComm()
        self.deliveryThread.sendLine.connect(self.recvLine)
        self.deliveryThread.start()
        
        self.ui.exitButton.clicked.connect(self.exit)
        self.ui.actionExit.triggered.connect(self.exit)
        self.ui.goHomeButton.clicked.connect(self.returnHome)
        self.ui.sendNextButton.clicked.connect(self.sendNextInstr)
        self.ui.stopSendingButton.clicked.connect(self.clearInstr)
        self.ui.adjustButton.clicked.connect(self.adjustment)
        self.ui.refreshViewButton.clicked.connect(self.refreshSingleFrame)
        self.ui.cleanUpButton.clicked.connect(self.cleanUpObject)
        
        #Setup Button Clicks
        self.ui.bttn_turnRight.clicked.connect(self.roverRight)
        self.ui.bttn_turnLeft.clicked.connect(self.roverLeft)
        self.ui.bttn_forward.clicked.connect(self.roverForward)
        self.ui.bttn_backward.clicked.connect(self.roverBackward)
        self.ui.bttn_stop.clicked.connect(self.roverStop)
        self.ui.bttn_testOn.clicked.connect(self.testOn)
        self.ui.bttn_testOff.clicked.connect(self.testOff)
        
        self.ui.showObjectDetectionCheckbox.stateChanged.connect(self.toggleObjDetection)
        self.ui.showObjectPathCheckbox.stateChanged.connect(self.togglePathShow)
        self.ui.toggleHomePtsCheckbox.stateChanged.connect(self.enableHomePts)
        self.ui.toggleHomePathCheckbox.stateChanged.connect(self.enableHomePath)
        
        self.capTimer = QTimer()
        self.capTimer.timeout.connect(self.capture)
        self.capTimer.start(5)
                
        self.adjTimer = QTimer()
        self.adjTimer.timeout.connect(self.adjTimerCallback)
        #self.adjTimer.start(5)
        
        self.checkDones = QTimer()
        self.checkDones.timeout.connect(self.checkDonesCallback)
        self.checkDones.start(5)
        
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
        self.activePathAdj = []
        self.path1 = []
        self.path2 = []
        self.path3 = []
        self.path4 = []
        self.path1Instr = []
        self.path2Instr = []
        self.path3Instr = []
        self.path4Instr = []
        self.path1InstrAdj = []
        self.path2InstrAdj = []
        self.path3InstrAdj = []
        self.path4InstrAdj = []
        self.pathToHome = []
        self.pathToHomeInstr = []
        self.pathToHomeInstrAdj = []
        self.pathInstr = []
        self.deliverPath1 = [hS,hSW,hW1,hE1,hNE]
        self.deliverPath2 = [hS,hSW,hW2,hE2,hNE]
        self.deliverPath3 = [hS,hSW,hW3,hE3,hNE]
        self.deliverPath4 = [hS,hSW,hW4,hE4,hNE]
        self.deliverPath1Instr, self.deliverPath1InstrAdj = self.getPathInstructions(self.deliverPath1, 0)
        self.deliverPath2Instr, self.deliverPath2InstrAdj = self.getPathInstructions(self.deliverPath2, 0)
        self.deliverPath3Instr, self.deliverPath3InstrAdj = self.getPathInstructions(self.deliverPath3, 0)
        self.deliverPath4Instr, self.deliverPath4InstrAdj = self.getPathInstructions(self.deliverPath4, 0)
        
        
        self.pathInstrCounter = 0
        self.pathOnToggle = 0
        self.objDetectionOnToggle = 0
        self.toggleHomePts = 0
        self.homePathOnToggle = 0
        self.checkAdj = 0
        self.lastFwd = 0
        self.adjCount = 0
        self.countDone = 0
        self.moveDone = 0
        self.sendNextReady = 0
        self.snDone = 0
        self.adjDone = 0
        self.backwardToEnd = 0
        self.executingCleanupDelivery = 0
        self.cleanupDeliverDone = 0
        self.backups = 0
        
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
                    distOut.append(str(float("{0:.2f}".format(min(distList)/pxPerCm)))+"cm")
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
        self.pathToHomeInstr, self.pathToHomeInstrAdj = self.getPathInstructions(self.pathToHome, 1)
        self.pathInstrCounter = 0
        self.pathInstr = self.pathToHomeInstr
        self.activePathAdj = self.pathToHomeInstrAdj
        backward(8)
        self.sendNextInstr()
        #sendInstrList(instructions)
            
    def getPathInstructions(self, argPath, returning):
        i=0
        r=0
        instructions = []
        pathBLOrigin = []
        headings = []
        pathAdj = []
        self.pathToHomeAdj = []
        for p1 in argPath:
            pathBLOrigin.append((p1[0],h-p1[1]))
        path = pathBLOrigin
        if returning == 1:
            curHeading = self.roverHeading
        else:
            curHeading = 90
        #if curHeading > 180:
        #        curHeading = curHeading - 360
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
            #if a1[2] > 180:
            #        ang = a1[2]-360
            #if curHeading<0 and a1[2]>0:
            #    dTheta = (180 + curHeading) + a1[2]
            #elif curHeading>0 and a1[2]<0:
            #    dTheta = (curHeading + (180 + a1[2]))*-1
            #else:
            #dTheta = curHeading - ang
            
            #d = abs(curHeading - a1[2]) % 360
            #if d > 180:
            #    r = 360 - d
            #diff = curHeading-a1[2]
            #if (diff>=0 and diff <=180) or (diff<=-180 and diff>=-360):
            #    pass
            #else:
            #    r = r*-1
            
            diff = curHeading - a1[2]
            if abs(diff) <= 180:
                pass
            else:
                tmp = 360 - abs(diff)
                if diff < 0:
                    diff = tmp
                else:
                    diff = -1*tmp
            instructions.append(diff)
            instructions.append(a1[1]/pxPerCm)
            print 'Angle between pts:', a1[2]
            print 'Angle changing by:', diff
            print 'Move forward:', a1[1], 'px or', a1[1]/pxPerCm, 'cm\n'
            #if returning == 1:
                #self.pathToHomeAdj.append([(path[i][0],h-path[i][1],a1[2]])
            #print a1[0][0],a1[0][1],'\t',instructions[len(instructions)-2],instructions[len(instructions)-1]
            headings.append(a1[2])
            headings.append(a1[2])
            curHeading = a1[2]
            i += 1
        idx = 0
        pIdx = 0
        for inst in instructions:
            if pIdx==0:
                pathAdj.append([(path[pIdx][0],h-path[pIdx][1]),headings[idx]])
                pIdx += 1
            else:
                if idx%2 == 0:
                    pathAdj.append([(path[pIdx][0],h-path[pIdx][1]),headings[idx]])
                    pIdx += 1
                else:
                    pathAdj.append([(path[pIdx][0],h-path[pIdx][1]),headings[idx]])
            idx += 1
        #print headings
        if returning == 1:
            if argPath[len(argPath)-2] == hSW:
                instructions.append(-90)
                headings.append(-90)
            elif argPath[len(argPath)-2] == hSE:
                instructions.append(90)
                headings.append(90)
            else:
                pass
            idx = 0
            pIdx = 0
            for inst in instructions:
                if pIdx==0:
                    self.pathToHomeAdj.append([(path[pIdx][0],h-path[pIdx][1]),headings[idx]])
                    pIdx += 1
                else:
                    if idx%2 == 0:
                        self.pathToHomeAdj.append([(path[pIdx][0],h-path[pIdx][1]),headings[idx]])
                        pIdx += 1
                    else:
                        self.pathToHomeAdj.append([(path[pIdx][0],h-path[pIdx][1]),headings[idx]])
                idx += 1
            print self.pathToHomeAdj
        else:
            pass
        print 'intsr ret:', instructions
        print 'path adj ret: ', pathAdj
        return instructions, pathAdj
        
    def sendNextInstr(self):
        #print 'sendNext instr list:', self.pathInstr
        print '\n-----NEW Instr'
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
        self.snDone = 1
        
    def checkDonesCallback(self):
        if self.snDone == 1:
            self.snDone = 0
            self.adjTimer.start(7000)
        if self.adjDone == 1:
            self.adjDone = 0
            self.sendNextInstr()
        if self.pathInstrCounter == len(self.pathInstr) and self.ui.sendNextButton.isEnabled():
            print '*****DONE WITH PATH'
            self.snDone = 0
            self.adjDone = 0
            self.pathInstrCounter = 0
            if self.activePathAdj[len(self.activePathAdj)-1][0] == hNE:
                self.executingCleanupDelivery = 0
                self.cleanupDeliverDone = 1
                self.returnHome()
                time.sleep(5)
                self.deliveryLogic()
            
    def deliveryLogic(self):
        if self.cleanupDeliverDone == 0 and self.executingCleanupDelivery == 0:
            if self.ui.candy_box.currentIndex() == 0:
                self.pathInstr = self.deliverPath1Instr
                self.activePathAdj = self.deliverPath1InstrAdj
            elif self.ui.candy_box.currentIndex() == 1:
                self.pathInstr = self.deliverPath2Instr
                self.activePathAdj = self.deliverPath2InstrAdj
            elif self.ui.candy_box.currentIndex() == 2:
                self.pathInstr = self.deliverPath3Instr
                self.activePathAdj = self.deliverPath3InstrAdj
            else: #self.ui.candy_box.currentIndex() == 3:
                self.pathInstr = self.deliverPath4Instr
                self.activePathAdj = self.deliverPath4InstrAdj
            self.executingCleanupDelivery = 1
            self.sendNextInstr()
        if self.cleanupDeliverDone == 1:
            if self.ui.dest_box.currentIndex() == 0:
                connDelivery.sendall(bytearray(['\x02', '\x14', '\x09', '\x08', '\x01', '\x03']))
            elif self.ui.dest_box.currentIndex() == 1:
                connDelivery.sendall(bytearray(['\x02', '\x14', '\x09', '\x08', '\x00', '\x03']))
            elif self.ui.dest_box.currentIndex() == 2:
                connDelivery.sendall(bytearray(['\x02', '\x14', '\x09', '\x09', '\x01', '\x03']))
            elif self.ui.dest_box.currentIndex() == 3:
                connDelivery.sendall(bytearray(['\x02', '\x14', '\x09', '\x09', '\x00', '\x03']))
            elif self.ui.dest_box.currentIndex() == 4:
                connDelivery.sendall(bytearray(['\x02', '\x14', '\x09', '\x0A', '\x01', '\x03']))
            elif self.ui.dest_box.currentIndex() == 5:
                connDelivery.sendall(bytearray(['\x02', '\x14', '\x09', '\x0A', '\x00', '\x03']))
            self.cleanupDeliverDone = 0
            
            
    def adjTimerCallback(self):
        self.adjustment()
            
    def adjustment(self):
        print '\n***New adjustment'
        self.adjTimer.stop()
        pAdj = self.activePathAdj
        #print '\nPath compares:' ,pAdj
        pos = self.roverPos
        #if self.roverHeading > 180:
        #    head = self.roverHeading - 360
        #else:
        head = self.roverHeading
        desiredPos = pAdj[self.pathInstrCounter-1][0]
        desiredHead = pAdj[self.pathInstrCounter-1][1]
        print 'curr vs desired:', pos, desiredPos
        print 'curr vs desired:', head, desiredHead
        dX = pos[0]-desiredPos[0]
        dY = pos[1]-desiredPos[1]
        if abs(dX)<15 and abs(dY)<15:
            print 'done adjusting'
            self.adjCount = 0
            diff = head - desiredHead
            if abs(diff) <= 180:
                pass
            else:
                tmp = 360 - abs(diff)
                if diff < 0:
                    diff = tmp
                else:
                    diff = -1*tmp
            tmpI = int(round(diff))
            if tmpI > 180:
                tmpI = tmpI - 360
            print tmpI, 'degrees'
            if tmpI<0:
                turnLeft(tmpI*-1)
            else:
                turnRight(tmpI)
            self.adjDone = 1
            return
        a = getAngle(pos[0],h-pos[1], desiredPos[0], h-desiredPos[1])
        rAng = a[2]
        diff = head - rAng
        if abs(diff) <= 180:
            pass
        else:
            tmp = 360 - abs(diff)
            if diff < 0:
                diff = tmp
            else:
                diff = -1*tmp
        print rAng, a[1]/pxPerCm
        tmpI = int(round(diff))
        if tmpI > 180:
            tmpI = tmpI - 360
        print tmpI, 'degrees'
        if -180 <= diff < -160 or 160 <= diff < 180:
            if dX>0 and dY<0:    #Q1
                pass
            elif dX<0 and dY<0:  #Q2
                pass
            elif dX<0 and dY>0:  #Q3
                pass
            elif dX>0 and dY>0:  #Q4
                pass
            else:
                pass
            backward(int(round(a[1]*.5/pxPerCm)))
        elif abs(head-desiredHead) < 10:
            backward(int(round(a[1]*.5/pxPerCm)))
            if tmpI<0:
                turnLeft(20)
            else:
                turnRight(25)
        else:                
            if tmpI<0:
                turnLeft(tmpI*-1)
            else:
                turnRight(tmpI)
            if self.adjCount > 2:
                dist = int(round(a[1]*.4/pxPerCm))
                if dist > 0:
                    forward(dist)
                self.adjCount = 0
            else:
                dist = int(round(a[1]*.4/pxPerCm))
                if dist > 0:
                    forward(dist)
        self.adjCount += 1
        self.adjTimer.start(6000)
        
    def clearInstr(self):
        self.pathInstrCounter = len(self.pathInstr)
        
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
            self.path1Instr, self.path1InstrAdj = self.getPathInstructions(self.path1, 0)
            self.path2Instr, self.path2InstrAdj = self.getPathInstructions(self.path2, 0)
            self.path3Instr, self.path3InstrAdj = self.getPathInstructions(self.path3, 0)
            self.path4Instr, self.path4InstrAdj = self.getPathInstructions(self.path4, 0)
            self.path1Instr[len(self.path1Instr)-1] -= 12
            self.path2Instr[len(self.path1Instr)-1] -= 12
            self.path3Instr[len(self.path1Instr)-1] -= 12
            self.path4Instr[len(self.path1Instr)-1] -= 12
        print self.path1
        print self.path2
        print self.path3
        print self.path4
        print self.path4Instr
        print self.path4InstrAdj
        
    def cleanUpObject(self):
        self.pathInstrCounter = 0
        if self.ui.pathSelectSpinBox.value() == 1:
            self.pathInstr = self.path1Instr
            self.activePathAdj = self.path1InstrAdj
        if self.ui.pathSelectSpinBox.value() == 2:
            self.pathInstr = self.path2Instr
            self.activePathAdj = self.path2InstrAdj
        if self.ui.pathSelectSpinBox.value() == 3:
            self.pathInstr = self.path3Instr
            self.activePathAdj = self.path3InstrAdj
        if self.ui.pathSelectSpinBox.value() == 4:
            self.pathInstr = self.path4Instr
            self.activePathAdj = self.path4InstrAdj
        self.sendNextInstr()
        
        
    def roverLeft(self):
        turnLeft(int(self.ui.txt_distance.text()))
    def roverRight(self):
        turnRight(int(self.ui.txt_distance.text()))
    def roverForward(self):
        forward(int(self.ui.txt_distance.text()))
    def roverBackward(self):
        backward(int(self.ui.txt_distance.text()))
    def roverStop(self):
        stopRover()
    # Button Functions
    def push(self):
        if self.stat == 'Running':
            self.stat = 'Pushing'
            self.ui.status.setText('Pushing')
            connCleanup.sendall(values3)
        elif self.stat == 'Pushing':
            self.stat = 'Running'
            self.ui.status.setText('Running')
            connCleanup.sendall(values3)
        
    def raw(self):
        self.dataFormat = False
        connCleanup.sendall(values1)
        
    def cm(self):
        self.dataFormat = True
        connCleanup.sendall(values2)
        
    def wait(self):
        if self.stat == 'Running':
            self.stat = 'Waiting'
            self.ui.status.setText('Waiting')
            connDelivery.sendall(values4)
        elif self.stat == 'Waiting':
            self.stat = 'Running'
            self.ui.status.setText('Running')
            connDelivery.sendall(values4)
        
    def adcDebug(self):
        connCleanup.sendall(values5)
    
    def lineDebug(self):
        connDelivery.sendall(values6)
        
    def clearDebug(self):
        connCleanup.sendall(values7)
        connDelivery.sendall(values7)
             
    def testOn(self):
        #send test on message
        connCleanup.sendall(bytearray(['\x02', '\x21', '\x24',   0x01 , '\x00', '\x03'])) 
    def testOff(self):
        #send test off message
        connCleanup.sendall(bytearray(['\x02', '\x21', '\x24',   0x00 , '\x00', '\x03'])) 
        
class CleanupComm(QThread):

    sendADC = pyqtSignal(int)
    sendSafe = pyqtSignal(str)
    sendDone = pyqtSignal()
    encoderSend = pyqtSignal(str);
    
    def __init__(self):      
        QThread.__init__(self)
       
    def __del__(self):
        self.wait()
            
    def run(self):
        while 1:
            try:
                responseCleanup = connCleanup.recv(4)
                #print 'CleanupThread: ' + str(int(binascii.hexlify(responseCleanup[1]), 16))
                if int(binascii.hexlify(responseCleanup[1]), 16) == 32:
                    self.sendSafe.emit('Not Safe')
                    print 'Not Safe'
                elif int(binascii.hexlify(responseCleanup[1]), 16) == 33:
                    self.sendSafe.emit('Safe')
                    print 'Safe'
                elif int(binascii.hexlify(responseCleanup[1]), 16) == 36:
                    self.encoderSend.emit(str(int(binascii.hexlify(responseCleanup[2]),16)) + "/" + str(int(binascii.hexlify(responseCleanup[3]),16)))
                elif int(binascii.hexlify(responseCleanup[1]), 16) == 35:
                    self.sendADC.emit(int(binascii.hexlify(responseCleanup[2]), 16) + 256*int(binascii.hexlify(responseCleanup[3]), 16))
                elif int(binascii.hexlify(responseCleanup[1]), 16) == 100:
                    self.sendDone.emit()
                    
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

        
class DeliveryComm(QThread):

    sendLine = pyqtSignal(int)
    
    def __init__(self):      
        QThread.__init__(self)
       
    def __del__(self):
        self.wait()
            
    def run(self):
        while 1:
            try:
                responseDelivery = connDelivery.recv(4)
                if int(binascii.hexlify(responseDelivery[1]), 16) == 34:
                    self.sendLine.emit(int(binascii.hexlify(responseDelivery[2]), 16))                
                elif int(binascii.hexlify(responseDelivery[1]), 16) == 36:
                    print 'Intersection'
                else:
                    print 'DeliveryThread: ' + str(int(binascii.hexlify(responseDelivery[1]), 16))
                
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
        
def turnRight(argDeg):
    if int(argDeg) < 1:
        argDeg = 1
    val = int(argDeg) + 5
    #print 'Right turn sending: ', val
    val = chr(val)
    connCleanup.sendall(bytearray(['\x02', '\x21', '\x0D',   val , '\x00', '\x03']))             
def turnLeft(argDeg):
    if int(argDeg) < 1:
        argDeg = 1
    val = int(argDeg) + 5
    #print 'Left turn sending: ' + str(int(val))
    val = chr(val)
    connCleanup.sendall(bytearray(['\x02', '\x21', '\x0E',   val , '\x00', '\x03']))
def forward(argDist):
    argDist = argDist*.8
    if int(argDist) < 1:
        argDist = 1    
    val = int(argDist) + 5
    #print 'Forward sending: ' + str(int(val))
    val = chr(val)
    connCleanup.sendall(bytearray(['\x02', '\x21', '\x0B',   val , '\x00', '\x03']))
def backward(argDist):
    if int(argDist) < 1:
        argDist = 1
    val = int(argDist) + 5
    #print 'Backward sending: ' + str(int(val))
    val = chr(val)
    connCleanup.sendall(bytearray(['\x02', '\x21', '\x0C',   val , '\x00', '\x03']))
def stopRover():
    connCleanup.sendall(bytearray(['\x02', '\x21', '\x00', '\x00', '\x00', '\x03']))
    
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
s.settimeout(10)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(1)
connCleanup, addrCleanup = s.accept()
greeting = connCleanup.recv(8)
print 'Connected by', addrCleanup
print greeting


connDelivery, addrDelivery = s.accept()
greeting = connDelivery.recv(8)
print 'Connected by', addrDelivery
print greeting

values1 = bytearray(['\x02', '\x34', '\x15', '\x00', '\x00', '\x03']) # Raw Data
values2 = bytearray(['\x02', '\x34', '\x16', '\x00', '\x00', '\x03']) # CM Data
values3 = bytearray(['\x02', '\x34', '\x17', '\x00', '\x00', '\x03']) # Push
values4 = bytearray(['\x02', '\x14', '\x09', '\x08', '\x00', '\x03']) # Wait
values5 = bytearray(['\x02', '\x74', '\x41', '\x00', '\x00', '\x03']) # Enable adc debugging
values6 = bytearray(['\x02', '\x74', '\x42', '\x00', '\x00', '\x03']) # Enable line sensor degbugging
values7 = bytearray(['\x02', '\x74', '\x40', '\x00', '\x00', '\x03']) # Clear Debugging
            
#if __name__ == '__main_GUI__':
app = QApplication(sys.argv)
myapp = MainApp()
#myapp.resize(950,600)
myapp.move(0,0)
myapp.show()
sys.exit(app.exec_())
