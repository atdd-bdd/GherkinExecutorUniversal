### Other Notes

#### Philosophy differences:

GherkinExecutor's design approach emphasizes ease of use.  Other frameworks using Gherkin place more of an emphasis on 
the phrasing of the step statements.    You can always add comments to a scenario to give additional information as to 
what is supposed to be going on.    

For those who use parameterized tests, the Gherkin data represents the data in those tests in a standard format (a table). 
You can use the concept of parameterized tests with any unit testing framework, as Gherkin provides the ability to run 
the same test with different sets of data.   .   

GE uses the native unit testing framework, so there are no additional tools are required.  You  use the same reporting
framework as the unit tests. 

GE does not create html tables with Red/Green.   Many people have found that no one looks at them.  The assert statements
in the framework will show what does not work (or at least the first row).  

#### Whitespace

All comments (line being with `#`) are ignored.  All blank lines are
ignored.  If you begin a line with other than a keyword or a table or a string,
it will be ignored.  You can see what lines are actually used in the `feature.txt` file in each feature directory. 






