#include "JavaAdapter.h"
#include "Configuration.h"

namespace gherkinexecutor {

std::string JavaAdapter::alterDataType(const std::string& dslType) {
    if (dslType == "Int" || dslType == "Integer") return "int";
    if (dslType == "Long")    return "long";
    if (dslType == "Double" || dslType == "Decimal") return "double";
    if (dslType == "Float")   return "float";
    if (dslType == "Boolean" || dslType == "boolean") return "boolean";
    if (dslType == "Char")    return "char";
    if (dslType == "Short")   return "short";
    if (dslType == "Byte" || dslType == "byte") return "byte";
    if (dslType == "Text" || dslType == "String" || dslType == "string") return "String";
    return dslType;
}

std::string JavaAdapter::convertFromString(const std::string& nativeType,
                                            const std::string& valueExpr) {
    if (nativeType == "int")     return "Integer.parseInt(" + valueExpr + ")";
    if (nativeType == "long")    return "Long.parseLong(" + valueExpr + ")";
    if (nativeType == "double")  return "Double.parseDouble(" + valueExpr + ")";
    if (nativeType == "float")   return "Float.parseFloat(" + valueExpr + ")";
    if (nativeType == "boolean") return "Boolean.parseBoolean(" + valueExpr + ")";
    if (nativeType == "char")    return valueExpr + ".charAt(0)";
    if (nativeType == "short")   return "(short) Integer.parseInt(" + valueExpr + ")";
    if (nativeType == "byte")    return "(byte) Integer.parseInt(" + valueExpr + ")";
    if (nativeType == "String")  return valueExpr;
    return valueExpr;
}

std::string JavaAdapter::fieldDeclaration(const std::string& type, const std::string& name,
                                           const std::string& defaultVal) {
    if (defaultVal.empty()) return "    " + type + " " + name + ";";
    return "    " + type + " " + name + " = " + defaultVal + ";";
}

std::vector<std::string> JavaAdapter::testFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("package " + packagePath + ";");
    inc.push_back("import org.junit.jupiter.api.Test;");
    inc.push_back("import java.util.List;");
    return inc;
}

std::string JavaAdapter::testMethodSignature(const std::string& scenarioName) {
    return "@Test\n    void test_" + scenarioName + "(){";
}

std::string JavaAdapter::glueInstantiation(const std::string& glueObj) {
    return glueClass + " " + glueObj + " = new " + glueClass + "();";
}

std::string JavaAdapter::multilineStringLiteral(const std::string& content) {
    return "\"\"\"\n" + content + "\"\"\"";
}

std::string JavaAdapter::equalsMethodSignature(const std::string& cls) {
    return "    @Override\n    public boolean equals (Object o) {";
}

std::string JavaAdapter::valueToString(const std::string& nativeType,
                                        const std::string& valueExpr) {
    if (nativeType == "String") return valueExpr;
    return "String.valueOf(" + valueExpr + ")";
}

std::vector<std::string> JavaAdapter::glueFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("package " + packagePath + ";");
    inc.push_back("import java.util.*;");
    inc.push_back("import static org.junit.jupiter.api.Assertions.*;");
    inc.push_back("import java.util.List;");
    return inc;
}

std::string JavaAdapter::glueMethodSignature(const std::string& name,
                                              const std::string& paramType,
                                              const std::string& paramName) {
    return "    void " + name + "(" + paramType + " " + paramName + " ) {";
}

} // namespace gherkinexecutor
