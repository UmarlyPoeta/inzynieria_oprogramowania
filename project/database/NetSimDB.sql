CREATE TABLE nodes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) UNIQUE NOT NULL,
    ip VARCHAR(45) DEFAULT NULL,
    type ENUM('host', 'router', 'switch', 'unknown') DEFAULT 'host',
    vlan_id INT DEFAULT NULL,
    failed BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE links (
    id INT AUTO_INCREMENT PRIMARY KEY,
    node_a_id INT NOT NULL,
    node_b_id INT NOT NULL,
    delay_ms INT DEFAULT 0,
    bandwidth_mbps INT DEFAULT 100,
    packet_loss FLOAT DEFAULT 0,
    FOREIGN KEY (node_a_id) REFERENCES nodes(id) ON DELETE CASCADE,
    FOREIGN KEY (node_b_id) REFERENCES nodes(id) ON DELETE CASCADE,
    UNIQUE(node_a_id, node_b_id)
);
CREATE TABLE packet_stats (
    id INT AUTO_INCREMENT PRIMARY KEY,
    src_node_id INT NOT NULL,
    dst_node_id INT NOT NULL,
    packet_count INT DEFAULT 0,
    last_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (src_node_id) REFERENCES nodes(id) ON DELETE CASCADE,
    FOREIGN KEY (dst_node_id) REFERENCES nodes(id) ON DELETE CASCADE,
    UNIQUE (src_node_id, dst_node_id)
);
CREATE TABLE vlans (
    id INT AUTO_INCREMENT PRIMARY KEY,
    vlan_id INT NOT NULL,
    name VARCHAR(100),
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE congestion (
    id INT AUTO_INCREMENT PRIMARY KEY,
    node_id INT NOT NULL,
    queue_size INT DEFAULT 0,
    is_congested BOOLEAN DEFAULT FALSE,
    FOREIGN KEY (node_id) REFERENCES nodes(id) ON DELETE CASCADE
);
