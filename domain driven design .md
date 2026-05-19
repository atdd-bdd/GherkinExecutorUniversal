# Design

Simple stuff - easy to understand, easy to program.    

But it's the complicated stuff (as Eric Evans refers to in his book)



The exceptions, the boundary conditions, etc. 



What is lacking in regular Gherkin files is an easy way to denote the datatypes of the domain terms.   And the domain terms themselves may not be transparent if an example or step table is not used.  

  If there are many columns in a table, then maintaining it (or even understanding it) can be difficult.    

By having the Data statement, the data types are transparent.   If someone is filling out more rows in a table, they can see what the valid values are.    In the glue code, all values are converted to the domain types indicated in the data table.    

By using classes to hold the field values in a table,  

 A contract - your implementation will work for a set or range of values.   If a value is supplied that is not in that set or range, there is no guarantee that is won't blow up.     



James and ...     necessary, but not sufficient . 



Not a should, but this is the way that domain experts think about the system. 



given   



Dan north: 

    Scenario: Account is in credit
    
    Given the account is in credit
    And the card is valid
    And the dispenser contains cash
    When the customer requests cash
    Then ensure the account is debited
    And ensure cash is dispensed
    And ensure the card is returned



    Scenario: Account is overdrawn past the overdraft limit
    
    Given the account is overdrawn
    And the card is valid
    When the customer requests cash
    Then ensure a rejection message is displayed
    And ensure cash is not dispensed
    And ensure the card is returned

Ideas:

The bigger picture - all of the things that the application interacts with.   

    Now these can be decomposed into smaller interactions. 

But if you don't have the big picture, then you have synthesis, rather than differentiation 

The Johnny Cash song 



And I havent even got started on the fees, the number of withdrawals allowed per day, the maximum amount of withdrawals, etc.   





Story: Account Holder withdraws cash
As an Account Holder
I want to withdraw cash from an ATM
So that I can get money when the bank is closed


Scenario 1: Account has sufficient funds
Given the account balance is \$100
And the card is valid
And the machine contains enough money
When the Account Holder requests \$20
Then the ATM should dispense \$20
And the account balance should be \$80
And the card should be returned






Scenario 2: Account has insufficient funds
Given the account balance is \$10
And the card is valid
And the machine contains enough money
When the Account Holder requests \$20
Then the ATM should not dispense any money
And the ATM should say there are insufficient funds
And the account balance should be \$20
And the card should be returned
Scenario 3: Card has been disabled
Given the card is disabled
When the Account Holder requests \$20
Then the ATM should retain the card
And the ATM should say the card has been retained
Scenario 4: The ATM has insufficient funds
...

Use a Define if the value in a table is long (it could use named parameters if desired ).   It could be a string of almost any length)










