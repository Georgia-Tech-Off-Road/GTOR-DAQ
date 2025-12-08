import os
import importlib.util #for importing
import tkinter as tk
import requests
import subprocess
import sys
import zipfile
from tkinter import filedialog

#define a variable to hold install path (needs to be seen by all functions (didn't wanna mess with passing this around between lambdas))
installPath = os.getcwd()

#List of libraries/modules to be installed
libraryNameList = []

#list for add data
addDataList =[]

#application being installed
appBeingInstalled = None

#requires your python to be on path and accessible as python
def build_executable(script_path):
    #make sure local machine has all the libraries downloaded
    for library in libraryNameList:
        install(library)
    print("Building executable with PyInstaller...")

    script_dir = os.path.dirname(script_path)
    exe_name = os.path.splitext(os.path.basename(script_path))[0] + ".exe"

    #think u need to add an --add-data every time you create a new folder we import stuff from (not sure what this means for subfolder stuff)
    cmd = [
        "python",
        "-m", "PyInstaller",
        "--onefile",
        "--name", exe_name,
        script_path
    ]
    for addData in addDataList:
        cmd.extend(["--add-data", f"{addData};{addData}"])
    
    #add additional imports our code detects
    for library in libraryNameList:
        cmd.extend(["--hidden-import", library])

    print(cmd)
    result = subprocess.run(cmd, capture_output=True, text = True, cwd=script_dir)
    print(result)
    exe_path = os.path.normpath(os.path.join(script_dir, exe_name))
    print("Executable created at:", exe_path)
    return exe_path

def install(package):
    try:
        subprocess.run(["python", "-m", "pip", "install", "--no-input", package], check=True)
    except:
        if (package != "pycuda.driver "):
            print(str(package) + " failed to install!")



#Appends libraries/modules to list
def imports(savePath):
    if ".py" not in savePath.split("/")[-1] or "__init__" in savePath.split("/")[-1] or "-checkpoint" in savePath.split("/")[-1]:
        return
    print(savePath + "IMPORTS")
    file = open(savePath, encoding="ISO-8859-1") #only encoding that seems to work
    addDataTime = False
    for line in file:
        print(line)
        if "#INSTALLER IMPORTS FINISHED" in line:
            break
        if '#ADD DATA STUFF' in line:
            addDataTime = True
            continue
        line = line.strip()
        if not line:
            continue
        if not addDataTime:
            if line.startswith("import "):
                lineList = line.split()
                if lineList[1] not in libraryNameList:
                    libraryNameList.append(lineList[1])
            elif line.startswith("from"):
                lineList = line.split()
                if lineList[1] not in libraryNameList:
                    libraryNameList.append(lineList[1])
        else:
           if line.startswith("import "):
                lineList = line.split()
                if lineList[1] not in addDataList:
                    addDataList.append(lineList[1]) 
    
    return libraryNameList

#Accesses GitAPI to create list of folders in Github repo to be downloaded
def getFilesInFolder(gitUrl, folderPath):
    apiUrl = f"{gitUrl.replace('github.com', 'api.github.com/repos')}/contents/{folderPath}"
    response = requests.get(apiUrl)
    items = response.json()
    if isinstance(items, list):
        return items
    else:
        print(f"Didn't work loser! Tried {items}")
        return []

#Uses OS to download every file
def downloadFile(gitUrl, filePath, savePath):
    url = f"{gitUrl}/raw/main/{filePath}"
    response = requests.get(url)
    if response.status_code == 200:
        os.makedirs(os.path.dirname(savePath), exist_ok=True) #creates folder to install to
        with open(savePath, 'wb') as f:
            f.write(response.content) #Downloads file
        print(f"Downloaded {filePath} to {savePath}")
        imports(savePath)
    else:
        print(f"Failed to download {filePath}")

#If the value in the API is a folder (dir), goes deeper into API until folder is reached,
#then adds folders to be downloaded
def downloadFolder(gitUrl, folderPath, saveFolder):
    items = getFilesInFolder(gitUrl, folderPath)
    if items:
        for item in items:
            itemPath = item['path'] #folder path
            itemType = item['type'] #checks whether item is a FILE or FOLDER (directory
            localSavePath = os.path.join(saveFolder, itemPath.replace("07 Applications/", "")) #creates save path
            if itemType == 'file': #if item is a FILE
                os.makedirs(os.path.dirname(localSavePath), exist_ok=True)
                downloadFile(gitUrl, itemPath, localSavePath)
            elif itemType == 'dir': #if item is a FOLDER
                downloadFolder(gitUrl, itemPath, saveFolder)

def installApplication(folderName, name):
    global installPath, addDataList, libraryNameList
    #Calls downloadFolder function to download files to saveFolder
    downloadFolder(gitUrl, folderName, installPath)

    #code to handle the .exe setup
    mainScript = None

    os.chdir(installPath)

    os.chdir(name)

    nameFile = open("nameOfMainProgram.txt")
    mainProgramName = nameFile.readline().strip()
    nameFile.close()

    mainScript = os.getcwd() + "/" + mainProgramName
    
    exe_path = build_executable(mainScript)
    addDataList = []
    libraryNameList = []


#function to let user choose installation path
def chooseInstallationPath(label):
    global installPath
    installPath = filedialog.askdirectory()
    label.config(text = installPath)

#define github URL
gitUrl = "https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ"

#create tkinter page
root = tk.Tk()
root.title("Main Page")
root.geometry("800x800")

#create the main frame
frame = tk.Frame(root)
frame.pack(pady=20)

# Create main label
programSelectLabel = tk.Label(frame, text="Please choose a program to install")
programSelectLabel.pack()


#create a label for the install path
installPathLabel = tk.Label(frame, text = installPath)
installPathLabel.pack()

#create a button to let the user change the install path
chooseOutputDirectoryButton = tk.Button(frame, text = "Choose Installation Path", command = lambda : chooseInstallationPath(installPathLabel))
chooseOutputDirectoryButton.pack()

#get all applications available
availableApplications = getFilesInFolder(gitUrl, "07 Applications")

#create a seperate frame for the labels and buttons below
appFrame = tk.Frame(root)
appFrame.pack()

#create labels and buttons for each program
placeCounter = 0
for application in availableApplications:
    #dont display anything thats just a file
    if (application["type"] == 'file'):
        continue

    applicationName = tk.Label(appFrame, text = str(application["name"])).grid(row = placeCounter // 6, column = placeCounter % 6, sticky = "w", padx = 5, pady=2)
    applicationButton = tk.Button(appFrame, text="Install")
    placeCounter += 1
    applicationButton.config(command=lambda app_path=application.get("path"), app_name=application.get("name"): installApplication(app_path, app_name))
    applicationButton.grid(row=placeCounter // 6, column=placeCounter % 6, sticky="w", padx=5, pady=2)
    placeCounter+=1

root.mainloop()



