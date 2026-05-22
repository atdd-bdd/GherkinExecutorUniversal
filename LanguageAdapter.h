#pragma once
#include <string>
#include <vector>

namespace gherkinexecutor {

class LanguageAdapter {
public:
    virtual ~LanguageAdapter() = default;

    // --- Context (set once per feature) ---
    virtual void setFeatureName(const std::string& name) = 0;
    virtual void setGlueClass(const std::string& cls) = 0;

    // --- Files ---
    virtual std::string sourceExtension() = 0;  // ".cpp", ".java", ".py", ".cs"
    virtual std::string headerExtension() = 0;  // ".h" or "" (no separate header)
    virtual bool usesHeaderFile() = 0;

    // --- Types ---
    virtual std::string stringType() = 0;
    virtual std::string listType(const std::string& elem) = 0;
    virtual std::string listOfListType() = 0;
    virtual std::string alterDataType(const std::string& dslType) = 0;
    virtual std::string convertFromString(const std::string& nativeType,
                                          const std::string& valueExpr) = 0;

    // --- Namespace / package ---
    virtual std::string namespaceOpen(const std::string& ns) = 0;
    virtual std::string namespaceClose() = 0;
    virtual std::string packageDeclaration(const std::string& pkg) { return ""; }
    virtual std::string namespaceBlockClose() { return ""; }  // one-shot close for C# namespace block
    virtual std::string importStatement(const std::string& what) = 0;

    // --- Class structure ---
    virtual std::string classOpen(const std::string& name, const std::string& base) = 0;
    virtual std::string classClose() = 0;
    virtual std::string fieldDeclaration(const std::string& type,
                                         const std::string& name,
                                         const std::string& defaultVal) = 0;
    virtual std::string accessSpecifier(const std::string& level) = 0;

    // --- Context extension ---
    virtual void setPackagePath(const std::string&) {}  // called once per feature, before testFileIncludes

    // --- Test framework ---
    virtual std::vector<std::string> testFileIncludes() = 0;
    virtual std::string testClassBase() = 0;
    virtual std::string testMethodAnnotation() = 0;
    virtual std::string testMethodSignature(const std::string& scenarioName) = 0;
    virtual std::string glueInstantiation(const std::string& glueObj) = 0;
    virtual std::string testBodyPrefix() { return "         "; }  // 9 spaces default (C++)
    virtual std::string failMacro(const std::string& msg) = 0;

    // Whether test methods live inside the test class (Java/Python/C#) vs outside (C++ TEST_F)
    virtual bool testClassWrapsScenarios() { return false; }
    // Closing brace/token for a test method body
    virtual std::string testMethodClose() { return "        }"; }
    // Log helper function body inserted into the test/glue class (empty = none)
    virtual std::string logFunction(const std::string& /*dirPath*/) { return ""; }
    // DNCString field declaration (language-specific)
    virtual std::string dncStringDeclaration() { return ""; }
    // Signature for the Background/Cleanup helper method on the test class
    virtual std::string backgroundMethodSignature(const std::string& name,
                                                   const std::string& cls,
                                                   const std::string& obj) {
        return "    static void test_" + name + "(" + cls + " & " + obj + "){";
    }
    // Call to Background/Cleanup method inside a test method
    virtual std::string backgroundCall(const std::string& name, const std::string& obj) {
        return "        test_" + name + "(" + obj + ");";
    }
    // Call to cleanup at end of previous scenario (between scenarios)
    virtual std::string cleanupCall(const std::string& obj) {
        return "         test_Cleanup(" + obj + "); // from previous";
    }
    // Call to cleanup at the very end of the feature (after last scenario)
    virtual std::string finalCleanupCall(const std::string& obj) {
        return "         test_Cleanup(" + obj + "); // at the end";
    }
    // Log call qualified with class name (needed when called outside class scope)
    virtual std::string qualifiedLogCall(const std::string& value) { return logCall(value); }
    // Imports / using statements for data class files (non-C++)
    virtual std::vector<std::string> dataFileImports() { return {}; }

