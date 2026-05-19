# Style

This article discusses some style and development issues.

## TDD versus BDD

In TDD, the basic cycle is Red-Green-Refactor.  With GherkinExecutor, you can perform the same cycle.  For business rules
and calculations, you start by writing down one or more rows in a table.  For example:

```
Scenario: Temperature 
Calculation Convert F to C # ListOfObject TemperatureCalculation 
| F    | C    | Notes       |
| 32   | 0    | Freezing    |
| 212  | 100  | Boiling     |
| -40  | -40  | Below zero  |
```

If you only have one row, then start with that, make it green, and then refactor.  

```
Scenario: Temperature 
Calculation Convert F to C # ListOfObject TemperatureCalculation
| F    | C    | Notes       |
| 32   | 0    | Freezing    |
```

If you have two or more rows, you can approach TDD in one of three ways.   

* You can experience a failure for the rows for which you haven't implemented the underlying code.
  You may wish to do this if you are working on a simple function or calculation and don't want to run` Translate`everytime you add a  row.

* You can comment out all the rows except for the ones you have completed and the one you are working one 
  As you uncomment them, you will need to run Translate again

* You can loop around the rows in the glue code and only do those you are currently working with
  You  need be sure to loop for all the values before leaving the glue code !!! 
    ```
          for (int i = 0; i < 3 && i < values.size(); i++) {
              value = values.get(i);
              System.out.println(value);
              TemperatureCalculationInternal i = value.toTemperatureCalculationInternal();
              int c = TemperatureCalculations.convertFahrenheitToCelsius(i.f);
              assertEquals(i.c, c, i.notes);
          }
  
  
  ```
### User Interface Testing

 You could use the values in the table as the input values to a User Interface (UI). The Given could set up the state.  The When could print out the values to input, and the Then could check that the state has changed appropriately.  

### Iterative Development of Feature File

   You can begin with a `Data` statement that just lists the  attributes (fields) and default values.  The glue function will be passed a list of objects that contain string values. It's up to you to convert the values as desired.    

## Comparison of Example Table and Step Table

  Here's an example of an Example table in other Gherkin implementations.   The Example statement does not exist in GherkinExecutor, as there are no values passed in steps.    
```
Feature: Calculator
Scenario Outline: Add two numbers
Given I have a calculator
When I add <number1> and <number2>
Then the result should be <result>
Examples:
| number1 | number2 | result |
| 2       | 3       | 5      |
| 10      | 20      | 30     |
| -1      | 1       | 0      |
```
The glue code looks like:   
```
public class CalculatorSteps {
  private Calculator calculator;
  private int result;
  @Given("I have a calculator")
  public void i_have_a_calculator() {
      calculator = new Calculator();
  }

  @When("I add {int} and {int}")
  public void i_add_and(int number1, int number2) {
      result = calculator.add(number1, number2);
  }

  @Then("the result should be {int}")
  public void the_result_should_be(int expectedResult) {
      assertEquals(expectedResult, result);
  }
}
```
With GherkinExecutor, the feature file could look like
```
Feature: Calculator

Scenario: Add two numbers
When I add two numbers I get the result # ListOfObject Calculation
| number1  | number2  | result  |
| 2        | 3        | 5       |
| 10       | 20       | 30      |
| -1       | 1        | 0       |

Data Calculation   @ describes the attributes of the fields 
| Name     | Default  | Datatype  |
| number1  | 0        | int       |
| number2  | 0        | int       |
| result   | 0        | int       |
      Data Calculation

```
The glue code looks like the following   Note the two lines of code you needed to write were the creation of the calculator 
and the assertEquals statement.   
```
    private void When_I_add_two_numbers_I_get_the_result(List<Calculation> values ) {
        for (Calculation value : values){
            CalculationInternal i = value.toCalculationInternal();
            assertEquals(i.result, calculator.add(i.number1, i.number2);
        }
```
### Difference in Parameterized Test and a Gherkin Table
Here is a parameterized test in a unit testing framework.   The format will differ between frameworks, but the concept is the same. 
```
    @ParameterizedTest
    @MethodSource("provideTemperaturesForConversion")
    void testFahrenheitToCelsius(int fahrenheit, int expectedCelsius) {
        assertEquals(expectedCelsius, TemperatureConverter.fahrenheitToCelsius(fahrenheit));
    }

    private static Stream<Arguments> provideTemperaturesForConversion() {
        return Stream.of(
            Arguments.of(32, 0),
            Arguments.of(212, 100),
            Arguments.of(-40, -40)
        );
    }
}
class TemperatureConverter {
   static int FahrenheitToCelsius(int fahrenheit) {
        return ((fahrenheit - 32) * 5) / 9;
    }
}
```
With GherkinExecutor, the feature file would look like the following:   
```
Scenario: Temperature 
# Business rule , Calculation 
Calculation Convert F to C # ListOfObject TemperatureCalculation 
| F    | C    | Notes       |
| 32   | 0    | Freezing    |
| 212  | 100  | Boiling     |
| -40  | -40  | Below zero  |

Data TemperatureCalculation
| Name   | Default  | DataType  | Notes  |
| F      | 0        | Int       |        |
| C      | 0        | Int       |        |
| Notes  |          | String    |        |
```
Following is the glue code, which is generated by Translate.  You
only need to add the same assertEquals.  Although in this case, 
you can add the note as a message.   

    void Calculation_Convert_F_to_C(List<TemperatureCalculation> values) {
        for (TemperatureCalculation value : values) {
            try {
                TemperatureCalculationInternal i = value.toTemperatureCalculationInternal();
                assertEquals(i.c, TemperatureCalculations.convertFahrenheitToCelsius(i.f), i.notes);;

            } catch (Exception e) {
                System.err.println("Argument Error " + value.toString() + TemperatureCalculationInternal.toDataTypeString());
            }
        }
    }
```
