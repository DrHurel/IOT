#include <testing/Runner.h>
/*
    REQUIRE - will immediately quit the test case if the assert fails and will mark the test case as failed.
    REQUIRE_CALL_SUCCEEDED - checks common::Result and output the get_error_message in case of failure
    REQUIRE_IL_ASSERTION - will immediately quit the test case if no IL_ASS or IL_ERR is triggered during the function call.
         REQUIRE_IL_ASSERTION(my_method(p1, p2, ...));
  Internal usage documentation here: https://gitlab.mgt.proxad.net/valo/valo-engine/-/wikis/Tools/Cpp-unit-tests
  Assertion documentation available here: https://github.com/doctest/doctest/blob/master/doc/markdown/assertions.md
  Testcase documentation available here: https://github.com/doctest/doctest/blob/master/doc/markdown/testcases.md
  Doctest documentation available here: https://github.com/doctest/doctest/blob/master/doc/markdown
*/
namespace $NAMESPACE$ {
TEST_SUITE("<YOUR CLASS NAME SPACE>::<YOUR CLASS UNDER TEST> tests") {
TEST_CASE("<Short description of your test case>") {
    REQUIRE(true == true);
    // REQUIRE_CALL_SUCCEEDED(common::Result::fails()("This test fails");
}
}
}
