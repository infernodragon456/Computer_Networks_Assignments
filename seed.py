import socket
import threading
import json
from datetime import datetime

class SeedNode:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.peer_list = set()  # Store (ip, port) tuples
        self.lock = threading.Lock()
        self.log_file = f"seed_{host}_{port}.log"
        self.output_file = "output.txt"
        
        # Setup TCP server socket
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((host, port))
        self.server.listen(5)
        self.log_message(f"Seed node starting on {host}:{port}")
        
    def log_message(self, message):
        """Log messages to console, seed log file, and output file."""
        timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")
        log_entry = f"{timestamp} [{self.host}:{self.port}] {message}"
        
        # Print to console
        print(log_entry)
        
        # Write to seed-specific log file
        with open(self.log_file, 'a') as f:
            f.write(log_entry + "\n")
            
        # Write to common output file
        with open(self.output_file, 'a') as f:
            f.write(log_entry + "\n")
        
    def start(self):
        while True:
            try:
                client_sock, address = self.server.accept()
                self.log_message(f"Accepted connection from {address}")
                threading.Thread(target=self.handle_client, 
                              args=(client_sock, address),
                              daemon=True).start()
            except Exception as e:
                self.log_message(f"Error accepting connection: {e}")
                
    def handle_client(self, client_sock, address):
        try:
            data = client_sock.recv(1024).decode()
            if not data:
                self.log_message("Received empty request, ignoring.")
                return
                
            message = json.loads(data)
            self.log_message(f"Received from {address}: {message}")
            
            if message['type'] == 'register':
                self.register_peer(message['ip'], message['port'], client_sock)
                
            elif message['type'] == 'get_peers':
                with self.lock:
                    response = {
                        'type': 'peers',
                        'peers': list(self.peer_list)
                    }
                client_sock.send(json.dumps(response).encode())
                self.log_message(f"Sent peer list to {address}")
                
            elif message['type'] == 'dead_peer':
                self.remove_dead_peer(message['ip'], message['port'])
                self.log_message(f"Processed dead peer notification: {message['message']}")
                
        except Exception as e:
            self.log_message(f"Error handling client {address}: {e}")
        finally:
            client_sock.close()
            
    def register_peer(self, ip, port, client_sock):
        """Register a new peer and send current peer list."""
        with self.lock:
            peer = (ip, port)
            self.peer_list.add(peer)
            self.log_message(f"New peer registered: {ip}:{port}")
            self.log_message(f"Current peers: {self.peer_list}")
            
            # Send current peer list to the new peer
            response = {
                'type': 'peers',
                'peers': list(self.peer_list),
                'status': 'success'
            }
            client_sock.send(json.dumps(response).encode())

    def remove_dead_peer(self, ip, port):
        """Remove a dead peer from the peer list."""
        with self.lock:
            dead_peer = (ip, port)
            if dead_peer in self.peer_list:
                self.peer_list.remove(dead_peer)
                self.log_message(f"Removed dead peer: {ip}:{port}")
                self.log_message(f"Current peers: {self.peer_list}")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python seed.py <host> <port>")
        sys.exit(1)
        
    host = sys.argv[1]
    port = int(sys.argv[2])
    
    # Add seed to config.txt if not already present
    config = f"{host}:{port}\n"
    with open("config.txt", "r+") as file:
        lines = file.readlines()
        if config not in lines:
            file.seek(0, 2)  # Go to end of file
            file.write(config)
    
    seed = SeedNode(host, port)
    seed.start() 