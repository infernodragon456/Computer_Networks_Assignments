# Peer-to-Peer Gossip Network

## Overview

This project implements a **Peer-to-Peer Gossip Protocol** with some **Seed Node** to manage peer discovery and failure detection. The system allows peer nodes to register, communicate, and broadcast messages to other connected peers using a **gossip mechanism**.

## Components

- **`peer.py`** - Represents an individual peer node that connects to a seed node, discovers other peers, and participates in gossip communication.
- **`seed.py`** - Acts as a central registry for peers, facilitating discovery and handling peer failures.
- **`config.txt`** - A configuration file listing seed node addresses in the format `IP:PORT`.

## Features

- **Peer Discovery**: Peers register with a seed node and get a list of other available peers.
- **Gossip Message Propagation**: Peers broadcast messages to connected nodes, which then propagate the message to others.
- **Failure Detection**: Periodic heartbeat checks ensure peers are alive, and unresponsive peers are removed.
- **Threaded Communication**: Each peer runs in a multi-threaded mode to handle concurrent connections.
- **Logging Mechanism**: Each seed and peer logs communication and system events for monitoring and debugging.

---

## Installation & Requirements

### Prerequisites

- **Python 3**
- No external dependencies (uses built-in Python modules)

### Setup

Clone the repository and navigate to the project directory:

```sh
git clone <repo-url>
cd <repo-directory>
```

### Ensure `config.txt` Exists

Ensure that `config.txt` exists and contains seed node addresses in the following format:

```txt
127.0.0.1:5000
127.0.0.1:5001
127.0.0.1:5002
```

## Running the Network

### 1. Start the Seed Nodes

Run the seed nodes first, which manages peer registration:
Run mUltiple seeds

```sh
python seed.py <seed_ip> <seed_port>
```

### 1. Start the Peer Nodes

Once the seed is running, start peer nodes:
Run mUltiple Peers

```sh
python peer.py <peer_ip> <peer_port> <config_file>
```

## How It Works

### Peer Discovery

- A peer reads `config.txt` and selects `(n/2) + 1` random seed nodes for registration.
- The seed node returns a list of connected peers.

### Message Gossiping

- Peers broadcast messages to others in a gossip-style propagation.
- Messages are uniquely identified using SHA-256 hashes.

### Failure Detection

- Each peer periodically pings its connected peers.
- If a peer fails to respond multiple times, it is marked as dead and removed.

### Sending Messages

- After starting `peer.py`, type messages in the console to broadcast them.
- Type `exit` to stop message input.

## Logging Mechanism

Each peer and seed maintains a log file to record all communication and system events.
Files name:-

### For Seed

```sh
seed_<ip_address>_<seed_port>.log
```

### Seed Logs

Each seed logs received messages with timestamps:

```sh
[2025-02-16 22:04:52] New peer registered: 127.0.0.1:6000. Current peers: {('127.0.0.1', 6000)}
[2025-02-16 22:13:46] New peer registered: 127.0.0.1:6001. Current peers: {('127.0.0.1', 6001)}
```

### For Peer

```sh
peer_<ip_address>_<peer_port>.log
```

### Peer Logs

Each peer logs received messages with timestamps in output.txt, as well individual logs.:

```sh
[2025-02-16 22:04:54] Received from ('127.0.0.1', 6000): 127.0.0.1:6001 - 2025-02-16 22:04:54:127.0.0.1:'Hi checking the working 0'
[2025-02-16 22:04:59] Received from ('127.0.0.1', 6000): 127.0.0.1:6001 - 2025-02-16 22:04:59:127.0.0.1:'Hi checking the working 1'
```
