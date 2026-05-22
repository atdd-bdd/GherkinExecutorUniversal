#include "CsharpStepConstruct.h"
#include "Configuration.h"
#include "Translate.h"
#include <algorithm>
#include <cctype>

namespace gherkinexecutor {

    std::string CsharpStepConstruct::csSetMethodName(const std::string& fieldName) {
        if (fieldName.empty()) return "Set";
        std::string result = "Set";
        result += (char)std::toupper((unsigned char)fieldName[0]);
        result += fieldName.substr(1);
        return result;
    }

    void CsharpStepConstruct::convertBarLineToParameter(const std::vector<std::string>& headers,
                                                         const std::vector<std::string>& values,
                                                         const std::string& className,
                                                         const std::string& comma,
                                                         bool compare) {
        size_t sz = std::min(headers.size(), values.size());
        if (headers.size() > values.size())
            error("not sufficient values, using what is there");

        testPrint("             " + comma + "new " + className + ".Builder()");
        if (compare)
            testPrint("             .SetCompare()");
        for (size_t i = 0; i < sz; ++i) {
            std::string val = values[i];
            char spaceChar = Configuration::spaceCharacters;
            std::replace(val.begin(), val.end(), spaceChar, ' ');
            testPrint("                ." + csSetMethodName(makeName(headers[i])) + "(\"" + val + "\")");
        }
        testPrint("                .Build()");
    }

} // namespace gherkinexecutor
