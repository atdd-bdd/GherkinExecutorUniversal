#pragma once
#include "DataConstruct.h"

namespace gherkinexecutor {

    class PythonDataConstruct : public DataConstruct {
    public:
        explicit PythonDataConstruct(Translate* parent) : DataConstruct(parent) {}

        void actOnData(const std::vector<std::string>& words) override;
        void endOneDataFile() override;
        std::string alterDataType(const std::string& s) override;

    private:
        std::string makeImportForDataDirectory();
        std::string makeImportForDataClass(const std::string& name);
        std::string makePyValueFromString(const DataValues& variable, bool addSelf);

        void pyCreateConstructor(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateInternalConstructor(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateEqualsMethod(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateInternalEqualsMethod(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateToStringMethod(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateToJsonMethod(const std::vector<DataValues>& variables);
        void pyCreateFromJsonMethod(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateTableToJsonMethod(const std::string& className);
        void pyCreateJsonToTableMethod(const std::string& className);
        void pyCreateBuilderMethod(const std::vector<DataValues>& variables, const std::string& className);
        void pyCreateConversionMethod(const std::string& internalClassName,
                                      const std::vector<DataValues>& variables);
        void pyCreateInternalClass(const std::string& className, const std::string& otherClassName,
                                   const std::vector<DataValues>& variables, bool providedClassName);
        void pyCreateDataTypeToStringObject(const std::string& className,
                                            const std::vector<DataValues>& variables);
        void pyCreateToStringObject(const std::string& className, const std::string& otherClassName,
                                    const std::vector<DataValues>& variables);
    };

} // namespace gherkinexecutor
