# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'motor_gui.ui'
#
# Created: Thu Mar 31 01:25:47 2016
#      by: PyQt4 UI code generator 4.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_MotorGUI(object):
    def setupUi(self, MotorGUI):
        MotorGUI.setObjectName(_fromUtf8("MotorGUI"))
        MotorGUI.resize(1016, 639)
        self.main_window = QtGui.QWidget(MotorGUI)
        self.main_window.setObjectName(_fromUtf8("main_window"))
        self.gridLayoutWidget = QtGui.QWidget(self.main_window)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(160, 40, 614, 197))
        self.gridLayoutWidget.setObjectName(_fromUtf8("gridLayoutWidget"))
        self.gridLayout = QtGui.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setMargin(0)
        self.gridLayout.setHorizontalSpacing(6)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.txt_degree = QtGui.QSpinBox(self.gridLayoutWidget)
        self.txt_degree.setMaximum(4)
        self.txt_degree.setObjectName(_fromUtf8("txt_degree"))
        self.gridLayout.addWidget(self.txt_degree, 0, 2, 1, 1)
        self.bttn_turnRight = QtGui.QPushButton(self.gridLayoutWidget)
        self.bttn_turnRight.setObjectName(_fromUtf8("bttn_turnRight"))
        self.gridLayout.addWidget(self.bttn_turnRight, 1, 2, 1, 1)
        self.bttn_turnLeft = QtGui.QPushButton(self.gridLayoutWidget)
        self.bttn_turnLeft.setObjectName(_fromUtf8("bttn_turnLeft"))
        self.gridLayout.addWidget(self.bttn_turnLeft, 1, 0, 1, 1)
        self.bttn_forward = QtGui.QPushButton(self.gridLayoutWidget)
        self.bttn_forward.setObjectName(_fromUtf8("bttn_forward"))
        self.gridLayout.addWidget(self.bttn_forward, 0, 1, 1, 1)
        self.bttn_backward = QtGui.QPushButton(self.gridLayoutWidget)
        self.bttn_backward.setObjectName(_fromUtf8("bttn_backward"))
        self.gridLayout.addWidget(self.bttn_backward, 2, 1, 1, 1)
        self.bttn_stop = QtGui.QPushButton(self.gridLayoutWidget)
        self.bttn_stop.setObjectName(_fromUtf8("bttn_stop"))
        self.gridLayout.addWidget(self.bttn_stop, 1, 1, 1, 1)
        self.txt_distance = QtGui.QLineEdit(self.gridLayoutWidget)
        self.txt_distance.setObjectName(_fromUtf8("txt_distance"))
        self.gridLayout.addWidget(self.txt_distance, 0, 0, 1, 1)
        self.gridLayout.setColumnMinimumWidth(0, 200)
        self.gridLayout.setColumnMinimumWidth(1, 200)
        self.gridLayout.setColumnMinimumWidth(2, 200)
        self.label = QtGui.QLabel(self.main_window)
        self.label.setGeometry(QtCore.QRect(160, 20, 58, 21))
        self.label.setObjectName(_fromUtf8("label"))
        self.label_2 = QtGui.QLabel(self.main_window)
        self.label_2.setGeometry(QtCore.QRect(570, 20, 58, 17))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        MotorGUI.setCentralWidget(self.main_window)

        self.retranslateUi(MotorGUI)
        QtCore.QMetaObject.connectSlotsByName(MotorGUI)

    def retranslateUi(self, MotorGUI):
        MotorGUI.setWindowTitle(_translate("MotorGUI", "Motor GUI", None))
        self.bttn_turnRight.setText(_translate("MotorGUI", "Turn Right", None))
        self.bttn_turnLeft.setText(_translate("MotorGUI", "Turn Left", None))
        self.bttn_forward.setText(_translate("MotorGUI", "Forward", None))
        self.bttn_backward.setText(_translate("MotorGUI", "Backward", None))
        self.bttn_stop.setText(_translate("MotorGUI", "STOP", None))
        self.label.setText(_translate("MotorGUI", "Distance", None))
        self.label_2.setText(_translate("MotorGUI", "Degree", None))

