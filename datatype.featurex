Feature: Data Types

Scenario: Use the data types 
Given type values are  # ListOfObject SomeTypes
| anInt  | aDouble  | aChar  | achar  |
| 0      | 0.0      | x      | y      |
| 111    | 222.2    | q      |        |
Then this should be equal # ListOfObject SomeTypes
# Note the order is changed just to show order is independent
| achar  | anInt  | aDouble  | aChar  |
| y      | 0      | 0.0      | x      |
|        | 111    | 222.2    | q      |

# There are more, these are common 

Data SomeTypes 
| Name     | Default  | Data Type  | Notes  |
| anInt    | 0        | int        |
| aDouble  | 0.0      | Double     |        |
| aChar    | x        | Character  |        |
| achar    | y        | char       |        |


