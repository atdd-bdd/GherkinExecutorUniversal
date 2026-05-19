---
name: GherkinExecutorUniversal Project Overview
description: High-level purpose and goal of the GherkinExecutorUniversal project
type: project
originSessionId: f3b3c682-9f05-44ee-b38b-b23f7b6043f1
---
GherkinExecutorUniversal is a C++ application that translates `.featurex` (extended Gherkin) files into unit test scaffolding code.

**Why:** The four existing translators (Java, Python, C#, C++) each have separate codebases. The universal version does the same job from a single C++ codebase, with output language selected via Configuration.yaml.

**Output languages:** C++ (gtest), Python (unittest/pytest), Java (JUnit 5), C# (MSTest)

**How to apply:** When implementing or extending, the core parsing logic is language-neutral; only code emission is language-specific. Use the LanguageAdapter strategy pattern.

**Key config addition:** `outputLanguage: "cpp"` (or "python", "java", "csharp") in Configuration.yaml
