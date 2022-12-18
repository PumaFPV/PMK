import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QTextEdit, QPushButton, QComboBox, QMessageBox
import serial
import win32com.client

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.textEdit = QTextEdit(self)
        self.textEdit.move(10, 10)
        self.textEdit.resize(280, 80)

        # Create a QComboBox for selecting the serial port
        self.portComboBox = QComboBox(self)
        self.portComboBox.move(10, 100)
        self.portComboBox.resize(100, 25)

        # Find available serial ports and add them to the combo box
        self.updatePorts()

        # Add a refresh button to update the list of available serial ports
        self.refreshButton = QPushButton('Refresh', self)
        self.refreshButton.move(120, 100)
        self.refreshButton.clicked.connect(self.updatePorts)

        # Create a QComboBox for selecting the serial speed
        self.speedComboBox = QComboBox(self)
        self.speedComboBox.move(230, 100)
        self.speedComboBox.resize(60, 25)

        # Add a list of available serial speeds to the combo box
        self.speedComboBox.addItems(['9600', '19200', '38400', '57600', '115200'])

        self.sendButton = QPushButton('Send', self)
        self.sendButton.move(10, 140)
        self.sendButton.clicked.connect(self.sendMessage)

        self.setGeometry(300, 300, 300, 200)
        self.setWindowTitle('Serial Communication')
        self.show()

    def updatePorts(self):
        # Clear the current list of ports in the combo box
        self.portComboBox.clear()

        # Find available serial ports and add them to the combo box
        wmi = win32com.client.GetObject("winmgmts:")
        for port in wmi.InstancesOf("Win32_SerialPort"):
            self.portComboBox.addItem(port.Name)

    def sendMessage(self):
        message = self.textEdit.toPlainText()
        port = self.portComboBox.currentText()
        speed = int(self.speedComboBox.currentText())
        try:
            self.serial = serial.Serial(port, speed)
            self.serial.write(message.encode())
            self.serial.close()
        except serial.serialutil.SerialException:
            QMessageBox.warning(self, 'Error', 'Invalid serial port')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())