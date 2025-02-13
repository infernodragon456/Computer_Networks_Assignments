import socket
import threading
import json
import time
import random
import logging
from datetime import datetime

class PeerNode:
    def __init__(self, host, port, config_file):
        self.host = host
        self.port = port
        self.seeds = self.read_config(config_file)
        self.connected_peers = set()  # Just store (ip, port) tuples
        self.message_list = set()  # Store message hashes we've seen
        
        # Setup UDP socket
        self.node = socket.socket(type=socket.SOCK_DGRAM)
        self.node.bind((host, port))
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
        # Start receiving messages
        threading.Thread(target=self.receive_message, daemon=True).start()
        
        # Register with (n/2)+1 random seeds
        num_seeds = len(self.seeds)
        num_connections = (num_seeds // 2) + 1
        selected_seeds = random.sample(self.seeds, num_connections)
        print(f"Connecting to {num_connections} seeds: {selected_seeds}")
        
        for seed in selected_seeds:
            self.register_with_seed(seed)
            
        # Start message input loop
        self.input_loop()
        
    def receive_message(self):
        while True:
            try:
                message, address = self.node.recvfrom(1024)
                data = json.loads(message.decode())
                
                if data['type'] == 'peers':
                    self.handle_peer_list(data['peers'])
                elif data['type'] == 'gossip':
                    self.handle_gossip(data['message'], address)
            except Exception as e:
                print(f"Error receiving message: {e}")
                
    def register_with_seed(self, seed):
        try:
            message = {
                'type': 'register',
                'ip': self.host,
                'port': self.port
            }
            self.node.sendto(json.dumps(message).encode(), seed)
            print(f"Registered with seed {seed}")
        except Exception as e:
            print(f"Failed to register with seed {seed}: {e}")
                
    def handle_peer_list(self, peers):
        for peer in peers:
            if tuple(peer) != (self.host, self.port):
                self.connected_peers.add(tuple(peer))
        print(f"Updated peer list: {self.connected_peers}")
        
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
        msg_data = {
            'type': 'gossip',
            'message': f"{self.host}:{self.port} - {message}"
        }
        encoded_msg = json.dumps(msg_data).encode()
        
        print(f"Broadcasting to {len(self.connected_peers)} peers")
        for peer in self.connected_peers:
            try:
                self.node.sendto(encoded_msg, peer)
                print(f"Sent message to {peer}")
            except Exception as e:
                print(f"Failed to send to {peer}: {e}")
                
    def handle_gossip(self, message, sender):
        if message not in self.message_list:
            print(f"\nNew message from {sender}: {message}")
            print("> ", end='', flush=True)  # Restore input prompt
            self.message_list.add(message)
            self.broadcast_message(message)

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