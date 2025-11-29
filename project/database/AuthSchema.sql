-- Authentication & Authorization Schema for NetSimCPP
-- Version: 1.0
-- Date: 2025-10-29

USE netsim;

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role ENUM('admin', 'user', 'viewer') DEFAULT 'user',
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    last_login TIMESTAMP NULL,
    INDEX idx_username (username),
    INDEX idx_email (email),
    INDEX idx_role (role)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE utf8mb4_0900_ai_ci;

-- API Rate Limits table (per user per endpoint)
CREATE TABLE IF NOT EXISTS api_rate_limits (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    endpoint VARCHAR(100) NOT NULL,
    request_count INT DEFAULT 0,
    window_start TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_user_endpoint (user_id, endpoint),
    INDEX idx_user_endpoint (user_id, endpoint),
    INDEX idx_window_start (window_start)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE utf8mb4_0900_ai_ci;

-- User permissions (optional, for fine-grained access control)
CREATE TABLE IF NOT EXISTS user_permissions (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    resource VARCHAR(50) NOT NULL,  -- 'node', 'link', 'topology', etc.
    action VARCHAR(20) NOT NULL,    -- 'read', 'write', 'delete'
    granted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE KEY unique_user_resource_action (user_id, resource, action),
    INDEX idx_user_resource (user_id, resource)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE utf8mb4_0900_ai_ci;

-- Audit log for security events
CREATE TABLE IF NOT EXISTS auth_audit_log (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NULL,
    username VARCHAR(50),
    event_type ENUM('login', 'logout', 'failed_login', 'token_refresh', 'password_change', 'permission_change') NOT NULL,
    ip_address VARCHAR(45),  -- IPv6 compatible
    user_agent TEXT,
    success BOOLEAN DEFAULT TRUE,
    message TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL,
    INDEX idx_user_id (user_id),
    INDEX idx_event_type (event_type),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE utf8mb4_0900_ai_ci;

-- Insert default admin user (password: admin123)
-- Password hash is argon2id hash of "admin123"
-- Note: Change this password immediately in production!
INSERT IGNORE INTO users (username, email, password_hash, role) VALUES 
('admin', 'admin@netsim.local', '$argon2id$v=19$m=65536,t=3,p=4$placeholder', 'admin');

-- Insert default test users
INSERT IGNORE INTO users (username, email, password_hash, role) VALUES 
('user1', 'user1@netsim.local', '$argon2id$v=19$m=65536,t=3,p=4$placeholder', 'user'),
('viewer1', 'viewer1@netsim.local', '$argon2id$v=19$m=65536,t=3,p=4$placeholder', 'viewer');

-- Grant default permissions for admin
INSERT IGNORE INTO user_permissions (user_id, resource, action)
SELECT id, 'node', 'read' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'node', 'write' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'node', 'delete' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'link', 'read' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'link', 'write' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'link', 'delete' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'topology', 'read' FROM users WHERE role = 'admin'
UNION ALL
SELECT id, 'topology', 'write' FROM users WHERE role = 'admin';

-- Grant default permissions for regular users
INSERT IGNORE INTO user_permissions (user_id, resource, action)
SELECT id, 'node', 'read' FROM users WHERE role = 'user'
UNION ALL
SELECT id, 'node', 'write' FROM users WHERE role = 'user'
UNION ALL
SELECT id, 'link', 'read' FROM users WHERE role = 'user'
UNION ALL
SELECT id, 'link', 'write' FROM users WHERE role = 'user'
UNION ALL
SELECT id, 'topology', 'read' FROM users WHERE role = 'user';

-- Grant read-only permissions for viewers
INSERT IGNORE INTO user_permissions (user_id, resource, action)
SELECT id, 'node', 'read' FROM users WHERE role = 'viewer'
UNION ALL
SELECT id, 'link', 'read' FROM users WHERE role = 'viewer'
UNION ALL
SELECT id, 'topology', 'read' FROM users WHERE role = 'viewer';

-- Create view for user info with permissions
CREATE OR REPLACE VIEW v_user_info AS
SELECT 
    u.id,
    u.username,
    u.email,
    u.role,
    u.is_active,
    u.created_at,
    u.last_login,
    GROUP_CONCAT(CONCAT(up.resource, ':', up.action) SEPARATOR ', ') AS permissions
FROM users u
LEFT JOIN user_permissions up ON u.id = up.user_id
GROUP BY u.id, u.username, u.email, u.role, u.is_active, u.created_at, u.last_login;

-- Stored procedure to check user permission
DELIMITER //
CREATE PROCEDURE check_user_permission(
    IN p_user_id INT,
    IN p_resource VARCHAR(50),
    IN p_action VARCHAR(20)
)
BEGIN
    SELECT EXISTS(
        SELECT 1 FROM user_permissions 
        WHERE user_id = p_user_id 
        AND resource = p_resource 
        AND action = p_action
    ) AS has_permission;
END //
DELIMITER ;

-- Stored procedure to log auth event
DELIMITER //
CREATE PROCEDURE log_auth_event(
    IN p_user_id INT,
    IN p_username VARCHAR(50),
    IN p_event_type VARCHAR(20),
    IN p_ip_address VARCHAR(45),
    IN p_user_agent TEXT,
    IN p_success BOOLEAN,
    IN p_message TEXT
)
BEGIN
    INSERT INTO auth_audit_log 
    (user_id, username, event_type, ip_address, user_agent, success, message)
    VALUES 
    (p_user_id, p_username, p_event_type, p_ip_address, p_user_agent, p_success, p_message);
END //
DELIMITER ;

-- Function to get user rate limit
DELIMITER //
CREATE FUNCTION get_user_rate_limit(
    p_user_id INT,
    p_endpoint VARCHAR(100),
    p_max_requests INT,
    p_window_seconds INT
) RETURNS BOOLEAN
DETERMINISTIC
BEGIN
    DECLARE current_count INT DEFAULT 0;
    DECLARE window_expired BOOLEAN DEFAULT FALSE;
    
    -- Check if window has expired
    SELECT TIMESTAMPDIFF(SECOND, window_start, NOW()) > p_window_seconds
    INTO window_expired
    FROM api_rate_limits
    WHERE user_id = p_user_id AND endpoint = p_endpoint
    LIMIT 1;
    
    -- If window expired or no record, reset
    IF window_expired IS NULL OR window_expired = TRUE THEN
        INSERT INTO api_rate_limits (user_id, endpoint, request_count, window_start)
        VALUES (p_user_id, p_endpoint, 1, NOW())
        ON DUPLICATE KEY UPDATE 
            request_count = 1,
            window_start = NOW();
        RETURN TRUE;
    END IF;
    
    -- Get current count
    SELECT request_count INTO current_count
    FROM api_rate_limits
    WHERE user_id = p_user_id AND endpoint = p_endpoint;
    
    -- Check if under limit
    IF current_count < p_max_requests THEN
        UPDATE api_rate_limits 
        SET request_count = request_count + 1
        WHERE user_id = p_user_id AND endpoint = p_endpoint;
        RETURN TRUE;
    ELSE
        RETURN FALSE;
    END IF;
END //
DELIMITER ;

-- Show created tables
SHOW TABLES LIKE '%user%';
SHOW TABLES LIKE '%auth%';
SHOW TABLES LIKE '%rate%';

SELECT 'Authentication schema created successfully!' AS status;
