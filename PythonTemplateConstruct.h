#pragma once
#include "TemplateConstruct.h"

namespace gherkinexecutor {

    class PythonTemplateConstruct : public TemplateConstruct {
    public:
        explicit PythonTemplateConstruct(Translate* parent) : TemplateConstruct(parent) {}

        void beginTemplate() override;
        void endTemplate() override;
        void makeFunctionTemplate(const std::string& dataType, const std::string& fullName) override;
        void makeFunctionTemplateIsList(const std::string& dataType, const std::string& fullName,
            const std::string& listElement) override;
        void makeFunctionTemplateNothing(const std::string& dataType, const std::string& fullName) override;
        void makeFunctionTemplateObject(const std::string& dataType, const std::string& fullName,
            const std::string& listElement) override;
    };

} // namespace gherkinexecutor
