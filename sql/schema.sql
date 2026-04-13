-- ============================================================
-- SwiftTrack Database Schema
-- SQLite3 Compatible
-- ============================================================

-- Enable foreign keys
PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;

-- ============================================================
-- TABLE 1: users
-- Stores all user accounts in the system
-- ============================================================
CREATE TABLE IF NOT EXISTS users (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    uuid            TEXT NOT NULL UNIQUE,          -- Public-safe ID (UUID v4)
    full_name       TEXT NOT NULL,
    email           TEXT NOT NULL UNIQUE,
    phone           TEXT,
    password_hash   TEXT NOT NULL,                  -- bcrypt hash
    role            TEXT NOT NULL DEFAULT 'SENDER', -- ADMIN|SENDER|AGENT|PUBLIC
    is_active       INTEGER NOT NULL DEFAULT 1,     -- 1=active, 0=deactivated
    profile_image   TEXT,                           -- URL or null
    address         TEXT,                           -- JSON string of address object
    city            TEXT,
    state           TEXT,
    pincode         TEXT,
    created_at      TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now')),
    last_login      TEXT
);

-- Index for fast email lookup (used in login)
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_users_role  ON users(role);
CREATE INDEX IF NOT EXISTS idx_users_uuid  ON users(uuid);

-- ============================================================
-- TABLE 2: hub_cities
-- Major distribution centers / hubs
-- ============================================================
CREATE TABLE IF NOT EXISTS hub_cities (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    city_name   TEXT NOT NULL UNIQUE,
    state       TEXT NOT NULL,
    hub_code    TEXT NOT NULL UNIQUE,   -- e.g., "MUM", "DEL", "BLR"
    latitude    REAL NOT NULL,
    longitude   REAL NOT NULL,
    is_active   INTEGER NOT NULL DEFAULT 1,
    created_at  TEXT NOT NULL DEFAULT (datetime('now'))
);

-- ============================================================
-- TABLE 3: routes
-- Routes between hub cities (graph edges)
-- ============================================================
CREATE TABLE IF NOT EXISTS routes (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    from_hub_id     INTEGER NOT NULL REFERENCES hub_cities(id),
    to_hub_id       INTEGER NOT NULL REFERENCES hub_cities(id),
    distance_km     REAL NOT NULL,
    transit_hours   INTEGER NOT NULL,   -- Expected transit time in hours
    is_active       INTEGER NOT NULL DEFAULT 1,
    created_at      TEXT NOT NULL DEFAULT (datetime('now')),
    UNIQUE(from_hub_id, to_hub_id)
);

