import socket
import threading
import time

from XbeeDataTranslator import XbeeDataTranslator

global currentData

#testing function thread can call to feed data to clients
def testerFunction(file):
    global currentData
    
    while True:
        currentData = XbeeDataTranslator.computeNewValues(file.readline())

# receive data from the client
def handleClient(client_socket):
    global currentData
    
    while True:
        client_socket.send(currentData.encode("utf-8"))
        time.sleep(0.000000000000000000000000000000000000000001)

# create a socket object
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_ip = "127.0.0.1"
port = 8000

# bind the socket to a specific address and port
server.bind((server_ip, port))

# listen for incoming connections
server.listen(20)
print(f"Listening on {server_ip}:{port}")

#run initial setup for xbee translator
XbeeDataTranslator.initialSetup("Config.txt")

#open file for testing data
file = open("2024-10-26 14_12_53")

#create a thread for the data tester thing
testerThread = threading.Thread(target=testerFunction, args=(file,))
testerThread.start()

#main loop
while True:
    # accept incoming connections
    client_socket, client_address = server.accept()
    thread = threading.Thread(target=handleClient, args=(client_socket,))
    thread.start()
    print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

