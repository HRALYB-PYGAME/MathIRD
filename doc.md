# MathIRD

## Variable

Variable is a class containing the definition of a variable. It does not contain its value as that is stored in [Game State](#game-state) object. Variable definition is made of following:

| Member | Type | Description |
| :--- | :--- | :--- |
| name | ```string``` | Name of the Variable, usually short cryptic name like ```x``` or ```lrun``` |  
| scoreParams | [```ScoreParams```](#scoreparams) | Score parameters of the Variable. Contribution of the Variable to total score depends on these. Made of these distinct parameters: knee, offset and polarity. More at [ScoreParams](#scoreparams). |
| unlockCondition | [```unique_ptr<Node>```](#node) | Variable is unlocked after this condition is true. |
| defaultValue | ```double``` | Value of the Variable after it is unlocked. This value is also used as a replacement for locked soft Variables. |
| granularity | ```double``` | Radius of the non-white part of a [Packet](#packet) depends on this and a Packet delta. The larger the granularity the smaller the radius. |
| homeButton | [```Button*```](#button) | A pointer to a home button. On such button current value of our Variable is often displayed. [Packets](#packet) travel towards a home button of go-to variable. |
| asDependencyInTerms | ```set<Term*>``` | Set of pointers to [Terms](#term) that depends on this Variable |
| asInputInTerms | ```set<Term*>``` | Set of pointers to [Terms](#term) that can be affected by a value of this Variable |
| asOutputInTerms | ```set<Term*>``` | Set of pointers to [Terms](#term) that can change the value of this Variable |
| asBlockerInTerms | ```set<Term*>``` | Set of pointers to [Terms](#term) that will be blocked when this Variable is |
| displayedAtButtons | ```set<Button*>``` | Set of pointers to [Buttons](#button) that value of this Variable is displayed on |

### ScoreParams

ScoreParams is a struct made of these 3 members. Together they define what effect does a value of a variable towards total score.

- knee - Defines where calculated score reaches ```100```.
- offset - Offset is subtracted from the variable value before any calculation are done.
- polarity - More in the table below.

Score is calculated slightly differently for each Polarity (formulas uses ```V``` as a variable value, ```O``` as offset and ```K``` as knee):

| Polarity | Formula | Description |
| :--- | :--- | :--- |
| Neutral | 0 | No effect on the score |
| Normal | ```[(log(V-O)/log(K)]*100``` | Larger values contribute towards score more. Values smaller or equal to offset dont contribute at all. |
| Inverted | ```[(log(O-V)/log(K)]*100``` | Smaller values contribute towards score more. Values greater or equal to offset dont contribute at all. |
| Absolute | ```[(log(\|V-O\|)/log(K)]*100``` | Values further from offset contribute towards score more. |

Keep in mind that if any value inside log() is not positive, final result will be 0.
Same goes for the entire expression inside square brackets.

## Game State

Game State is a class. It contains current state of the game.

## Node

## Packet

## Button

## Term
