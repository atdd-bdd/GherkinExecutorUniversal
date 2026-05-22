#include "PythonStepConstruct.h"
#include "Configuration.h"
#include "Translate.h"
#include <algorithm>

namespace gherkinexecutor {

    void PythonStepConstruct::stringToList(const std::vector<std::string>& table,
                                           const std::string& fullName) {
        std::string s = std::to_string(getStepNumberInScenario());
        testPrint("        string_list" + s + " = [");
        std::string comma = "";
        for (const std::string& line : table) {
            testPrint("            " + comma + "\"" + line + "\"");
            comma = ",";
        }
        testPrint("            ]");
        testPrint("        " + getGlueObject() + "." + fullName + "(string_list" + s + ")");
        getTemplateConstruct().makeFunctionTemplateIsList("List[str]", fullName, "str");
    }

    void PythonStepConstruct::stringToString(const std::vector<std::string>& table,
                                              const std::string& fullName) {
        std::string s = std::to_string(getStepNumberInScenario());
        testPrint("        string" + s + " =\"\"\"");
        for (const std::string& line : table)
            testPrint(line);
        testPrint("\"\"\".strip()");
        testPrint("        " + getGlueObject() + "." + fullName + "(string" + s + ")");
        getTemplateConstruct().makeFunctionTemplate("str", fullName);
    }

    void PythonStepConstruct::tableToListOfListOfObject(const std::vector<std::string>& table,
                                                         const std::string& fullName,
                                                         const std::string& className) {
        std::string s = std::to_string(getStepNumberInScenario());
        std::string dataType = "List[List[str]]";
        testPrint("        string_list_list" + s + ":  " + dataType + "  = [");
        std::string comma = "";
        for (const std::string& line : table) {
            convertBarLineToList(line, comma);
            comma = ",";
        }
        testPrint("            ]");
        testPrint("        " + getGlueObject() + "." + fullName + "(string_list_list" + s + ")");
        getTemplateConstruct().makeFunctionTemplateObject(dataType, fullName, className);
        createConvertTableToListOfListOfObjectMethod(className);
    }

    void PythonStepConstruct::tableToListOfList(const std::vector<std::string>& table,
                                                 const std::string& fullName) {
        std::string s = std::to_string(getStepNumberInScenario());
        std::string dataType = "List[List[str]]";
        testPrint("        string_list_list" + s + " = [");
        std::string comma = "";
        for (const std::string& line : table) {
            convertBarLineToList(line, comma);
            comma = ",";
        }
        testPrint("            ]");
        testPrint("        " + getGlueObject() + "." + fullName + "(string_list_list" + s + ")");
        getTemplateConstruct().makeFunctionTemplateIsList(dataType, fullName, "List[str]");
    }

    void PythonStepConstruct::tableToString(const std::vector<std::string>& table,
                                             const std::string& fullName) {
        std::string s = std::to_string(getStepNumberInScenario());
        testPrint("        table" + s + " = \"\"\"");
        for (const std::string& line : table)
            testPrint(line);
        testPrint("\"\"\".strip()");
        testPrint("        " + getGlueObject() + "." + fullName + "(table" + s + ")");
        getTemplateConstruct().makeFunctionTemplate("str", fullName);
    }

    void PythonStepConstruct::tableToListOfObject(const std::vector<std::string>& table,
                                                   const std::string& fullName,
                                                   const std::string& className,
                                                   bool doTranspose, bool compare) {
        trace("TableToListOfObject class_names " + className);
        std::string s = std::to_string(getStepNumberInScenario());
        std::string dataType = "List[" + className + "]";
        testPrint("        object_list" + s + ": " + dataType + " = [");
        bool inHeaderLine = true;
        auto dataList = convertToListList(table, doTranspose);
        std::vector<std::string> headers;
        std::string comma = "";
        for (const auto& row : dataList) {
            if (inHeaderLine) {
                headers = row;
                for (const std::string& dataName : row) {
                    if (!findDataClassName(className, makeName(dataName)))
                        error("Data name " + dataName + " not in Data for " + className);
                }
                inHeaderLine = false;
                continue;
            }
            convertBarLineToParameter(headers, row, className, comma, compare);
            comma = ",";
        }
        testPrint("            ]");
        testPrint("        " + getGlueObject() + "." + fullName + "(object_list" + s + ")");
        getTemplateConstruct().makeFunctionTemplateIsList(dataType, fullName, className);
    }

    void PythonStepConstruct::createConvertTableToListOfListOfObjectMethod(const std::string& toClass) {
        std::string tmpl =
            "\n    def convert_list(self, string_list: list) -> list:\n"
            "        class_list = []\n"
            "        for inner_list in string_list:\n"
            "            inner_class_list = [" + toClass + "(s) for s in inner_list]\n"
            "        class_list.append(inner_class_list)\n"
            "        return class_list\n";
        getLinesToAddToEndOfGlue().push_back(tmpl);
    }

    void PythonStepConstruct::convertBarLineToList(const std::string& lineIn,
                                                    const std::string& commaIn) {
        std::string line = lineIn;
        size_t hashPos = line.find('#');
        if (hashPos != std::string::npos) line = line.substr(0, hashPos);
        while (!line.empty() && (line.back() == ' ' || line.back() == '\t')) line.pop_back();

        testPrint("            " + commaIn + "[");
        std::vector<std::string> elements = parseLine(line);
        std::string comma = "";
        for (const std::string& element : elements) {
            testPrint("            " + comma + "\"" + element + "\"");
            comma = ",";
        }
        testPrint("            ]");
    }

    void PythonStepConstruct::convertBarLineToParameter(const std::vector<std::string>& headers,
                                                         const std::vector<std::string>& values,
                                                         const std::string& className,
                                                         const std::string& comma,
                                                         bool compare) {
        trace("Headers " + headers[0]);
        size_t sz = std::min(headers.size(), values.size());
        if (headers.size() > values.size())
            error("not sufficient values, using what is there");
        testPrint("            " + comma + className + ".Builder()");
        if (compare)
            testPrint("             .set_compare()");
        for (size_t i = 0; i < sz; ++i) {
            std::string val = values[i];
            char spaceChar = Configuration::spaceCharacters;
            std::replace(val.begin(), val.end(), spaceChar, ' ');
            testPrint("                ." + makeBuildName(headers[i]) + "(\"" + val + "\")");
        }
        testPrint("                .build()");
    }

    void PythonStepConstruct::noParameter(const std::string& fullName) {
        testPrint("        " + getGlueObject() + "." + fullName + "()");
        getTemplateConstruct().makeFunctionTemplateNothing("", fullName);
    }

} // namespace gherkinexecutor
