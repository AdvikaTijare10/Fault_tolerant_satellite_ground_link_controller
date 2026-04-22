# 🛰️ Real-Time Satellite Telemetry & Command System (FreeRTOS Simulation)

## 📌 Overview
This project simulates a real-time onboard satellite flight software system using FreeRTOS concepts. It models telemetry generation, command handling, downlink transmission, and system health monitoring.

The system demonstrates how embedded flight software manages concurrent tasks, ensures reliable data flow, and handles fault conditions in mission-critical environments.

---

## 🎯 Objectives
- Simulate real-time satellite subsystems  
- Implement RTOS-based task scheduling  
- Design inter-task communication using queues  
- Implement event-driven state management  
- Detect and handle system faults  
- Validate system behavior through scenario-based simulation  

---

## 🧠 System Architecture

### High-Level Architecture
          
 <img width="711" height="727" alt="Image" src="https://github.com/user-attachments/assets/94d684f7-aa46-49d2-a431-b7bda355e321" />

### Core Components
- Tasks (4 total)
- Queue (Telemetry Buffer)
- Event Groups (System State Control)
- Mutex (Serial Synchronization)

---

## ⚙️ System Design

### 🛰️ Command Task
- Simulates ground station commands  
- Toggles between NORMAL and SAFE modes  
- Handles FAULT state recovery  

---

### 📡 Telemetry Task
- Generates telemetry packets containing:
  - APID (source identifier)
  - Sequence number
  - Timestamp
  - Sensor data
  - Checksum  
- Pushes data to queue  
- Reduces rate in SAFE mode  

---

### 📥 Downlink Task
- Receives telemetry packets from queue  
- Simulates transmission to ground station  

---

### 🧠 Health Monitor Task
- Monitors system using flags:
  - TELEMETRY_OK  
  - DOWNLINK_OK  
- Detects failures  
- Triggers FAULT STATE  

---

## 🔄 System States & Transitions

### Modes
- NORMAL MODE  
- SAFE MODE  
- FAULT STATE  

### Behavior

| State  | Behavior |
|--------|---------|
| NORMAL | Full operation |
| SAFE   | Reduced telemetry rate |
| FAULT  | Recovery mode |

### Transitions
- Command Task toggles NORMAL ↔ SAFE  
- Monitor Task triggers FAULT  
- Recovery returns system to NORMAL  

---

## 📦 Data Flow

### Telemetry Packet Structure
- APID → Source ID  
- Sequence → Packet ordering  
- Timestamp → Generation time  
- Sensor Data → Payload  
- Checksum → Error detection  

### Flow
Telemetry Task → Queue → Downlink Task → Ground

---

## 🔐 Synchronization & Communication
- Queue → Data transfer between tasks  
- Event Groups → System state management  
- Mutex → Protect shared resources (Serial output)  

---

## ⚡ Key Design Decisions

### Non-blocking Queue
- Drops packets instead of blocking  
- Ensures real-time responsiveness  

### Event-driven Architecture
- Tasks react to system state flags  
- Reduces tight coupling  

### Fault Detection
- Independent monitoring via Health Monitor Task  

---

## 🧪 Simulation & Validation Methodology

Since hardware execution was not available, the system was validated using a **manual scenario-based simulation approach**.

---

### ⚙️ Approach

The system was analyzed by simulating execution in discrete time steps (T0, T1, T2...), where each step represents task execution based on RTOS scheduling behavior.

For each scenario:
- Active tasks were identified  
- Event group bits were tracked  
- Queue state was monitored  
- System outputs were derived from code logic  

---

### 🔍 What Was Tracked

During simulation, the following system components were continuously evaluated:

- **Task behavior** (Command, Telemetry, Downlink, Monitor)  
- **Event flags** (NORMAL, SAFE, FAULT, TELEMETRY_OK, DOWNLINK_OK)  
- **Queue state** (empty, filling, full, overflow)  
- **Data flow** between tasks  

---

### 🧪 Validation Strategy

Instead of testing only normal execution, multiple scenarios were analyzed to validate system robustness:

- Normal operation  
- Mode switching  
- Telemetry failure  
- Downlink failure  
- Queue overflow (load condition)  
- Full system failure  
- Fault recovery  

Each scenario was executed step-by-step to verify:
- Correct task interaction  
- Proper event-driven state transitions  
- Accurate fault detection  
- Stable system behavior under stress  

---

### 🧠 Key Idea

The focus of validation was not just whether the system works, but how it behaves under different conditions.

This approach ensures:
- Clear understanding of system dynamics  
- Verification of real-time design decisions  
- Confidence in fault handling logic  

---

### ✅ Outcome

The system was successfully validated across all scenarios, demonstrating:
- Reliable inter-task communication  
- Correct event-driven control  
- Robust fault detection and recovery  
- Stable performance under varying conditions  

##  Test Scenarios

###  Scenario 1: Normal Operation

