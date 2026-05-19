#include <string>
#include <algorithm>
#include "StepConstruct.h"
#include "Translate.h"

namespace gherkinexecutor {

    void StepConstruct::actOnStep(const std::string& fullName, const std::vector<std::string>& comment) {
        getStepNumberInScenario()++;
        auto follow = lookForFollow();
        std::string followType = follow.first;
        std::vector<std::string> table = follow.second;

        testPrint("");

        if (followType == "TABLE")
            createTheTable(comment, table, fullName);
        else if (followType == "NOTHING")
            noParameter(fullName);
        else if (followType == "STRING")
            createTheStringCode(comment, table, fullName);
        else
            error("Internal Error - Follow type " + followType);
    }

    void StepConstruct::createTheStringCode(const std::vector<std::string>& comment,
                                             const std::vector<std::string>& table,
                                             const std::string& fullName) {
        std::string option = "String";
        if (!comment.empty() && !comment[0].empty())
            option = comment[0];

        if (option == "ListOfString")
            stringToList(table, fullName);
        else
            stringToString(table, fullName);
    }

    void StepConstruct::stringToList(const std::vector<std::string>& table,
                                     const std::string& fullName) {
        LanguageAdapter* adapter = outer->getAdapter();
        std::string s = std::to_string(getStepNumberInScenario());
        std::string stype = adapter->stringType();
        std::string dataType = adapter->listType(stype);

        testPrint("        " + adapter->listVarDecl(stype, "stringList" + s) +
                  adapter->listInitOpen());
        std::string comma = "";
        for (const auto& line : table) {
            testPrint("            " + comma + "\"" + line + "\"");
            comma = ",";
        }
        testPrint("            " + adapter->listInitClose());
        testPrint("        " + getGlueObject() + "." + fullName + "(stringList" + s + ");");
        getTemplateConstruct().makeFunctionTemplateIsList(dataType, fullName, stype);
    }

    void StepConstruct::stringToString(const std::vector<std::string>& table,
                                       const std::string& fullName) {
        LanguageAdapter* adapter = outer->getAdapter();
        std::string s = std::to_string(getStepNumberInScenario());
        std::string stype = adapter->stringType();

        testPrint("        " + stype + " string" + s + " = " + adapter->multilineStringDeclSuffix());
        bool isFirst = true;
        for (const auto& line : table) {
            testPrint(isFirst ? adapter->multilineStringFirstLine(line)
                              : adapter->multilineStringBodyLine(line));
            isFirst = false;
        }
        testPrint(adapter->multilineStringClose() + ";");
        testPrint("        " + getGlueObject() + "." + fullName + "(string" + s + ");");
        getTemplateConstruct().makeFunctionTemplate(stype, fullName);
    }

    void StepConstruct::tableToListOfListOfObject(const std::vector<std::string>& table,
                                                   const std::string& fullName,
                                                   const std::string& className) {
        LanguageAdapter* adapter = outer->getAdapter();
        std::string s = std::to_string(getStepNumberInScenario());
        std::string dataType = adapter->listOfListType();

        testPrint("        " + adapter->listVarDecl(adapter->listType(adapter->stringType()),
                                                    "stringListList" + s) +
                  adapter->listInitOpen());
        std::string comma = "";
        for (const auto& line : table) {
            convertBarLineToList(line, comma);
            comma = ",";
        }
        testPrint("            " + adapter->listInitClose());
        testPrint("        " + getGlueObject() + "." + fullName + "(stringListList" + s + ");");
        getTemplateConstruct().makeFunctionTemplateObject(dataType, fullName, className);
        createConvertTableToListOfListOfObjectMethod(className);
    }

    void StepConstruct::tableToListOfList(const std::vector<std::string>& table,
                                          const std::string& fullName) {
        LanguageAdapter* adapter = outer->getAdapter();
        std::string s = std::to_string(getStepNumberInScenario());
        std::string stype = adapter->stringType();
        std::string dataType = adapter->listOfListType();

        testPrint("        " + adapter->listVarDecl(adapter->listType(stype),
                                                    "stringListList" + s) +
                  adapter->listInitOpen());
        std::string comma = "";
        for (const auto& line : table) {
            convertBarLineToList(line, comma);
            comma = ",";
        }
        testPrint("            " + adapter->listInitClose());
        testPrint("        " + getGlueObject() + "." + fullName + "(stringListList" + s + ");");
        getTemplateConstruct().makeFunctionTemplateIsList(dataType, fullName,
                                                          adapter->listType(stype));
    }

