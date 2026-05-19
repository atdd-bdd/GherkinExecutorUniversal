#include <iostream>
#include <memory>
#include "Configuration.h"
#include "LanguageAdapter.h"
#include "CppAdapter.h"
#include "JavaAdapter.h"
#include "PythonAdapter.h"
#include "CsharpAdapter.h"
#include "Translate.h"

using namespace gherkinexecutor;

int main(int argc, char* argv[]) {
    Configuration::readFromYamlFile("Configuration.yaml");

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);
    if (!args.empty())
        Translate::processArguments(args);

    if (Configuration::featureFiles.empty()) {
        Translate::readFilterList();
        Translate::readOptionList();
        if (Configuration::searchTree)
            Translate::readFeatureList();
    }

    if (Configuration::featureFiles.empty()) {
        std::cerr << "No feature files specified." << std::endl;
        return 1;
    }

    std::unique_ptr<LanguageAdapter> adapter;
    if (Configuration::outputLanguage == "java")
        adapter = std::make_unique<JavaAdapter>();
    else if (Configuration::outputLanguage == "python")
        adapter = std::make_unique<PythonAdapter>();
    else if (Configuration::outputLanguage == "csharp")
        adapter = std::make_unique<CsharpAdapter>();
    else
        adapter = std::make_unique<CppAdapter>();

    std::cout << "GherkinExecutorUniversal — output language: "
              << Configuration::outputLanguage << std::endl;

    for (const std::string& name : Configuration::featureFiles) {
        std::cout << "Processing: " << name << std::endl;
        Translate translate(adapter.get());
        translate.translateInTests(name);
    }

    return 0;
}
