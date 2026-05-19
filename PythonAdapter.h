#pragma once
#include "LanguageAdapter.h"
#include "Configuration.h"

namespace gherkinexecutor {

class PythonAdapter : public LanguageAdapter {
public:
    void setFeatureName(const std::string& name) override { featureName = name; }
    void setGlueClass(const std::string& cls) override { glueClass = cls; }
    void setPackagePath(const std::string& path) override { packagePath = path; }

    std::string sourceExtension() override { return ".py"; }
    std::string headerExtension() override { return ""; }
    bool usesHeaderFile() override { return false; }

    std::string stringType() override { return "str"; }
    std::string listType(const std::string& elem) override { return "List[" + elem + "]"; }
    std::string listOfListType() override { return "List[List[str]]"; }
    std::string alterDataType(const std::string& dslType) override;
    std::string convertFromString(const std::string& nativeType,
                                  const std::string& valueExpr) override;

    std::string namespaceOpen(const std::string&) override { return ""; }
    std::string namespaceClose() override { return ""; }
    std::string packageDeclaration(const std::string&) override { return ""; }
    std::string importStatement(const std::string& what) override {
        return "import " + what;
    }

    std::string classOpen(const std::string& name, const std::string& base) override {
        if (base.empty()) return "class " + name + ":";
        return "class " + name + "(" + base + "):";
    }
    std::string classClose() override { return ""; }
    std::string fieldDeclaration(const std::string& type, const std::string& name,
                                 const std::string& defaultVal) override;
    std::string accessSpecifier(const std::string&) override { return ""; }

    std::vector<std::string> testFileIncludes() override;
    std::string testClassBase() override { return "unittest.TestCase"; }
    std::string testMethodAnnotation() override { return ""; }
    std::string testMethodSignature(const std::string& scenarioName) override;
    std::string glueInstantiation(const std::string& glueObj) override;
    std::string failMacro(const std::string& msg) override {
        return "self.fail(\"" + msg + "\")";
    }

    bool testClassWrapsScenarios() override { return true; }
    std::string testMethodClose() override { return ""; }
    std::string dncStringDeclaration() override {
        return "    DNCString = \"" + Configuration::doNotCompare + "\"";
    }
    std::string backgroundMethodSignature(const std::string& name,
                                           const std::string&,
                                           const std::string& obj) override {
        return "    def test_" + name + "(self, " + obj + "):";
    }
    std::string backgroundCall(const std::string& name, const std::string& obj) override {
        return "        self.test_" + name + "(" + obj + ")";
    }
    std::string cleanupCall(const std::string& obj) override {
        return "        self.test_Cleanup(" + obj + ") # from previous";
    }

    std::string listVarDecl(const std::string& elemType, const std::string& varName) override {
        return varName + " : " + listType(elemType);
    }
    std::string listInitOpen() override { return " = ["; }
    std::string listInitClose() override { return "]"; }
    std::string builderCreate(const std::string& cls) override { return cls + ".Builder()"; }
    std::string builderSet(const std::string& field, const std::string& value) override {
        return "." + field + "(" + value + ")";
    }
    std::string builderBuild() override { return ".build()"; }
    std::string multilineStringLiteral(const std::string& content) override {
        return "\"\"\"\n" + content + "\"\"\"";
    }

    std::string headerGuard() override { return ""; }
    std::string toStringMethodName() override { return "__str__"; }
    std::string equalsMethodSignature(const std::string& cls) override;
    std::string valueToString(const std::string& nativeType,
                              const std::string& valueExpr) override;

    std::vector<std::string> glueFileIncludes() override;
    std::string glueMethodSignature(const std::string& name,
                                    const std::string& paramType,
                                    const std::string& paramName) override;
    std::string consoleOutput(const std::string& expr) override {
        return "        print(" + expr + ")";
    }
    std::string logCall(const std::string& value) override {
        return "        self.log(" + value + ")";
    }
    std::string forEachBegin(const std::string& elemType, const std::string& listName) override {
        return "        for value in " + listName + ":";
    }
    std::string forEachEnd() override { return ""; }

    std::vector<std::string> dataFileImports() override { return {}; }

private:
    std::string featureName;
    std::string glueClass;
    std::string packagePath;

    static std::string toSnakeCase(const std::string& name);
};

} // namespace gherkinexecutor
