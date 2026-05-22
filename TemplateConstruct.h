#pragma once
#include "Translate.h"
namespace gherkinexecutor {
    class Translate;
    class TemplateConstruct {
    public:
        std::unique_ptr<std::ofstream> glueTemplateFile;
        std::unique_ptr<std::ofstream> glueTemplateFileHeader;

        int processNamespaces(const std::string& packagePath);
        void endNamespace(const std::string& packagePath);

        explicit TemplateConstruct(Translate* parent) : parent(parent) {}
        virtual ~TemplateConstruct() = default;

        virtual void beginTemplate();
        virtual void endTemplate();
        virtual void makeFunctionTemplate(const std::string& dataType, const std::string& fullName);
        virtual void makeFunctionTemplateIsList(const std::string& dataType, const std::string& fullName,
            const std::string& listElement);
        virtual void makeFunctionTemplateNothing(const std::string& dataType, const std::string& fullName);
        virtual void makeFunctionTemplateObject(const std::string& dataType, const std::string& fullName,
            const std::string& listElement);

    protected:
        Translate* parent;
        void templatePrint(const std::string& line);
        void templateHeaderPrint(const std::string& line);
        bool checkForExistingTemplate(const std::string& dataType, const std::string& fullName);
    };
}


