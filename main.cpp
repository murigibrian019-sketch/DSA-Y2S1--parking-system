#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace std;

// Structure to store an active parking session
struct Ticket {
    string ticketId;
    string plateNumber;
    int slotId;
    chrono::system_clock::time_point entryTime;
};

// Structure to log completed parking transactions
struct TransactionRecord {
    string ticketId;
    string plateNumber;
    int slotId;
    double durationHours;
    double fee;
    string paymentStatus;
};

class SmartParkingSystem {
private:
    int totalCapacity;
    
    // Min-Heap: Prioritizes allocation of lowest-numbered available slots (O(log n))
    priority_queue<int, vector<int>, greater<int>> availableSlots;
    
    // Hash Table: Fast lookups for parked vehicles by license plate (O(1) average)
    unordered_map<string, Ticket> activeParkedVehicles;
    
    // Log history of processed vehicles
    vector<TransactionRecord> transactionLogs;

    // Billing rates (in KES)
    const double BASE_RATE = 100.0;    // First hour charge
    const double HOURLY_RATE = 50.0;   // Additional hourly charge

public:
    SmartParkingSystem(int capacity) : totalCapacity(capacity) {
        for (int i = 1; i <= totalCapacity; ++i) {
            availableSlots.push(i);
        }
    }

    // Display module: Simulates the digital display screen before vehicle entry
    void displayAvailableSlots() const {
        cout << "\n========================================" << endl;
        cout << "   SMART PARKING SYSTEM DISPLAY PANEL   " << endl;
        cout << "========================================" << endl;
        cout << " Total Capacity  : " << totalCapacity << endl;
        cout << " Available Slots : " << availableSlots.size() << endl;
        if (availableSlots.empty()) {
            cout << " Status          : [PARKING FULL - NO ENTRY]" << endl;
        } else {
            cout << " Status          : [SPACES AVAILABLE]" << endl;
        }
        cout << "========================================\n" << endl;
    }

    // Module A: Vehicle Entry Management
    void processVehicleEntry(const string& plateNumber) {
        cout << ">> Incoming Vehicle: " << plateNumber << endl;

        // Verify capacity
        if (availableSlots.empty()) {
            cout << "[-] ACCESS DENIED: Facility is at full capacity." << endl;
            cout << "[-] Barrier status: LOCKED (CLOSED)\n" << endl;
            return;
        }

        // Check if the vehicle is already recorded inside
        if (activeParkedVehicles.find(plateNumber) != activeParkedVehicles.end()) {
            cout << "[-] ERROR: Duplicate entry! Plate " << plateNumber << " is already parked inside.\n" << endl;
            return;
        }

        // Allocate optimal parking slot (lowest ID via min-heap)
        int assignedSlot = availableSlots.top();
        availableSlots.pop();

        // Generate ticket details
        string ticketId = "TKT-" + plateNumber + "-S" + to_string(assignedSlot);
        auto entryTime = chrono::system_clock::now();

        Ticket newTicket{ticketId, plateNumber, assignedSlot, entryTime};
        activeParkedVehicles[plateNumber] = newTicket;

        // Confirm entry and simulate barrier gate actuation
        cout << "[+] TICKET ISSUED SUCCESSFULLY" << endl;
        cout << "    Ticket ID     : " << ticketId << endl;
        cout << "    Vehicle Plate : " << plateNumber << endl;
        cout << "    Assigned Bay  : Slot #" << assignedSlot << endl;
        cout << "    Barrier Action: OPEN -> VEHICLE ENTERED -> BARRIER CLOSED" << endl;

        displayAvailableSlots();
    }

    // Module B: Vehicle Exit & Billing
    // Note: simulatedHoursParked lets you test various durations directly.
    void processVehicleExit(const string& plateNumber, double simulatedHoursParked = 0.0) {
        cout << ">> Outgoing Vehicle: " << plateNumber << endl;

        auto it = activeParkedVehicles.find(plateNumber);
        if (it == activeParkedVehicles.end()) {
            cout << "[-] ERROR: Vehicle plate " << plateNumber << " not found in active records.\n" << endl;
            return;
        }

        Ticket ticket = it->second;
        double duration = simulatedHoursParked;

        // If no simulated hours provided, calculate actual elapsed wall-clock time
        if (duration <= 0.0) {
            auto exitTime = chrono::system_clock::now();
            chrono::duration<double> diff = exitTime - ticket.entryTime;
            duration = diff.count() / 3600.0;
            if (duration < 0.01) duration = 1.0; // Minimum default charge unit
        }

        int billedHours = static_cast<int>(ceil(duration));
        double fee = calculateFee(billedHours);

        cout << "[+] BILLING DETAILS" << endl;
        cout << "    Ticket ID     : " << ticket.ticketId << endl;
        cout << "    Slot Released : Bay #" << ticket.slotId << endl;
        cout << "    Time Elapsed  : " << fixed << setprecision(1) << duration << " hr(s) (" << billedHours << " billable hr)" << endl;
        cout << "    Amount Due    : KES " << setprecision(2) << fee << endl;

        // Payment verification routine
        cout << "    Payment Gateway: Awaiting payment confirmation..." << endl;
        cout << "    Payment Status : SUCCESSFUL (Settled via M-Pesa / Card)" << endl;
        cout << "    Barrier Action : OPEN -> VEHICLE EXITED -> BARRIER CLOSED" << endl;

        // Return the freed slot back to the heap pool
        availableSlots.push(ticket.slotId);

        // Record completed transaction history
        transactionLogs.push_back({ticket.ticketId, plateNumber, ticket.slotId, duration, fee, "PAID"});

        // Remove vehicle from current active registry
        activeParkedVehicles.erase(it);

        displayAvailableSlots();
    }

    double calculateFee(int billedHours) const {
        if (billedHours <= 1) {
            return BASE_RATE;
        }
        return BASE_RATE + ((billedHours - 1) * HOURLY_RATE);
    }
};

int main() {
    // Instantiate parking system with a capacity of 4 bays
    SmartParkingSystem lot(4);

    // Initial entrance display
    lot.displayAvailableSlots();

    // 1. Vehicles arrive and are assigned slots
    lot.processVehicleEntry("KDB 123A");
    lot.processVehicleEntry("KDA 987Z");
    lot.processVehicleEntry("KDC 456B");

    // 2. Vehicles exit (supplying simulated parked durations for test calculations)
    lot.processVehicleExit("KDA 987Z", 0.5); // 30 minutes -> 1 billed hour: KES 100
    lot.processVehicleExit("KDB 123A", 3.2); // 3.2 hours -> 4 billed hours: KES 250

    // 3. Fill up remaining spaces to demonstrate barrier locking
    lot.processVehicleEntry("KDD 111C");
    lot.processVehicleEntry("KDE 222D");
    lot.processVehicleEntry("KDF 333E");

    // Attempt entry when full
    lot.processVehicleEntry("KDG 444F");

    return 0;
}