    void StepConstruct::createTheTable(const std::vector<std::string>& comment,
                                        const std::vector<std::string>& table,
                                        const std::string& fullName) {
        std::string option = "ListOfList";
        std::string className;

        if (!comment.empty() && !comment[0].empty())
            option = comment[0];

        if (option == "ListOfList") {
            tableToListOfList(table, fullName);
        }
        else if (option == "ListOfListOfObject") {
            if (comment.size() < 2) { error("No class name specified"); return; }
            className = comment[1];
            tableToListOfListOfObject(table, fullName, className);
        }
        else if (option == "String" || option == "string") {
            tableToString(table, fullName);
        }
        else if (option == "ListOfObject") {
            if (comment.size() < 2) { error("No class name specified"); return; }
            className = comment[1];
            bool transpose = false;
            bool compare   = false;
            if (comment.size() > 2) {
                std::string action = comment[2];
                if (action == "compare" || action == "Compare")
                    compare = true;
                else if (action == "transpose" || action == "Transpose")
                    transpose = true;
                else
                    error("Action not recognized " + action);
            }
            tableToListOfObject(table, fullName, className, transpose, compare);
        }
        else {
            error("Option not found, default to ListOfList " + option);
            tableToListOfList(table, fullName);
        }
    }

    void StepConstruct::createConvertTableToListOfListOfObjectMethod(const std::string& toClassOriginal) {
        outer->trace("Creating convertTableToListOfListOfObjectMethod for " + toClassOriginal);
        std::string toClass = outer->dataConstruct->alterDataType(toClassOriginal);
        DataConstruct::DataValues variable("s", "default", toClass, "notes");
        std::string convert = outer->makeValueFromString(variable, true);
        std::string template_str = R"(
                static std::vector<std::vector<)" + toClass + R"(>> convertListTo)" + toClass + R"((const std::vector<std::vector<std::string>>& stringList) {
                    std::vector<std::vector<)" + toClass + R"(>> classList;
                    for (const auto& innerList : stringList) {
                        std::vector<)" + toClass + R"(> innerClassList;
                        for (const auto& s : innerList) {
                            innerClassList.push_back()" + convert + R"();
                        }
                        classList.push_back(innerClassList);
                    }
                return classList;
                }
        )";
        getLinesToAddToEndOfGlue().push_back(template_str);
    }

    void StepConstruct::tableToString(const std::vector<std::string>& table,
                                      const std::string& fullName) {
        LanguageAdapter* adapter = outer->getAdapter();
        std::string s = std::to_string(getStepNumberInScenario());
        std::string stype = adapter->stringType();

        testPrint("        " + stype + " table" + s + " = " + adapter->multilineStringDeclSuffix());
        bool isFirst = true;
        for (const auto& line : table) {
            testPrint(isFirst ? adapter->multilineStringFirstLine(line)
                              : adapter->multilineStringBodyLine(line));
            isFirst = false;
        }
        testPrint(adapter->multilineStringClose() + ";");
        testPrint("        " + getGlueObject() + "." + fullName + "(table" + s + ");");
        getTemplateConstruct().makeFunctionTemplate(stype, fullName);
    }

    void StepConstruct::convertBarLineToList(const std::string& lineIn,
                                              const std::string& commaIn) {
        std::string line = lineIn;
        size_t pos = line.find('#');
        if (pos != std::string::npos) line = line.substr(0, pos);
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        LanguageAdapter* adapter = outer->getAdapter();
        testPrint("           " + commaIn + adapter->innerListOpen());
        std::vector<std::string> elements = parseLine(line);
        std::string comma = "";
        for (const auto& element : elements) {
            testPrint("            " + comma + "\"" + element + "\"");
            comma = ",";
        }
        testPrint("            " + adapter->innerListClose());
    }

    void StepConstruct::tableToListOfObject(const std::vector<std::string>& table,
                                             const std::string& fullName,
                                             const std::string& className,
                                             bool transpose,
                                             bool compare) {
        LanguageAdapter* adapter = outer->getAdapter();
        trace("TableToListOfObject classNames " + className);
        std::string s = std::to_string(getStepNumberInScenario());
        std::string dataType = adapter->listType(className);

        testPrint("        " + adapter->listVarDecl(className, "objectList" + s) +
                  adapter->listInitOpen());
        bool inHeaderLine = true;
        std::vector<std::vector<std::string>> dataList = convertToListList(table, transpose);
        std::vector<std::string> headers;
        std::string comma = "";

        for (const auto& row : dataList) {
            if (inHeaderLine) {
                headers = row;
                for (const auto& dataName : row) {
                    if (!findDataClassName(className, makeName(dataName)))
                        error("Data name " + dataName + " not in Data for " + className);
                }
                inHeaderLine = false;
                continue;
            }
            convertBarLineToParameter(headers, row, className, comma, compare);
            comma = ",";
        }
        testPrint("            " + adapter->listInitClose());
        testPrint("        " + getGlueObject() + "." + fullName + "(objectList" + s + ");");
        getTemplateConstruct().makeFunctionTemplateIsList(dataType, fullName, className);
    }

    std::vector<std::vector<std::string>>
    StepConstruct::convertToListList(const std::vector<std::string>& table, bool doTranspose) {
        std::vector<std::vector<std::string>> temporary;
        for (const auto& line : table)
            temporary.push_back(parseLine(line));
        return doTranspose ? this->transpose(temporary) : temporary;
    }

    bool StepConstruct::findDataClassName(const std::string& className,
                                          const std::string& dataName) {
        std::string compare = className + "#" + dataName;
        for (const auto& value : getClassDataNames()) {
            if (value == compare) return true;
        }
        return false;
    }

    void StepConstruct::convertBarLineToParameter(const std::vector<std::string>& headers,
                                                   const std::vector<std::string>& values,
                                                   const std::string& className,
                                                   const std::string& comma,
                                                   bool compare) {
        LanguageAdapter* adapter = outer->getAdapter();
        size_t size = headers.size();
        if (headers.size() > values.size()) {
            size = values.size();
            error("not sufficient values, using what is there");
        }

        testPrint("            " + comma + " " + adapter->builderCreate(className));
        if (compare)
            testPrint("             " + adapter->builderSet("setCompare", ""));

        for (size_t i = 0; i < size; i++) {
            std::string temp = values[i];
            std::replace(temp.begin(), temp.end(), Configuration::spaceCharacters, ' ');
            std::string value = "\"" + temp + "\"";
            testPrint("                " + adapter->builderSet(makeBuildName(headers[i]), value));
        }
        testPrint("                " + adapter->builderBuild());
    }

    void StepConstruct::noParameter(const std::string& fullName) {
        testPrint("        " + getGlueObject() + "." + fullName + "();");
        getTemplateConstruct().makeFunctionTemplateNothing("", fullName);
    }

    std::vector<std::vector<std::string>>
    StepConstruct::transpose(const std::vector<std::vector<std::string>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return {};
        std::vector<std::vector<std::string>> transposed;
        for (size_t i = 0; i < matrix[0].size(); i++) {
            std::vector<std::string> row;
            for (size_t j = 0; j < matrix.size(); j++)
                row.push_back(matrix[j][i]);
            transposed.push_back(row);
        }
        return transposed;
    }

    // Delegation helpers
    std::pair<std::string, std::vector<std::string>> StepConstruct::lookForFollow() {
        return outer->lookForFollow();
    }
    void StepConstruct::testPrint(const std::string& line) { outer->testPrint(line); }
    void StepConstruct::error(const std::string& message)  { outer->error(message); }
    void StepConstruct::trace(const std::string& message)  { outer->trace(message); }
    std::vector<std::string> StepConstruct::parseLine(const std::string& line) { return outer->parseLine(line); }
    std::string StepConstruct::makeName(const std::string& input) { return outer->makeName(input); }
    std::string StepConstruct::makeBuildName(const std::string& s) { return outer->makeBuildName(s); }
    int& StepConstruct::getStepNumberInScenario() { return outer->stepNumberInScenario; }
    std::string& StepConstruct::getGlueObject() { return outer->glueObject; }
    TemplateConstruct& StepConstruct::getTemplateConstruct() { return *outer->templateConstruct; }
    std::vector<std::string>& StepConstruct::getClassDataNames() { return outer->classDataNames; }
    std::vector<std::string>& StepConstruct::getLinesToAddToEndOfGlue() { return outer->linesToAddToEndOfGlue; }

} // namespace gherkinexecutor
