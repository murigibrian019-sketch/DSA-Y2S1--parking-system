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
    TriggerEntryBarrier(OPEN)