### 🎯 Objective
Verify correct system behavior when all subsystems are functioning normally.

---

### 📊 Execution Trace

| Step | Active Task     | Action Performed | System State / Flags |
|------|----------------|-----------------|----------------------|
| T0   | Setup          | System initialized, NORMAL_MODE set | NORMAL=1 |
| T1   | Command Task   | Toggles to SAFE_MODE | SAFE=1 |
| T2   | Telemetry Task | Generates packet, sends to queue | TELEMETRY_OK=1 |
| T3   | Downlink Task  | Reads packet from queue | DOWNLINK_OK=1 |
| T4   | Monitor Task   | Verifies system health | No FAULT |
| T5   | Next Cycle     | System continues execution | Stable |

---

### 📦 Data Flow
Telemetry → Queue → Downlink → Monitor

---

### 📊 Expected Logs
```
[COMMAND] Switching to SAFE MODE
[TELEMETRY] Generated packet SEQ: 1
[DOWNLINK] Data received...
[MONITOR] Telemetry and downlink active
```
---

### 🧠 Key Observations
- Smooth data flow across tasks  
- Event bits correctly synchronize system  
- No blocking or delays observed  

---

### ✅ Conclusion
System operates correctly under normal conditions with stable task coordination and data flow.

##  Scenario 2: Telemetry Failure

### 🎯 Objective
Verify that the system detects failure when telemetry data is not generated.

---

### 📊 Execution Trace

| Step | Active Task     | Action Performed | System State / Flags |
|------|----------------|-----------------|----------------------|
| T0   | Setup          | System initialized, NORMAL_MODE set | NORMAL=1 |
| T1   | Telemetry Task | ❌ No packet generated (failure simulated) | TELEMETRY_OK=0 |
| T2   | Downlink Task  | No data received from queue | DOWNLINK_OK=0 |
| T3   | Monitor Task   | Detects missing telemetry | FAULT=1 |
| T4   | Command Task   | Enters recovery mode (skips normal toggle) | FAULT persists |

---

### 📦 Data Flow
Telemetry ❌ → Queue (empty) → Downlink (idle)

---

### 📊 Expected Logs
```
[TELEMETRY] FAULT MODE
[MONITOR] ERROR: Telemetry failure
[COMMAND] System in FAULT STATE, attempting recovery...
```

---

### 🧠 Key Observations
- Absence of telemetry is correctly detected  
- Downlink remains idle due to empty queue  
- Monitor triggers FAULT state within one cycle  
- Command task shifts to recovery behavior  

---

### ✅ Conclusion
The system successfully detects telemetry failure and transitions to FAULT state, demonstrating effective fault detection and response.

## Scenario 3: Downlink Failure

### 🎯 Objective
Verify that the system detects failure when telemetry is generated but not transmitted via downlink.

---

### 📊 Execution Trace

| Step | Active Task     | Action Performed | System State / Flags |
|------|----------------|-----------------|----------------------|
| T0   | Setup          | System initialized, NORMAL_MODE set | NORMAL=1 |
| T1   | Telemetry Task | Generates packet and pushes to queue | TELEMETRY_OK=1 |
| T2   | Downlink Task  | ❌ Fails to read from queue (failure simulated) | DOWNLINK_OK=0 |
| T3   | Queue          | Packet remains in queue (accumulating) | Queue not empty |
| T4   | Monitor Task   | Detects missing downlink activity | FAULT=1 |
| T5   | Command Task   | Enters recovery mode | FAULT persists |

---

### 📦 Data Flow
Telemetry → Queue (data accumulates) → Downlink ❌

---

### 📊 Expected Logs
```
[TELEMETRY] Generated packet SEQ: 1
[MONITOR] ERROR: Downlink failure
[COMMAND] System in FAULT STATE, attempting recovery...
```
---

### 🧠 Key Observations
- Telemetry generation continues normally  
- Queue begins accumulating packets  
- Downlink inactivity is correctly detected  
- Monitor triggers FAULT despite active telemetry  

---

### ✅ Conclusion
The system correctly identifies downlink failure even when telemetry is functioning, ensuring independent validation of subsystems.



### 📦 Scenario 5: Queue Overflow
- Telemetry faster than downlink  
- Queue becomes full  

**Result:** Packet drops observed (non-blocking design)  

---
##  Scenario 4: Queue Overflow

### 🎯 Objective
Verify system behavior when telemetry generation rate exceeds downlink consumption rate, leading to queue saturation.

---

### 📊 Execution Trace

| Step | Active Task     | Action Performed | System State / Flags |
|------|----------------|-----------------|----------------------|
| T0   | Setup          | System initialized | Queue empty |
| T1   | Telemetry Task | Generates packets rapidly | Queue filling |
| T2   | Downlink Task  | Slower consumption rate | Queue growing |
| T3   | Queue          | Reaches max capacity (10 packets) | Queue FULL |
| T4   | Telemetry Task | Attempts to push → ❌ fails (non-blocking) | Packet dropped |
| T5   | Monitor Task   | Telemetry + Downlink still active | No FAULT |

