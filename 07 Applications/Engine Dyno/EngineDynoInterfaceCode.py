import serial
import tkinter as tk
import threading
import serial.tools.list_ports

#ADD DATA STUFF


import EngineDynoSupporterCode

#INSTALLER IMPORTS FINISHED


#define main page
root = tk.Tk()
root.title("Main Page")
root.geometry("900x600")

#define a frame for all the com ports
portsFrame = tk.Frame(root)
portsFrame.pack()

#function to connect to a com port
def connectToSerialPort(port, connectionButton):
    #try to open serial ports
    try:
        EngineDynoSupporterCode.Globals.engineDyno = serial.Serial(port=port, baudrate=19200)
        connectionButton['bg'] = "green"
        EngineDynoSupporterCode.Globals.engineDynoSerialBool = True
    except:
        connectionButton['bg'] = "red"
        EngineDynoSupporterCode.Globals.engineDynoSerialBool = False

#function to show com ports
def refreshComPorts(frame):
    for widget in portsFrame.winfo_children():
        widget.destroy()
    
    ports = serial.tools.list_ports.comports()

    placeCounter = 0

    for port in ports:
        tk.Label(frame, text=port.device).grid(row = placeCounter // 6, column = placeCounter % 6, sticky = "w", padx = 5, pady=2)
        placeCounter += 1
        button = tk.Button(frame, text="Connect")
        button.config(command=lambda p=port.device, b=button: connectToSerialPort(p, b))
        button.grid(row=placeCounter // 6, column=placeCounter % 6, sticky="w", padx=5, pady=2)
        placeCounter+=1

def startEngineDyno(root):
    engineDynoThread = threading.Thread(target = EngineDynoSupporterCode.DynoRunCode.startDynoRun, args =(root,))
    engineDynoThread.start()


#pack the main page
frame = tk.Frame(root)
frame.pack(pady=20)

#pack the portsFrame
portsFrame.pack()

#create start Dyno Run Button
startDynoRunButton = tk.Button(frame, text="Start Dyno Run", command = lambda : startEngineDyno(root))
startDynoRunButton.pack()

#create refresh com port button
refreshComPortButton = tk.Button(frame, text="Refresh Com Ports", command = lambda : refreshComPorts(portsFrame))
refreshComPortButton.pack()


root.mainloop()

            


