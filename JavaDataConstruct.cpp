#include "JavaDataConstruct.h"
#include "Configuration.h"
#include "Translate.h"
#include <fstream>
#include <algorithm>

namespace gherkinexecutor {

    bool JavaDataConstruct::isJavaPrimitive(const std::string& type) {
        return type == "int" || type == "long" || type == "double" || type == "float" ||
               type == "boolean" || type == "char" || type == "short" || type == "byte";
    }

    std::string JavaDataConstruct::alterDataType(const std::string& s) {
        return parent->getAdapter()->alterDataType(s);
    }

    void JavaDataConstruct::openJavaFile(const std::string& className) {
        std::string path = Configuration::testSubDirectory +
            parent->featureDirectory + parent->featureName + "/" + className + ".java";
        dataDefinitionFile = std::make_unique<std::ofstream>(path);
    }

    void JavaDataConstruct::javaWriteHeader(const std::string& className) {
        dataPrintLn("package " + parent->packagePath + ";");
        dataPrintLn("import java.util.*;");
        for (const std::string& line : parent->linesToAddForDataAndGlue)
            dataPrintLn(line);
    }

    void JavaDataConstruct::actOnData(const std::vector<std::string>& words) {
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

        javaCreateStringClass(className, internalClassName, variables, doInternal);

        if (doInternal) {
            parent->dataNamesInternal[internalClassName] = "";
            javaCreateInternalClass(internalClassName, className, variables);
        }
    }

    void JavaDataConstruct::javaCreateStringClass(const std::string& className,
                                                    const std::string& internalClassName,
                                                    const std::vector<DataValues>& variables,
                                                    bool doInternal) {
        openJavaFile(className);
        javaWriteHeader(className);
        dataPrintLn("class " + className + "{");

        // String fields with defaults
        for (const DataValues& variable : variables)
            dataPrintLn("    String " + variable.name + " = \"" + variable.defaultVal + "\";");

        javaCreateConstructors(variables, className);
        javaCreateEqualsMethod(variables, className);
        javaCreateBuilderMethod(variables, className);
        javaCreateToStringMethod(variables, className);
        javaCreateToJsonMethod(variables);
        javaCreateFromJsonMethod(variables, className);
        javaCreateListToJsonMethod(className);
        javaCreateListFromJsonMethod(className);
        if (doInternal)
            javaCreateConversionMethod(internalClassName, variables);
        dataPrintLn("    }");

        dataDefinitionFile->close();
    }

    void JavaDataConstruct::javaCreateConstructors(const std::vector<DataValues>& variables,
                                                    const std::string& className) {
        dataPrintLn("    public " + className + "() { }");
        dataPrintLn("    public " + className + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            dataPrintLn("        " + comma + "String " + v.name);
            comma = ",";
        }
        dataPrintLn("        ){");
        for (const DataValues& v : variables)
            dataPrintLn("        this." + v.name + " = " + v.name + ";");
        dataPrintLn("        }");
    }

    void JavaDataConstruct::javaCreateEqualsMethod(const std::vector<DataValues>& variables,
                                                    const std::string& className) {
        std::string dnc = parent->quoteIt(Configuration::doNotCompare);
        dataPrintLn("    @Override");
        dataPrintLn("    public boolean equals (Object o) {");
        dataPrintLn("        if (this == o) return true;");
        dataPrintLn("        if (o == null || getClass() != o.getClass())");
        dataPrintLn("             return false;");
        dataPrintLn("        " + className + " _" + className + " = (" + className + ") o;");
        dataPrintLn("            boolean result = true;");
        for (const DataValues& v : variables) {
            dataPrintLn("         if (");
            dataPrintLn("             !this." + v.name + ".equals(" + dnc + ")");
            dataPrintLn("                && !_" + className + "." + v.name + ".equals(" + dnc + "))");
            dataPrintLn("                if (! _" + className + "." + v.name + ".equals(this." + v.name + ")) result = false;");
        }
        dataPrintLn("             return result;  }");
    }

    void JavaDataConstruct::javaCreateInternalEqualsMethod(const std::vector<DataValues>& variables,
                                                            const std::string& className) {
        dataPrintLn("    @Override");
        dataPrintLn("    public boolean equals (Object o) {");
        dataPrintLn("        if (this == o) return true;");
        dataPrintLn("        if (o == null || getClass() != o.getClass()) return false;");
        dataPrintLn("        " + className + " _" + className + " = (" + className + ") o;");
        std::string result = "         return \n";
        std::string andStr = "";
        for (const DataValues& v : variables) {
            if (isJavaPrimitive(v.dataType))
                result += "                " + andStr + "( _" + className + "." + v.name + " == (this." + v.name + "))\n";
            else
                result += "                " + andStr + "( _" + className + "." + v.name + ".equals(this." + v.name + "))\n";
            andStr = " && ";
        }
        result += "             ;";
        dataPrintLn(result);
        dataPrintLn("    }");
    }

