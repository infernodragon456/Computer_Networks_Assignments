import socket
import threading
import json
import logging
from datetime import datetime

class SeedNode:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.peer_list = set()  # Store (ip, port) tuples
        self.lock = threading.Lock()
        
        # Setup TCP server socket
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((host, port))
        self.server.listen(5)
        print(f"Seed node starting on {host}:{port}")
        
    def start(self):
        while True:
            try:
                client_sock, address = self.server.accept()
                threading.Thread(target=self.handle_client, 
                              args=(client_sock, address),
                              daemon=True).start()
            except Exception as e:
                print(f"Error accepting connection: {e}")
                
    def handle_client(self, client_sock, address):
        try:
            data = client_sock.recv(1024).decode()
            if data:
                message = json.loads(data)
                print(f"Received from {address}: {message}")
                
                if message['type'] == 'register':
                    self.register_peer(message['ip'], message['port'], client_sock)
                    
        except Exception as e:
            print(f"Error handling client {address}: {e}")
        finally:
            client_sock.close()
            
    def register_peer(self, ip, port, client_sock):
        with self.lock:
            peer = (ip, port)
            self.peer_list.add(peer)
            print(f"New peer registered: {ip}:{port}. Current peers: {self.peer_list}")
            
            # Send current peer list to the new peer
            response = {
                'type': 'peers',
                'peers': list(self.peer_list)
            }
            client_sock.send(json.dumps(response).encode())

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python seed.py <host> <port>")
        sys.exit(1)
        
    host = sys.argv[1]
    port = int(sys.argv[2])
    seed = SeedNode(host, port)
    seed.start() 