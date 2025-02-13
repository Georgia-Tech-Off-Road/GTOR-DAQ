import socket

# create a socket object
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_ip = "127.0.0.1"  # replace with the server's IP address
server_port = 8000  # replace with the server's port number
# establish connection with server
client.connect((server_ip, server_port))

while True:
    # receive message from the server
    response = client.recv(1024)
    response = response.decode("utf-8")
    print(f"Received: {response}")


