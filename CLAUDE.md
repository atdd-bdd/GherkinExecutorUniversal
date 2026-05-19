# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Purpose

GherkinExecutorUniversal is a C++ application that translates `.featurex` (extended Gherkin) files into unit test scaffolding for multiple target languages. It is a language-universal rewrite of four separate implementations:

| Language | Reference repo |
|----------|---------------|
| C++ (gtest) | `C:\Users\user\source\repos\GherkinExecutorCPP\` |
| Python (unittest/pytest) | `C:\Users\user\Dropbox\Projects\GherkinExecutorForPython\` |
| Java (JUnit 5) | `C:\Users\user\ideaprojects\GherkinExecutorForJava\` |
| C# (MSTest) | `C:\Users\user\source\repos\GherkinExecutorForCSharp\` |

Documentation and `.featurex` examples live in `C:\Users\user\source\repos\GherkinExecutorBase\`.

## Build

The project targets Visual Studio on Windows. Build with:

```
# Visual Studio
GherkinExecutorUniversal.sln → Build Solution

# Or from command line with g++
g++ *.cpp -o GherkinExecutorUniversal
```

## Running

```
GherkinExecutorUniversal.exe <featurex-file>
```

Run from the directory containing `Configuration.yaml` and the `.featurex` files. Output is written under `gherkinexecutor/Feature_<name>/`.

## Configuration

`Configuration.yaml` controls all runtime behavior. Key fields:

- `outputLanguage` — `"cpp"`, `"python"`, `"java"`, or `"csharp"` (the new universal field)
- `testFramework` — e.g. `"gtest"`, `"junit5"`, `"unittest"`, `"mstest"`
- `packageName` — namespace/package wrapping generated code
- `logIt` / `traceOn` / `inTest` — logging and test-mode flags
- `featureFiles` — list of `.featurex` files to translate
- `filterExpression` / `tagFilter` — scenario filtering

See `C:\Users\user\source\repos\GherkinExecutorCPP\Configuration.yaml` for a full annotated example.

## Architecture

The translator follows a pipeline:

1. **`InputIterator`** — reads and preprocesses the `.featurex` file (strips blanks/comments, handles `Include` directives).
2. **`Translate`** — top-level orchestrator. Iterates lines and dispatches to keyword handlers (`actOnFeature`, `actOnScenario`, `actOnBackground`, etc.). Owns all output file handles.
3. **Construct classes** — each handles one grammar construct, accessed as friends of `Translate`:
   - `StepConstruct` — `Given`/`When`/`Then`/`And` steps → glue function stubs
   - `DataConstruct` — `Data` statements → typed data class files (string form + optional internal form)
   - `TemplateConstruct` — step tables (e.g. `ListOfObject`) → list initialization code in tests
   - `ImportConstruct` — `Import` statements → language imports in generated files
   - `DefineConstruct` — `Define` statements → macro/constant substitutions
4. **`Configuration`** — static class; loaded from `Configuration.yaml` at startup; referenced everywhere.

Output per feature file (`Feature_<Name>/`):
- `Feature_<Name>_test.*` — unit test file (one test per Scenario)
- `Feature_<Name>_glue.*` / `*.tmpl` — glue stubs the developer fills in
- `Feature_<Name>_data.*` — data class definitions generated from `Data` statements

### Language emission

The C++ reference implementation emits only C++. The universal version adds an `outputLanguage` config key and routes all code-emission calls through a language adapter (strategy pattern). Parsing logic (`InputIterator`, keyword dispatch, construct parsing) is shared; only the string templates and file extensions differ per language.

## Testing strategy

There are no conventional unit tests. Correctness is verified end-to-end:

1. Translate a `.featurex` file (e.g. `full_test.featurex`).
2. Compile and run the generated test suite — the trace output confirms correct step dispatch.
3. Compare generated files to `.exp` golden files; if expected changes occurred, copy new output over the `.exp` files.

See `C:\Users\user\source\repos\GherkinExecutorBase\design.md` for the full maintenance testing protocol.

## Key `.featurex` syntax

```
Feature: Name

Scenario: Description
Given step description:         # ListOfObject DataType
| Col1 | Col2 |
| val1 | val2 |

Data DataType
| Name  | Default | DataType | Notes |
| Col1  | 0       | Integer  |       |
| Col2  |         | Text     |       |
```

`~` is the default `spaceCharacters` substitution (encodes spaces in table cells). `?DNC?` is the `doNotCompare` sentinel (cell value is not asserted).
