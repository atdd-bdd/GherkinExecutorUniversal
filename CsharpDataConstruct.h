#pragma once
#include "DataConstruct.h"

namespace gherkinexecutor {

    class CsharpDataConstruct : public DataConstruct {
    public:
        explicit CsharpDataConstruct(Translate* parent) : DataConstruct(parent) {}
        void actOnData(const std::vector<std::string>& words) override;
        void endOneDataFile() override {}  // C# uses one file per class; no-op
        std::string alterDataType(const std::string& s) override;

    private:
        void openCsFile(const std::string& className);
        void csWriteHeader(const std::string& className);
        void csWriteFooter();
        void csCreateStringClass(const std::string& className,
                                  const std::string& internalClassName,
                                  const std::vector<DataValues>& variables,
                                  bool doInternal);
        void csCreateInternalClass(const std::string& className,
                                   const std::string& otherClassName,
                                   const std::vector<DataValues>& variables);
        void csCreateConstructors(const std::vector<DataValues>& variables,
                                  const std::string& className);
        void csCreateEqualsMethod(const std::vector<DataValues>& variables,
                                  const std::string& className);
        void csCreateInternalEqualsMethod(const std::vector<DataValues>& variables,
                                          const std::string& className);
        void csCreateGetHashCode(const std::vector<DataValues>& variables);
        void csCreateBuilderMethod(const std::vector<DataValues>& variables,
                                   const std::string& className);
        void csCreateToStringMethod(const std::vector<DataValues>& variables,
                                    const std::string& className);
        void csCreateToJsonMethod(const std::vector<DataValues>& variables,
                                  const std::string& className);
        void csCreateFromJsonMethod(const std::vector<DataValues>& variables,
                                    const std::string& className);
        void csCreateListToJsonMethod(const std::string& className);
        void csCreateListFromJsonMethod(const std::string& className);
        void csCreateComparerClass(const std::string& className);
        void csCreateConversionMethod(const std::string& internalClassName,
                                      const std::vector<DataValues>& variables);
        void csCreateToDataTypeString(const std::string& className,
                                      const std::vector<DataValues>& variables);
        void csCreateReverseConversion(const std::string& className,
                                       const std::string& otherClassName,
                                       const std::vector<DataValues>& variables);
        std::string csSetMethodName(const std::string& fieldName);
        static bool isCsPrimitive(const std::string& type);
    };

} // namespace gherkinexecutor
