import os
import requests #for accessing API
import importlib.util #for importing

#List of libraries/modules to be installed
libraryNameList = []

#Appends libraries/modules to list
def imports(savePath):
    file = open(savePath, encoding="ISO-8859-1") #only encoding that seems to work
    for line in file:
        line = line.strip()
        if not line:
            continue
        if line.startswith("import "):
            lineList = line.split()
            libraryNameList.append(lineList[1])
        elif line.startswith("from"):
            lineList = line.split()
            libraryNameList.append(lineList[1])
    
    return libraryNameList

#Accesses GitAPI to create list of folders in Github repo to be downloaded
def listFilesInFolder(gitUrl, folderPath):
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
    items = listFilesInFolder(gitUrl, folderPath)
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

def runUpdater():
    #User inputs, github URL
    gitUrl = "https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ"
    #go back two directories so we can obtain the save folder
    os.chdir("..")
    os.chdir("..")
    
    saveFolder = os.getcwd()
    
    folderPath = "7%20Data%20Translation"

    #Removes quotes if user uses them in folder paths
    folderPath = folderPath.replace("\"","")
    saveFolder = saveFolder.replace("\"","")

    #Calls downloadFolder function to download files to saveFolder
    downloadFolder(gitUrl, folderPath, saveFolder)

    #Loops through items in libraryNameList to import necessary modules/libraries
    print(libraryNameList) #unnecessary but helpful for debugging
    for library in libraryNameList:
        print(f"Processing: '{library}'")
        if importlib.util.find_spec(library):
            print(f"{library} is already installed. Skipping...")
            continue
        result = os.system(f"py -m pip install --no-input {library}")
        if result == 0:
            print(f"Successfully installed {library}.")
        else:
            print(f"Failed to install {library}. Error code: {result}")
        

    print("Done!")



