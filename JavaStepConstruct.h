#pragma once
#include "StepConstruct.h"

namespace gherkinexecutor {

    class JavaStepConstruct : public StepConstruct {
    public:
        explicit JavaStepConstruct(Translate* translate_ptr) : StepConstruct(translate_ptr) {}

    protected:
        // Java builder methods use the field name directly (no "set" prefix)
        void convertBarLineToParameter(const std::vector<std::string>& headers,
            const std::vector<std::string>& values, const std::string& className,
            const std::string& comma, bool compare) override;
    };

} // namespace gherkinexecutor
