# TO DO

## priority 5

- work on basic insights
- predictive insights
- look into variable unlocking/locking

## priority 4

- expandable chunk type
- variable insights

## priority 3

- color switching for DisplayChunks

## priority 2

- responsive button grid (kinda)
- centered hover displays
- operator parsing map
- processes

## priority 1

- scrollable insights
- tests

## WHAT and when to update

### new packet is sent on journey

- dealt with in the addPacket function
- real values needs to be updated ^

### packet reaches its goal and changes are applied

- current insight
- variable/term/button unlocks/states
- process states
- real values dont change

### process generates a new packet

- generate new entry in the calendar if needed

### 📅 The Master State Synchronization Matrix (Day 26 Refactor)

| Trigger Event | **Value** (Physical) | **Real Value** (Predicted) | **Block Counters** | **Current Insight** | **VAR/BTN/PROC unlocks** | **Calendar (Processes)** | **Calendar (Packets)** | **Immediate Events** | **VAR version** |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Packet Sent** | | Update real value of **target variable** | **Increment** `Block counter` of all listed `blockers` | Update | | | **Push** | | |
| **Value Change** | **Value += delta** | | | Update | Update | | | | **Increment** |
| **Packet Arrives** | | | **Decrement** `Block counter` of all listed `blockers` (last action) | | | | **Pop** | Value Change (delta = **evaluate()**) | |
| **Non Packet Value Changes** (non existent yet) | | Update (after Value Change) | | Update | Update | | | Value Change | |
| **Process Ticks** | | | | Update | | **Pop** and **Push** if `endCondition` **NOT** met | | Packet Sent if `endCondition` **NOT** met and not `blocked` | |
| **Button Left Click** | | | | | | | | Packet Sent (can be multiple) | |
| **Button Right Click** | | | | Update (new Insightable) | | | | | |

---

### 🔍 Operational Logic Details

#### 1. Insight Generation
* **Variable Insight:** Display `Value` and `Real Value` (Predicted) side-by-side. If `blockCounter > 0`, highlight the block source.
* **Term Insight:** Render the expression tree. Variables with `blockCounter > 0` should be colored **Yellow** unless they have the **Fluid (`~`)** bypass.
* **Process Insight:** Show the frequency and a countdown to the next timestamp found in the **Calendar**.

#### 2. Discovery Cascade (The "Pulse")
When a `Packet Arrives`, the following checks must run in sequence:
1.  **Global Unlocks:** Do any variables now satisfy conditions to move from `Locked` to `Unlocked`?
2.  **Logic Update:** Do any `Inactive` Terms now satisfy their boolean expressions to become `Active`?
3.  **Process Boot:** If a `Process` was just unlocked, its initial `ProcessTick` must be calculated and pushed to the Calendar.

#### 3. Real Value Recalculation
Because Packets are "Observers" and your math uses `double`, the "Predicted" value of a variable is the sum of its `currentValue` + the current `delta` of every packet targeting it.
* **The Cascade:** When a variable changes, you must iterate through all packets in the air, refresh their deltas, and subsequently update the `Real Value` of whatever variables *those* packets are targeting.
