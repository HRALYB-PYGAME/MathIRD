# TO DO

## priority 5

- add time/clock to GameState
- add seed to GameState
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

### Trigger Event Table

| Trigger Event | **Value** (Physical) | **Real Value** (Predicted) | **Block Counters** | **Current Insight** | **VAR/BTN/PROC unlocks** | **Calendar (Processes)** | **Calendar (Packets)** | **Immediate Events** | **VAR version** | **Packet Update** |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Packet Sent** | | Update real value of **target variable** | **Increment** `Block counter` of all listed `blockers` | Update | | | **Push** | | | Probably |
| **Value Change** | **Value += delta** | | | Update | Update | | | | **Increment** | |
| **Packet Arrives** | | | **Decrement** `Block counter` of all listed `blockers` (last action) | | | | **Pop** | Value Change (delta = **evaluate()**) | | |
| **Process Ticks** | | | | Update | | **Pop** and **Push** if `endCondition` **NOT** met | | Packet Sent if `endCondition` **NOT** met and not `blocked` | | |
| **Button Left Click** | | | | | | | | Packet Sent (can be multiple) | | |
| **Button Right Click** | | | | Update (new Insightable) | | | | | | |

---
