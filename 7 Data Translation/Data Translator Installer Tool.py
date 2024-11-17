import os

os.system("py -m pip install --no-input requests")

import requests

libraryNameList = []

def imports(savePath):
    file = open(savePath, encoding = 'ISO-8859-1')
    for line in file:
        if "import" in line:
            lineList = line.strip().split(" ")
            libraryNameList.append(lineList[1])
            if "from" in line:
                libraryNameList.append(lineList [-1])
        if len(line.strip()) == 0:
            break
    file.close()


def listFilesInFolder(gitUrl, folderPath):
    apiUrl = f"{gitUrl.replace('github.com', 'api.github.com/repos')}/contents/{folderPath}"
    response = requests.get(apiUrl)
    items = response.json()
    if isinstance(items, list):
        return items
    else:
        print(f"Didn't work loser! Why? Well, {items}")
        return []

def downloadFile(gitUrl, filePath, savePath):
    if filePath == "7 Data Translation/Data Translator Installer Tool.py":
        print(f"Skipping {filePath}...")
        return
    url = f"{gitUrl}/raw/main/{filePath}"
    response = requests.get(url)
    if response.status_code == 200:
        os.makedirs(os.path.dirname(savePath), exist_ok=True)
        with open(savePath, 'wb') as f:
            f.write(response.content)
        print(f"Downloaded {filePath} to {savePath}")
        imports(savePath)
    else:
        print(f"Failed to download {filePath}")

def downloadFolder(gitUrl, folderPath, saveFolder):
    items = listFilesInFolder(gitUrl, folderPath)
    if items:
        for item in items:
            itemPath = item['path']
            itemType = item['type']
            localSavePath = os.path.join(saveFolder, itemPath)
            if itemType == 'file':
                os.makedirs(os.path.dirname(localSavePath), exist_ok=True)
                downloadFile(gitUrl, itemPath, localSavePath)
            elif itemType == 'dir':
                downloadFolder(gitUrl, itemPath, saveFolder)


gitUrl = "https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ"
saveFolder = input(r"Enter the local folder path where the file will be saved (e.g., C:\Users\<name>\Documents): ")
folderPath = input(r"Enter the folder path within the GitHub repository that you want to download (e.g., 7%20Data%20Translation). If you want to download the Data Translation folder, type Data: ")

if folderPath == "Data":
    folderPath = "7%20Data%20Translation"

folderPath = folderPath.replace("\"","")
saveFolder = saveFolder.replace("\"","")


#folderPath = "7%20Data%20Translation"
#saveFolder = r"C:\Users\josep\Downloads"

#downloadFolder(gitUrl, folderPath, saveFolder)

import subprocess

def install_libraries(library_list):
    for library in library_list:
        try:
            subprocess.run(
                ["python", "-m", "pip", "install", "--no-input", library],
                check=True,  # Raises CalledProcessError if the command fails
                capture_output=True,  # Captures stdout and stderr
                text=True  # Decodes output to text
            )
            print(f"Successfully installed {library}.")
        except subprocess.CalledProcessError as e:
            print(f"Failed to install {library}. Error: {e.stderr}")

# Example usage
libraryNameList = ["numpy", "pandas", "nonexistentlibrary"]
install_libraries(libraryNameList)


print("Done!")

