Feature: Gherkin Translator Full Test
# Uses the testfeature file to see if the created code continues to match expectations

Scenario: Run the Test and check the output 

Given feature file is 
| full_test.feature.sav |

When translated

Then test file should match expected # ListOfObject FileNames transpose
| Expected  | gherkinexecutor/Feature_Full_Test/Feature_Full_Test.exp  |
| Actual    | gherkinexecutor/Feature_Full_Test/Feature_Full_Test.java |

And glue template file should match expected # ListOfObject FileNames transpose
| Expected  | gherkinexecutor/Feature_Full_Test/Feature_Full_Test_glue.exp   |
| Actual    | gherkinexecutor/Feature_Full_Test/Feature_Full_Test_glue.tmpl  |

Data FileNames
| Name      | Default     |
| Expected  | NoFileName  |
| Actual    | NoFileName  |





