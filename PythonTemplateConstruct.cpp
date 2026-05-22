#include "PythonTemplateConstruct.h"
#include "Configuration.h"
#include "Translate.h"

namespace gherkinexecutor {

    static std::string makeImportForDataDirectory(const std::string& packagePath) {
        return "from " + packagePath + " import *";
    }

    void PythonTemplateConstruct::beginTemplate() {
        for (const std::string& line : parent->linesToAddForDataAndGlue)
            templatePrint(line);

        templatePrint(makeImportForDataDirectory(parent->packagePath));
        templatePrint("from typing import List");
        templatePrint("import sys");
        templatePrint("");
        templatePrint("class " + parent->glueClass + ":");
        templatePrint("    DNCString = \"" + Configuration::doNotCompare + "\"");
        std::string logFn = parent->logIt();
        if (!logFn.empty()) templatePrint(logFn);
        templatePrint("");
    }

    void PythonTemplateConstruct::endTemplate() {
        for (const std::string& line : parent->linesToAddToEndOfGlue)
            templatePrint(line);
        try {
            if (glueTemplateFile) glueTemplateFile->close();
        }
        catch (const std::exception&) { parent->error("Error in closing"); }
    }

    void PythonTemplateConstruct::makeFunctionTemplate(const std::string& dataType,
                                                         const std::string& fullName) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;

        templatePrint("    def " + fullName + "(self, value: " + dataType + ") -> None:");
        templatePrint("        print(\"---  \" + \"" + fullName + "\")");
        if (Configuration::logIt) {
            templatePrint("        self.log(\"---  \" + \"" + fullName + "\")");
            templatePrint("        self.log(str(value))");
        }
        templatePrint("        print(value)");
        if (!Configuration::inTest)
            templatePrint("        raise NotImplementedError(\"Must implement\")");
        templatePrint("");
    }

    void PythonTemplateConstruct::makeFunctionTemplateIsList(const std::string& dataType,
                                                              const std::string& fullName,
                                                              const std::string& listElement) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;

        templatePrint("    def " + fullName + "(self, values: " + dataType + ") -> None:");
        templatePrint("        print(\"---  \" + \"" + fullName + "\")");
        if (Configuration::logIt) {
            templatePrint("        self.log(\"---  \" + \"" + fullName + "\")");
            templatePrint("        self.log(str(values))");
        }
        std::string internalName = listElement + "Internal";
        templatePrint("        for value in values:");
        templatePrint("            print(value)");
        templatePrint("             # Add calls to production code and asserts");
        if (dataType != "List[List[str]]" && listElement != "str" &&
            parent->dataNamesInternal.find(internalName) != parent->dataNamesInternal.end())
            templatePrint("            i = value.to_" + internalName + "()");
        if (!Configuration::inTest)
            templatePrint("        raise NotImplementedError(\"Must implement\")");
        templatePrint("");
    }

    void PythonTemplateConstruct::makeFunctionTemplateNothing(const std::string& dataType,
                                                               const std::string& fullName) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;

        templatePrint("    def " + fullName + "(self) -> None:");
        templatePrint("        print(\"---  \" + \"" + fullName + "\")");
        if (Configuration::logIt)
            templatePrint("        self.log(\"---  \" + \"" + fullName + "\")");
        if (!Configuration::inTest)
            templatePrint("        raise NotImplementedError(\"Must implement\")");
        templatePrint("");
    }

    void PythonTemplateConstruct::makeFunctionTemplateObject(const std::string& dataType,
                                                              const std::string& fullName,
                                                              const std::string& listElement) {
        if (checkForExistingTemplate(dataType, fullName)) return;
        parent->glueFunctions[fullName] = dataType;

        templatePrint("    def " + fullName + "(self, values: " + dataType + ") -> None:");
        templatePrint("        its = self.convert_list(values)");
        templatePrint("        print(its)");
        if (Configuration::logIt)
            templatePrint("        self.log(\"---  \" + \"" + fullName + "\")");
        if (!Configuration::inTest)
            templatePrint("        raise NotImplementedError(\"Must implement\")");
        templatePrint("");
    }

} // namespace gherkinexecutor
