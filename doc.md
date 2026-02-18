# MathIRD

## Variable

The `Variable` class serves as the blueprint for a game variable. It only stores the **definition** and metadata rather than the current state. All live values are maintained within the [Game State](#game-state) object.

### Definition Structure

| Member | Type | Description |
| :--- | :--- | :--- |
| **name** | `string` | A unique identifier for the variable, typically short or cryptic (e.g., `x`, `lrun`). |
| **scoreParams** | [`ScoreParams`](#scoreparams) | Defines how the variable contributes to the total score based on knee, offset, and polarity. |
| **unlockCondition** | [`unique_ptr<Node>`](#node) | A pointer to the root of an expression tree that must evaluate to true for the variable to unlock. |
| **defaultValue** | `double` | The initial value assigned upon unlocking. It also acts as the fallback value for "locked" soft variables. |
| **granularity** | `double` | Controls the visual representation of a [Packet](#packet). Higher granularity results in a smaller radius for the non-white center. |
| **homeButton** | [`Button*`](#button) | Usually displays the current value and serves as the destination for incoming [Packets](#packet). |
| **asDependencyInTerms** | `set<Term*>` | Terms that require this variable to be unlocked. |
| **asInputInTerms** | `set<Term*>` | Terms that utilize this variable's value as an input for their internal logic. |
| **asOutputInTerms** | `set<Term*>` | Terms that, when processed, can modify the value of this variable. |
| **asBlockerInTerms** | `set<Term*>` | Terms that will be blocked if this variable becomes blocked. |
| **displayedAtButtons** | `set<Button*>` | All buttons that displays this variable's value. |

### Unlock Conditions

A variable is unlocked once its specific condition is met. This state is validated every time a variable's **Current** or **Real** value changes, as well as during the initial loading or starting of a new game. Unlocking a variable can trigger a chain reaction, unlocking new [terms](#term) or [buttons](#button) as well. Once a variable is unlocked it stays unlocked forever.

To enhance the player experience, we can calculate a **Progress Value** to determine how close the player is to meeting a requirement. This is achieved by comparing the distance between the **Current** value and the target against the distance between the **Default** value and the target.

* **Logic:** If the current value is moving toward the target, the progress bar fills. **0** means same or worse progress than at the start and **1** means unlocked.
* **Edge Case:** If a condition is technically false but the variable is equal to the target value (e.g., `x > 5` when `x` is `5`), **0.99** is returned.

### Value Types

Variable values are categorized into three distinct states. While **Current** and **Real** values are managed within the active [GameState](#game-state), the **Default** value is retrieved from the static variable definitions.

* **Default**: The value of a variable at the start of a new game. This is also usefull in calculating the progress towards a specific goal (written as a condition).
* **Current**: The value presently visible to the player (e.g., on a button). This is the "active" state used for the vast majority of logic.
* **Real (Eventual)**: The projected value of a variable after all in-flight "packets" reach their destination. This represents the expected future state of the variable once all travelling packets arrive.

### Variable Data Types

While the internal engine stores all variable values as a `double`, each variable is assigned a specific **Type** that governs how it is formatted in the UI, how it handles assignments, and how the progress engine interprets its state. When no data type is provided in a variable config `double` is used as a default.

| Type | Config Representation | Description |
| :--- | :---: | :--- |
| **int** | I | Represents a whole numbers. Values are floored during assignment. Insights will display values as whole numbers. |
| **double** | D | The default type for continuous values. Represents a floating point number. |
| **perc** | P | Represents a value between `0.0` and `1.0`. Insights will format `0.5` as `50%`. When assigning a value is a clamped to the `0-1` range. |
| **enum** | E | Represents a selection from a predefined list of states. Arithmetic operations are discouraged. Progress is binary (0% or 100%) based on whether the specific state is matched. |
| **bool** | B | Stored as `1.0` (True) or `0.0` (False). Values assigned to a bool are normalized (nonzero becomes `1.0`). Similarly to `enum` progress is binary. |

* **Assignments**: When a value is assigned a constraint method is called. This will return a new value that is valid for the given type.
* **Progress**: For `double` and `perc`, progress is a linear distance calculation. For `enum` and `bool`, the engine skips distance math and treats the goal as a discrete state—you have either reached it or you haven't.
* **Insight**: Depending on the type, values can not only be displayed differently but also change the way an insight is generated.

### Home Button

Each variable is mapped to a specific **Home Button**, though a single button may act as the home for multiple variables.

* **Routing**: Every time a [packet](#packet) is generated, it sets its destination to the Home Button associated with its target variable.
* **Visualization**: The Home Button typically displays the **Current** value of its assigned variables. Such value can also be displayed on multiple buttons.

### ScoreParams

The `ScoreParams` struct defines how a specific variable's value contributes to the total score. It consists of three members.

* **Knee ($K$):** The value at which the calculated score reached exactly **100**.
* **Offset ($O$):** A value subtracted from (or compared against) the variable value before calculations begin.
* **Polarity:** Determines the direction and logic of the scoring curve.

#### Score Formula

Scores are calculated based on Polarity. In the formulas below, $V$ represents the variable value, $O$ is the offset, and $K$ is the knee:

| Polarity | Formula | Description |
| :--- | :--- | :--- |
| **Neutral** | $0$ | The variable has no effect on the score. |
| **Normal** | $\left[ \frac{\log(V-O)}{\log(K)} \right] \times 100$ | **Growth-oriented:** Higher values increase the score. Values $\le O$ contribute $0$. |
| **Inverted** | $\left[ \frac{\log(O-V)}{\log(K)} \right] \times 100$ | **Reduction-oriented:** Lower values increase the score. Values $\ge O$ contribute $0$. |
| **Absolute** | $\left[ \frac{\log(\|V-O\|)}{\log(K)} \right] \times 100$ | **Distance-oriented:** Values further from the offset (in either direction) increase the score. |

> **Note on Constraints:**
>
> * If any value inside a $\log()$ function is zero or negative ($> 0$), the final result is **0**.
> * If the entire expression inside the square brackets is negative, the final result is **0**.

### Variable Flags

Each variable within a [Node](#node) tree can be assigned some of these flags. These modify how the variable behaves during evaluation, packet generation, and term processing. In the table below, `N` represents the base variable name.

| Flag | Config Representation | Description |
| :--- | :--- | :--- |
| **Soft** | `~N` | **Lock Bypass:** Allows the variable to be used even while locked. In this state, it is substituted by its `defaultValue`. If a **locked** Soft variable is the target (left side) of an assignment in a [Term](#term), the assignment is ignored rather than the entire term being locked. |
| **Constant** | `$N` | **Pre-evaluation:** When used in a [Packet](#packet) expression, the variable's value is calculated at the moment the packet is created. |
| **Fluid** | `N~` | **Blocker Bypass:** These variables ignore being blocked. A [Packet](#packet) can still be created even if a Fluid variable is currently blocked. |
| **Real** | `N?` | **Projection:** Refers to the **Real (Eventual)** value of the variable. Like the Constant flag, it is evaluated at the start of a packet's journey. This is done to prevent circular dependencies. |

> **Warning:** While these flags are not mutually exclusive, certain combinations can lead to unexpected behaviors. For example, combining **Constant** (`$N`) and **Real** (`N?`) may end up differently than you expect.

---

## Game State

Game State is a class. It contains current state of the game.

---

## Node

A `Node` represents a pointer to binary expression tree. Depending on the node's type, it can represent a variable name, a constant, or an expression.

In the context of [Term](#term) or [Packet](#packet) expressions, Nodes are managed via an `Expression` wrapper. This wrapper includes a vector of `variableLocks`. A list of specific variables that are temporarily blocked while a packet containing that expression is in transit. This locking mechanism is essential to maintain game integrity and prevent unexpected value fluctuations or "cheating."

### Methods

* **Evaluation**: Given a [`GameState`](#game-state), calculates the current value of the expression tree.
* **Simulation**: Returns [Variable Changes](#variable-changes) instead of a double (this function may become unnecessary later).
* **Progress**: Determines how close the player is to satisfying an expression (the "goal").
  * **0.0** represents a state where the player is no closer to the goal than they were at the start of the game.
  * **1.0** represents that the expression has been evaluated as **True**.
* **Insight**: Generates an explanation of the expression in arithmetical or "human readable" format. More at [Insight](#insight).

### Operands

Expressions can contain different types of operations including arithmetic (`-`, `*`), comparisons (`==`, `>`), logical gates (`!`, `||`), and functions such as `max`, `abs`, and `if`.

#### Formatting Rules

When defining expressions within a configuration file, adhere to the following syntax and logic rules:

* **Whitespace:** Spaces are ignored by the tokenizer.
* **Normalization:** Be aware that the logical NOT (`!`) is removed during the [normalization](#node) process.
* **Function Syntax:** It is highly recommended to use square brackets for function calls: `max[1,2]`. Internally, the parser translates `[` to `((` and `,` to `)(`.
* **Precedence:** Standard mathematical order of operations applies. Use parentheses `()` to explicitly define evaluation priority.
* **Validation:** * **Syntactic Errors:** Any expression with invalid syntax will cause the entire [Term](#term) to be ignored.
  * **Non defined variable:** Using a variable name that has not been defined will result in the entire term being ignored.
* **Enum arithmetics:** It is strongly advised **not** to perform arithmetic operations on Enum or Boolean variables, as the resulting "progress" and "insight" calculations may become nonsensical.
* **Reserved Keywords:** The use of `_R` and `_NR` is restricted (see [Specific Restrictions](#restrictions)). While these restrictions can be bypassed, doing so will likely result in broken or uninformative Insights.

### From Text to Object

The transition from a raw string in an object consists of these four steps.

1. **Tokenization**: The raw string is broken down into a vector of **Tokens**. Such process is done by the Tokenizer which also checks for potentional invalid characters. If some are found steps 2-4 are ignored.
2. **Construcion**: The constructor transforms the vector of **Tokens** into a valid `Node` tree. If tokens contains some syntactic issues no tree is generated and steps 3-4 are ignored.
3. **Normalization**: Normalization is a process of removing logical NOTs and replacing expressions with an equivalent ones not containing them.
4. **Validation**: In the final step it is checked whether all variables contained within the expressions are defined.

If a any expression contained within term or any other element fails (syntacticly wrong, contains invalid characters or non-defined variables) the entire object is ignored.

---

## Packet

---

## Button

---

## Process

---

## Term

A **Term** is consists of a logic condition and a set of [Expressions](#node). Each term have a unique parent [Button](#button) or [Process](#process).

### Variable Associations

Each term contains these sets of [variable](#variable) names:

* **Dependencies**: Variables that **must** be unlocked for the term to become available.
* **Inputs**: Variables that can affect its behavior.
* **Outputs**: Variables whose values can be modified when the term is processed.
* **Blockers**: If any variable in this set is currently [Blocked](#node), the term itself becomes blocked also.

### Term States

| State | Condition |
| :---: | :--- |
| **Locked** | At least one variable in the **Dependencies** set is currently locked. |
| **Inactive** | All dependencies are unlocked, but the term's specific **Condition** (Node expression) evaluates to `false`. |
| **Blocked** | The term is active, but one or more variables in the **Blockers** set are currently blocked by an in-flight [Packet](#packet). |
| **Unblocked** | All criteria are met: Unlocked, Condition met, and no Blockers being blocked. The term is ready for use. |

#### Other non-primary states

* **Unlocked**: The term is either **Inactive**, **Blocked**, or **Unblocked** (i.e., it is *not* Locked).
* **Active**: The term is either **Blocked** or **Unblocked** (i.e., it is *not* Locked and the Condition is met).

---

## Variable Changes

## Insight

## Randomness

You can introduce random behavior to your definitions using the `_R` and `_NR` constants.

* **`_NR` (New Random):** Generates and returns a fresh random value between $0.0$ and $1.0$.
* **`_R` (Random):** Returns the most recently generated random value. Calling `_R` multiple times will always result in the same result until a new `_NR` is used.

### Restrictions

To ensure that the [Insight](#insight) and Progress systems can accurately interpret and visualize your logic, you must follow these rules when using `_R` or `_NR` in expressions:

* **Random modifications:** When used in comparisons, `_R` and `_NR` should only be modified by:
  * Addition or Subtraction.
  * Multiplication or Division by a **positive constant**.
* **Division:** A divisor must **never** contain `_R` or `_NR`.
* **Monotonicity:** Expressions containing these constants should be **non-decreasing**. As the random value increases, the output of the expression should either increase or stay the same.

> **Warning:** While the engine will not prevent you from breaking these rules, doing so will cause **Insights** to become uninformative or misleading, as the system will struggle to explain the "logic" behind a chaotic or non-linear random variable.

---
