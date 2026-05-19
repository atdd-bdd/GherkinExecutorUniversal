#include "PythonAdapter.h"
#include "Configuration.h"
#include <algorithm>
#include <cctype>

namespace gherkinexecutor {

std::string PythonAdapter::toSnakeCase(const std::string& name) {
    std::string result;
    for (char c : name) {
        if (std::isupper(c) && !result.empty())
            result += '_';
        result += std::tolower(c);
    }
    return result;
}

std::string PythonAdapter::alterDataType(const std::string& dslType) {
    if (dslType == "Int" || dslType == "Integer") return "int";
    if (dslType == "Long")    return "int";
    if (dslType == "Double" || dslType == "Decimal" || dslType == "Float") return "float";
    if (dslType == "Boolean" || dslType == "boolean") return "bool";
    if (dslType == "Char")    return "str";
    if (dslType == "Short" || dslType == "Byte" || dslType == "byte") return "int";
    if (dslType == "Text" || dslType == "String" || dslType == "string") return "str";
    return dslType;
}

std::string PythonAdapter::convertFromString(const std::string& nativeType,
                                              const std::string& valueExpr) {
    if (nativeType == "int")   return "int(" + valueExpr + ")";
    if (nativeType == "float") return "float(" + valueExpr + ")";
    if (nativeType == "bool")  return "(" + valueExpr + " in (\"true\", \"1\", \"True\"))";
    if (nativeType == "str")   return valueExpr;
    return valueExpr;
}

std::string PythonAdapter::fieldDeclaration(const std::string& type, const std::string& name,
                                             const std::string& defaultVal) {
    if (defaultVal.empty()) return "    " + name + ": " + type;
    return "    " + name + ": " + type + " = " + defaultVal;
}

std::vector<std::string> PythonAdapter::testFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("import unittest");
    inc.push_back("from typing import List");
    inc.push_back("from " + packagePath + " import *");
    inc.push_back("from " + packagePath + "." + glueClass + " import " + glueClass);
    return inc;
}

std::string PythonAdapter::testMethodSignature(const std::string& scenarioName) {
    return "def test_" + scenarioName + "(self):";
}

std::string PythonAdapter::glueInstantiation(const std::string& glueObj) {
    return glueObj + " = " + glueClass + "()";
}

std::string PythonAdapter::equalsMethodSignature(const std::string&) {
    return "    def __eq__(self, other) -> bool:";
}

std::string PythonAdapter::valueToString(const std::string&, const std::string& valueExpr) {
    return "str(" + valueExpr + ")";
}

std::vector<std::string> PythonAdapter::glueFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("from typing import List");
    inc.push_back("from " + packagePath + " import *");
    return inc;
}

std::string PythonAdapter::glueMethodSignature(const std::string& name,
                                                const std::string& paramType,
                                                const std::string& paramName) {
    return "    def " + name + "(self, " + paramName + ": " + paramType + ") -> None:";
}

} // namespace gherkinexecutor
