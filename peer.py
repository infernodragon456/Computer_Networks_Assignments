import socket
import threading
import json
import time
import random
import logging
from datetime import datetime
import hashlib

class PeerNode:
    def __init__(self, host, port, config_file):
        self.host = host
        self.port = port
        self.seeds = self.read_config(config_file)
        self.connected_peers = {}  # {(ip, port): socket}
        self.message_list = {}  # {msg_hash: set(peers_who_have_message)}
        
        # Setup TCP server socket
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((host, port))
        self.server.listen(5)
        print(f"Peer node starting on {host}:{port}")
        
    def read_config(self, config_file):
        seeds = []
        with open(config_file, 'r') as f:
            for line in f:
                if line.strip() and not line.startswith('#'):
                    ip, port = line.strip().split(':')
                    seeds.append((ip, int(port)))
        return seeds
        
    def start(self):
        # Start accepting connections
        threading.Thread(target=self.accept_connections, daemon=True).start()
        
        # Register with (n/2)+1 random seeds
        num_seeds = len(self.seeds)
        num_connections = (num_seeds // 2) + 1
        selected_seeds = random.sample(self.seeds, num_connections)
        print(f"Connecting to {num_connections} seeds: {selected_seeds}")
        
        for seed in selected_seeds:
            self.register_with_seed(seed)
            
        # Start message input loop
        self.input_loop()
        
    def accept_connections(self):
        while True:
            try:
                client_sock, address = self.server.accept()
                print(f"Accepted connection from {address}")
                threading.Thread(target=self.handle_peer_connection, 
                               args=(client_sock, address), 
                               daemon=True).start()
            except Exception as e:
                print(f"Error accepting connection: {e}")
                
    def handle_peer_connection(self, sock, address):
        """Handle incoming peer connection"""
        try:
            self.connected_peers[address] = sock
            while True:
                data = sock.recv(1024).decode()
                if not data:
                    break
                    
                message = json.loads(data)
                if message['type'] == 'gossip':
                    self.handle_gossip(message, address)
                    
        except Exception as e:
            print(f"Error handling peer {address}: {e}")
        finally:
            if address in self.connected_peers:
                del self.connected_peers[address]
            sock.close()
            
    def register_with_seed(self, seed):
        try:
            # Connect to seed with TCP
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect(seed)
            
            # Send registration
            message = {
                'type': 'register',
                'ip': self.host,
                'port': self.port
            }
            sock.send(json.dumps(message).encode())
            
            # Wait for peer list
            data = sock.recv(1024).decode()
            if data:
                response = json.loads(data)
                if response['type'] == 'peers':
                    self.connect_to_peers(response['peers'])
                    
            sock.close()
            print(f"Registered with seed {seed}")
        except Exception as e:
            print(f"Failed to register with seed {seed}: {e}")
            
    def connect_to_peers(self, peers):
        for peer in peers:
            peer = tuple(peer)  # Convert list to tuple
            if peer != (self.host, self.port) and peer not in self.connected_peers:
                try:
                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    sock.connect(peer)
                    print(f"Connected to peer {peer}")
                    self.connected_peers[peer] = sock
                    # Start thread to handle messages from this peer
                    threading.Thread(target=self.handle_peer_connection, 
                                  args=(sock, peer),
                                  daemon=True).start()
                except Exception as e:
                    print(f"Failed to connect to peer {peer}: {e}")
                    
    def input_loop(self):
        print("\nEnter messages to broadcast (Ctrl+C to exit):")
        while True:
            try:
                message = input("> ")
                if message:
                    self.broadcast_message(message)
            except KeyboardInterrupt:
                print("\nExiting...")
                break
            
    def broadcast_message(self, message):
        """Broadcast a message to peers who haven't seen it yet"""
        msg_data = {
            'type': 'gossip',
            'message': f"{self.host}:{self.port} - {message}",  # Include sender info in message
            'timestamp': time.time()  # Add timestamp for uniqueness
        }
        msg_str = json.dumps(msg_data)
        msg_hash = hashlib.sha256(msg_str.encode()).hexdigest()
        
        # Initialize message in ML if it's new
        if msg_hash not in self.message_list:
            self.message_list[msg_hash] = set()
            
        print(f"Broadcasting message {msg_hash[:8]}")
        dead_peers = []
        
        for peer, sock in self.connected_peers.items():
            # Only send to peers who haven't seen this message
            if peer not in self.message_list[msg_hash]:
                try:
                    sock.send(msg_str.encode())
                    self.message_list[msg_hash].add(peer)
                    print(f"Sent to {peer}")
                except Exception as e:
                    print(f"Failed to send to {peer}: {e}")
                    dead_peers.append(peer)
                    
        # Clean up dead connections
        for peer in dead_peers:
            if peer in self.connected_peers:
                self.connected_peers[peer].close()
                del self.connected_peers[peer]
                
    def handle_gossip(self, message_data, sender):
        """Handle incoming gossip message"""
        msg_str = json.dumps(message_data)
        msg_hash = hashlib.sha256(msg_str.encode()).hexdigest()
        
        # If we haven't seen this message before
        if msg_hash not in self.message_list:
            print(f"\nReceived: {message_data['message']}")
            print("> ", end='', flush=True)  # Restore input prompt
            
            # Add to message list and mark as received from sender
            self.message_list[msg_hash] = {sender}
            
            # Forward to all peers except the sender
            for peer, sock in self.connected_peers.items():
                if peer != sender and peer not in self.message_list[msg_hash]:
                    try:
                        sock.send(msg_str.encode())
                        self.message_list[msg_hash].add(peer)
                        print(f"Forwarded to {peer}")
                    except Exception as e:
                        print(f"Failed to forward to {peer}: {e}")
        else:
            print(f"\nDuplicate message {msg_hash[:8]}")
            print("> ", end='', flush=True)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 4:
        print("Usage: python peer.py <host> <port> <config_file>")
        sys.exit(1)
        
    host = sys.argv[1]
    port = int(sys.argv[2])
    config_file = sys.argv[3]
    
    peer = PeerNode(host, port, config_file)
    peer.start() 