---

### 📦 Data Flow
Telemetry (fast) → Queue (FULL) → Downlink (slow)

---

### 📊 Expected Logs
```
[TELEMETRY] Generated packet SEQ: 10
[TELEMETRY] Generated packet SEQ: 11 (dropped)
[DOWNLINK] Data received...
[MONITOR] Telemetry and downlink active
```
---

### 🧠 Key Observations
- Queue reaches capacity under high load  
- New packets are dropped instead of blocking  
- Downlink continues processing existing data  
- System remains stable despite data loss  

---

### ✅ Conclusion
The system handles overload conditions gracefully by dropping excess packets, ensuring real-time performance without blocking critical tasks.

##  Scenario 5: Full System Fault

### 🎯 Objective
Verify system behavior when both telemetry and downlink subsystems fail simultaneously.

---

### 📊 Execution Trace

| Step | Active Task     | Action Performed | System State / Flags |
|------|----------------|-----------------|----------------------|
| T0   | Setup          | System initialized | NORMAL=1 |
| T1   | Telemetry Task | ❌ No packet generated | TELEMETRY_OK=0 |
| T2   | Downlink Task  | ❌ No data received | DOWNLINK_OK=0 |
| T3   | Monitor Task   | Detects both failures | FAULT=1 |
| T4   | System         | Enters FAULT STATE | All tasks adapt behavior |

---

### 📦 Data Flow
Telemetry ❌ → Queue (empty) → Downlink ❌

---

### 📊 Expected Logs
```
[TELEMETRY] FAULT MODE
[DOWNLINK] Alert: System in FAULT STATE..
[MONITOR] ERROR: Telemetry failure
[MONITOR] ERROR: Downlink failure
```
---

### 🧠 Key Observations
- Both subsystems fail independently  
- Monitor detects combined failure condition  
- System transitions immediately to FAULT state  
- All tasks modify behavior based on FAULT flag  

---

### ✅ Conclusion
The system correctly handles complete subsystem failure, demonstrating robust fault detection across multiple components.

##  Scenario 6: Recovery from Fault

### 🎯 Objective
Verify that the system attempts recovery and returns to normal operation after a fault condition.

---

### 📊 Execution Trace

| Step | Active Task     | Action Performed | System State / Flags |
|------|----------------|-----------------|----------------------|
| T0   | System         | FAULT STATE active | FAULT=1 |
| T1   | Command Task   | Detects FAULT → initiates recovery delay | Recovery mode |
| T2   | Telemetry Task | Resumes packet generation | TELEMETRY_OK=1 |
| T3   | Downlink Task  | Resumes data transmission | DOWNLINK_OK=1 |
| T4   | Monitor Task   | Detects system restored | FAULT=0 |
| T5   | System         | Returns to NORMAL MODE | Stable |

---

### 📦 Data Flow
Telemetry → Queue → Downlink (restored)

---

### 📊 Expected Logs
```
[COMMAND] System in FAULT STATE, attempting recovery...
[TELEMETRY] Generated packet SEQ: X
[DOWNLINK] Data received...
[MONITOR] Telemetry and downlink active
```

---

### 🧠 Key Observations
- Command task pauses normal operation during fault  
- System components resume independently  
- Monitor clears FAULT once both subsystems recover  
- System returns to stable execution  

---

### ✅ Conclusion
The system successfully recovers from fault conditions and resumes normal operation, validating fault recovery logic.

## 🧠 Key Concepts Demonstrated
- Real-time task scheduling  
- Inter-task communication  
- Event-driven system design  
- Fault detection & recovery  
- Resource synchronization  
- Embedded system architecture  

---

## 🚀 Applications
- Satellite flight software  
- Embedded control systems  
- Autonomous systems  
- Real-time monitoring platforms  

---

## ⚡ Limitations
- No physical hardware implementation  
- Sensor data is simulated  
- Timing is not cycle-accurate  

---

## Future Work

- Deploy the system on actual hardware (ESP32 / STM32) to observe real RTOS scheduling and timing behavior  
- Replace simulated sensor input with real sensors (e.g., IMU, temperature)  
- Add a watchdog timer to automatically reset the system during critical failures  
- Extend telemetry packet structure with more realistic parameters (multi-sensor data)  
- Implement logging to external storage or serial monitor for better debugging and analysis  
- Explore integrating filtering techniques (e.g., Kalman Filter) for cleaner sensor data  


---

## Key Takeaways

- Understood how multiple tasks interact in a real-time system instead of running independently  
- Learned how event-driven design helps control system behavior without tightly coupling components  
- Realized that in real-time systems, dropping data is sometimes better than blocking execution  
- Gained insight into how faults are detected based on system behavior, not just errors  
- Understood the importance of validating not just normal operation, but also failure scenarios  
- Learned how to think about systems as a whole rather than focusing on individual functions  

