#include "CsharpDataConstruct.h"
#include "Configuration.h"
#include "Translate.h"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace gherkinexecutor {

    bool CsharpDataConstruct::isCsPrimitive(const std::string& type) {
        return type == "int" || type == "long" || type == "double" || type == "float" ||
               type == "bool" || type == "char" || type == "short" || type == "byte" ||
               type == "decimal" || type == "uint" || type == "ulong" || type == "ushort" ||
               type == "sbyte";
    }

    std::string CsharpDataConstruct::csSetMethodName(const std::string& fieldName) {
        if (fieldName.empty()) return "Set";
        std::string result = "Set";
        result += (char)std::toupper((unsigned char)fieldName[0]);
        result += fieldName.substr(1);
        return result;
    }

    std::string CsharpDataConstruct::alterDataType(const std::string& s) {
        return parent->getAdapter()->alterDataType(s);
    }

    void CsharpDataConstruct::openCsFile(const std::string& className) {
        std::string path = Configuration::testSubDirectory +
            parent->featureDirectory + parent->featureName + "/" + className + ".cs";
        dataDefinitionFile = std::make_unique<std::ofstream>(path);
    }

    void CsharpDataConstruct::csWriteHeader(const std::string& className) {
        dataPrintLn("namespace " + parent->packagePath);
        dataPrintLn("{");
        dataPrintLn("    using System;");
        dataPrintLn("    using System.Collections.Generic;");
        dataPrintLn("    using System.Text;");
        for (const std::string& line : parent->linesToAddForDataAndGlue)
            dataPrintLn(line);
    }

    void CsharpDataConstruct::csWriteFooter() {
        dataPrintLn("}");
    }

    void CsharpDataConstruct::actOnData(const std::vector<std::string>& words) {
        if (words.size() < 2) { parent->error("Need to specify data class name"); return; }

        std::string className = words[1];
        bool providedOtherClassName = words.size() > 2;
        std::string internalClassName = providedOtherClassName ? words[2] : className + "Internal";

        auto follow = parent->lookForFollow();
        if (follow.first != "TABLE") {
            parent->error("Error table does not follow data " + words[0] + " " + words[1]);
            return;
        }
        std::vector<std::string> table = follow.second;

        if (parent->dataNames.find(className) != parent->dataNames.end()) {
            className += std::to_string(parent->stepCount);
            parent->warning("Data name is duplicated, has been renamed " + className);
            if (!providedOtherClassName) internalClassName = className + "Internal";
        }

        parent->trace("Creating class for " + className);
        parent->dataNames[className] = "";

        doInternal = false;
        if (!table.empty()) {
            auto headers = parent->parseLine(table[0]);
            doInternal = headers.size() > 2;
        }

        std::vector<DataValues> variables;
        createVariableList(table, variables, doInternal);

        for (const DataValues& variable : variables)
            parent->classDataNames.push_back(className + "#" + variable.name);

        csCreateStringClass(className, internalClassName, variables, doInternal);

        if (doInternal) {
            parent->dataNamesInternal[internalClassName] = "";
            csCreateInternalClass(internalClassName, className, variables);
        }
    }

    void CsharpDataConstruct::csCreateStringClass(const std::string& className,
                                                    const std::string& internalClassName,
                                                    const std::vector<DataValues>& variables,
                                                    bool doInternal) {
        openCsFile(className);
        csWriteHeader(className);
        dataPrintLn("    public class " + className);
        dataPrintLn("    {");

        for (const DataValues& v : variables)
            dataPrintLn("        public string " + v.name + " = \"" + v.defaultVal + "\";");

        csCreateConstructors(variables, className);
        csCreateEqualsMethod(variables, className);
        csCreateGetHashCode(variables);
        csCreateBuilderMethod(variables, className);
        csCreateToStringMethod(variables, className);
        csCreateToJsonMethod(variables, className);
        csCreateFromJsonMethod(variables, className);
        csCreateListToJsonMethod(className);
        csCreateListFromJsonMethod(className);
        csCreateComparerClass(className);
        if (doInternal)
            csCreateConversionMethod(internalClassName, variables);

        dataPrintLn("    }");
        csWriteFooter();
        dataDefinitionFile->close();
    }

    void CsharpDataConstruct::csCreateConstructors(const std::vector<DataValues>& variables,
                                                    const std::string& className) {
        dataPrintLn("        public " + className + "() { }");
        dataPrintLn("        public " + className + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            dataPrintLn("            " + comma + "string " + v.name);
            comma = ", ";
        }
        dataPrintLn("            )");
        dataPrintLn("        {");
        for (const DataValues& v : variables)
            dataPrintLn("            this." + v.name + " = " + v.name + ";");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateEqualsMethod(const std::vector<DataValues>& variables,
                                                    const std::string& className) {
        std::string dnc = parent->quoteIt(Configuration::doNotCompare);
        dataPrintLn("        public override bool Equals(object? o)");
        dataPrintLn("        {");
        dataPrintLn("            if (this == o) return true;");
        dataPrintLn("            if (o == null || GetType() != o.GetType()) return false;");
        dataPrintLn("            " + className + " _" + className + " = (" + className + ")o;");
        dataPrintLn("            bool result = true;");
        for (const DataValues& v : variables) {
            dataPrintLn("            if (");
            dataPrintLn("                !this." + v.name + ".Equals(" + dnc + ")");
            dataPrintLn("                && !_" + className + "." + v.name + ".Equals(" + dnc + "))");
            dataPrintLn("                if (!_" + className + "." + v.name + ".Equals(this." + v.name + ")) result = false;");
        }
        dataPrintLn("            return result;");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateInternalEqualsMethod(const std::vector<DataValues>& variables,
                                                            const std::string& className) {
        dataPrintLn("        public override bool Equals(object? o)");
        dataPrintLn("        {");
        dataPrintLn("            if (this == o) return true;");
        dataPrintLn("            if (o == null || GetType() != o.GetType()) return false;");
        dataPrintLn("            " + className + " _" + className + " = (" + className + ")o;");
        std::string result = "            return\n";
        std::string andStr = "";
        for (const DataValues& v : variables) {
            if (isCsPrimitive(v.dataType))
                result += "                " + andStr + "(_" + className + "." + v.name + " == (this." + v.name + "))\n";
            else
                result += "                " + andStr + "(_" + className + "." + v.name + ".Equals(this." + v.name + "))\n";
            andStr = " && ";
        }
        result += "            ;";
        dataPrintLn(result);
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateGetHashCode(const std::vector<DataValues>& variables) {
        dataPrintLn("        public override int GetHashCode()");
        dataPrintLn("        {");
        dataPrintLn("            int hashCode = 1;");
        for (const DataValues& v : variables) {
            if (isCsPrimitive(v.dataType))
                dataPrintLn("            hashCode ^= " + v.name + ".GetHashCode();");
            else
                dataPrintLn("            hashCode ^= " + v.name + " == null ? 0 : " + v.name + ".GetHashCode();");
        }
        dataPrintLn("            return hashCode;");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateBuilderMethod(const std::vector<DataValues>& variables,
                                                     const std::string& className) {
        dataPrintLn("        public class Builder");
        dataPrintLn("        {");
        for (const DataValues& v : variables)
            dataPrintLn("            private string " + v.name + " = \"" + v.defaultVal + "\";");
        for (const DataValues& v : variables) {
            dataPrintLn("            public Builder " + csSetMethodName(v.name) + "(string " + v.name + ")");
            dataPrintLn("            {");
            dataPrintLn("                this." + v.name + " = " + v.name + ";");
            dataPrintLn("                return this;");
            dataPrintLn("            }");
        }
        dataPrintLn("            public Builder SetCompare()");
        dataPrintLn("            {");
        for (const DataValues& v : variables)
            dataPrintLn("                " + v.name + " = " + parent->quoteIt(Configuration::doNotCompare) + ";");
        dataPrintLn("                return this;");
        dataPrintLn("            }");
        dataPrintLn("            public " + className + " Build()");
        dataPrintLn("            {");
        dataPrintLn("                return new " + className + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            dataPrintLn("                    " + comma + v.name);
            comma = ", ";
        }
        dataPrintLn("                   );");
        dataPrintLn("            }");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateToStringMethod(const std::vector<DataValues>& variables,
                                                      const std::string& className) {
        std::string code = "        public override string ToString()\n        {\n"
                           "            return \"" + className + " {\"";
        for (const DataValues& v : variables)
            code += " + \" " + v.name + " = \" + " + v.name + " + \" \"";
        code += " + \"} \" + \"\\\\n\";\n        }";
        dataPrintLn(code);
    }

    void CsharpDataConstruct::csCreateToJsonMethod(const std::vector<DataValues>& variables,
                                                    const std::string& className) {
        std::string code = "        public string ToJson()\n        {\n            return \" {\"";
        std::string comma = "";
        for (const DataValues& v : variables) {
            code += "\n            + \"" + comma + v.name + ": \" + \"\\\"\" + " + v.name + " + \"\\\"\"";
            comma = ",";
        }
        code += "\n            + \"} \";\n        }";
        dataPrintLn(code);
    }

    void CsharpDataConstruct::csCreateFromJsonMethod(const std::vector<DataValues>& variables,
                                                      const std::string& className) {
        dataPrintLn("        public static " + className + " FromJson(string json)");
        dataPrintLn("        {");
        dataPrintLn("            " + className + " instance = new " + className + "();");
        dataPrintLn("");
        dataPrintLn("            json = json.Replace(\"\\\\s\", \"\");");
        dataPrintLn("            string[] keyValuePairs = json.Replace(\"{\", \"\").Replace(\"}\", \"\").Split(',');");
        dataPrintLn("");
        dataPrintLn("            foreach (string pair in keyValuePairs)");
        dataPrintLn("            {");
        dataPrintLn("                string[] entry = pair.Split(':');");
        dataPrintLn("                string key = entry[0].Replace(\"\\\"\", \"\").Trim();");
        dataPrintLn("                string value = entry[1].Replace(\"\\\"\", \"\").Trim();");
        dataPrintLn("");
        dataPrintLn("                switch (key)");
        dataPrintLn("                {");
        for (const DataValues& v : variables) {
            dataPrintLn("                    case \"" + v.name + "\":");
            dataPrintLn("                        instance." + v.name + " = value;");
            dataPrintLn("                        break;");
        }
        dataPrintLn("                    default:");
        dataPrintLn("                        Console.Error.WriteLine(\"Invalid JSON element \" + key);");
        dataPrintLn("                        break;");
        dataPrintLn("                }");
        dataPrintLn("            }");
        dataPrintLn("            return instance;");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateListToJsonMethod(const std::string& className) {
        dataPrintLn("        public static string ListToJson(List<" + className + "> list)");
        dataPrintLn("        {");
        dataPrintLn("            StringBuilder jsonBuilder = new StringBuilder();");
        dataPrintLn("            jsonBuilder.Append(\"[\");");
        dataPrintLn("            for (int i = 0; i < list.Count; i++)");
        dataPrintLn("            {");
        dataPrintLn("                jsonBuilder.Append(list[i].ToJson());");
        dataPrintLn("                if (i < list.Count - 1)");
        dataPrintLn("                {");
        dataPrintLn("                    jsonBuilder.Append(\",\");");
        dataPrintLn("                }");
        dataPrintLn("            }");
        dataPrintLn("            jsonBuilder.Append(\"]\");");
        dataPrintLn("            return jsonBuilder.ToString();");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateListFromJsonMethod(const std::string& className) {
        dataPrintLn("        public static List<" + className + "> ListFromJson(string json)");
        dataPrintLn("        {");
        dataPrintLn("            List<" + className + "> list = new List<" + className + ">();");
        dataPrintLn("            json = json.Replace(\"\\\\s\", \"\");");
        dataPrintLn("            json = json.Replace(\"[\", \"\").Replace(\"]\", \"\");");
        dataPrintLn("            string[] jsonObjects = json.Split(new[] { \"},\\\\s*{\" }, StringSplitOptions.None);");
        dataPrintLn("            foreach (string jsonObject in jsonObjects)");
        dataPrintLn("            {");
        dataPrintLn("                list.Add(" + className + ".FromJson(jsonObject));");
        dataPrintLn("            }");
        dataPrintLn("            return list;");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateComparerClass(const std::string& className) {
        dataPrintLn("        public class " + className + "Comparer : IEqualityComparer<" + className + ">");
        dataPrintLn("        {");
        dataPrintLn("            public bool Equals(" + className + "? x, " + className + "? y)");
        dataPrintLn("            {");
        dataPrintLn("                if (x == null) return false;");
        dataPrintLn("                return x.Equals(y);");
        dataPrintLn("            }");
        dataPrintLn("            public int GetHashCode(" + className + " obj)");
        dataPrintLn("            {");
        dataPrintLn("                return obj.GetHashCode();");
        dataPrintLn("            }");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateConversionMethod(const std::string& internalClassName,
                                                        const std::vector<DataValues>& variables) {
        dataPrintLn("        public " + internalClassName + " To" + internalClassName + "()");
        dataPrintLn("        {");
        dataPrintLn("            return new " + internalClassName + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            std::string expr = parent->getAdapter()->convertFromString(v.dataType, v.name);
            dataPrintLn("             " + comma + expr);
            comma = ", ";
        }
        dataPrintLn("            );");
        dataPrintLn("        }");
    }

    void CsharpDataConstruct::csCreateInternalClass(const std::string& className,
                                                     const std::string& otherClassName,
                                                     const std::vector<DataValues>& variables) {
        if (parent->dataNames.find(className) != parent->dataNames.end()) {
            parent->warning("Internal data name is duplicated: " + className);
        }
        parent->trace("Creating internal class for " + className);
        parent->dataNames[className] = "";

        openCsFile(className);
        csWriteHeader(className);
        dataPrintLn("    public class " + className);
        dataPrintLn("    {");

        for (const DataValues& v : variables)
            dataPrintLn("        public " + v.dataType + " " + v.name + ";");

        csCreateToDataTypeString(className, variables);
        csCreateReverseConversion(className, otherClassName, variables);

        // Constructor
        dataPrintLn("        public " + className + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            dataPrintLn("            " + comma + v.dataType + " " + v.name);
            comma = ", ";
        }
        dataPrintLn("            )");
        dataPrintLn("        {");
        for (const DataValues& v : variables)
            dataPrintLn("            this." + v.name + " = " + v.name + ";");
        dataPrintLn("        }");

        csCreateInternalEqualsMethod(variables, className);
        csCreateGetHashCode(variables);
        dataPrintLn("        public class " + className + "Comparer : IEqualityComparer<" + className + ">");
        dataPrintLn("        {");
        dataPrintLn("            public bool Equals(" + className + "? x, " + className + "? y)");
        dataPrintLn("            {");
        dataPrintLn("                if (x == null) return false;");
        dataPrintLn("                return x.Equals(y);");
        dataPrintLn("            }");
        dataPrintLn("            public int GetHashCode(" + className + " obj)");
        dataPrintLn("            {");
        dataPrintLn("                return obj.GetHashCode();");
        dataPrintLn("            }");
        dataPrintLn("        }");
        csCreateToStringMethod(variables, className);

        dataPrintLn("    }");
        csWriteFooter();
        dataDefinitionFile->close();
    }

    void CsharpDataConstruct::csCreateToDataTypeString(const std::string& className,
                                                        const std::vector<DataValues>& variables) {
        std::string code = "        public static string ToDataTypeString()\n        {\n"
                           "            return \"" + className + " {\"";
        for (const DataValues& v : variables)
            code += "\n            + \"" + v.dataType + " \"";
        code += "\n            + \"} \";\n        }";
        dataPrintLn(code);
    }

    void CsharpDataConstruct::csCreateReverseConversion(const std::string& className,
                                                         const std::string& otherClassName,
                                                         const std::vector<DataValues>& variables) {
        dataPrintLn("        public " + otherClassName + " To" + otherClassName + "()");
        dataPrintLn("        {");
        dataPrintLn("            return new " + otherClassName + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            std::string expr;
            if (v.dataType == "string") expr = v.name;
            else expr = parent->getAdapter()->valueToString(v.dataType, v.name);
            dataPrintLn("            " + comma + expr);
            comma = ", ";
        }
        dataPrintLn("            );");
        dataPrintLn("        }");
    }

} // namespace gherkinexecutor