    void JavaDataConstruct::javaCreateBuilderMethod(const std::vector<DataValues>& variables,
                                                     const std::string& className) {
        dataPrintLn("    public static class Builder {");
        for (const DataValues& v : variables)
            dataPrintLn("        private String " + v.name + " = \"" + v.defaultVal + "\";");
        for (const DataValues& v : variables) {
            dataPrintLn("        public Builder " + v.name + "(String " + v.name + ") {");
            dataPrintLn("            this." + v.name + " = " + v.name + ";");
            dataPrintLn("            return this;");
            dataPrintLn("            }");
        }
        dataPrintLn("        public Builder  setCompare() {");
        for (const DataValues& v : variables)
            dataPrintLn("            " + v.name + " = " + parent->quoteIt(Configuration::doNotCompare) + ";");
        dataPrintLn("            return this;");
        dataPrintLn("            }");
        dataPrintLn("        public " + className + " build(){");
        dataPrintLn("             return new " + className + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            dataPrintLn("                 " + comma + v.name);
            comma = ",";
        }
        dataPrintLn("                );   } ");
        dataPrintLn("        } ");
    }

    void JavaDataConstruct::javaCreateToStringMethod(const std::vector<DataValues>& variables,
                                                      const std::string& className) {
        dataPrintLn("    @Override");
        std::string code = "    public String toString() {\n        return \"" + className + " {\"";
        for (const DataValues& v : variables)
            code += "\n        +\"" + v.name + " = \" + " + v.name + " + \" \"";
        code += "\n            + \"} \" + \"\\n\"; }  ";
        dataPrintLn(code);
    }

    void JavaDataConstruct::javaCreateToJsonMethod(const std::vector<DataValues>& variables) {
        std::string code = "    public String toJson() {\n        return \" {\"";
        std::string comma = "";
        for (const DataValues& v : variables) {
            code += "\n        +" + parent->quoteIt(comma + v.name + ":") +
                    " + \"\\\"\" + " + v.name + " + \"\\\"\"";
            comma = ",";
        }
        code += "\n            + \"} \" + \"\\n\"; }  ";
        dataPrintLn(code);
    }

    void JavaDataConstruct::javaCreateFromJsonMethod(const std::vector<DataValues>& variables,
                                                      const std::string& className) {
        dataPrintLn("        public static " + className + " fromJson(String json) {");
        dataPrintLn("              " + className + " instance = new " + className + "();");
        dataPrintLn("");
        dataPrintLn("              \tjson = json.replaceAll(\"\\\\s\", \"\");");
        dataPrintLn("                String[] keyValuePairs = json.replace(\"{\", \"\").replace(\"}\", \"\").split(\",\");");
        dataPrintLn("");
        dataPrintLn("                // Iterate over the key-value pairs");
        dataPrintLn("                for (String pair : keyValuePairs) {");
        dataPrintLn("                    // Split each pair by the colon");
        dataPrintLn("                    String[] entry = pair.split(\":\");");
        dataPrintLn("");
        dataPrintLn("                    // Remove the quotes from the key and value");
        dataPrintLn("                    String key = entry[0].replace(\"\\\"\", \"\").trim();");
        dataPrintLn("                    String value = entry[1].replace(\"\\\"\", \"\").trim();");
        dataPrintLn("");
        dataPrintLn("");
        dataPrintLn("          // Assign the value to the corresponding field");
        dataPrintLn("                    switch (key) {");
        for (const DataValues& v : variables) {
            dataPrintLn("              case \"" + v.name + "\":");
            dataPrintLn("                  instance." + v.name + " = value;");
            dataPrintLn("                  break;");
        }
        dataPrintLn("        \t\t\tdefault:");
        dataPrintLn("        \t\t\t    System.err.println(\"Invalid JSON element \" + key);");
        dataPrintLn("                    }");
        dataPrintLn("                }");
        dataPrintLn("                return instance;");
        dataPrintLn("            }");
        dataPrintLn("");
    }

