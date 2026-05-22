#include "JavaStepConstruct.h"
#include "Configuration.h"
#include "Translate.h"
#include <algorithm>

namespace gherkinexecutor {

    void JavaStepConstruct::convertBarLineToParameter(const std::vector<std::string>& headers,
                                                       const std::vector<std::string>& values,
                                                       const std::string& className,
                                                       const std::string& comma,
                                                       bool compare) {
        size_t sz = std::min(headers.size(), values.size());
        if (headers.size() > values.size())
            error("not sufficient values, using what is there");

        testPrint("            " + comma + " " + "new " + className + ".Builder()");
        if (compare)
            testPrint("             .setCompare()");
        for (size_t i = 0; i < sz; ++i) {
            std::string val = values[i];
            char spaceChar = Configuration::spaceCharacters;
            std::replace(val.begin(), val.end(), spaceChar, ' ');
            testPrint("                ." + makeName(headers[i]) + "(\"" + val + "\")");
        }
        testPrint("                .build()");
    }

} // namespace gherkinexecutor
