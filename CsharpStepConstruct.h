#pragma once
#include "StepConstruct.h"

namespace gherkinexecutor {

    class CsharpStepConstruct : public StepConstruct {
    public:
        explicit CsharpStepConstruct(Translate* translate_ptr) : StepConstruct(translate_ptr) {}

    protected:
        // C# builder methods use "Set" + PascalCase (e.g. SetAnInt) and .Build()
        void convertBarLineToParameter(const std::vector<std::string>& headers,
            const std::vector<std::string>& values, const std::string& className,
            const std::string& comma, bool compare) override;

    private:
        static std::string csSetMethodName(const std::string& fieldName);
    };

} // namespace gherkinexecutor
