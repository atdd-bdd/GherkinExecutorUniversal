---
name: Source Implementation References
description: Paths to the four existing language-specific GherkinExecutor implementations used as reference
type: reference
originSessionId: f3b3c682-9f05-44ee-b38b-b23f7b6043f1
---
All under C:\Users\user\source\repos\:

- **C++ (GherkinExecutorCPP):** `GherkinExecutorCPP\` — Translate.cpp/.h, DataConstruct.cpp/.h, StepConstruct.cpp/.h, TemplateConstruct.cpp/.h, Configuration.yaml
- **C# (GherkinExecutorForCSharp):** `GherkinExecutorForCSharp\Translaste.cs` — single-file translator; generated output in `GherkinExecutor\Feature_*\`
- **Python (GherkinExecutorForPython):** `C:\Users\user\Dropbox\Projects\GherkinExecutorForPython\` — `gherkinexecutor\Translate.py`; generated output in `tests\gherkinexecutor\Feature_*\`
- **Java (GherkinExecutorForJava):** `C:\Users\user\ideaprojects\GherkinExecutorForJava\src\main\java\gherkinexecutor\Translate.java`; generated output in `src\test\java\gherkinexecutor\Feature_*\`
- **Documentation/examples:** `GherkinExecutorBase\` — details.md, examples.md, all .featurex sample files
