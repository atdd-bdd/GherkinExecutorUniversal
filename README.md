# Gherkin Executor

Gherkin Executor translates Gherkin feature files into tests using the test framework of the native language.   The Gherkin feature files represent the detailed external behavior of an application and serve as a collaborative tool as well as executable documentation.    

These feature files have a different syntax than standard Gherkin feature files.  They include information on the data types of the columns in step tables, which helps to emphasize the domain terms and their relationships in an application.   Here is a brief example of temperature conversion: 

```
Scenario: Temperature Conversion
Calculation Convert F to C             # ListOfObject FandC
| F    | C    | Notes       |
| 32   | 0    | Freezing    |
| 212  | 100  | Boiling     |
| -40  | -40  | Below zero  |

Data FandC
| Name   | Default  | DataType  | Notes  |
| F      | 0        | Integer   |        |
| C      | 0        | Integer   |        |
| Notes  |          | Text      |        |
```

The Data statement declares the domain types of each of the columns in the scenario step.   The Integer describes that the values must be integers, rather than numbers with decimal points.   If you use abstract data types, they could be of type `Temperature`

Gherkin Executor creates a set of files from the feature file.   One is the unit test file which consists of a separate unit test for each scenario.    The second is the glue file, which is called by the unit test file and passed the data for each step.   The developer alters the glue file to call the production code.    Additionally, separate files are created for the data elements 

A quick introduction to Gherkin Executor is here:   [Behavior Driven Development with Gherkin Executor - YouTube](https://www.youtube.com/watch?v=OYALWe8X1yY)

The common documentation and examples are in this repository.   Separate repositories exist for each language that Gherkin Executor has been ported to: 

Java: [GitHub - atdd-bdd/GherkinExecutorForJava: Gherkin Executor for Java translates Gherkin files into unit tests](https://github.com/atdd-bdd/GherkinExecutorForJava)

C#: [GitHub - atdd-bdd/GherkinExecutorForCSharp](https://github.com/atdd-bdd/GherkinExecutorForCSharp)

Python: [GitHub - atdd-bdd/GherkinExecutorForPython](https://github.com/atdd-bdd/GherkinExecutorForPython)

C++: [GitHub - atdd-bdd/GherkinExecutorCPP](https://github.com/atdd-bdd/GherkinExecutorCPP)    

Each repository has the examples in the appropriate language, except for C++.   The examples are in 

[GitHub - atdd-bdd/TestsForGherkinExecutorForCPP](https://github.com/atdd-bdd/TestsForGherkinExecutorForCPP)

The detaiiled documentation includes some examples of an implementation in Java just to demonstrate the flow.   The individual language repositories have the examples in each project that show the implementation in the particular language. 

## Information

- [Overview.md](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/Overview.md) - This gives an overview of Gherkin Executor and how it flows.  

- [Detail.md](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/details.md) - This has details the Data, Import, and Define statements, as well as on tables and multi-line strings,

- [Examples.md](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/examples.md) - More detail on the examples.  

- [Design.md](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/design.md) – the approach taken as well as maintenance notes  

- [Other.md](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/other.md) – stuff that just does not seem to fit elsewhere  

- [Style.md](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/style.md) – some thoughts on how to organize your feature files

- [GherkinExecutorBase/why gherkin executor.md at main · atdd-bdd/GherkinExecutorBase · GitHub](https://github.com/atdd-bdd/GherkinExecutorBase/blob/main/why%20gherkin%20executor.md) - why Gherkin Executor was created
  
  #### Videos

- [Behavior Driven Development with Gherkin Executor - YouTube](https://www.youtube.com/watch?v=OYALWe8X1yY)  

- [Setup of Gherkin Executor - YouTube](https://www.youtube.com/watch?v=uH9CgctrGgk)  

- [Test Driven Development with Gherkin Executor - YouTube](https://www.youtube.com/watch?v=CQphLJ6ndok)  

- [Domain Driven Design and Gherkin Executor - YouTube](https://www.youtube.com/watch?v=N4J3L_KEQeU)

#### Gherkin Editor

There is an editor that runs on Windows to edit extended Gherkin files.  It features a spreadsheet-like dialog for editing tables.   

See [Release Initial Release · atdd-bdd/GherkinEditor · GitHub](https://github.com/atdd-bdd/GherkinEditor/releases/tag/V1.0)



### Example of Domain Terms with Gherkin Executor

Eric Evans has a model for shipping shown in Chapter Nine: Making Implicit Concepts Explicit of' Domain-Driven Design: Tackling Complexity in the Heart of Software.  (https://www.amazon.com/Domain-Driven-Design-Tackling-Complexity-Software/dp/0321125215/). 

Following is a feature file that documents part of that model - the creation of an itinerary for a cargo ship.    As you can see, the domain terms (e.g. origin) and domain data types (e.g. Location) are shown in the file.   One might begin with this and then add 

```
Feature: Shipping 

Scenario: Cargo has an Itinerary    
Given cargo is:                                 # ListOfObject Cargo 
| ID  | Origin | Destination | Weight |
| 234 | SGSIN  | NLRTM       | 2000   |
When itinerary request is made 
Then itineary produced:   # ListOfObject Itinerary
| Origin | Destination | Arrival Time     |
| SGSIN  | NLRTM       | 2025-07-22T14:30 |
And legs are # ListOfObject Leg
| Vessel      | load  | LoadTime         | unload | unloadTime       | 
| IMO 9319466 | SGSIN | 2025-07-13T02:30 | EGPSD  | 2025-07-18T02:30 | 
| IMO 3234345 | EGPSD | 2025-07-18T14:30 | NLRTM  | 2025-07-22T14:30 | 

Scenario:  Domain Term Location 
* These are examples of IDs for each port 
| ID     | City               | Country      |
| SGSIN  | Port of Singapore  | Singapore    |
| EGPSD  | Port Said          | Egypt        |
| NLRTM  | Port of Rotterdam  | Netherlands  |

Data Leg 
| Name       | Default | DataType | Notes |
| vessel     |         | VesselID |       |
| load       |         | Location |       |
| loadTime   |         | DateTime |       |
| unload     |         | Location |       |
| unloadTime |         | DateTime |       |


Data Itinerary 
| Name         | Default | DataType | Notes |
| Origin       |         | Location |       |
| Destination  |         | Location |       |
| Arrival Time |         | DateTime |       |

Data  Cargo 
| Name        | Default | DataType   | Notes |
| Cargo       |         | CargoID    |       |
| Origin      |         | Location   |       |
| Destination |         | Location   |       |
| Weight      |         | Pounds     |       |

Data Location 
| Name     | Default  | DataType     | Notes                       |
| ID       |          | LocationID   |                             |
| City     |          | Text         | Includes state or province  |
| Country  |          | Country      |                             |

```

# 
