import warnings
import tkinter as tk
from tkinter import ttk
import json

def split(filePath, splitNum, fileSplitPage):
    # Create UI indicator
    label1 = tk.Label(fileSplitPage, text="Splitting files...")
    label1.pack()

    progressBar = ttk.Progressbar(fileSplitPage, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    fileCount = 1
    print(f"Splitting file using chunk sizes of {splitNum}")
    with open(filePath, 'r', encoding='utf-8', errors='ignore') as f:
        # Read full content and parse as JSON
        data = json.loads(f.read())
    # Split into chunks of splitNum
    for i in range(0, len(data), splitNum):
        chunk = data[i:i + splitNum]
        outPath = f"{filePath}_part{fileCount}.txt"
        with open(outPath, 'w', encoding='utf-8') as out:
            json.dump(chunk, out, ensure_ascii=False, indent=2)
        fileCount += 1

    progressBar.stop()
    fileSplitPage.destroy()