-- ============================================================
-- TABLE 4: shipments
-- Core shipment records
-- ============================================================
CREATE TABLE IF NOT EXISTS shipments (
    id                      INTEGER PRIMARY KEY AUTOINCREMENT,
    tracking_number         TEXT NOT NULL UNIQUE,   -- e.g., "SWT2024ABCD1234"
    sender_id               INTEGER NOT NULL REFERENCES users(id),
    
    -- Sender Details (snapshot at time of booking)
    sender_name             TEXT NOT NULL,
    sender_phone            TEXT NOT NULL,
    sender_email            TEXT,
    sender_address          TEXT NOT NULL,
    sender_city             TEXT NOT NULL,
    sender_state            TEXT NOT NULL,
    sender_pincode          TEXT NOT NULL,
    
    -- Recipient Details
    recipient_name          TEXT NOT NULL,
    recipient_phone         TEXT NOT NULL,
    recipient_email         TEXT,
    recipient_address       TEXT NOT NULL,
    recipient_city          TEXT NOT NULL,
    recipient_state         TEXT NOT NULL,
    recipient_pincode       TEXT NOT NULL,
    
    -- Package Details
    package_type            TEXT NOT NULL,  -- DOCUMENT|PARCEL|FRAGILE|HEAVY
    weight_grams            REAL NOT NULL,
    length_cm               REAL,
    width_cm                REAL,
    height_cm               REAL,
    package_description     TEXT,
    declared_value          REAL DEFAULT 0, -- Insurance value
    
    -- Service Details
    service_type            TEXT NOT NULL,  -- STANDARD|EXPRESS|SAME_DAY
    
    -- Routing
    origin_hub_id           INTEGER REFERENCES hub_cities(id),
    destination_hub_id      INTEGER REFERENCES hub_cities(id),
    route_path              TEXT,           -- JSON array of hub IDs
    
    -- Assignment
    assigned_agent_id       INTEGER REFERENCES users(id),
    
    -- Status & Tracking
    current_status          TEXT NOT NULL DEFAULT 'BOOKED',
    -- Statuses: BOOKED|PICKED_UP|IN_TRANSIT|AT_HUB|OUT_FOR_DELIVERY
    --           |DELIVERED|FAILED_DELIVERY|RETURNED|CANCELLED
    
    current_location        TEXT,           -- Current city/location
    
    -- Pricing
    base_cost               REAL NOT NULL DEFAULT 0,
    tax_amount              REAL NOT NULL DEFAULT 0,
    total_cost              REAL NOT NULL DEFAULT 0,
    
    -- Dates
    booked_at               TEXT NOT NULL DEFAULT (datetime('now')),
    pickup_scheduled_at     TEXT,
    estimated_delivery_at   TEXT,
    actual_delivery_at      TEXT,
    
    -- Additional
    special_instructions    TEXT,
    is_fragile              INTEGER NOT NULL DEFAULT 0,
    requires_signature      INTEGER NOT NULL DEFAULT 0,
    is_cod                  INTEGER NOT NULL DEFAULT 0,  -- Cash on delivery
    cod_amount              REAL DEFAULT 0,
    failure_reason          TEXT,           -- If FAILED_DELIVERY
    
    created_at              TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at              TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_shipments_tracking  ON shipments(tracking_number);
CREATE INDEX IF NOT EXISTS idx_shipments_sender    ON shipments(sender_id);
CREATE INDEX IF NOT EXISTS idx_shipments_agent     ON shipments(assigned_agent_id);
CREATE INDEX IF NOT EXISTS idx_shipments_status    ON shipments(current_status);
CREATE INDEX IF NOT EXISTS idx_shipments_booked_at ON shipments(booked_at);

-- ============================================================
-- TABLE 5: tracking_events
-- Complete audit trail of every status change
-- ============================================================
CREATE TABLE IF NOT EXISTS tracking_events (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    shipment_id     INTEGER NOT NULL REFERENCES shipments(id) ON DELETE CASCADE,
    status          TEXT NOT NULL,
    location        TEXT NOT NULL,        -- City/location name
    latitude        REAL,                 -- For map visualization
    longitude       REAL,
    description     TEXT NOT NULL,        -- Human readable message
    updated_by      INTEGER REFERENCES users(id),  -- Agent/admin who updated
    is_automated    INTEGER NOT NULL DEFAULT 0,    -- 1 if system-generated
    created_at      TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_tracking_shipment  ON tracking_events(shipment_id);
CREATE INDEX IF NOT EXISTS idx_tracking_created   ON tracking_events(created_at);

-- ============================================================
-- TABLE 6: agent_assignments
-- Tracks all assignment history (not just current)
-- ============================================================
CREATE TABLE IF NOT EXISTS agent_assignments (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    shipment_id     INTEGER NOT NULL REFERENCES shipments(id),
    agent_id        INTEGER NOT NULL REFERENCES users(id),
    assigned_by     INTEGER NOT NULL REFERENCES users(id),
    assigned_at     TEXT NOT NULL DEFAULT (datetime('now')),
    unassigned_at   TEXT,               -- NULL if still assigned
    notes           TEXT
);

-- ============================================================
-- TABLE 7: notifications
-- In-app notification system
-- ============================================================
CREATE TABLE IF NOT EXISTS notifications (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER NOT NULL REFERENCES users(id),
    title           TEXT NOT NULL,
    message         TEXT NOT NULL,
    type            TEXT NOT NULL,  -- INFO|SUCCESS|WARNING|ERROR|TRACKING
    is_read         INTEGER NOT NULL DEFAULT 0,
    related_id      INTEGER,        -- shipment_id if tracking notification
    related_type    TEXT,           -- "shipment" | "system"
    created_at      TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_notifications_user ON notifications(user_id, is_read);

-- ============================================================
-- TABLE 8: analytics_cache
-- Cached analytics data to avoid heavy queries every request
-- ============================================================
CREATE TABLE IF NOT EXISTS analytics_cache (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    cache_key   TEXT NOT NULL UNIQUE,
    cache_value TEXT NOT NULL,      -- JSON string
    expires_at  TEXT NOT NULL,
    created_at  TEXT NOT NULL DEFAULT (datetime('now'))
);

-- ============================================================
-- TABLE 9: token_blacklist
-- JWT blacklist for logout
-- ============================================================
CREATE TABLE IF NOT EXISTS token_blacklist (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    token_jti   TEXT NOT NULL UNIQUE,   -- JWT ID claim
    expires_at  TEXT NOT NULL,          -- When the token would have expired
    created_at  TEXT NOT NULL DEFAULT (datetime('now'))
);

-- ============================================================
-- SEED DATA — Initial setup
-- ============================================================

-- Default Admin User (password: admin123)
-- bcrypt hash of "admin123" with cost factor 12
INSERT OR IGNORE INTO users (uuid, full_name, email, phone, password_hash, role, city, state)
VALUES (
    'a1b2c3d4-e5f6-7890-abcd-ef1234567890',
    'System Administrator',
    'admin@swifttrack.com',
    '9999999999',
    '$2b$12$LQv3c1yqBWVHxkd0LQ1Tc.IqCqsMYH.eVSvH.gFqJvTGBe8gJz2mO',
    'ADMIN',
    'Mumbai',
    'Maharashtra'
);

-- Hub Cities (Major Indian Cities)
INSERT OR IGNORE INTO hub_cities (city_name, state, hub_code, latitude, longitude) VALUES
('Mumbai',      'Maharashtra', 'MUM', 19.0760, 72.8777),
('Delhi',       'Delhi',       'DEL', 28.7041, 77.1025),
('Bangalore',   'Karnataka',   'BLR', 12.9716, 77.5946),
('Chennai',     'Tamil Nadu',  'CHN', 13.0827, 80.2707),
('Kolkata',     'West Bengal', 'KOL', 22.5726, 88.3639),
('Hyderabad',   'Telangana',   'HYD', 17.3850, 78.4867),
('Pune',        'Maharashtra', 'PUN', 18.5204, 73.8567),
('Ahmedabad',   'Gujarat',     'AMD', 23.0225, 72.5714),
('Jaipur',      'Rajasthan',   'JAI', 26.9124, 75.7873),
('Lucknow',     'Uttar Pradesh','LKO',26.8467, 80.9462),
('Aurangabad',  'Maharashtra', 'AUR', 19.8762, 75.3433),
('Nagpur',      'Maharashtra', 'NGP', 21.1458, 79.0882);
