#include "tests.hpp"

TEST_SUITE("Main tests")
{

  TEST("Should pass")
  {
    ASSERT(true, "This should pass");

    ASSERT_SHOULD_FAIL();
    ASSERT(false, "This should also pass");
  }

}
