# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'sensor_debug.ui'
#
# Created: Wed Mar 16 06:42:03 2016
#      by: PyQt4 UI code generator 4.11.2
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

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(828, 148)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.distance = QtGui.QLabel(self.centralwidget)
        self.distance.setGeometry(QtCore.QRect(110, 80, 113, 32))
        self.distance.setObjectName(_fromUtf8("distance"))
        self.proximity_alert = QtGui.QLabel(self.centralwidget)
        self.proximity_alert.setGeometry(QtCore.QRect(20, 70, 41, 32))
        self.proximity_alert.setObjectName(_fromUtf8("proximity_alert"))
        self.raw_button = QtGui.QPushButton(self.centralwidget)
        self.raw_button.setGeometry(QtCore.QRect(220, 39, 61, 41))
        self.raw_button.setObjectName(_fromUtf8("raw_button"))
        self.status = QtGui.QLabel(self.centralwidget)
        self.status.setGeometry(QtCore.QRect(480, 70, 61, 32))
        self.status.setObjectName(_fromUtf8("status"))
        self.label_5 = QtGui.QLabel(self.centralwidget)
        self.label_5.setGeometry(QtCore.QRect(20, 40, 61, 32))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.cm_button = QtGui.QPushButton(self.centralwidget)
        self.cm_button.setGeometry(QtCore.QRect(220, 90, 61, 41))
        self.cm_button.setObjectName(_fromUtf8("cm_button"))
        self.label = QtGui.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(480, 40, 41, 32))
        self.label.setObjectName(_fromUtf8("label"))
        self.push_button = QtGui.QPushButton(self.centralwidget)
        self.push_button.setGeometry(QtCore.QRect(290, 60, 81, 41))
        self.push_button.setObjectName(_fromUtf8("push_button"))
        self.label_3 = QtGui.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(110, 40, 113, 32))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.wait_button = QtGui.QPushButton(self.centralwidget)
        self.wait_button.setGeometry(QtCore.QRect(380, 60, 81, 41))
        self.wait_button.setObjectName(_fromUtf8("wait_button"))
        self.label_4 = QtGui.QLabel(self.centralwidget)
        self.label_4.setGeometry(QtCore.QRect(540, 40, 113, 32))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.line_value = QtGui.QLabel(self.centralwidget)
        self.line_value.setGeometry(QtCore.QRect(540, 80, 113, 32))
        self.line_value.setObjectName(_fromUtf8("line_value"))
        self.debug_line = QtGui.QPushButton(self.centralwidget)
        self.debug_line.setGeometry(QtCore.QRect(660, 39, 71, 31))
        self.debug_line.setObjectName(_fromUtf8("debug_line"))
        self.debug_adc = QtGui.QPushButton(self.centralwidget)
        self.debug_adc.setGeometry(QtCore.QRect(660, 90, 71, 31))
        self.debug_adc.setObjectName(_fromUtf8("debug_adc"))
        self.clear_debug = QtGui.QPushButton(self.centralwidget)
        self.clear_debug.setGeometry(QtCore.QRect(740, 60, 81, 31))
        self.clear_debug.setObjectName(_fromUtf8("clear_debug"))
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.distance.setText(_translate("MainWindow", "0", None))
        self.proximity_alert.setText(_translate("MainWindow", "Safe", None))
        self.raw_button.setText(_translate("MainWindow", "raw", None))
        self.status.setText(_translate("MainWindow", "Running", None))
        self.label_5.setText(_translate("MainWindow", "Proximity:", None))
        self.cm_button.setText(_translate("MainWindow", "cm", None))
        self.label.setText(_translate("MainWindow", "Status:", None))
        self.push_button.setText(_translate("MainWindow", "Push", None))
        self.label_3.setText(_translate("MainWindow", "Current Distance:", None))
        self.wait_button.setText(_translate("MainWindow", "Wait", None))
        self.label_4.setText(_translate("MainWindow", "Current Line Status:", None))
        self.line_value.setText(_translate("MainWindow", "00000000", None))
        self.debug_line.setText(_translate("MainWindow", "Debug Line", None))
        self.debug_adc.setText(_translate("MainWindow", "Debug ADC", None))
        self.clear_debug.setText(_translate("MainWindow", "Clear Debug", None))

