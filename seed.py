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
        
        # Setup UDP socket
        self.node = socket.socket(type=socket.SOCK_DGRAM)
        self.node.bind((host, port))
        print(f"Seed node starting on {host}:{port}")
        
    def start(self):
        while True:
            try:
                data, address = self.node.recvfrom(1024)
                message = json.loads(data.decode())
                
                if message['type'] == 'register':
                    self.register_peer(message['ip'], message['port'], address)
                elif message['type'] == 'get_peers':
                    self.send_peer_list(address)
            except Exception as e:
                print(f"Error handling message: {e}")
                
    def register_peer(self, ip, port, address):
        with self.lock:
            peer = (ip, port)
            self.peer_list.add(peer)
            print(f"New peer registered: {ip}:{port}. Current peers: {self.peer_list}")
            
            # Send current peer list to the new peer
            response = {
                'type': 'peers',
                'peers': list(self.peer_list)
            }
            self.node.sendto(json.dumps(response).encode(), address)
            
    def send_peer_list(self, address):
        with self.lock:
            peers = list(self.peer_list)
            response = {
                'type': 'peers',
                'peers': peers
            }
            print(f"Sending peer list: {response}")
            self.node.sendto(json.dumps(response).encode(), address)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python seed.py <host> <port>")
        sys.exit(1)
        
    host = sys.argv[1]
    port = int(sys.argv[2])
    seed = SeedNode(host, port)
    seed.start() 