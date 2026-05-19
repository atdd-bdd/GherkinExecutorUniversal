Feature: Define 

Define 
| Name       | Value | Notes                 |
| HIGH_DATA  | 100   | Highest allowed input |
| LOW_DATA   | 1     | Lowest allowed input  |

Scenario:  Simple Replacement
Given this data: # ListOfObject IDValue
| ID  | Value     |
| A   | HIGH_DATA |
| B   | LOW_DATA  |
Then should be equal to data: # ListOfObject IDValue
| ID  | Value     |
| A   | 100       |
| B   | 1         |

Define 
| Name           | Value                     | Notes                             |
| AVERAGE_VALUE  | (LOW_DATA + HIGH_DATA)/2  | will be passed at "(1 + 100)/2)"  |

Scenario:  Try out replacements with a calculation
Given this data: # ListOfObject IDValue
| ID  | Value          |
| A   | HIGH_DATA      |
| B   | LOW_DATA       |
| C   | AVERAGE_VALUE  |
Then should be equal to data: # ListOfObject IDValue
| ID  | Value        |
| A   | 100          |
| B   | 1            |
| C   | (1 + 100)/2  |

Data IDValue 
| Name | Default |
| ID   |         |
| Value|         | 




