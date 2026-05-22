#include "PythonDataConstruct.h"
#include "Configuration.h"
#include "Translate.h"
#include <algorithm>
#include <fstream>

namespace gherkinexecutor {

    std::string PythonDataConstruct::makeImportForDataDirectory() {
        return "from " + parent->packagePath + " import *";
    }

    std::string PythonDataConstruct::makeImportForDataClass(const std::string& name) {
        return "from " + parent->packagePath + "." + name + " import " + name;
    }

    std::string PythonDataConstruct::alterDataType(const std::string& s) {
        if (s == "Integer" || s == "Int") return "int";
        if (s == "Character" || s == "Char" || s == "char" || s == "String" || s == "string" || s == "Text") return "str";
        if (s == "Decimal" || s == "Double" || s == "double" || s == "Float" || s == "float") return "float";
        if (s == "Boolean") return "bool";
        return s;
    }

    std::string PythonDataConstruct::makePyValueFromString(const DataValues& variable, bool addSelf) {
        std::string value = parent->makeName(variable.name);
        if (addSelf) value = "self." + value;
        const std::string& dt = variable.dataType;
        if (dt == "str") return value;
        if (dt == "int") return "int(" + value + ")";
        if (dt == "float") return "float(" + value + ")";
        if (dt == "bool") return "bool(" + value + ")";
        return dt + "(" + value + ")";
    }

    void PythonDataConstruct::actOnData(const std::vector<std::string>& words) {
        if (words.size() < 2) {
            parent->error("Need to specify data class name");
            return;
        }

        std::string className = words[1];
        bool providedOtherClassName = words.size() > 2;
        std::string internalClassName = providedOtherClassName ? words[2] : className + "Internal";

        auto follow = parent->lookForFollow();
        std::string followType = follow.first;
        std::vector<std::string> table = follow.second;

        if (followType != "TABLE") {
            parent->error("Error table does not follow data " + words[0] + " " + words[1]);
            return;
        }

        if (parent->dataNames.find(className) != parent->dataNames.end()) {
            className += std::to_string(parent->stepCount);
            parent->warning("Data name is duplicated, has been renamed " + className);
            if (!providedOtherClassName) internalClassName = className + "Internal";
        }

        parent->trace("Creating class for " + className);
        parent->dataNames[className] = "";

        // Python: doInternal is determined by header column count (>2 means typed fields exist)
        doInternal = false;
        if (!table.empty()) {
            auto headers = parent->parseLine(table[0]);
            doInternal = headers.size() > 2;
        }

        std::vector<DataValues> variables;
        createVariableList(table, variables, doInternal);

        for (const DataValues& variable : variables)
            parent->classDataNames.push_back(className + "#" + variable.name);

        startDataFile(className, false);

        dataPrintLn("import sys");
        dataPrintLn("import re");
        dataPrintLn("from typing import List");
        for (const std::string& line : parent->linesToAddForDataAndGlue)
            dataPrintLn(line);
        dataPrintLn("");
        dataPrintLn("");
        dataPrintLn("class " + className + ":");

        pyCreateConstructor(variables, className);
        pyCreateEqualsMethod(variables, className);
        dataPrintLn("");
        pyCreateToStringMethod(variables, className);
        dataPrintLn("");
        pyCreateToJsonMethod(variables);
        pyCreateFromJsonMethod(variables, className);
        pyCreateTableToJsonMethod(className);
        pyCreateJsonToTableMethod(className);

        if (doInternal) {
            parent->dataNamesInternal[internalClassName] = "";
            dataPrintLn("");
            pyCreateConversionMethod(internalClassName, variables);
        }
        pyCreateBuilderMethod(variables, className);
        endDataFile();

        if (doInternal)
            pyCreateInternalClass(internalClassName, className, variables, providedOtherClassName);
    }

    void PythonDataConstruct::endOneDataFile() {
        if (!Configuration::oneDataFile || !oneDataFileStarted) return;
        try {
            if (dataDefinitionFile) {
                dataDefinitionFile->close();
                oneDataFileStarted = false;
            }
        }
        catch (const std::exception&) {}

        std::string initPath = Configuration::testSubDirectory +
            parent->featureDirectory + parent->featureName + "/__init__.py";
        try {
            std::ofstream initFile(initPath);
            std::string dataModule = parent->featureName + "_data";
            initFile << "from " << parent->packagePath << "." << dataModule << " import *\n";
        }
        catch (const std::exception&) { parent->error("Cannot write __init__.py"); }
    }

    void PythonDataConstruct::pyCreateConstructor(const std::vector<DataValues>& variables,
                                                   const std::string& className) {
        dataPrintLn("    def __init__(self,");
        std::string comma = " ";
        for (const DataValues& variable : variables) {
            dataPrintLn("                " + comma + " " + parent->makeName(variable.name) +
                        ": str = \"" + variable.defaultVal + "\"");
            comma = ",";
        }
        dataPrintLn("                ) -> None:");
        for (const DataValues& variable : variables)
            dataPrintLn("        self." + parent->makeName(variable.name) + " = " +
                        parent->makeName(variable.name));
    }

