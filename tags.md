Gherkin Executor Tags 



You can tag a Scenario or a Feature with tokens and then selectively execute various scenarios.   

Tags can be alphanumeric characters.  They can start with a @ or any other character, except for #  

    Tags are case insensitive      They cannot be NOT AND or OR.   

Scenario:  An example of tags # manual indevelopment 

Feature:  A tag example # longrunning 



If no tags are spedified at execution, all scenarios are run.   

The tag selection can be in a "tags.txt" file or specified on the command line.   

Put into Configuration.tags (a string)

--tags longrunning 

You can use AND OR and NOT in simple combinations

--tags "NOT longrunning"

-tags "NOT indevelopment"

--tags "manual AND longrunning"



For a feature or a scenario, if there are comments, then these are the tags 

Call checkTagsForSkip (comments)   returns true if skip.   

For a feature, set input interator step to the end after processing the feature 

For a scenario, background, cleanup, set skip step to true and skip processing.   In step, skip processing.    



import java.util.*;

public class TagFilterEvaluator {
    public static boolean shouldExecute(Set<String> scenarioTags, String filterExpression) {
        List<Set<String>> requiredConditions = new ArrayList<>();
        Set<String> excludedTags = new HashSet<>();
        // Parse the expression into required and excluded conditions
        parseExpression(filterExpression, requiredConditions, excludedTags);

        // Check if scenario contains any excluded tags
        boolean hasExcludedTag = scenarioTags.stream().anyMatch(excludedTags::contains);

        // Check if scenario matches any required condition group (OR logic)
        boolean matchesRequired = requiredConditions.isEmpty() || 
                                  requiredConditions.stream().anyMatch(scenarioTags::containsAll);

        // Execute if it meets a required condition AND does NOT have an excluded tag
        return matchesRequired && !hasExcludedTag;
    }

    private static void parseExpression(String expression, List<Set<String>> requiredConditions, Set<String> excludedTags) {
        // Split by "OR" to get groups
        String[] groups = expression.split(" OR ");
        for (String group : groups) {
            Set<String> tags = new HashSet<>();

            // Split each group by "AND"
            String[] elements = group.trim().split(" AND ");
            for (String element : elements) {
                element = element.trim();
                if (element.startsWith("NOT ")) {
                    excludedTags.add(element.replace("NOT ", "")); // Store excluded tags
                } else {
                    tags.add(element); // Store required tags
                }
            }

            if (!tags.isEmpty()) {
                requiredConditions.add(tags);
            }
        }
    }

    public static void main(String[] args) {
        Set<String> scenarioTags = Set.of("@t1", "@t2");  // Example scenario tags
        String filterExpression = "NOT @t1 OR @t2 AND @t3"; // Example filter

        boolean result = shouldExecute(scenarioTags, filterExpression);
        System.out.println("Should execute? " + result);
    }

}



import org.junit.jupiter.api.Test;
import java.util.Set;
import static org.junit.jupiter.api.Assertions.*;

public class TagFilterEvaluatorTest {
    @Test
    void testSingleTag() {
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@A"), "@A"), "Should execute when @A is present");
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@B"), "@A"), "Should NOT execute when @A is missing");
    }

    @Test
    void testAndCondition() {
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@A", "@B"), "@A AND @B"), "Should execute when @A and @B are present");
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@A"), "@A AND @B"), "Should NOT execute when @B is missing");
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@B"), "@A AND @B"), "Should NOT execute when @A is missing");
    }

    @Test
    void testOrCondition() {
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@A"), "@A OR @B"), "Should execute when @A is present");
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@B"), "@A OR @B"), "Should execute when @B is present");
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@C"), "@A OR @B"), "Should NOT execute when neither @A nor @B is present");
    }

    @Test
    void testNotCondition() {
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@A"), "NOT @A"), "Should NOT execute when @A is present");
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@B"), "NOT @A"), "Should execute when @A is missing");
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@B", "@C"), "NOT @B"), "Should NOT execute when @B is present");
    }

    @Test
    void testComplexExpression() {
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@A", "@B"), "@A AND @B OR @C"), "Should execute when @A and @B are present");
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@C"), "@A AND @B OR @C"), "Should execute when @C is present");
        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@D"), "@A AND @B OR @C"), "Should NOT execute when no valid group matches");

        assertFalse(TagFilterEvaluator.shouldExecute(Set.of("@A", "@B"), "NOT @A AND NOT @B"), "Should NOT execute when both @A and @B are present");
        assertTrue(TagFilterEvaluator.shouldExecute(Set.of("@C"), "NOT @A AND NOT @B"), "Should execute when @A and @B are missing");
    }

}




