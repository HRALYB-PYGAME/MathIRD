# MathIRD Well-Defined Logic Standards

| Rule # | Rule Name | Description |
| :--- | :--- | :--- |
| **1** | **Linearity Constraint** | Random variables (`_R`, `_NR`) used in comparisons must only be modified by addition, subtraction, or multiplication by positive constants. |
| **2** | **Zero-Divisor Safety** | No division operations may have a divisor that contains a random variable. |
| **3** | **Monotonicity** | Random expressions must be strictly non-decreasing. If `_R` increases, the output must either increase or stay the same. |
| **4** | **Single-Random Scoping** | In a single comparison or term, only use one type of random (`_R` OR `_NR`) to avoid inconsistent "event" logic during insight generation. |
| **5** | **No Inversion** | Multiplication of a random variable by a negative number is prohibited, as it violates the Monotonicity rule and flips the range. |
| **6** | **Root-Level Conditionals** | `IfNodes` must only exist as Root Nodes of an expression. This means clean and consistent expression definitions. |
