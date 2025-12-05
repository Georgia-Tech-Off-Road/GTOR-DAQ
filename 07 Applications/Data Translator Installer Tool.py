import os
import importlib.util #for importing
import tkinter as tk
import requests
import subprocess
import sys
from tkinter import filedialog

#define a variable to hold install path (needs to be seen by all functions (didn't wanna mess with passing this around between lambdas))
installPath = os.getcwd()

def install(package):
    subprocess.run([sys.executable, "-m", "pip", "install", "--no-input", package], check=True)

#List of libraries/modules to be installed
libraryNameList = []

#Appends libraries/modules to list
def imports(savePath):
    file = open(savePath, encoding="ISO-8859-1") #only encoding that seems to work
    for line in file:
        line = line.strip()
        if not line:
            continue
        if line == "#INSTALLER IMPORTS FINISHED":
            break
        if line.startswith("import "):
            lineList = line.split()
            if lineList[1] not in libraryNameList:
                libraryNameList.append(lineList[1])
        elif line.startswith("from"):
            lineList = line.split()
            if lineList[1] not in libraryNameList:
                libraryNameList.append(lineList[1])
    
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
            localSavePath = os.path.join(saveFolder, itemPath) #creates save path
            if itemType == 'file': #if item is a FILE
                os.makedirs(os.path.dirname(localSavePath), exist_ok=True)
                downloadFile(gitUrl, itemPath, localSavePath)
            elif itemType == 'dir': #if item is a FOLDER
                downloadFolder(gitUrl, itemPath, saveFolder)

def installApplication(folderName):
    global installPath
    #Calls downloadFolder function to download files to saveFolder
    downloadFolder(gitUrl, folderName, installPath)

    #Loops through items in libraryNameList to import necessary modules/libraries
    for library in libraryNameList:
        print(f"Processing: '{library}'")
        if importlib.util.find_spec(library):
            print(f"{library} is already installed. Skipping...")
            continue
        try:
            result = os.system(f"py -m pip install --no-input {library}")
        except:
            result = os.system(f"python -m pip install --no-input {library}")
        if result == 0:
            print(f"Successfully installed {library}.")
        else:
            print(f"Failed to install {library}. Error code: {result}")
        

    print("Done!")

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
    applicationButton.config(command=lambda f=str(application["path"]) : installApplication(f))
    applicationButton.grid(row=placeCounter // 6, column=placeCounter % 6, sticky="w", padx=5, pady=2)
    placeCounter+=1

root.mainloop()



