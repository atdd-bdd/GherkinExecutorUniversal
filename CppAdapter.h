#pragma once
#include "LanguageAdapter.h"

namespace gherkinexecutor {

class CppAdapter : public LanguageAdapter {
public:
    void setFeatureName(const std::string& name) override { featureName = name; }
    void setGlueClass(const std::string& cls) override { glueClass = cls; }

    std::string sourceExtension() override { return ".cpp"; }
    std::string headerExtension() override { return ".h"; }
    bool usesHeaderFile() override { return true; }

    std::string stringType() override { return "std::string"; }
    std::string listType(const std::string& elem) override { return "std::vector<" + elem + ">"; }
    std::string listOfListType() override { return "std::vector<std::vector<std::string>>"; }
    std::string alterDataType(const std::string& dslType) override;
    std::string convertFromString(const std::string& nativeType,
                                  const std::string& valueExpr) override;

    std::string namespaceOpen(const std::string& ns) override { return "namespace " + ns + " {"; }
    std::string namespaceClose() override { return "}"; }
    std::string packageDeclaration(const std::string&) override { return ""; }
    std::string importStatement(const std::string& what) override { return "#include " + what; }

    std::string classOpen(const std::string& name, const std::string& base) override;
    std::string classClose() override { return "};"; }
    std::string glueClassClose() override { return "    };"; }
    std::string fieldDeclaration(const std::string& type, const std::string& name,
                                 const std::string& defaultVal) override;
    std::string accessSpecifier(const std::string& level) override { return level + ":"; }

    std::vector<std::string> testFileIncludes() override;
    std::string testClassBase() override { return "::testing::Test"; }
    std::string testMethodAnnotation() override { return ""; }
    std::string testMethodSignature(const std::string& scenarioName) override;
    std::string glueInstantiation(const std::string& glueObj) override;
    std::string failMacro(const std::string& msg) override { return "FAIL() << \"" + msg + "\";"; }

    std::string listVarDecl(const std::string& elemType, const std::string& varName) override;
    std::string listInitOpen() override { return " = {"; }
    std::string listInitClose() override { return "};"; }
    std::string builderCreate(const std::string& cls) override { return cls + "::Builder()"; }
    std::string builderSet(const std::string& field, const std::string& value) override;
    std::string builderBuild() override { return ".build()"; }
    std::string multilineStringLiteral(const std::string& content) override;

    std::string headerGuard() override { return "#pragma once"; }
    std::string toStringMethodName() override { return "to_string"; }
    std::string equalsMethodSignature(const std::string& cls) override;
    std::string valueToString(const std::string& nativeType,
                              const std::string& valueExpr) override;

    std::string dncStringDeclaration() override {
        return "inline static std::string DNCString = \"?DNC?\";";
    }
    std::string logFunction(const std::string& dirPath) override;

    std::vector<std::string> glueFileIncludes() override;
    std::string glueMethodSignature(const std::string& name,
                                    const std::string& paramType,
                                    const std::string& paramName) override;
    std::string consoleOutput(const std::string& expr) override;
    std::string logCall(const std::string& value) override;
    std::string traceConsoleOutput(const std::string& funcName) override {
        return "        std::cout << \"---  \" << \"" + funcName + "\" << std::endl;";
    }
    std::string scalarGlueMethodSignature(const std::string& name, const std::string& paramType,
                                          const std::string& paramName) override {
        return "    void " + glueClass + "::" + name + "(" + paramType + " " + paramName + " ) {";
    }
    std::string singleValueTrace(const std::string& funcName) override {
        return "        std::cout << \"--- << " + funcName + "\" << std::endl;";
    }
    std::string singleValueFail(const std::string& msg) override {
        return "FAIL(\"" + msg + "\");";
    }
    std::string objectListElementPrint(const std::string& expr) override {
        return "            std::cout << " + expr + " << \" \";";
    }
    std::string objectListNewline() override {
        return "        std::cout << std::endl;";
    }
    std::string logCallInline(const std::string& value) override {
        return "log(" + value + ");";
    }
    std::string forEachBegin(const std::string& elemType, const std::string& listName) override;
    std::string forEachEnd() override { return "              }"; }
    std::string qualifiedLogCall(const std::string& value) override {
        return "        " + featureName + "::log(" + value + ");";
    }
    std::string finalCleanupCall(const std::string& obj) override {
        return "         test_Cleanup(" + obj + "); // at the end";
    }

private:
    std::string featureName;
    std::string glueClass;
};

} // namespace gherkinexecutor