    void JavaDataConstruct::javaCreateListToJsonMethod(const std::string& className) {
        dataPrintLn("");
        dataPrintLn("             public static String listToJson(List<" + className + "> list) {");
        dataPrintLn("                 StringBuilder jsonBuilder = new StringBuilder();");
        dataPrintLn("                 jsonBuilder.append(\"[\");");
        dataPrintLn("");
        dataPrintLn("                 for (int i = 0; i < list.size(); i++) {");
        dataPrintLn("                     jsonBuilder.append(list.get(i).toJson());");
        dataPrintLn("                     if (i < list.size() - 1) {");
        dataPrintLn("                         jsonBuilder.append(\",\");");
        dataPrintLn("                     }");
        dataPrintLn("                 }");
        dataPrintLn("");
        dataPrintLn("                 jsonBuilder.append(\"]\");");
        dataPrintLn("                 return jsonBuilder.toString();");
        dataPrintLn("             }");
        dataPrintLn("");
    }

    void JavaDataConstruct::javaCreateListFromJsonMethod(const std::string& className) {
        dataPrintLn("             public static List<" + className + "> listFromJson(String json) {");
        dataPrintLn("                    List<" + className + "> list = new ArrayList<>();");
        dataPrintLn("            \t\tjson = json.replace (\"\\\\s\", \"\");");
        dataPrintLn("                    String[] jsonObjects = json.replace(\"[\", \"\").replace(\"]\", \"\").split(\"\\\\},\\\\{\");");
        dataPrintLn("");
        dataPrintLn("                    for (String jsonObject : jsonObjects) {");
        dataPrintLn("                        jsonObject = \"{\" + jsonObject.replace(\"{\", \"\").replace(\"}\", \"\") + \"}\";");
        dataPrintLn("                        list.add(" + className + ".fromJson(jsonObject));");
        dataPrintLn("                    }");
        dataPrintLn("                    return list;");
        dataPrintLn("                }");
        dataPrintLn("");
    }

    void JavaDataConstruct::javaCreateConversionMethod(const std::string& internalClassName,
                                                        const std::vector<DataValues>& variables) {
        dataPrintLn("    " + internalClassName + " to" + internalClassName + "() {");
        dataPrintLn("        return new " + internalClassName + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            std::string expr = parent->getAdapter()->convertFromString(v.dataType, v.name);
            dataPrintLn("         " + comma + expr);
            comma = ",";
        }
        dataPrintLn("        ); }");
    }

    void JavaDataConstruct::javaCreateInternalClass(const std::string& className,
                                                     const std::string& otherClassName,
                                                     const std::vector<DataValues>& variables) {
        if (parent->dataNames.find(className) != parent->dataNames.end()) {
            parent->warning("Internal data name is duplicated: " + className);
        }
        parent->trace("Creating internal class for " + className);
        parent->dataNames[className] = "";

        openJavaFile(className);
        javaWriteHeader(className);
        dataPrintLn("class " + className + "{");

        // Typed fields
        for (const DataValues& v : variables)
            dataPrintLn("     " + v.dataType + " " + v.name + ";");
        dataPrintLn("     ");

        javaCreateToDataTypeString(className, variables);
        javaCreateReverseConversion(className, otherClassName, variables);

        // Constructor
        dataPrintLn("    public " + className + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            dataPrintLn("        " + comma + v.dataType + " " + v.name);
            comma = ",";
        }
        dataPrintLn("        )  {");
        for (const DataValues& v : variables)
            dataPrintLn("        this." + v.name + " = " + v.name + ";");
        dataPrintLn("        }");

        javaCreateInternalEqualsMethod(variables, className);
        javaCreateToStringMethod(variables, className);
        dataPrintLn("    }");

        dataDefinitionFile->close();
    }

    void JavaDataConstruct::javaCreateToDataTypeString(const std::string& className,
                                                        const std::vector<DataValues>& variables) {
        std::string code = "    public static String toDataTypeString() {\n"
                           "        return \"" + className + " {\"";
        for (const DataValues& v : variables)
            code += "\n        +\"" + v.dataType + " \" ";
        code += "\n            + \"} \"; }  ";
        dataPrintLn(code);
    }

    void JavaDataConstruct::javaCreateReverseConversion(const std::string& className,
                                                         const std::string& otherClassName,
                                                         const std::vector<DataValues>& variables) {
        dataPrintLn("    " + otherClassName + " to" + otherClassName + "() {");
        dataPrintLn("        return new " + otherClassName + "(");
        std::string comma = "";
        for (const DataValues& v : variables) {
            std::string expr;
            if (v.dataType == "String") expr = v.name;
            else expr = parent->getAdapter()->valueToString(v.dataType, v.name);
            dataPrintLn("        " + comma + expr);
            comma = ",";
        }
        dataPrintLn("        ); }");
    }

} // namespace gherkinexecutor