    void PythonDataConstruct::pyCreateInternalConstructor(const std::vector<DataValues>& variables,
                                                           const std::string& className) {
        dataPrintLn("    def __init__(self,");
        std::string comma = " ";
        for (const DataValues& variable : variables) {
            dataPrintLn("               " + comma + " " + parent->makeName(variable.name) +
                        ": " + variable.dataType);
            comma = ",";
        }
        dataPrintLn("                ) -> None:");
        for (const DataValues& variable : variables)
            dataPrintLn("        self." + parent->makeName(variable.name) + " = " +
                        parent->makeName(variable.name));
    }

    void PythonDataConstruct::pyCreateEqualsMethod(const std::vector<DataValues>& variables,
                                                    const std::string& className) {
        dataPrintLn("");
        dataPrintLn("    def __eq__(self, other) -> bool:");
        dataPrintLn("        if self is other:");
        dataPrintLn("            return True");
        dataPrintLn("        if other is None or not isinstance(other, self.__class__):");
        dataPrintLn("            return False");
        std::string varName = "_" + className;
        dataPrintLn("        " + varName + " = other");
        dataPrintLn("        result1 = True");
        for (const DataValues& variable : variables) {
            std::string dnc = parent->quoteIt(Configuration::doNotCompare);
            dataPrintLn("        if not (self." + variable.name + " == " + dnc +
                        " or " + varName + "." + variable.name + " == " + dnc + "):");
            dataPrintLn("            if not " + varName + "." + variable.name +
                        " == self." + variable.name + ":");
            dataPrintLn("                result1 = False");
        }
        dataPrintLn("        return result1");
    }

    void PythonDataConstruct::pyCreateInternalEqualsMethod(const std::vector<DataValues>& variables,
                                                            const std::string& className) {
        dataPrintLn("    def __eq__(self, other) -> bool:");
        dataPrintLn("        if self is other:");
        dataPrintLn("            return True");
        dataPrintLn("        if other is None or not isinstance(other, self.__class__):");
        dataPrintLn("            return False");
        std::string varName = "_" + className;
        dataPrintLn("        " + varName + " = other");
        std::string result = "        return ";
        std::string andStr = "";
        for (const DataValues& variable : variables) {
            result += andStr + "( " + varName + "." + variable.name + " == self." + variable.name + ")";
            andStr = " and ";
        }
        dataPrintLn(result);
    }

    void PythonDataConstruct::pyCreateToStringMethod(const std::vector<DataValues>& variables,
                                                      const std::string& className) {
        std::string code = "\n    def __str__(self) -> str:\n        return \"" + className + " {\" + \\\n        ";
        for (const DataValues& variable : variables)
            code += " \" " + variable.name + " = \" + str(self." + variable.name + ") + \" \" ";
        code += " \"} \"";
        if (Configuration::addLineToString)
            code += " + \"\\n\"";
        dataPrintLn(code);
    }

    void PythonDataConstruct::pyCreateToJsonMethod(const std::vector<DataValues>& variables) {
        std::string code = "    def to_json(self) -> str:\n";
        code += "        return \"{\" + \\\n";
        std::string comma = "";
        for (const DataValues& variable : variables) {
            code += "            " + comma + "\"\\\"" + variable.name + "\\\": \\\"\" + str(self." +
                    variable.name + ") + \"\\\"\" +  \\\n";
            comma = "\",\" + ";
        }
        code += "            \"}\"";
        dataPrintLn(code);
    }

    void PythonDataConstruct::pyCreateFromJsonMethod(const std::vector<DataValues>& variables,
                                                      const std::string& className) {
        std::string code =
            "\n    @staticmethod\n"
            "    def from_json(json: str):\n"
            "        instance = " + className + "()\n"
            "        json = re.sub(r\"\\\\s+\", \"\", json)\n"
            "        json = json.replace(\"{\",\"\").replace(\"}\",\"\")\n"
            "        key_value_pairs = json.split(\",\")\n"
            "        for pair in key_value_pairs:\n"
            "            entry = pair.split(\":\")\n"
            "            key = entry[0].replace('\"', \"\").strip()\n"
            "            value = entry[1].replace('\"', \"\").strip()\n"
            "            switch = {\n";
        for (const DataValues& variable : variables)
            code += "                \"" + variable.name + "\": lambda: setattr(instance, \"" +
                    variable.name + "\", value),\n";
        code +=
            "                }\n"
            "            switch.get(key, lambda: print(f\"Invalid JSON element {key}\", file=sys.stderr))()\n"
            "        return instance\n";
        dataPrintLn(code);
    }

    void PythonDataConstruct::pyCreateTableToJsonMethod(const std::string& className) {
        std::string code =
            "\n    @staticmethod\n"
            "    def list_to_json(lst: list) -> str:\n"
            "        json_builder = []\n"
            "        json_builder.append(\"[\")\n"
            "        for i in range(len(lst)):\n"
            "            json_builder.append(lst[i].to_json())\n"
            "            if i < len(lst) - 1:\n"
            "                json_builder.append(\",\")\n"
            "        json_builder.append(\"]\")\n"
            "        return \"\".join(json_builder)\n";
        dataPrintLn(code);
    }

