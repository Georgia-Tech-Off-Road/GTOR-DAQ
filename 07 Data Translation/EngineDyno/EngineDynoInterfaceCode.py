from EngineDyno.EngineDynoSupporterCode import DynoRunCode
from EngineDyno.EngineDynoSupporterCode import Globals



#try to open serial ports
try:
    Globals.engineDyno = serial.Serial(port='COM13', baudrate=19200)
    Globals.engineDynoSerialBool = True
except:
    Globals.engineDynoSerialBool = False


#create serial link with engine dyno
def openSerialConnection(serialConnection, connectionButton, connectioBool):
    if Globals.engineDynoSerialBool:
        return
    try:
        Globals.engineDyno = serial.Serial(port='COM13', baudrate=19200)
        connectionButton['bg'] = "green"
        Globals.engineDynoSerialBool = True
    except:
        connectionButton['bg'] = "red"
        Globals.engineDynoSerialBool = False

def startEngineDyno(root):
    engineDynoThread = threading.Thread(target = DynoRunCode.startDynoRun, args =(root,))
    engineDynoThread.start()

# Create Engine Dyno Label
engineDynoSerialButton = tk.Button(frame, text="Engine Dyno Serial Connection", command = lambda : openSerialConnection(Globals.engineDyno, engineDynoSerialButton, Globals.engineDynoSerialBool), bg="red" if not Globals.engineDynoSerialBool else "green")
engineDynoSerialButton.pack()

#create start Dyno Run Button
startDynoRunButton = tk.Button(frame, text="Start Dyno Run", command = lambda : startEngineDyno(root))
startDynoRunButton.pack()


root.mainloop()

            


