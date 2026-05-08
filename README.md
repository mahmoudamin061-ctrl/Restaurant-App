# Restaurant Simulation System

This project is a discrete-time simulation system designed to model and optimize the operations of a restaurant, managing various order types, staff (chefs), and resources (tables and delivery scooters).

## 📋 Table of Contents

* [Project Overview](https://www.google.com/search?q=%23project-overview)
* [Key Features](https://www.google.com/search?q=%23key-features)
* [System Architecture](https://www.google.com/search?q=%23system-architecture)
* [Resource Management](https://www.google.com/search?q=%23resource-management)
* [Simulation Modes](https://www.google.com/search?q=%23simulation-modes)
* [Team Contributions](https://www.google.com/search?q=%23team-contributions)
* [File Structure](https://www.google.com/search?q=%23file-structure)

## 🚀 Project Overview

The simulation tracks orders through multiple states: **Pending**, **Cooking**, **Ready**, **In-Service**, and **Finished**. It processes different types of customers including Dine-in, Takeaway, and Delivery, each with specific resource requirements and priority rules.

## ✨ Key Features

* **Diverse Order Handling**: Supports six distinct order types: Normal Dine-in (ODN), General Dine-in (ODG), Takeaway (OT), Normal Delivery (OVN), General Delivery (OVG), and Cancellation-eligible Delivery (OVC).
* **Dynamic Cancellation**: Allows for the cancellation of OVC orders at various stages (Pending, Cooking, or Ready), with immediate resource recovery.
* **Advanced Priority Queues**: Uses a weighted formula (based on price, size, and distance) to prioritize General Delivery (OVG) orders.
* **Overwait Protection**: Automatically escalates Ready OVG orders to a high-priority "Overwait" queue if they exceed a specific time threshold (TH).
* **Scooter Maintenance**: Implements a maintenance lifecycle where scooters are sent for repair after completing a set number of deliveries.
* **Table Sharing & Best-Fit**: Optimized dine-in logic that selects the smallest suitable table and supports sharing for compatible orders.

## 🏗 System Architecture

The core logic resides in the `Restaurant` class, which manages the simulation loop and resource synchronization.

### Core Data Structures

The project utilizes custom-built data structures for high-performance simulation:

* **`LinkedQueue`**: Standard FIFO queue for most pending and ready lists.
* **`priQueue`**: A priority queue implemented as a sorted linked list for priority-based assignments.
* **`LinkedList`**: Used for OVC order tracking and cancellation lookups.

## 🛠 Resource Management

### Chefs

* **Normal Chefs (CN)**: Primarily handle Normal and Cancellation orders.
* **Specialist Chefs (CS)**: Primarily handle General Dine-in and General Delivery orders.
* **Fallback Logic**: If a primary chef is unavailable, the system can assign a fallback chef to prevent bottlenecks.

### Scooters

* Assigned based on the **Shortest Cumulative Distance** rule to ensure even wear across the fleet.
* Tracked through `Available`, `In-Service`, `Returning`, and `Maintenance` states.

### Tables

* Managed via a **Best-Fit** algorithm to maximize capacity.
* Supports releasing partial seats for shared tables.

## 🖥 Simulation Modes

The program supports two primary UI modes:

1. **Interactive Mode (MODE_INTR)**: Provides step-by-step visual feedback of the restaurant's state at every timestep.
2. **Silent Mode (MODE_SILENT)**: Runs the simulation to completion and generates a comprehensive statistics file.

## 👥 Team Contributions

* **Mahmoud Yossef**: Order loading, cancellation logic (RemoveOrderOVC), and scooter maintenance/rescue systems.
* **Nourhan Karim**: Main simulation loop (RunSimulation), pending assignments, and order overwait logic.
* **Omar Ehab**: Table best-fit logic, takeaway order finalization, and interactive UI support.
* **Abdelrahman Nagy**: Statistics collection, output file formatting, and table sharing logic.

## 📂 File Structure

* `Restaurant.h/cpp`: Central simulation controller.
* `Order.h`: Order entity and priority calculations.
* `Chef.h`, `Table.h`, `Scooter.h`: Resource entity definitions.
* `UI.h/cpp`: Interface management.
* `Action.h`: Base class for simulation events (Request, Cancel).
* `include/`: Custom data structure implementations (`LinkedQueue.h`, `priQueue.h`, `LinkedList.h`).
