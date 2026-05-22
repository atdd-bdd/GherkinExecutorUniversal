#include "CsharpAdapter.h"
#include "Configuration.h"

namespace gherkinexecutor {

std::string CsharpAdapter::alterDataType(const std::string& dslType) {
    if (dslType == "Int" || dslType == "Integer") return "int";
    if (dslType == "Long")    return "long";
    if (dslType == "Double" || dslType == "Decimal") return "double";
    if (dslType == "Float")   return "float";
    if (dslType == "Boolean" || dslType == "boolean") return "bool";
    if (dslType == "Char")    return "char";
    if (dslType == "Short")   return "short";
    if (dslType == "Byte" || dslType == "byte") return "byte";
    if (dslType == "Text" || dslType == "String" || dslType == "string") return "string";
    return dslType;
}

std::string CsharpAdapter::convertFromString(const std::string& nativeType,
                                              const std::string& valueExpr) {
    if (nativeType == "int")    return "Int32.Parse(" + valueExpr + ")";
    if (nativeType == "long")   return "Int64.Parse(" + valueExpr + ")";
    if (nativeType == "double") return "Double.Parse(" + valueExpr + ")";
    if (nativeType == "float")  return "Single.Parse(" + valueExpr + ")";
    if (nativeType == "bool")   return "Boolean.Parse(" + valueExpr + ")";
    if (nativeType == "char")   return valueExpr + "[0]";
    if (nativeType == "short")  return "(short)Int32.Parse(" + valueExpr + ")";
    if (nativeType == "byte")   return "(byte)Int32.Parse(" + valueExpr + ")";
    if (nativeType == "string") return valueExpr;
    return valueExpr;
}

std::string CsharpAdapter::classOpen(const std::string& name, const std::string& base) {
    if (base.empty()) return "public class " + name + " {";
    return "public class " + name + " : " + base + " {";
}

std::string CsharpAdapter::fieldDeclaration(const std::string& type, const std::string& name,
                                             const std::string& defaultVal) {
    if (defaultVal.empty()) return "    public " + type + " " + name + ";";
    return "    public " + type + " " + name + " = " + defaultVal + ";";
}

std::vector<std::string> CsharpAdapter::testFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("namespace " + packagePath + "{");
    inc.push_back("using System.IO;");
    inc.push_back("[TestClass]");
    return inc;
}

std::string CsharpAdapter::testMethodSignature(const std::string& scenarioName) {
    return "[TestMethod]\n    public void Test_" + scenarioName + "(){";
}

std::string CsharpAdapter::glueInstantiation(const std::string& glueObj) {
    return glueClass + " " + glueObj + " = new " + glueClass + "();";
}

std::string CsharpAdapter::equalsMethodSignature(const std::string& cls) {
    return "    public override bool Equals(object? o) {";
}

std::string CsharpAdapter::valueToString(const std::string& nativeType,
                                          const std::string& valueExpr) {
    if (nativeType == "string") return valueExpr;
    if (nativeType == "bool")
        return "(" + valueExpr + " ? \"true\" : \"false\")";
    return valueExpr + ".ToString()";
}

std::vector<std::string> CsharpAdapter::glueFileIncludes() {
    std::vector<std::string> inc;
    inc.push_back("namespace " + packagePath + " {");
    inc.push_back("using System;");
    inc.push_back("using System.Collections.Generic;");
    inc.push_back("using System.Text;");
    inc.push_back("using System.Text.RegularExpressions;");
    inc.push_back("using static Microsoft.VisualStudio.TestTools.UnitTesting.Assert;");
    inc.push_back("using System.IO;");
    return inc;
}

std::string CsharpAdapter::glueMethodSignature(const std::string& name,
                                                const std::string& paramType,
                                                const std::string& paramName) {
    return "    public void " + name + "(" + paramType + " " + paramName + " ) {";
}

std::string CsharpAdapter::logFunction(const std::string& dirPath) {
    if (!Configuration::logIt) return "";
    std::string logFile = dirPath + "/log.txt";
    return
        "    private static void Log(string value) {\n"
        "        try {\n"
        "            using (var sw = new System.IO.StreamWriter(\"" + logFile + "\", true)) {\n"
        "                sw.WriteLine(value);\n"
        "            }\n"
        "        } catch (Exception e) {\n"
        "            Console.Error.WriteLine(\"*** Cannot write to log: \" + e.Message);\n"
        "        }\n"
        "    }";
}

} // namespace gherkinexecutor
