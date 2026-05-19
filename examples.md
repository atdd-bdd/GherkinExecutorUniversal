# Examples

Here are a  some details on examples as well as some other examples. 

## Example Feature

Here is the example feature file again:

```
Feature: Examples

Scenario: Temperature 
# Business rule , Calculation 
Calculation Convert F to C # ListOfObject TemperatureCalculation 
| F    | C    | Notes       |
| 32   | 0    | Freezing    |
| 212  | 100  | Boiling     |
| -40  | -40  | Below zero  |

Data TemperatureCalculation
| Name   | Default  | DataType  | Notes  |
| F      | 0        | Integer   |        |
| C      | 0        | Integer   |        |
| Notes  |          | String    |        |
```

In the test directory, it is named `examples.featurex`.  The words after the keyword 
`Feature` are combined into the name of the feature.  Let's assume that you are using the translator with Java 
(language suffix `.java`).  The operation is the same, the output code depends on the language. 

To translate this feature file, you can pass it as a parameter to the `Translate` main method, 
add it to the Configuration list of feature files, or let the directory tree search find it.      

```
featureFiles.add("examples.featurex");
```

A unit test file with the name `Feature_Examples.java` (with language appropriate suffix) 
is created in a package with the name `gherkinexecutor.Feature_Examples` with the same name. 
Another file called `Feature_Examples_glue.tmpl` is also created.  This contains templates for the glue 
code that is called 
from `Feature_Examples.java`.  One or two files are created for every `Data` statement. Since DataTypes are 
in this Data statement, a class `TemperatureCalculation` with the attributes as Strings, 
and a class `TemperatureCalculationInternal` are created. Some common methods for each class are also included.

The single step in the `Scenario` ("`Convert F to C` ") has additional information `# ListOfObject TemperatureComparison`.  So 
the test file / glue code parameters use a `List<TemperatureComparison>`to pass this data between the two.  This
class has all attributes as Strings (so is referenced as the string object).   

Note that a new glue object is created for each Scenario.   This makes each test independent.   If you need 
to share values between scenarios, create a class variable (e.g. static or companion object).  

### Test File

The `Feature_Examples.java` example file contains code that looks like

```
    @Test
    void test_Scenario_Temperature(){
        Feature_Examples_glue feature_Examples_glue_object = new Feature_Examples_glue();

        List<TemperatureCalculation> objectList1 = List.of(
             new TemperatureCalculation.Builder()
                .f("32")
                .c("0")
                .notes("Freezing")
                .build()
        // plus two more 
            );
        feature_Examples_glue_object.Calculation_Convert_F_to_C(objectList1);
        }
```

This file is recreated every time Translate is run.  You run Translate every time you change the feature file.    

### Glue File

The `Feature_Examples_glue.tmpl` file has the following in it. 

```
   void Calculation_Convert_F_to_C(List<TemperatureCalculation> values ) {
        System.out.println("---  " + "Calculation_Convert_F_to_C");
        for (TemperatureCalculation value : values){
             System.out.println(value);
             // Add calls to production code and asserts
              TemperatureCalculationInternal i = value.toTemperatureCalculationInternal();
              }
        fail("Must implement"); 
    }

```

It's possible that the values in the table are not valid for the types for the field.   For example, 
if `a` is in the table for a field with datatype `int`, then an IllegalArgumentException is thrown and will be
caught by the test framework.   

You can eliminate the `println`.  It is there so that you can see the data passed to the glue code, without having to 
look back at the feature file.   

The first time you run the Translator, you should rename the glue file `Feature_Examples_glue.tmpl`to the language appropriate suffix
(e.g. rename it from `.tmpl` to `.java`).  You will be making changes in this file to 
call your production code.  If you add new steps to the feature, you can copy a template for the new steps from 
the template file (`.tmpl`) to the glue source file (`.java`).  Alternatively, you can just let the IDE suggest that you need 
a new method in  Feature_Examples_glue.

Here's a possible call to an implementation in the glue file (with the `println `eliminated).  Note you provide essentially
the same code that you would in a unit test. The data is provided in the parameter, as would be in a parameterized unit
test.   

```
    void Calculation_Convert_F_to_C(List<TemperatureCalculation> values) {
         for (TemperatureCalculation value : values) {
                TemperatureCalculationInternal i = value.toTemperatureCalculationInternal();
                int c = TemperatureCalculations.convertFahrenheitToCelsius(i.f);
                assertEquals(i.c, c, i.notes);
        }
    }
```

An example production implementation might look like: 

```
public class TemperatureCalculations {
    static int convertFahrenheitToCelsius(int input) {
        return ((input - 32) * 5) / 9;
    }
}
```

You can use any step keyword for the step in the feature file, but `Calculation, Rule, or *` might be appropriate.  `*` is
from standard Gherkin.

## Domain Term

Here's an example for a domain term that was introduced previously.  The example is an ID which must have
the characteristics shown in the scenario. 

```
Scenario: Domain Term ID
Rule ID must have exactly 5 letters and begin with Q # ListOfObject ValueValid
| Value   | Valid  | Notes              |
| Q1234   | true   |                    |
| Q123    | false  | Too short          |
| Q12345  | false  | Too long           |
| A1234   | false  | Must begin with Q  |

Data ValueValid
| Name   | Default  | DataType  |
| Value  | 0        | String    |
| Valid  | false    | Boolean   |
| Notes  |          | String    |
```

In the step method, you implement a call to the method that turns a string into the object.
In this example, the constructor is called with each value in the table.  The constructor throws 
an IllegalArgumentException if the value is not valid.  

```
        void Rule_ID_must_have_exactly_5_letters_and_begin_with_Q(List<ValueValid> values) {
        System.out.println("---  " + "Rule_ID_must_have_exactly_5_letters_and_begin_with_Q");
        for (ValueValid value : values) {
            System.out.println(value);
            boolean result = false;
            boolean expectedException = !Boolean.valueOf(value.valid);
            try {
                new ID(value.value);
                if (expectedException) {
                    fail("Invalid value did not throw exeception "
                            + value.value + " " + value.notes);
                }
            } catch (Exception e) {
                if (!expectedException)
                    fail("Valid value threw exeception "
                            + value.value + " " + value.notes);
            }
        }
    }

```

The constructor might look like: 

```
   public ID(String value){

        if (value.length() < 5 )
            throw new IllegalArgumentException("Too short");
        if (value.length() > 5)
            throw new IllegalArgumentException("Too long");
        if (value.charAt(0) != 'Q')
            throw new IllegalArgumentException("Must begin with Q");
    }
```

### A Method Providing Validation

If you are using a method to perform the validation, the method might look like this:

```
boolean isValid() {
   if (value.length() < 5 )
       return false
   if (value.length() > 5)
       return false
   if (value.charAt(0) != 'Q')
       return false
   return true
   }
```

Instead of catching an` IllegalArgumentException`, you would just check the return valid of `isValid()`
