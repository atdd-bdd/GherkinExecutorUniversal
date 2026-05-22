#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <set>

#include "DataConstruct.h"
#include "TemplateConstruct.h"
#include "Translate.h"
namespace gherkinexecutor {
    class Translate;
    class StepConstruct {
    public:
        StepConstruct(Translate* translate_ptr) : outer(translate_ptr) {}
        virtual ~StepConstruct() = default;

        void actOnStep(const std::string& fullName, const std::vector<std::string>& comment);

    protected:
        Translate* outer;

        virtual void stringToList(const std::vector<std::string>& table, const std::string& fullName);
        virtual void stringToString(const std::vector<std::string>& table, const std::string& fullName);
        virtual void tableToListOfListOfObject(const std::vector<std::string>& table,
            const std::string& fullName, const std::string& className);
        virtual void tableToListOfList(const std::vector<std::string>& table, const std::string& fullName);
        virtual void tableToString(const std::vector<std::string>& table, const std::string& fullName);
        virtual void tableToListOfObject(const std::vector<std::string>& table,
            const std::string& fullName, const std::string& className,
            bool transpose, bool compare);
        virtual void createConvertTableToListOfListOfObjectMethod(const std::string& className);
        virtual void convertBarLineToList(const std::string& lineIn, const std::string& commaIn);
        virtual void convertBarLineToParameter(const std::vector<std::string>& headers,
            const std::vector<std::string>& values, const std::string& className,
            const std::string& comma, bool compare);
        virtual void noParameter(const std::string& fullName);

        // Helpers accessible to subclasses
        std::pair<std::string, std::vector<std::string>> lookForFollow();
        void testPrint(const std::string& line);
        void error(const std::string& message);
        void trace(const std::string& message);
        std::vector<std::string> parseLine(const std::string& line);
        std::string makeName(const std::string& input);
        std::string makeBuildName(const std::string& s);
        std::string makeValueFromString(const DataConstruct::DataValues& variable, bool makeNameValue);
        int& getStepNumberInScenario();
        std::string& getGlueObject();
        TemplateConstruct& getTemplateConstruct();
        std::vector<std::string>& getClassDataNames();
        std::vector<std::string>& getLinesToAddToEndOfGlue();
        std::vector<std::vector<std::string>> convertToListList(const std::vector<std::string>& table, bool transpose);
        bool findDataClassName(const std::string& className, const std::string& dataName);
        std::vector<std::vector<std::string>> transpose(const std::vector<std::vector<std::string>>& matrix);

    private:
        void createTheTable(const std::vector<std::string>& comment,
            const std::vector<std::string>& table, const std::string& fullName);
        void createTheStringCode(const std::vector<std::string>& comment,
            const std::vector<std::string>& table, const std::string& fullName);
    };
}


