// Message definitions for Remote Execution simulation
// By B22CS061 & B22CS062

// Base message type
message BaseMessage {
    string sourceId;     // ID of the sender
    string destinationId; // ID of the recipient
    int timestamp;       // Timestamp for ordering
}

// Task message from client to server
message TaskMessage extends BaseMessage {
    int taskId;          // Unique ID for the task
    int subtaskId;       // ID of the subtask
    int values[];        // Array of integers for the subtask
}

// Result message from server to client
message ResultMessage extends BaseMessage {
    int taskId;          // Corresponding task ID
    int subtaskId;       // Corresponding subtask ID
    int result;          // Result of the computation (e.g., maximum value)
    bool isValid = true; // Flag to indicate if result is valid (honest) or not (malicious)
}

// Gossip message for score propagation
message GossipMessage extends BaseMessage {
    string content;      // Content in format <timestamp>:<IP>:<Score>
    bool firstTime;      // Whether this is the first time receiving this message
}

// Rating message for server evaluation
message RatingMessage extends BaseMessage {
    int serverId;        // ID of the server being rated
    double rating;       // Rating value
} 