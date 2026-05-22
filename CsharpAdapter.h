#pragma once
#include "LanguageAdapter.h"
#include "Configuration.h"

namespace gherkinexecutor {

class CsharpAdapter : public LanguageAdapter {
public:
    void setFeatureName(const std::string& name) override { featureName = name; }
    void setGlueClass(const std::string& cls) override { glueClass = cls; }
    void setPackagePath(const std::string& path) override { packagePath = path; }

    std::string sourceExtension() override { return ".cs"; }
    std::string headerExtension() override { return ""; }
    bool usesHeaderFile() override { return false; }

    std::string stringType() override { return "string"; }
    std::string listType(const std::string& elem) override { return "List<" + elem + ">"; }
    std::string listOfListType() override { return "List<List<string>>"; }
    std::string alterDataType(const std::string& dslType) override;
    std::string convertFromString(const std::string& nativeType,
                                  const std::string& valueExpr) override;

    // C# uses a single namespace block (not per-part); handled via packageDeclaration
    std::string namespaceOpen(const std::string&) override { return ""; }
    std::string namespaceClose() override { return ""; }
    std::string packageDeclaration(const std::string& pkg) override {
        return "namespace " + pkg + "{";
    }
    std::string namespaceBlockClose() override { return "}"; }
    std::string importStatement(const std::string& what) override {
        return "using " + what + ";";
    }

    std::string classOpen(const std::string& name, const std::string& base) override;
    std::string classClose() override { return "    }"; }
    std::string fieldDeclaration(const std::string& type, const std::string& name,
                                 const std::string& defaultVal) override;
    std::string accessSpecifier(const std::string&) override { return ""; }

    std::vector<std::string> testFileIncludes() override;
    std::string testClassBase() override { return ""; }
    std::string testMethodAnnotation() override { return "[TestMethod]"; }
    std::string testMethodSignature(const std::string& scenarioName) override;
    std::string glueInstantiation(const std::string& glueObj) override;
    std::string failMacro(const std::string& msg) override {
        return "Assert.Fail(\"" + msg + "\");";
    }

    bool testClassWrapsScenarios() override { return true; }
    std::string testMethodClose() override { return "        }"; }
    std::string dncStringDeclaration() override {
        return "    const string DNCString = \"" + Configuration::doNotCompare + "\";";
    }
    std::string backgroundMethodSignature(const std::string& name,
                                           const std::string& cls,
                                           const std::string& obj) override {
        return "    private void test_" + name + "(" + cls + " " + obj + "){";
    }
    std::string backgroundCall(const std::string& name, const std::string& obj) override {
        return "        test_" + name + "(" + obj + ");";
    }
    std::string cleanupCall(const std::string& obj) override {
        return "         test_Cleanup(" + obj + "); // from previous";
    }

    std::string listVarDecl(const std::string& elemType, const std::string& varName) override {
        m_currentElemType = elemType;
        return listType(elemType) + " " + varName;
    }
    std::string listInitOpen() override {
        return " = new List<" + m_currentElemType + ">{";
    }
    std::string listInitClose() override { return "};"; }
    std::string builderCreate(const std::string& cls) override {
        return "new " + cls + ".Builder()";
    }
    std::string builderSet(const std::string& field, const std::string& value) override {
        return "." + field + "(" + value + ")";
    }
    std::string builderBuild() override { return ".Build()"; }
    std::string multilineStringLiteral(const std::string& content) override {
        return "@\"\n" + content + "\"";
    }

    std::string headerGuard() override { return ""; }
    std::string toStringMethodName() override { return "ToString"; }
    std::string equalsMethodSignature(const std::string& cls) override;
    std::string valueToString(const std::string& nativeType,
                              const std::string& valueExpr) override;

    std::vector<std::string> glueFileIncludes() override;
    std::string glueMethodSignature(const std::string& name,
                                    const std::string& paramType,
                                    const std::string& paramName) override;
    std::string consoleOutput(const std::string& expr) override {
        return "        Console.WriteLine(" + expr + ");";
    }
    std::string logCall(const std::string& value) override {
        return "        Log(" + value + ");";
    }
    std::string forEachBegin(const std::string& elemType, const std::string& listName) override {
        return "        foreach (" + elemType + " value in " + listName + "){";
    }
    std::string forEachEnd() override { return "        }"; }

    std::vector<std::string> dataFileImports() override {
        return {
            "using System;",
            "using System.Collections.Generic;",
            "using System.Text;",
            "using System.Text.RegularExpressions;"
        };
    }
    std::string traceConsoleOutput(const std::string& funcName) override {
        return consoleOutput("\"---  \" + \"" + funcName + "\"");
    }
    std::string traceLogCall(const std::string& funcName) override {
        return logCall("\"---  \" + \"" + funcName + "\"");
    }
    std::string forEachElementLog(const std::string& elemType, const std::string& listName, const std::string& logExpr) override {
        return "        foreach (" + elemType + " v in " + listName + ") { " + logCallInline(logExpr) + " }";
    }
    std::string testBodyPrefix() override { return "            "; }  // 12 spaces (C# indentation inside namespace+class)
    std::string conversionMethodPrefix() override { return "To"; }
    std::string logFunction(const std::string& dirPath) override;
    std::string logCallInline(const std::string& value) override { return "Log(" + value + ");"; }
    std::string innerListOpen() override { return "new List<string>{"; }
    std::string innerListClose() override { return "}"; }
    std::string listOfListOuterLoopBegin(const std::string& listName) override {
        return "        foreach (List<string> row in " + listName + ") {";
    }
    std::string listOfListInnerLoopBegin() override {
        return "            foreach (string element in row) {";
    }
    std::string listOfListInnerLoopEnd() override { return "            }"; }
    std::string listOfListOuterLoopEnd() override { return "        }"; }

private:
    std::string featureName;
    std::string glueClass;
    std::string packagePath;
    mutable std::string m_currentElemType;
};

} // namespace gherkinexecutor
