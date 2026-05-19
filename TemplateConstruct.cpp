#include <string>
#include <memory>
#include <fstream>
#include <algorithm>
#include "TemplateConstruct.h"
#include "Configuration.h"

namespace gherkinexecutor {

    class Translate;

    // When adapter doesn't use header files, route header prints to source file.
    void TemplateConstruct::templateHeaderPrint(const std::string& line) {
        if (!parent->getAdapter()->usesHeaderFile()) {
            templatePrint(line);
            return;
        }
        try {
            if (glueTemplateFileHeader)
                *glueTemplateFileHeader << line << std::endl;
        }
        catch (const std::exception&) { parent->error("IO ERROR"); }
    }

    void TemplateConstruct::templatePrint(const std::string& line) {
        try {
            if (glueTemplateFile)
                *glueTemplateFile << line << std::endl;
        }
        catch (const std::exception&) { parent->error("IO ERROR"); }
    }

    bool TemplateConstruct::checkForExistingTemplate(const std::string& dataType,
                                                      const std::string& fullName) {
        auto it = parent->glueFunctions.find(fullName);
        if (it != parent->glueFunctions.end()) {
            if (it->second != dataType)
                parent->error("function " + fullName + " datatype " + it->second +
                              " not equals " + dataType);
            return true;
        }
        return false;
    }

    void TemplateConstruct::makeFunctionTemplate(const std::string& dataType,
                                                  const std::string& fullName) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;
        LanguageAdapter* adapter = parent->getAdapter();

