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
Command Task ───────┐
                    ↓
Event Group (System State)
                    ↑
Telemetry Task → Queue → Downlink Task
      ↓
Health Monitor Task

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

## 🧪 Simulation & Validation

Since no hardware was used, the system was validated using **manual scenario-based simulation and execution tracing**.

### Methodology
- Step-by-step time progression  
- Task-wise execution tracking  
- State monitoring using event flags  

---

## 🔥 Test Scenarios

### ✅ Scenario 1: Normal Operation
- Telemetry generates packets  
- Downlink transmits data  
- Monitor confirms system health  

**Result:** System operates correctly  

---

### 🔁 Scenario 2: Mode Switching
- Command toggles NORMAL → SAFE  
- Telemetry rate decreases  

**Result:** System adapts correctly  

---

### ⚠️ Scenario 3: Telemetry Failure
- No packets generated  
- Monitor detects failure  

**Result:** FAULT STATE triggered  

---

### ⚠️ Scenario 4: Downlink Failure
- Queue not consumed  
- Data accumulates  

**Result:** FAULT STATE triggered  

---

### 📦 Scenario 5: Queue Overflow
- Telemetry faster than downlink  
- Queue becomes full  

**Result:** Packet drops observed (non-blocking design)  

---

### 🚨 Scenario 6: Full System Fault
- Telemetry and downlink both fail  

**Result:** System enters FAULT STATE  

---

### 🧠 Scenario 7: Recovery
- System in FAULT state  
- Command attempts recovery  

**Result:** System returns to NORMAL  

---

## 📊 Expected Logs

---

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

## 🔮 Future Work
- Deploy on ESP32 / STM32  
- Integrate real sensors  
- Implement Kalman Filter  
- Add watchdog timer  
- Add persistent logging  

---

## 💡 Key Takeaway
This project demonstrates how real-time embedded systems are designed, validated, and tested using scenario-based analysis, similar to real-world aerospace flight software systems.
