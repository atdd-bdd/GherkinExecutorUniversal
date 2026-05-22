#pragma once
#include "StepConstruct.h"

namespace gherkinexecutor {

    class PythonStepConstruct : public StepConstruct {
    public:
        explicit PythonStepConstruct(Translate* translate_ptr) : StepConstruct(translate_ptr) {}

    protected:
        void stringToList(const std::vector<std::string>& table, const std::string& fullName) override;
        void stringToString(const std::vector<std::string>& table, const std::string& fullName) override;
        void tableToListOfListOfObject(const std::vector<std::string>& table,
            const std::string& fullName, const std::string& className) override;
        void tableToListOfList(const std::vector<std::string>& table, const std::string& fullName) override;
        void tableToString(const std::vector<std::string>& table, const std::string& fullName) override;
        void tableToListOfObject(const std::vector<std::string>& table,
            const std::string& fullName, const std::string& className,
            bool transpose, bool compare) override;
        void createConvertTableToListOfListOfObjectMethod(const std::string& className) override;
        void convertBarLineToList(const std::string& lineIn, const std::string& commaIn) override;
        void convertBarLineToParameter(const std::vector<std::string>& headers,
            const std::vector<std::string>& values, const std::string& className,
            const std::string& comma, bool compare) override;
        void noParameter(const std::string& fullName) override;
    };

} // namespace gherkinexecutor
