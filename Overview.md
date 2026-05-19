# Gherkin Executor

## Summary

The Gherkin Executor simplifies the creation of test code from a Gherkin feature file. 
It converts scenarios into unit tests.   It is designed to work the same way as any 
implementation language.   For Java, the application is available at 
https://github.com/atdd-bdd/GherkinExecutorForJava.  It works with JUnit5, Junit4, and TestNG.  
Versions in other languages (Python, C++, C#, Kotlin, etc.) are being created. 

## Inspiration

The table form for expressing shared understanding came from Ward Cunningham's FIT
(Framework for Integrated Testing).  Gherkin was created by Aslak Hellesøy.  
The process for creating unit tests (ala SpecFlow) was from Gáspár Nagy. Dan North coined the term “Behavior Driven Development”. 

### More Information

This README file gives an overview of operation.  See these other files for more information.  

* [Detail.md](https://github.com/atdd-bdd/GherkinExecutorForJava/blob/main/details.md) - This has details the Data, Import, and Define statements, as well as on tables and multi-line strings,

* [Examples.md](https://github.com/atdd-bdd/GherkinExecutorForJava/blob/main/examples.md) - More detail on the examples.

* [Design.md](https://github.com/atdd-bdd/GherkinExecutorForJava/blob/main/design.md) – the approach taken as well as maintenance notes

* [Other.md](https://github.com/atdd-bdd/GherkinExecutorForJava/blob/main/other.md) – stuff that just does not seem to fit elsewhere

* [Style.md](https://github.com/atdd-bdd/GherkinExecutorForJava/blob/main/style.md) – some thoughts on how to organize your feature files  
  
  

## Videos

- [Behavior Driven Development with Gherkin Executor - YouTube](https://www.youtube.com/watch?v=OYALWe8X1yY)

- [Setup of Gherkin Executor - YouTube](https://www.youtube.com/watch?v=uH9CgctrGgk)

- [Test Driven Development with Gherkin Executor - YouTube](https://www.youtube.com/watch?v=CQphLJ6ndok)

- [Domain Driven Design and Gherkin Executor - YouTube](https://www.youtube.com/watch?v=N4J3L_KEQeU)

## Why Use Gherkin?

Gherkin feature files are readable executable documentation.  If a requirement / story
includes business rules, they provide a convenient way to collaborate with non-programmers.   They 
also provide a way to document parts of a domain driven design. 
For a developer they provide an alternative way to specify the values used 
to call a parametrized test method.  The scenarios can be used in user documentation.

For domains which have two-dimensional objects, the tabular format can be much easier
to comprehend. (See the tic-tac-toe example.)

You can think of a Gherkin feature file as representing the logical portion of the test (ala the logical and physical views of a database).

### Introductory Example

Here are a few introductory examples. You can view the `examples.featurex` file in the main test directory 
and the associated files in the gherkinexecutor.Feature_Examples package in the test directory.   The first scenario shows a calculation from Fahrenheit to
Celsius.  When it is translated, three files are created - one is a unit test file, one 
is glue code which is revised to connect to the production code, and the third are 
the  data files which declare the class used to connect the unit test code to the glue code.
The full code is shown later.

```
Scenario: Temperature Conversion
Calculation Convert F to C # ListOfObject FandC 
| F    | C    | Notes       |
| 32   | 0    | Freezing    |
| 212  | 100  | Boiling     |
| -40  | -40  | Below zero  |
```

An Excel-style table is a familiar construct to many non-programmers.
(Ward Cunningham introduced this style with FIT).  So this form is easily understandable.
To add another variation, you simply add another row to the table.  The comments
`# ListOfObject FandC` are used to create the unit tests (details shown shortly)

The Triad (Customer, Developer, Tester perspectives) can collaborate on the detailed behavior.  This
separates the production implementation from the representation of the logic and calculations.  For example,
one of the Triad might ask about the result for this calculation:  

```
| F    | C       | Notes                                              |
| 33   | .555    | How many decimal digits or what sort of roundoff?  |
```

The Triad would then discuss what the behavior should be.  Note that
the internal representation of F or C is irrelevant - it could be double, Double, BigInteger, or
something else.   

If your code style is to use domain specific data types, you can create a table to show examples of 
allowable values (or all allowed values, in the case of an enum).

```
Scenario: Domain Term ID 
Rule ID must have exactly 5 letters and begin with Q # ListOfObject ValidValue
| Value   | Valid  | Notes              |
| Q1234   | true   |                    |
| Q123    | false  | Too short          |
| Q12345  | false  | Too long           |
| A1234   | false  | Must begin with Q  |
```

This table could be used in a help file to show examples of valid IDs. 

```

#### Run it

Run Translate and you should see a new package in the test directory, `gherkinexecutuor.Feature_Simple_Test`
The name comes from the Feature name in the file, not the name of the feature file. 

In this package, you will see four files.  Rename the file `Feature_Simple_Test_glue.tmpl` 
to `Feature_Simple_Test_glue.java`.   This will be the only time you will do this for each feature. 

In this glue file  you will be making changes to add calls to production 
code and make asserts.

If you add additional Scenarios that have new steps, you will need to create the step method in the glue code.
You can do this by either by copying the sample code from the ".tmpl" file or by using the IDE to create it.   
(You'll get a little more template code if you copy it in from ".tmpl") 

Run Simple_Test and you should get a test failure.   The lines in the output should read something like:

```
---  Given_table_is
ATest {anInt = 1 aString = something aDouble = 1.2 }  
Must implement
```

This is printed by the glue code, which you can take out whenever you desire.  

The first line is the method name, the next two lines show the inputs to the glue method. 
A version of the object named in the step (`# ListOfObject ATest`) with all String attributes
is passed to the glue code. A conversion method to a form of the object with attributes given in 
the Data statement is provided.   This referred to as the internal object - ATestInternal`  

No calls to production code are in this glue code, as the Scenario is completely independent of the code.

Congratulations, you're ready to start with your project.   You can delete the `SimpleTest` directory (or make the tests pass :) ) 

### Your Project

You need to import your production classes  into the glue code and possibly the data code.  
You can alter the glue code, or you can add the imports in Configuration with: 

```
linesToAddForDataAndGlue.add("import production_package;"); 
```

Now create a feature file and put it into the feature directory.   If you want, you can create a `features`directory and set `searchTree`to true and feature files will be searched in that directory.   

```
public static final String featureSubDirectory = "src/test/java/";
```

Add the name of the feature file as a program argument to the run, or add it to the file list in Configuration, e.g.

```
featureFiles.add("my_feature.featurex");
```

Note: If you already have a feature file editor in your IDE, the editor may show some errors on `Include`, `Data` and `Import` lines.  Simply change these lines to `* Include`, `* Data` and `* Import`.

Run Translate.  You should see a new package in tests. Let's say you have `Feature My Feature` as the first line of this feature file. The package will be: `gherkinexecutor\Feature_My_Feature
`  
Rename the `.tmpl` glue file to `.java` and run the test file (the one named `Feature_My_Feature`). 

You should get a failure for every scenario.   Note that` fail() `is put into each step, so you will continue to get failure for a Scenario 
 until you implement all the steps.   

If you don't use any comments on the steps, all data to glue methods will be passed as a `List<List<String>` 

You can use `examples.featurex` as a guide for your feature file.  The key difference between this feature file and one
you may see other places is the addition of `# ListOfObject FandC` after the step and a Data statement.

```
Data FandC
| Name   | Default  | DataType  | Notes  |
| F      | 0        | Integer   |        |
| C      | 0        | Integer   |        |
| Notes  |          | String    |        |
```

The key is that the name of the Data `FandC` must match the name in the step.   And the names of the 
attributes (e.g. `F`) must match the headers in the step table. You should get errors from Translate if they do not match and 
 will get compile errors if you ignore those errors.  (There is probably some condition where this does not occur in 
some unexpected condition.) 

#### A Few Quick Notes

You can use the name of a production class as the second part of Data statement. A file will be created with that name with the extension `.tmpl`. You can use this file as a starting point for your production class if you do not already have it implemented.  If the production class exists, you just need to add an `import` to the Configuration, such as:

```
        static {
            linesToAddForDataAndGlue.add("import production.*;");
        }
```

The names of the attributes should match the attributes in the production class. If the attributes have non-primitive
data types, you will need to create an `Import` statement that describes the constructor and the package. 

Do not create a Data statement with the first part being the name you use in production.    
Add additional characters to it, such as `In` or you will get a data clash.

## Converting a Gherkin feature file

You can convert an existing Gherkin feature file to one usable by GherkinExecutor 
in a few steps.

- If there is no data passed to a step, then nothing needs to be done. 

- If a step is passed a multiline string, no change. 

- If a step has a value specified within it, move that value to a new line 
  after the step and surround it with `|`,   e.g.  `|4|`

- If the step uses a table with a header that represents the names of the fields, 
  create a `Data` statement with the names of those fields and default values. The values will be passed as strings 
* Put `# ListOfObject _className_` on the end of the step line.

* If the old glue code used a `transpose` for a table, enter that on the step in the feature file. e.g. `# ListOfObject _className_ transpose`

Now if your production code uses primitives or abstract data types, add
data types to the `Data` statement, if you want to get an internal object with non-string attributes.      

To implement the tests, move the glue code in the existing glue steps to 
the corresponding functions in the new glue class.   Now you'll need to
convert the glue parameter to that expected by the old glue code or 
alter the code to use the new parameter name;   

If your test code depends upon "fancy stuff" of your existing framework, wait on converting it until you’ve had a little experience with GherkinExecutor.  

## How It Works in Brief

The translator converts a Gherkin feature file into a unit test file.
The unit test file calls the glue methods which the developer edits to call the code under 
test. The translator also creates a template for the glue file. Each Data statement produces either one or two class files.
(a class with all String attributes and optionally a class file with the declared DataTypes.)    

The Translator is a single file containing all the necessary components. 
To translate a feature file, you supply the feature files you want converted by either adding them 
to a list in the Translator, adding them as program arguments, or letting the directory tree search find them.    

Unlike other implementations of Gherkin, each feature file is associated with one unit
test file, its glue file, and multiple class file in the same package.   

If you need the same data types in multiple files, you can use the `Include` statement
to add those data types to a feature. For example, `Include "datadefs.txt"`would add whatever
is in that file to the feature file before it is translated.   

## Why Not Use Existing Frameworks?

I've been using Cucumber, one of the most common applications that uses 
Gherkin, for a number of years.  You can have a table after each step.   However, you
need to add additional code to use that table as a list of objects.  The method for doing 
so has changed from version to version and has gotten more complex.  It appears that conversion
of tables into lists of objects has been removed from the latest version.

The Translator converts the Gherkin tables into initialized lists. The developer just needs to 
specify the class of the objects that will be in those lists. It does not depend on 
features of a specific languages, such as introspection. 

A feature file written for one language should work
in the other languages.  The only issues would be replacing the Datatype of a field (e.g. Int)
with the appropriate type in another language.  

Since the entire source code is supplied, a developer can alter the translation to
their preferred style.  For example, if methods should not have underscores, 
that can be changed in a single line.  

## How It Differs from Other Frameworks

The Translator passes data to steps only through tables or multi-line strings.  There are no values embedded in the
step statement.  The domain term which a value represents is the header of a column
which contains the values.  

The tables are passed to the glue methods as a list of objects or a list of strings.  

The glue code for each feature resides in the feature package.  There is no regular expression matching. 
The step statement is converted into the glue code function name.   

If you need the same code to execute for multiple glue code methods, you can delegate to a common implementation,
just like you would do with production code. 

The `Translate` file resides in your application source control.  The way it works does not change.  If Translate works for you, 
there is no reason to change it.    If you want additional features (not sure what they might be), then you decide
when you want to drop in a new version.  If there are any issues, you revert to the old one using git (or your
source code control framework).  You can use your existing framework (Cucumber, SpecFlow, etc.) to process your existing 
feature files and just use GherkinExecutor for new ones.  

The tests are unit tests of your test framework.  So the reporting of your framework works just like your existing reporting. 
 Your customization of the glue code is the same as you would do for a regular unit test.   The only difference would be that you
need to save in glue code class variables any values required for communication between separate steps in a scenario. 

You can alter Translate if you want more output or less from the glue code.       

### Keywords

There are more keywords than in standard Gherkin. You can add additional ones by modifying Translate.  
In addition to the keywords below, lines can begin with 

`#`   comment

`|`  beginning of a table

`“””` beginning of a multi-line string 

Lines beginning with anything else are ignored.  

The` “feature.txt”` file in each feature directory shows the lines that 
Actually used for that feature.   

```
    "Feature" - first non-blank line in a file - gives name of feature               
    "Scenario" - starts a unit test 

    "Given"  - all of these are a step, which is a method in the glue code.  
    "When"
    "Then"
    "And"
    "Star"
    "Arrange"
    "Act"
    "Assert"
    "Trigger"
	"Verify"
	"Assemble"
	"Activate"
	"Preconditions"
	"MainCourse"
	"Exception"
    "Postconditions"
    "Rule"   -  this is just a step, not something special as in Cucumber. 
    "Calculation"
    "*"  - this is the * from Gherkin.   Note it is converted to `Star` for the glue code method

    "Background" - specifies one or more glue code methods to run at beginning of a scenario 
    "Cleanup" - specifies one of more glue code methods to run at the end of a scenario 

    "Data" - describes the attributes of an object
    "Import" - describes the constructor and package for non-primitive attributes 
    "Define" - defines words and their values that can replace the words in tables.  
`   "* Data" - alternative for Data, to have Gherkin editors not report error
    "* Import" - alternative for Import, 
    "* Define" - alternative  
```

## A Collection Example

In `examples.featurex` is an example of dealing with a collection.  There is a collection 
of records (objects of the `LabelValue` type).   In the glue code for the Given, they are added to the solution object. 
The example uses the ID type for the label with the validation shown previously.   

The interface to this object includes a method to set a filter and a method to retrieve a sum of records/objects
that match that filter.   This might be accomplished in a single operation.   It's been split into two to show
variations in how you can pass data to the glue classes.  

Both scenarios start with a Given that provides the records.  In the first version, the values are provided using data objects.  
Notice on the When, the table is transposed, so that it looks as you might enter this data into a dialog box.  

```
Scenario: Filter Data Another Way  
# filters data 
Given list of numbers # ListOfObject LabelValue
| ID     | Value  |
| Q1234  | 1      |
| Q9999  | 2      |
| Q1234  | 3      |
When filtered by # ListOfObject FilterValue transpose
| Name   | ID     |
| Value  | Q1234  |
Then result # ListOfObject ResultValue 
| Sum  |
| 4    |

* Data FilterValue
| Name   | Default  | DataType  | Notes  |
| Name   |          | String    |        |
| Value  | Q0000    | ID        |        |

* Data ResultValue
| Name  | Default  | DataType  | Notes  |
| Sum   |          | Integer   |        |

* Data LabelValue
| Name   | Default  | DataType  | Notes  |
| ID     |          | ID        |        |
| Value  | 0        | Integer   |        |
```

### Passing Only One Value

Now because there is only one value that is passed to each glue function, there is an alternative.  The data will be
passed as a List<List<String>.   In the glue code, you use the only element with `values.get(0).get(0)` and convert it to the desired data type.  It is your decision as to how you want to handle this.  

```
Scenario: Filter Data 
# filters data 
Given list of numbers # ListOfObject LabelValue 
| ID     | Value  |
| Q1234  | 1      |
| Q9999  | 2      |
| Q1234  | 3      |
When filtered by ID with value
| Q1234  |
Then sum is 
| 4 | ```
```

## Configuration

Here are the `Configuration` parameters in `Translate`.   You can alter four of them on the command line with `searchTree`,  inTest, traceOn, and `logIt.   `

These values should work for the default JetBrains layout.  You can alter them for another project layout.

You can move the configuration class out of Translate into a separate file.   That way, if you ever reload
Translate, you can delete the `Configuration` class in the new `Translate` and keep using the one you have changed.

```

    static class Configuration {

        public static boolean logIt = false;
                 // Set to true for logging during the tests to log.txt
        public static final boolean inTest = false;  // switch to true for development of Translator
        public static final boolean traceOn = false; // Set to true to see trace

        public static final char spaceCharacters = '~'; // Will replace with space in table values

        public static final String doNotCompare = "?DNC?";  // Value used for not comparing an attribute
        public static String currentDirectory = "";  // For printout to check where IDE is running 

        public static final String featureSubDirectory = "src/test/java/"; // where features are to be found
        public static final String packageName = "gherkinexecutor";

        public static String addToPackageName = "";  // change to "test.java." for Eclipse

        public static final String testSubDirectory = "src/test/java/" + packageName + "/"; 
                // where the files containing the test, glue, and data files (inside package with Feature name)

        public static final String dataDefinitionFileExtension = "java"; // "tmpl"; 
            // change to .tmpl if altering data classe, becuase data class files are rewritten every time. 

       public static final String testFramework = "JUnit5"; // Could be "JUnit4" or "TestNG"

        public static final List<String> linesToAddForDataAndGlue = new ArrayList<>();
        // These lines will be added to the glue file and the data class files 
        // Must include semicolon if needed
        static {
            linesToAddForDataAndGlue.add("import java.util.*;");
        }

        public static final List<String> featureFiles = new ArrayList<>();
        // These feature files will always be translated.
        static {
            featureFiles.add("import.featurex");
            featureFiles.add("include.featurex");
```

### A Larger Domain Example

Here is domain related example  to show how domain terms could be incorporated into a feature file.   A` Forecast` is a Domain Term that has multiple attributes.  The data type for each attribute is shown, along with a default value.  

```
Data Forecast
| Name        | Default   | Datatype           | Notes               |
| Day         | 1/1/2025  | Date               |                     |
| Time        | 12:00 am  | Time               |                     |
| High        | 100       | Temperature        | High Temperature    |
| Low         | 0         | Temperature        | Low Temperature     |
| Rain        | 0         | Percentage         | Chance of Rain      |
| Wind Speed  | 0         | Speed              |                     |
| Direction   | N         | CompassDirection   |                     |
| Condition   | Clear     | WeatherCondition   | Cloudy, Rain, etc.  |
```

Here's a possible scenario to search forecasts for those that meet certain conditions:

```

Scenario: Search Forecast
Given forecast is           # ListOfObject Forecast
| Day       | Time      | High  | Low  | Rain  | Wind Speed  | Direction  | Condition  |
| 1/1/2025  | 12:00 am  | 70    | 60   | 0     | 1           | N          | Clear      |
| 1/3/2025  | 12:00 am  | 60    | 40   | 10    | 5           | S          | Cloudy     |

# And many more  (or read from CSV file)

When searching for          # ListOfObject # ForecastSearchCriteria 
| Field        | Relationship  | Value  |
| High         | >             | 65     |
| Wind Speed   | <             | 5      |
Then results are # ListOfObject Forecast 
| Day       | Condition  |
| 1/1/2025  | Clear      |
```

The Given step uses the domain terms defined in the previous block. The data could be
listed here, read from a CSV file, or put onto a database. The When step gives the search
criteria.  The Triad would define how these criteria should work.
Finally, the Then step shows the expected results.   
