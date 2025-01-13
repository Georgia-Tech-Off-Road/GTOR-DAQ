import socket
import threading
import time

from XbeeDataTranslator import XbeeDataTranslator

global currentData

# receive data from the client
def handleClient(client_socket):
    while True:
        client_socket.send("test".encode("utf-8"))
        time.sleep(1)

# create a socket object
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_ip = "127.0.0.1"
port = 8000

# bind the socket to a specific address and port
server.bind((server_ip, port))

# listen for incoming connections
server.listen(20)
print(f"Listening on {server_ip}:{port}")

XbeeDataTranslator.initialSetup(configFileName)

while True:
    currentData = XbeeDataTranslator.computeNewValues(dataString)
    # accept incoming connections
    client_socket, client_address = server.accept()
    thread = threading.Thread(target=handleClient, args=(client_socket,))
    thread.start()
    print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

