#pragma once
#include "DataConstruct.h"

namespace gherkinexecutor {

    class JavaDataConstruct : public DataConstruct {
    public:
        explicit JavaDataConstruct(Translate* parent) : DataConstruct(parent) {}
        void actOnData(const std::vector<std::string>& words) override;
        void endOneDataFile() override {}  // Java uses one file per class; no-op here
        std::string alterDataType(const std::string& s) override;

    private:
        void openJavaFile(const std::string& className);
        void javaWriteHeader(const std::string& className);
        void javaCreateStringClass(const std::string& className,
                                   const std::string& internalClassName,
                                   const std::vector<DataValues>& variables,
                                   bool doInternal);
        void javaCreateInternalClass(const std::string& className,
                                     const std::string& otherClassName,
                                     const std::vector<DataValues>& variables);
        void javaCreateConstructors(const std::vector<DataValues>& variables,
                                    const std::string& className);
        void javaCreateEqualsMethod(const std::vector<DataValues>& variables,
                                    const std::string& className);
        void javaCreateInternalEqualsMethod(const std::vector<DataValues>& variables,
                                            const std::string& className);
        void javaCreateBuilderMethod(const std::vector<DataValues>& variables,
                                     const std::string& className);
        void javaCreateToStringMethod(const std::vector<DataValues>& variables,
                                      const std::string& className);
        void javaCreateToJsonMethod(const std::vector<DataValues>& variables);
        void javaCreateFromJsonMethod(const std::vector<DataValues>& variables,
                                      const std::string& className);
        void javaCreateListToJsonMethod(const std::string& className);
        void javaCreateListFromJsonMethod(const std::string& className);
        void javaCreateConversionMethod(const std::string& internalClassName,
                                        const std::vector<DataValues>& variables);
        void javaCreateToDataTypeString(const std::string& className,
                                        const std::vector<DataValues>& variables);
        void javaCreateReverseConversion(const std::string& className,
                                         const std::string& otherClassName,
                                         const std::vector<DataValues>& variables);
        static bool isJavaPrimitive(const std::string& type);
    };

} // namespace gherkinexecutor