        templateHeaderPrint("   void " + fullName + "(" + dataType + " values); ");
        templatePrint(adapter->scalarGlueMethodSignature(fullName, dataType, "value"));
        templatePrint(adapter->singleValueTrace(fullName));
        if (Configuration::logIt)
            templatePrint(adapter->logCall("\"--- " + fullName + "\""));
        templatePrint(adapter->consoleOutput("value"));
        if (!Configuration::inTest)
            templatePrint("        " + adapter->singleValueFail("Must implement"));
        templatePrint("    }");
        templatePrint("");
    }

    void TemplateConstruct::makeFunctionTemplateIsList(const std::string& dataType,
                                                        const std::string& fullName,
                                                        const std::string& listElement) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;
        LanguageAdapter* adapter = parent->getAdapter();
        std::string stype = adapter->stringType();
        std::string lsType = adapter->listType(stype);
        std::string llsType = adapter->listOfListType();

        templateHeaderPrint("   void " + fullName + "(const " + dataType + "& values); ");
        templatePrint(adapter->glueMethodSignature(fullName, dataType, "values"));
        templatePrint(adapter->traceConsoleOutput(fullName));

        if (dataType == lsType || dataType == "const " + lsType) {
            if (Configuration::logIt) {
                templatePrint(adapter->logCall("\"---  \" + std::string(\"" + fullName + "\")"));
                templatePrint("        for (const auto& v : values) { " +
                              adapter->logCallInline("v") + " }");
            }
            templatePrint(adapter->forEachBegin(listElement, "values"));
            templatePrint("     " + adapter->consoleOutput("value"));
            templatePrint("             // Add calls to production code and asserts");
            templatePrint(adapter->forEachEnd());
        }
        else if (dataType == llsType) {
            templatePrint("        for (const auto& row : values) {");
            templatePrint("            for (const auto& element : row) {");
            templatePrint("    " + adapter->objectListElementPrint("element"));
            if (Configuration::logIt)
                templatePrint("        " + adapter->logCall("element"));
            templatePrint("            }");
            templatePrint("    " + adapter->objectListNewline());
            templatePrint("        }");
            if (Configuration::logIt)
                templatePrint(adapter->logCall("\"---  \" + std::string(\"" + fullName + "\")"));
        }
        else {
            if (Configuration::logIt) {
                templatePrint(adapter->logCall("\"---  \" + std::string(\"" + fullName + "\")"));
                templatePrint("        for (const auto& v : values) { " +
                              adapter->logCallInline("v." + adapter->toStringMethodName() + "()") + " }");
            }
            std::string internalName = listElement + "Internal";
            templatePrint(adapter->forEachBegin(listElement, "values"));
            templatePrint("     " + adapter->consoleOutput("value." + adapter->toStringMethodName() + "()"));
            templatePrint("             // Add calls to production code and asserts");
            if (listElement != stype &&
                parent->dataNamesInternal.find(internalName) != parent->dataNamesInternal.end())
                templatePrint("              " + internalName + " i = value.to" + internalName + "();");
            templatePrint(adapter->forEachEnd());
        }

        if (!Configuration::inTest)
            templatePrint("        " + adapter->failMacro("Must implement"));
        templatePrint("    }");
        templatePrint("");
    }

    void TemplateConstruct::makeFunctionTemplateNothing(const std::string& dataType,
                                                         const std::string& fullName) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;
        LanguageAdapter* adapter = parent->getAdapter();

        templateHeaderPrint("   void " + fullName + "(); ");
        templatePrint("    void " + parent->glueClass + "::" + fullName + "(){");
        templatePrint(adapter->traceConsoleOutput(fullName));
        if (Configuration::logIt)
            templatePrint(adapter->logCall("\"---  \" + std::string(\"" + fullName + "\")"));
        if (!Configuration::inTest)
            templatePrint("        " + adapter->failMacro("Must implement"));
        templatePrint("    }");
        templatePrint("");
    }

    void TemplateConstruct::makeFunctionTemplateObject(const std::string& dataType,
                                                        const std::string& fullName,
                                                        const std::string& listElement) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;
        LanguageAdapter* adapter = parent->getAdapter();

        templateHeaderPrint("   void " + fullName + "(const " + dataType + "& values); ");
        templatePrint(adapter->glueMethodSignature(fullName, dataType, "values"));
        if (Configuration::logIt)
            templatePrint(adapter->logCall("\"---  \" + std::string(\"" + fullName + "\")"));
        templatePrint("    for (const auto& row : values) {");
        templatePrint("        for (const auto& element : row) {");
        templatePrint(adapter->objectListElementPrint("element"));
        if (Configuration::logIt)
            templatePrint("    " + adapter->logCall("element"));
        templatePrint("        }");
        templatePrint(adapter->objectListNewline());
        templatePrint("    }");
        if (!Configuration::inTest)
            templatePrint("        " + adapter->failMacro("Must implement"));
        templatePrint("    }");
        templatePrint("");
    }

    int TemplateConstruct::processNamespaces(const std::string& packagePath) {
        LanguageAdapter* adapter = parent->getAdapter();
        // Java/C#: package/namespace already emitted by glueFileIncludes(); only C++ uses per-part opens
        std::vector<std::string> parts = parent->extractNamespaceParts(packagePath);
        for (const auto& part : parts) {
            std::string s = adapter->namespaceOpen(part);
            if (!s.empty()) {
                templatePrint(s);
                templateHeaderPrint(s);
            }
        }
        return static_cast<int>(parts.size());
    }

    void TemplateConstruct::endNamespace(const std::string& packagePath) {
        LanguageAdapter* adapter = parent->getAdapter();
        size_t count = std::count(packagePath.begin(), packagePath.end(), '.');
        for (size_t i = 0; i < count + 1; ++i) {
            std::string s = adapter->namespaceClose();
            if (!s.empty()) {
                templatePrint(s);
                templateHeaderPrint(s);
            }
        }
    }

    void TemplateConstruct::beginTemplate() {
        LanguageAdapter* adapter = parent->getAdapter();

        for (const std::string& inc : adapter->glueFileIncludes())
            templatePrint(inc);

        for (const std::string& line : parent->linesToAddForDataAndGlue)
            templatePrint(line);

        if (adapter->usesHeaderFile()) {
            templateHeaderPrint(adapter->importStatement("<vector>"));
            templateHeaderPrint(adapter->importStatement("<string>"));
            templatePrint(adapter->importStatement("\"" + parent->glueClass + ".h\""));
            templatePrint(adapter->importStatement("\"" + parent->dataHeaderFilename + "\""));
            templateHeaderPrint(adapter->importStatement("\"" + parent->dataHeaderFilename + "\""));
            templatePrint("using namespace gherkinexecutor::" + parent->featureName + "; ");
        }

        processNamespaces(parent->packagePath);
        templatePrint("");

        templateHeaderPrint(adapter->classOpen(parent->glueClass, ""));
        templateHeaderPrint(adapter->accessSpecifier("public"));
        std::string dncDecl = adapter->dncStringDeclaration();
        if (!dncDecl.empty()) templateHeaderPrint(dncDecl);
        std::string logFn = parent->logIt();
        if (!logFn.empty()) templateHeaderPrint(logFn);
        templateHeaderPrint("");
    }

    void TemplateConstruct::endTemplate() {
        LanguageAdapter* adapter = parent->getAdapter();

        for (const std::string& line : parent->linesToAddToEndOfGlue)
            templatePrint(line);

        templateHeaderPrint(adapter->glueClassClose());
        endNamespace(parent->packagePath);
        std::string blockClose = adapter->namespaceBlockClose();
        if (!blockClose.empty()) templatePrint(blockClose);

        try {
            if (parent->testFile) parent->testFile->close();
            if (glueTemplateFile) glueTemplateFile->close();
        }
        catch (const std::exception&) { parent->error("Error in closing"); }
        try {
            if (glueTemplateFileHeader) glueTemplateFileHeader->close();
        }
        catch (const std::exception&) { parent->error("Error in closing"); }
    }

} // namespace gherkinexecutor
