# 🍽️ Restaurant Simulation System

A **discrete-time simulation engine** built in C++ that models the full lifecycle of a restaurant — from order placement to delivery or dine-in completion — using custom-built data structures and resource scheduling algorithms.

> **Course Project** · Faculty of Engineering · Cairo University  
> **Language:** C++ · **Team:** 4 members

---

## 📋 Table of Contents

- [Overview](#overview)
- [Demo](#demo)
- [Key Features](#key-features)
- [System Architecture](#system-architecture)
- [Data Structures](#data-structures)
- [Resource Management](#resource-management)
- [Simulation Modes](#simulation-modes)
- [How to Build & Run](#how-to-build--run)
- [Input Format](#input-format)
- [Output & Statistics](#output--statistics)
- [File Structure](#file-structure)
- [Team](#team)

---

## Overview

The system simulates a real restaurant environment at each timestep, managing:

- **6 order types** with distinct rules and priorities
- **2 chef types** (Normal and Specialist) with fallback assignment logic
- **Tables** allocated via a Best-Fit algorithm with partial sharing support
- **Delivery scooters** with a full maintenance lifecycle

Orders move through these states:

```
PENDING → COOKING → READY → IN-SERVICE → FINISHED
```

---

## Demo

**Interactive Mode** — step through each timestep and see the system state live:

```
╔══════════════════════════════════════════════╗
║         TIMESTEP: 5                          ║
╠══════════════════════════════════════════════╣
║ Pending Orders:   ODN[3], OVN[7], OVC[2]    ║
║ Cooking Orders:   ODG[1] (Chef CS-2)         ║
║ Ready Orders:     OVG[4] ← OVERWAIT          ║
║ In-Service:       OVN[6] (Scooter S-1)       ║
║ Finished:         OT[5]                      ║
╚══════════════════════════════════════════════╝
```

**Silent Mode** — runs to completion and writes a statistics file:

```
Simulation finished at timestep: 42
Total orders processed: 25
  Dine-in:   8  |  Takeaway: 5  |  Delivery: 12
Average wait time: 3.4 timesteps
Cancelled orders: 2  (OVC)
Overwait orders:  1  (8.3%)
```

---

## Key Features

| Feature | Description |
|---|---|
| **6 Order Types** | ODN, ODG, OT, OVN, OVG, OVC — each with unique assignment rules |
| **Dynamic Cancellation** | OVC orders can be cancelled at Pending, Cooking, or Ready state with instant resource recovery |
| **Priority Formula** | OVG orders ranked by `f(price, size, distance)` using a sorted priority queue |
| **Overwait Escalation** | Ready OVG orders exceeding threshold `TH` are promoted to a high-priority overwait queue |
| **Scooter Maintenance** | Scooters track cumulative deliveries and enter maintenance after a set number of trips |
| **Best-Fit Tables** | Selects the smallest available table that fits; supports partial seat sharing |
| **Chef Fallback** | If primary chef type is unavailable, system falls back to avoid bottlenecks |

---

## System Architecture

```
┌─────────────────────────────────────────────────┐
│                  Restaurant.cpp                 │
│              (Main Simulation Loop)             │
└────────┬──────────┬──────────┬──────────────────┘
         │          │          │
    ┌────▼───┐ ┌────▼───┐ ┌───▼────┐
    │ Order  │ │  Chef  │ │ Table  │
    │  .h    │ │   .h   │ │  .h    │
    └────────┘ └────────┘ └───┬────┘
                              │
                        ┌─────▼──────┐
                        │  Scooter   │
                        │    .h      │
                        └────────────┘
```

Each timestep, `Restaurant::RunSimulation()`:
1. Moves new actions from the action list to the correct pending queues
2. Assigns pending orders to available chefs
3. Promotes cooked orders to the ready queues
4. Assigns tables (dine-in) or scooters (delivery)
5. Checks for finished orders and frees resources
6. Handles scooter returns and maintenance cycles

---

## Data Structures

All data structures are implemented from scratch (no STL containers used for core logic):

| Structure | File | Used For |
|---|---|---|
| `LinkedQueue<T>` | `include/LinkedQueue.h` | FIFO pending/ready lists for most order types |
| `priQueue<T>` | `include/priQueue.h` | Sorted priority queue for OVG orders and overwait |
| `LinkedList<T>` | `include/LinkedList.h` | OVC order tracking (supports mid-list removal) |
| `ArrayStack<T>` | `include/ArrayStack.h` | General utility stack |

The `priQueue` is implemented as a **sorted linked list** — `enqueue` inserts in O(n) to maintain order, and `dequeue` pops the head in O(1).

---

## Resource Management

### Chefs
- **Normal Chefs (CN):** Handle ODN, OT, OVC orders
- **Specialist Chefs (CS):** Handle ODG, OVG orders  
- **Fallback:** If primary type is fully busy, the other type can be assigned to prevent starvation

### Tables
- Allocated using **Best-Fit**: smallest table that fits the party size is chosen first
- Shared tables: a large table may serve two compatible orders simultaneously; seats are released independently when each order finishes

### Scooters
- Assigned by **Shortest Cumulative Distance** to balance wear across the fleet
- State machine: `Available → In-Service → Returning → Maintenance → Available`
- A scooter enters maintenance after `MAX_TRIPS` deliveries and rejoins the fleet after `MAINT_TIME` timesteps

---

## Simulation Modes

### Interactive Mode (`MODE_INTR`)
Pauses at each timestep and displays the full system state. Useful for debugging and tracing order flow.

### Silent Mode (`MODE_SILENT`)
Runs the full simulation without output, then writes a statistics summary to an output file. Used for grading and performance analysis.

---

## How to Build & Run

### Requirements
- Visual Studio 2022 (Windows) — solution file included
- C++17 or later

### Build
1. Open `Restaurant-App.slnx` in Visual Studio
2. Select `x64 | Debug` or `x64 | Release`
3. Build → Build Solution (`Ctrl+Shift+B`)

### Run
```
Restaurant-App.exe <input_file.txt>
```
The program will prompt you to choose Interactive or Silent mode.

---

## Input Format

```
<number_of_normal_chefs> <number_of_specialist_chefs>
<number_of_tables>
<table_1_capacity> <table_2_capacity> ...
<number_of_scooters>
<max_trips_before_maintenance> <maintenance_duration>
<number_of_actions>
<timestep> <action_type> <order_id> [order-specific fields...]
...
```

Example action line for a Normal Delivery:
```
3 OVN 101 15 2 8.5
```
*(at timestep 3, order 101, price=15, size=2, distance=8.5)*

---

## Output & Statistics

Silent mode writes a `.txt` file with:
- Total orders per type
- Average wait time per stage (Pending → Cooking → Ready → In-Service)
- Number of cancelled orders
- Overwait percentage for OVG orders
- Scooter utilization and maintenance count

---

## File Structure

```
Restaurant-App/
├── Restaurant.h / .cpp     ← Core simulation controller
├── Order.h                 ← Order entity + priority formula
├── Chef.h                  ← Chef resource
├── Table.h                 ← Table resource (best-fit + sharing)
├── Scooter.h               ← Scooter resource (maintenance lifecycle)
├── UI.h / .cpp             ← Interactive/Silent mode interface
├── Action.h                ← Base class for Request & Cancel events
└── include/
    ├── LinkedQueue.h       ← Custom FIFO queue
    ├── priQueue.h          ← Custom sorted priority queue
    ├── LinkedList.h        ← Custom doubly-accessible linked list
    ├── Node.h / priNode.h  ← Node definitions
    ├── ArrayStack.h        ← Array-based stack
    └── QueueADT.h / StackADT.h  ← Abstract interfaces
```

---

## Team

| Name | ID | Contribution |
|---|---|---|
| Mahmoud Yossef | 4250196 | Order loading, OVC cancellation logic, scooter maintenance & rescue |
| Nourhan Karim | 1240274 | Main simulation loop, chef/scooter/table assignment, overwait system |
| Omar Ehab | 4250195 | Table best-fit logic, takeaway finalization, interactive UI |
| Abdelrahman Nagy | 4250207 | Statistics collection, output file generation, table sharing logic |
