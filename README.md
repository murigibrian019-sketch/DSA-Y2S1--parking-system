# Automated Smart Parking System

## 1. System Overview
An automated parking management system designed in C++ compliant with client terms of reference. The platform automates vehicle tracking, entry/exit gating, multi-channel payment verification (M-Pesa, Card, Cash), and real-time bay availability broadcasting.

## 2. Core Functional Modules

### a) Entry Lane & Live Display Module
- Continuously calculates capacity and broadcasts availability to entrance boards and web/mobile API consumers.
- Issues time-stamped entrance tickets and assigns optimal bays using a **Min-Heap** (`std::priority_queue`) in $O(\log n)$ time.
- Triggers entry gate actuation upon confirmed allocation.

### b) Exit Billing & Barrier Control Module
- Matches exiting vehicles against active sessions using an average $O(1)$ **Hash Map** (`std::unordered_map`).
- Computes parking durations and applies dynamic tiered tariffs.
- Validates payment across M-Pesa, Debit/Credit Card, or Cash channels before actuating the exit barrier.

### c) Dynamic Tariff Management Module
- Enables management to update base rates, base durations, hourly incremental fees, and VAT configurations dynamically at runtime without recompilation.

### d) Financial Reconciliation & VAT Audit Ledger
- Records complete financial transactions.
- Computes statutory 16% VAT splits (`Net Revenue` vs. `VAT Payable`) for tax compliance and audit trails.

## 3. Data Structures & Complexity

| Component | Data Structure | Time Complexity | Justification |
| :--- | :--- | :--- | :--- |
| **Bay Allocation** | Min-Heap (`std::priority_queue`) | $O(\log n)$ | Dynamically assigns and recycles the lowest-numbered physical bay. |
| **Active Registry** | Hash Map (`std::unordered_map`) | $O(1)$ avg | Prevents exit bottlenecks by ensuring immediate record lookup. |
| **Financial Ledger** | Vector (`std::vector`) | $O(1)$ amortized append | Maintains an immutable chronological audit trail for daily reconciliations. |

## 4. Dynamic Relational Database Design

```sql
-- 1. Physical Parking Bays
CREATE TABLE parking_bays (
    bay_id INT PRIMARY KEY AUTO_INCREMENT,
    bay_number VARCHAR(10) NOT NULL UNIQUE,
    floor_level INT NOT NULL,
    status ENUM('AVAILABLE', 'OCCUPIED', 'RESERVED', 'MAINTENANCE') DEFAULT 'AVAILABLE'
);

-- 2. Dynamic Tariff Configuration
CREATE TABLE tariff_configurations (
    tariff_id INT PRIMARY KEY AUTO_INCREMENT,
    base_rate DECIMAL(10,2) NOT NULL,
    base_hours INT NOT NULL,
    hourly_rate DECIMAL(10,2) NOT NULL,
    vat_percentage DECIMAL(5,2) DEFAULT 16.00,
    is_active BOOLEAN DEFAULT TRUE,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- 3. Active Vehicle Sessions
CREATE TABLE active_sessions (
    session_id VARCHAR(64) PRIMARY KEY,
    plate_number VARCHAR(15) NOT NULL,
    bay_id INT NOT NULL,
    entry_time DATETIME NOT NULL,
    FOREIGN KEY (bay_id) REFERENCES parking_bays(bay_id)
);

-- 4. Audit & VAT Settlement Ledger
CREATE TABLE financial_audit_ledger (
    transaction_id VARCHAR(64) PRIMARY KEY,
    session_id VARCHAR(64) NOT NULL,
    plate_number VARCHAR(15) NOT NULL,
    duration_hours DECIMAL(5,2) NOT NULL,
    net_amount DECIMAL(10,2) NOT NULL,
    vat_amount DECIMAL(10,2) NOT NULL,
    gross_amount DECIMAL(10,2) NOT NULL,
    payment_method ENUM('M-PESA', 'CARD', 'CASH') NOT NULL,
    settlement_status ENUM('SUCCESSFUL', 'FAILED') DEFAULT 'SUCCESSFUL',
    settled_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
