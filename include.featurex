Feature: Include 

Scenario: An include 
Given a string include
""" 
Include "string.inc"
"""
Then should be equal to 
"""
This is an include string from the main directory
"""
Scenario: An include from base directory 
Given a string include
""" 
Include 'string.inc'
"""
Then should be equal to 
"""
This is an include string from the main directory
"""


Scenario: An include of CSV file
Given a table # ListOfObject CSVContents
Include "TableExample.csv"
Then Should be equal to table # ListOfObject CSVContents 
| A  | B    | C   |
| a  | b,c  | d,  |
| 1  | 2    | 3   |

Data CSVContents 
| Name  | Default  |
| A     |          |
| B     |          |
| C     |          |


