#include "CppAdapter.h"
#include "Configuration.h"

namespace gherkinexecutor {

std::string CppAdapter::alterDataType(const std::string& dslType) {
    if (dslType == "Int")      return "int";
    if (dslType == "Integer")  return "int";
    if (dslType == "Char")     return "char";
    if (dslType == "Decimal")  return "double";
    if (dslType == "Double")   return "double";
    if (dslType == "Float")    return "float";
    if (dslType == "Long")     return "long";
    if (dslType == "Short")    return "short";
    if (dslType == "Byte")     return "unsigned char";
    if (dslType == "byte")     return "unsigned char";
    if (dslType == "Boolean")  return "bool";
    if (dslType == "boolean")  return "bool";
    if (dslType == "Text")     return "std::string";
    if (dslType == "String")   return "std::string";
    if (dslType == "string")   return "std::string";
    return dslType;
}

std::string CppAdapter::convertFromString(const std::string& nativeType,
                                           const std::string& valueExpr) {
    if (nativeType == "int")                return "std::stoi(" + valueExpr + ")";
    if (nativeType == "double")             return "std::stod(" + valueExpr + ")";
    if (nativeType == "float")              return "std::stof(" + valueExpr + ")";
    if (nativeType == "long")               return "std::stol(" + valueExpr + ")";
    if (nativeType == "unsigned long")      return "std::stoul(" + valueExpr + ")";
    if (nativeType == "long long")          return "std::stoll(" + valueExpr + ")";
    if (nativeType == "unsigned long long") return "std::stoull(" + valueExpr + ")";
    if (nativeType == "long double")        return "static_cast<long double>(std::stold(" + valueExpr + "))";
    if (nativeType == "short")              return "static_cast<short>(std::stoi(" + valueExpr + "))";
    if (nativeType == "unsigned short")     return "static_cast<unsigned short>(std::stoul(" + valueExpr + "))";
    if (nativeType == "unsigned int")       return "static_cast<unsigned int>(std::stoul(" + valueExpr + "))";
    if (nativeType == "bool")               return "(" + valueExpr + " == \"true\" || " + valueExpr + " == \"1\")";
    if (nativeType == "char")               return "(" + valueExpr + ".length() > 0 ? " + valueExpr + "[0] : ' ')";
    if (nativeType == "signed char")        return "static_cast<signed char>((" + valueExpr + ".length() > 0 ? " + valueExpr + "[0] : ' '))";
    if (nativeType == "unsigned char")      return "static_cast<unsigned char>((" + valueExpr + ".length() > 0 ? " + valueExpr + "[0] : ' '))";
    if (nativeType == "std::string")        return valueExpr;
    return nativeType + "(" + valueExpr + ")";
}

std::string CppAdapter::classOpen(const std::string& name, const std::string& base) {
    if (base.empty()) return "class " + name + " {";
    return "class " + name + " : public " + base + "{";
}

std::string CppAdapter::fieldDeclaration(const std::string& type, const std::string& name,
                                          const std::string& defaultVal) {
    if (defaultVal.empty()) return "    " + type + " " + name + ";";
    return "    " + type + " " + name + " = " + defaultVal + ";";
}

std::vector<std::string> CppAdapter::testFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("#include <gtest/gtest.h>");
    inc.push_back("#include <vector>");
    inc.push_back("#include <string>");
    if (Configuration::logIt) {
        inc.push_back("#include <fstream>");
        inc.push_back("#include <iostream>");
        inc.push_back("#include <filesystem>");
    }
    return inc;
}

std::string CppAdapter::testMethodSignature(const std::string& scenarioName) {
    return "TEST_F(" + featureName + "," + scenarioName + ") {";
}

std::string CppAdapter::glueInstantiation(const std::string& glueObj) {
    return glueClass + " " + glueObj + ";";
}

std::string CppAdapter::listVarDecl(const std::string& elemType, const std::string& varName) {
    return listType(elemType) + " " + varName;
}

std::string CppAdapter::builderSet(const std::string& field, const std::string& value) {
    return "." + field + "(" + value + ")";
}

std::string CppAdapter::multilineStringLiteral(const std::string& content) {
    return "R\"(\n" + content + "\n)\"";
}

std::string CppAdapter::equalsMethodSignature(const std::string& cls) {
    return "    bool " + cls + "::operator==(const " + cls + " & other) const { ";
}

std::string CppAdapter::valueToString(const std::string& nativeType, const std::string& valueExpr) {
    if (nativeType == "std::string") return valueExpr;
    if (nativeType == "bool")
        return "std::string(" + valueExpr + " ? \"true\" : \"false\")";
    if (nativeType == "char" || nativeType == "signed char" || nativeType == "unsigned char")
        return "std::string(1, " + valueExpr + ")";
    if (nativeType == "short" || nativeType == "unsigned short")
        return "std::to_string(static_cast<int>(" + valueExpr + "))";
    if (nativeType == "int" || nativeType == "unsigned int" ||
        nativeType == "long" || nativeType == "unsigned long" ||
        nativeType == "long long" || nativeType == "unsigned long long" ||
        nativeType == "float" || nativeType == "double" || nativeType == "long double")
        return "std::to_string(" + valueExpr + ")";
    return valueExpr + ".to_string()";
}

std::vector<std::string> CppAdapter::glueFileIncludes() {
    std::vector<std::string> inc;
    if (Configuration::testFramework == "gtest")
        inc.push_back("#include <gtest/gtest.h>");
    else
        inc.push_back("#include <gtest/gtest.h>");
    if (Configuration::logIt) {
        inc.push_back("#include <fstream>");
        inc.push_back("#include <iostream>");
        inc.push_back("#include <filesystem>");
    }
    return inc;
}

std::string CppAdapter::glueMethodSignature(const std::string& name,
                                             const std::string& paramType,
                                             const std::string& paramName) {
    return "    void " + glueClass + "::" + name + "(const " + paramType + "& " + paramName + " ) {";
}

std::string CppAdapter::consoleOutput(const std::string& expr) {
    return "        std::cout << " + expr + " << std::endl;";
}

std::string CppAdapter::logCall(const std::string& value) {
    return "        log(" + value + ");";
}

std::string CppAdapter::forEachBegin(const std::string& elemType, const std::string& listName) {
    return "        for (const " + elemType + "& value : " + listName + "){";
}

std::string CppAdapter::logFunction(const std::string& dirPath) {
    if (!Configuration::logIt) return "";
    std::string logFile = dirPath + "/log.txt";
    return "public:  static void log(const std::string& value) {\n" +
        std::string(
            "try {\n"
            "  std::ofstream logFile(\"" + logFile + "\", std::ios::app); // append mode\n"
            "  if (logFile.is_open()) {\n"
            "    logFile << value << \"\\n\";\n"
            "    logFile.close();\n"
            "  }\n"
            "  else {\n"
            "    std::cerr << \"*** Cannot write to log\" << std::endl;\n"
            "  }\n"
            "}\n"
            "catch (const std::exception& e) {\n"
            "std::cerr << \" * **Cannot write to log : \" << e.what() << std::endl;\n"
            "}\n"
            " }\n");
}

} // namespace gherkinexecutor