    // --- Collection initialisation (test body) ---
    virtual std::string listVarDecl(const std::string& elemType,
                                    const std::string& varName) = 0;
    virtual std::string listInitOpen() = 0;
    virtual std::string listInitClose() = 0;
    virtual std::string builderCreate(const std::string& cls) = 0;
    virtual std::string builderSet(const std::string& field,
                                   const std::string& value) = 0;
    virtual std::string builderBuild() = 0;
    virtual std::string multilineStringLiteral(const std::string& content) = 0;

    // --- Data class helpers ---
    virtual std::string headerGuard() = 0;                                // "#pragma once" or ""
    virtual std::string toStringMethodName() = 0;                         // "to_string", "toString", "__str__", "ToString"
    virtual std::string equalsMethodSignature(const std::string& cls) = 0;
    virtual std::string valueToString(const std::string& nativeType,
                                      const std::string& valueExpr) = 0;  // convert typed value to string rep

    // --- Glue template ---
    virtual std::vector<std::string> glueFileIncludes() = 0;
    virtual std::string glueMethodSignature(const std::string& name,
                                            const std::string& paramType,
                                            const std::string& paramName) = 0;
    virtual std::string consoleOutput(const std::string& expr) = 0;
    virtual std::string logCall(const std::string& value) = 0;
    virtual std::string logCallInline(const std::string& value) { return logCall(value); }
    virtual std::string traceConsoleOutput(const std::string& funcName) { return consoleOutput("\"---  \" + std::string(\"" + funcName + "\")"); }
    virtual std::string traceLogCall(const std::string& funcName) {
        return logCall("\"---  \" + std::string(\"" + funcName + "\")");
    }
    virtual std::string forEachElementLog(const std::string& /*elemType*/, const std::string& listName, const std::string& logExpr) {
        return "        for (const auto& v : " + listName + ") { " + logCallInline(logExpr) + " }";
    }
    // Prefix for the conversion method name (e.g. "to" → toFandCInternal(), "To" → ToFandCInternal())
    virtual std::string conversionMethodPrefix() { return "to"; }
    // No-parameter glue method signature (default: delegate to glueMethodSignature with empty params)
    virtual std::string noParamGlueMethodSignature(const std::string& name) {
        return glueMethodSignature(name, "", "");
    }
    // Scalar (single-value) glue method: may differ from list signature (e.g. no const&)
    virtual std::string scalarGlueMethodSignature(const std::string& name,
                                                   const std::string& paramType,
                                                   const std::string& paramName) {
        return glueMethodSignature(name, paramType, paramName);
    }
    // Trace output for scalar glue method (may differ from list trace)
    virtual std::string singleValueTrace(const std::string& funcName) {
        return traceConsoleOutput(funcName);
    }
    // Fail macro for scalar glue method (may differ from list fail)
    virtual std::string singleValueFail(const std::string& msg) {
        return failMacro(msg);
    }
    virtual std::string objectListElementPrint(const std::string& expr) { return consoleOutput(expr + " + \" \""); }
    virtual std::string objectListNewline() { return consoleOutput("\"\""); }
    virtual std::string forEachBegin(const std::string& elemType,
                                     const std::string& listName) = 0;
    virtual std::string forEachEnd() = 0;
    virtual std::string innerListOpen() { return "{"; }
    virtual std::string innerListClose() { return "}"; }
    // Nested loops for ListOfList / ListOfListOfObject glue stubs (8-space base indent)
    virtual std::string listOfListOuterLoopBegin(const std::string& listName) {
        return "        for (const auto& row : " + listName + ") {";
    }
    virtual std::string listOfListInnerLoopBegin() {
        return "            for (const auto& element : row) {";
    }
    virtual std::string listOfListInnerLoopEnd() { return "            }"; }
    virtual std::string listOfListOuterLoopEnd() { return "        }"; }
    virtual std::string glueClassClose() { return classClose(); }
    // Multiline string components (for line-by-line emission)
    virtual std::string multilineStringDeclSuffix() { return ""; }
    virtual std::string multilineStringFirstLine(const std::string& line) { return "R\"(" + line; }
    virtual std::string multilineStringBodyLine(const std::string& line) { return line; }
    virtual std::string multilineStringClose() { return ")\""; }
};

} // namespace gherkinexecutor
