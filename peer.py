import socket
import threading
import json
import time
import random
from datetime import datetime
import hashlib

class PeerNode:
    def __init__(self, host, port, config_file):
        self.host = host
        self.port = port
        self.seeds = self.read_config(config_file)
        self.connected_peers = {}  # {(ip, port): socket}
        self.peers_lock = threading.Lock()
        self.message_list = {}  # {msg_hash: set(peers_who_have_message)}
        self.message_lock = threading.Lock()  # Add lock for message_list
        self.ping_failures = {}  # Track ping failures for each peer
        self.failures_lock = threading.Lock()  # Add lock for ping_failures
        self.ping_interval = 13
        self.max_ping_failures = 3
        self.message_counter = 0
        self.running = True  # Add running flag for clean shutdown

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

        # Uncomment the liveness checking
        threading.Thread(target=self.liveness_check_loop, daemon=True).start()
        
        # Register with (n/2)+1 random seeds
        num_seeds = len(self.seeds)
        num_connections = (num_seeds // 2) + 1
        selected_seeds = random.sample(self.seeds, num_connections)
        print(f"Connecting to {num_connections} seeds: {selected_seeds}")
        
        for seed in selected_seeds:
            self.register_with_seed(seed)
            
        # Start message input loop
        self.generate_messages()
        
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
            with self.peers_lock:
                self.connected_peers[address] = sock
            
            while True:
                data = sock.recv(1024).decode()
                if not data:
                    break
                    
                message = json.loads(data)
                if message['type'] == 'gossip':
                    self.handle_gossip(message, address)
                elif message['type'] == 'ping':
                    sock.send(json.dumps({"type": "pong"}).encode())
                    
        except Exception as e:
            print(f"Error handling peer {address}: {e}")
        finally:
            with self.peers_lock:
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
                    self.log_to_file(f"Connected to peer {peer}")
                    self.connected_peers[peer] = sock
                    threading.Thread(target=self.handle_peer_connection, 
                                  args=(sock, peer),
                                  daemon=True).start()
                except Exception as e:
                    print(f"Failed to connect to peer {peer}: {e}")

    def generate_messages(self):
        """Generate messages every 5 seconds, stop after 10 messages."""
        while self.message_counter < 10:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            message = f"{timestamp}:{self.host}:'Hi checking the working {self.message_counter}'"
            self.broadcast_message(message)
            self.message_counter += 1
            time.sleep(5)
            
    def broadcast_message(self, message):
        """Broadcast a message to peers who haven't seen it yet"""
        msg_data = {
            'type': 'gossip',
            'message': f"{self.host}:{self.port} - {message}",
            'timestamp': time.time()
        }
        msg_str = json.dumps(msg_data)
        msg_hash = hashlib.sha256(msg_str.encode()).hexdigest()
        
        with self.message_lock:
            if msg_hash not in self.message_list:
                self.message_list[msg_hash] = set()
        
        print(f"Broadcasting message {msg_hash[:8]}")
        dead_peers = []
        
        # Create a copy of peers while holding the lock
        with self.peers_lock:
            peers_to_message = list(self.connected_peers.items())
        
        for peer, sock in peers_to_message:
            with self.message_lock:
                if peer not in self.message_list[msg_hash]:
                    try:
                        sock.send(msg_str.encode())
                        self.message_list[msg_hash].add(peer)
                        print(f"Sent to {peer}")
                    except Exception as e:
                        print(f"Failed to send to {peer}: {e}")
                        dead_peers.append(peer)
        
        # Clean up dead connections
        if dead_peers:
            with self.peers_lock:
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
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            received_msg = f"[{timestamp}] Received from {sender}: {message_data['message']}"
            print(f"\n{received_msg}")
            self.log_to_file(received_msg)
            print("> ", end='', flush=True)  # Restore input prompt
            
            # Add to message list and mark as received from sender
            self.message_list[msg_hash] = {sender}
            
            # Forward to all peers except the sender
            for peer, sock in self.connected_peers.items():
                if peer != sender and peer not in self.message_list[msg_hash]:
                    try:
                        sock.send(msg_str.encode())
                        self.message_list[msg_hash].add(peer)
                        self.log_to_file(f"Forwarded to {peer}")
                    except Exception as e:
                        print(f"Failed to forward to {peer}: {e}")
        else:
            print(f"\nDuplicate message {msg_hash[:8]}")
            print("> ", end='', flush=True)

    # def ping_peer(self, peer):
    #     """Send a heartbeat message to check if the peer is alive."""
    #     try:
    #         sock = self.connected_peers.get(peer)
    #         if sock:
    #             sock.send(json.dumps({'type': 'heartbeat'}).encode())
    #             return True
    #     except Exception as e:
    #         print(f"Error sending heartbeat to {peer}: {e}")
    #     return False

    def liveness_check_loop(self):
        """Periodically check the liveness of connected peers."""
        while self.running:  # Add loop condition
            dead_peers = []
            
            # Create a copy of peers while holding the lock
            with self.peers_lock:
                peers_to_check = list(self.connected_peers.keys())
            
            for peer in peers_to_check:
                peer_ip = peer[0]
                peer_port = peer[1]
                try:
                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    sock.settimeout(5)
                    sock.connect((peer_ip, peer_port))
                    sock.send(json.dumps({"type": "ping"}).encode())
                    response = json.loads(sock.recv(1024).decode())
                    if response["type"] == "pong":
                        with self.failures_lock:
                            self.ping_failures[(peer_ip, peer_port)] = 0
                    sock.close()
                except Exception as e:
                    with self.failures_lock:
                        self.ping_failures[peer] = self.ping_failures.get(peer, 0) + 1
                        failures = self.ping_failures[peer]
                    
                    self.log_to_file(f"Ping failed for {peer_ip}:{peer_port}: {e}")
                    
                    if failures >= self.max_ping_failures:
                        dead_msg = f"Dead Node:{peer_ip}:{peer_port}:{datetime.now()}:{self.host}"
                        print(f"\n{dead_msg}")
                        self.log_to_file(dead_msg)
                        self.notify_seed_of_dead_peer(peer)
                        dead_peers.append(peer)

            # Clean up dead peers
            if dead_peers:
                with self.peers_lock:
                    for peer in dead_peers:
                        self.cleanup_dead_peer(peer)
                    
            time.sleep(self.ping_interval)  # Add sleep to prevent tight loop

    def notify_seed_of_dead_peer(self, dead_peer):
        """Notify seed nodes that a peer is dead."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        dead_msg = f"Dead Node:{dead_peer[0]}:{dead_peer[1]}:{timestamp}:{self.host}"
        
        for seed in self.seeds:
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.connect(seed)
                message = {
                    'type': 'dead_peer',
                    'ip': dead_peer[0],
                    'port': dead_peer[1],
                    'message': dead_msg
                }
                sock.send(json.dumps(message).encode())
                sock.close()
            except Exception as e:
                print(f"Failed to notify seed {seed} about dead peer {dead_peer}: {e}")

    def cleanup_dead_peer(self, dead_peer):
        """Clean up a dead peer from all data structures."""
        with self.peers_lock:
            if dead_peer in self.connected_peers:
                self.connected_peers[dead_peer].close()
                del self.connected_peers[dead_peer]
        
        with self.failures_lock:
            if dead_peer in self.ping_failures:
                del self.ping_failures[dead_peer]
        
        with self.message_lock:
            for msg_hash in self.message_list:
                if dead_peer in self.message_list[msg_hash]:
                    self.message_list[msg_hash].remove(dead_peer)

    def log_to_file(self, message):
        """Log messages to output file."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(f"peer_{self.host}_{self.port}.log", 'a') as f:
            f.write(f"[{timestamp}] {message}\n")

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