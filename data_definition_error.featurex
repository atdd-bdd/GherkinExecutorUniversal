Feature: Data Definition Error

Scenario Simple Table with int bad
Given table is # ListOfObject ATest 
| anInt  | aString    | aDouble  |
| q      | something  | 1.1      |

Scenario Simple Table with double bad
Given table is # ListOfObject ATest 
| anInt  | aString    | aDouble  |
| 1      | something  | r        |


Scenario Simple Table with initializer bad
Given table is bad initializer # ListOfObject ATestBad 
| anInt  | 
| 1      | 

# will create an internal class name 
Data ATest 
| Name     | Default  | Datatype  | Note  |
| anInt    | 0        | Int       |       |
| aString  | ~        | String    |       |
| aDouble  | 4.0      | Double    |       |

Data ATestBad 
| Name     | Default  | Datatype  | Note  |
| anInt    | a        | Int       |       |
| aString  | ~        | String    |       |
| aDouble  | b        | Double    |       |
