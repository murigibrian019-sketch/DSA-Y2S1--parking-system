# Smart Parking System (DSA Task 1)

An automated parking management system designed for parking facilities in Kenya, featuring real-time visual slot availability displays, automated vehicle check-in/check-out, tariff billing, and barrier access control.

---

## Task Requirements Breakdown

### a) System Modules & Algorithms

#### 1. Entry & Display Module
Monitors live capacity, updates the pre-entry digital display, allocates the nearest available slot, generates a secure parking ticket, and opens the entry barrier.

```text
ALGORITHM ProcessVehicleEntry(plate_number):
    IF available_slots_count <= 0 THEN
        Display "PARKING FULL - ACCESS DENIED"
        Keep Entry Barrier LOCKED
        RETURN
    END IF

    IF ActiveParkingTable.contains(plate_number) THEN
        Display "DUPLICATE VEHICLE DETECTED"
        RETURN
    END IF

    allocated_slot = AvailableSlotPool.extract_min()
    entry_time = GetCurrentTimestamp()
    ticket_id = GenerateUniqueTicket(plate_number, allocated_slot, entry_time)

    ActiveParkingTable.insert(plate_number, {ticket_id, allocated_slot, entry_time})
    available_slots_count = available_slots_count - 1

    UpdateDisplayScreen(available_slots_count)
    PrintTicket(ticket_id, plate_number, allocated_slot, entry_time)
  ALGORITHM ProcessVehicleExit(plate_number, exit_time):
    session = ActiveParkingTable.lookup(plate_number)
    IF session is NULL THEN
        Display "Invalid Record / Vehicle Not Found"
        RETURN
    END IF

    duration_hours = CEIL((exit_time - session.entry_time) / 3600)

    // Tiered pricing: KES 100 base for 1st hr + KES 50/hr thereafter
    IF duration_hours <= 1 THEN
        amount_due = 100
    ELSE
        amount_due = 100 + (duration_hours - 1) * 50
    END IF

    Display "Total Due: KES " + amount_due
    payment_status = Process Payment(amount_due, session.ticket_id)

    IF payment_status == SUCCESS THEN
        Trigger Exit Barrier(OPEN)
        Available Slot Pool.insert(session.allocated_slot)
        available_slots_count = available_slots_count + 1

        Log Transaction(session.ticket_id, plate_number, duration_hours, amount_due, "SETTLED")
        Active Parking Table.delete(plate_number)
        Update Display Screen(available_slots_count)
    ELSE
        Display "Payment Failed. Barrier Locked."
    END IF  Trigger Entry Barrier(OPEN)

-- 1. Parking Bays/Slots Table
CREATE TABLE slots (
    slot_id INT PRIMARY KEY AUTO_INCREMENT,
    slot_number VARCHAR(10) NOT NULL UNIQUE,
    floor_level INT NOT NULL,
    status ENUM('AVAILABLE', 'OCCUPIED', 'RESERVED', 'MAINTENANCE') DEFAULT 'AVAILABLE'
);

-- 2. Active Parking Sessions Table
CREATE TABLE active_sessions (
    session_id VARCHAR(64) PRIMARY KEY,
    vehicle_plate VARCHAR(15) NOT NULL,
    slot_id INT NOT NULL,
    entry_timestamp DATETIME NOT NULL,
    status ENUM('ACTIVE', 'PENDING_PAYMENT', 'COMPLETED') DEFAULT 'ACTIVE',
    FOREIGN KEY (slot_id) REFERENCES slots(slot_id)
);

-- 3. Dynamic Billing Tariffs
CREATE TABLE billing_rates (
    rate_id INT PRIMARY KEY AUTO_INCREMENT,
    base_rate DECIMAL(10,2) NOT NULL DEFAULT 100.00,
    base_duration_mins INT NOT NULL DEFAULT 60,
    hourly_rate DECIMAL(10,2) NOT NULL DEFAULT 50.00,
    grace_period_mins INT NOT NULL DEFAULT 15
);

-- 4. Audit & Payment Transactions Table
CREATE TABLE parking_transactions (
    transaction_id VARCHAR(64) PRIMARY KEY,
    session_id VARCHAR(64) NOT NULL,
    vehicle_plate VARCHAR(15) NOT NULL,
    entry_timestamp DATETIME NOT NULL,
    exit_timestamp DATETIME NOT NULL,
    duration_hours DECIMAL(5,2) NOT NULL,
    amount_paid DECIMAL(10,2) NOT NULL,
    payment_channel ENUM('M-PESA', 'CARD', 'CASH') DEFAULT 'M-PESA',
    payment_status ENUM('PENDING', 'SETTLED', 'FAILED') DEFAULT 'SETTLED',
    FOREIGN KEY (session_id) REFERENCES active_sessions(session_id)
);