    void PythonDataConstruct::pyCreateJsonToTableMethod(const std::string& className) {
        std::string code =
            "\n    @staticmethod\n"
            "    def list_from_json(json: str) -> list:\n"
            "        result_list = []\n"
            "        json = re.sub(r\"\\\\s+\", \"\", json)\n"
            "        json = json.replace(\"[\", \"\").replace(\"]\", \"\")\n"
            "        json_objects = re.split(r\"(?<=}),\\\\s*(?={)\", json)\n"
            "        for json_object in json_objects:\n"
            "            result_list.append(" + className + ".from_json(json_object))\n"
            "        return result_list\n";
        dataPrintLn(code);
    }

    void PythonDataConstruct::pyCreateBuilderMethod(const std::vector<DataValues>& variables,
                                                     const std::string& className) {
        dataPrintLn("");
        dataPrintLn("    class Builder:");
        dataPrintLn("        def __init__(self) -> None:");
        for (const DataValues& variable : variables)
            dataPrintLn("            self." + variable.name + " = " + parent->quoteIt(variable.defaultVal));
        for (const DataValues& variable : variables) {
            dataPrintLn("");
            dataPrintLn("        def " + parent->makeBuildName(variable.name) +
                        "(self, " + variable.name + ": str) -> 'Builder':");
            dataPrintLn("            self." + variable.name + " = " + variable.name);
            dataPrintLn("            return self");
        }
        dataPrintLn("");
        dataPrintLn("        def set_compare(self) -> 'Builder':");
        for (const DataValues& variable : variables)
            dataPrintLn("            self." + variable.name + " = " +
                        parent->quoteIt(Configuration::doNotCompare));
        dataPrintLn("            return self");
        dataPrintLn("");
        dataPrintLn("        def build(self):");
        dataPrintLn("            return " + className + "(");
        std::string comma = "";
        for (const DataValues& variable : variables) {
            dataPrintLn("                " + comma + "self." + variable.name);
            comma = ",";
        }
        dataPrintLn("            )");
    }

    void PythonDataConstruct::pyCreateConversionMethod(const std::string& internalClassName,
                                                        const std::vector<DataValues>& variables) {
        dataPrintLn("    def to_" + internalClassName + "(self):");
        if (!Configuration::oneDataFile)
            dataPrintLn("        " + makeImportForDataClass(internalClassName));
        dataPrintLn("        return " + internalClassName + "(");
        std::string comma = "";
        for (const DataValues& variable : variables) {
            dataPrintLn("            " + comma + makePyValueFromString(variable, true));
            comma = ",";
        }
        dataPrintLn("        )");
    }

    void PythonDataConstruct::pyCreateInternalClass(const std::string& className,
                                                     const std::string& otherClassName,
                                                     const std::vector<DataValues>& variables,
                                                     bool providedClassName) {
        std::string classNameInternal = className;
        if (parent->dataNames.find(classNameInternal) != parent->dataNames.end()) {
            classNameInternal += std::to_string(parent->stepCount);
            parent->warning("Internal data name is duplicated, has been renamed " + classNameInternal);
        }

        parent->trace("Creating internal class for " + classNameInternal);
        parent->dataNames[classNameInternal] = "";

        startDataFile(className, providedClassName);

        for (const std::string& line : parent->linesToAddForDataAndGlue)
            dataPrintLn(line);
        dataPrintLn("");
        dataPrintLn("");
        dataPrintLn("class " + className + ":");
        dataPrintLn("");

        pyCreateDataTypeToStringObject(className, variables);
        dataPrintLn("");
        pyCreateToStringObject(otherClassName, className, variables);
        dataPrintLn("");
        pyCreateInternalConstructor(variables, className);
        dataPrintLn("");
        pyCreateInternalEqualsMethod(variables, className);
        dataPrintLn("");
        pyCreateToStringMethod(variables, className);

        endDataFile();
    }

    void PythonDataConstruct::pyCreateDataTypeToStringObject(const std::string& className,
                                                              const std::vector<DataValues>& variables) {
        dataPrintLn("    @staticmethod");
        dataPrintLn("    def to_data_type_string() -> str:");
        dataPrintLn("        return " + parent->quoteIt(className + " {"));
        for (const DataValues& variable : variables)
            dataPrintLn("        + " + parent->quoteIt(variable.dataType + " "));
        dataPrintLn("        + " + parent->quoteIt("} "));
    }

    void PythonDataConstruct::pyCreateToStringObject(const std::string& className,
                                                      const std::string& otherClassName,
                                                      const std::vector<DataValues>& variables) {
        dataPrintLn("    def to_" + className + "(self):");
        if (!Configuration::oneDataFile)
            dataPrintLn("        " + makeImportForDataClass(className));
        dataPrintLn("        return " + className + "(");
        std::string comma = "";
        for (const DataValues& variable : variables) {
            std::string method = "str(self." + parent->makeName(variable.name) + ")";
            dataPrintLn("        " + comma + " " + method);
            comma = ",";
        }
        dataPrintLn("        )");
    }

} // namespace gherkinexecutor
