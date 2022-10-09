#pragma once
#include <flectron.hpp>

enum ReturnCode
{
  SUCCESS = 0,
  FAILURE = 1,
  UNHANDLED_EXCEPTION = 2
};

struct AssertResult
{
  std::string message;
  bool success;
  bool skip;
  bool should_fail;
};

struct TestResult
{
  std::string name;
  std::vector<AssertResult> results;
  bool skip;
  bool should_fail;

  bool is_success() const
  {
    if (skip)
      return false;
    for (const auto& result : results)
    {
      if (result.skip)
        continue;
      if (!result.success)
        return false;
    }
    return true;
  }

  bool is_skip() const
  {
    return skip;
  }

  bool is_failure() const
  {
    return !is_success() && !is_skip();
  }

  std::string get_message() const
  {
    std::stringstream ss;
    for (const auto& result : results)
    {
      if (result.skip)
        continue;
      if (!result.success)
        ss << result.message << std::endl;
    }
    return ss.str();
  }
};

struct Context
{
  TestResult test;
  AssertResult assert;
};

#define UNKNOWN_STATE "__unknown_state"

class Test
{
public:
  ReturnCode return_code;
  std::vector<TestResult> results;

  Context context;

private:
  Test()
    : return_code(SUCCESS), results(), context({ { UNKNOWN_STATE, {}, false, false }, { UNKNOWN_STATE, false, false, false } })
  {}

public:
  static Test& instance() 
  {
    static Test instance;
    return instance;
  }

  void reset_context()
  {
    if (context.test.name == UNKNOWN_STATE)
      return;

    reset_assert_context();
    results.push_back(context.test);
    context = { { UNKNOWN_STATE, {}, false }, { UNKNOWN_STATE, false, false } };
  }

  void reset_assert_context()
  {
    if (context.assert.message == UNKNOWN_STATE)
      return;

    context.test.results.push_back(context.assert);
    context.assert = { UNKNOWN_STATE, false, false };
  }

  int num_success() const
  {
    int count = 0;
    for (const auto& result : results)
      if (result.is_success())
        count++;
    return count;
  }

  int num_fail() const
  {
    int count = 0;
    for (const auto& result : results)
      if (result.is_failure())
        count++;
    return count;
  }

  int num_skip() const
  {
    int count = 0;
    for (const auto& result : results)
      if (result.is_skip())
        count++;
    return count;
  }

  int num_total() const
  {
    return results.size();
  }
};

#define TEST_I Test::instance()

#define SKIP_TEST() \
  TEST_I.reset_context(); \
  TEST_I.context.test.skip = true

#define TEST_SHOULD_FAIL() \
  TEST_I.reset_context(); \
  TEST_I.context.test.should_fail = true

#define TEST(name_) \
  TEST_I.reset_context(); \
  TEST_I.context.test.name = name_;

template<typename T>
std::string print(const T& value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

#define SKIP_ASSERT() \
  TEST_I.reset_assert_context(); \
  TEST_I.context.assert.skip = true

#define ASSERT_SHOULD_FAIL() \
  TEST_I.reset_assert_context(); \
  TEST_I.context.assert.should_fail = true

#define ASSERT(expr_, message_) \
  TEST_I.reset_assert_context(); \
  TEST_I.context.assert.message = "'" message_ "' at " __FILE__ ":" + print(__LINE__); \
  if (!TEST_I.context.assert.skip && !TEST_I.context.test.skip) \
  { \
    if (expr_) \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
      else \
      { \
        TEST_I.context.assert.success = true; \
      } \
    } \
    else \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = true; \
      } \
      else \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
    } \
  }

#define ASSERT_EQUAL(a_, b_) \
  ASSERT((a_) == (b_), "Expected " #a_ " == " #b_ ", but got " + print(a_) + " != " + print(b_) + "")

#define ASSERT_NOT_EQUAL(a_, b_) \
  ASSERT((a_) != (b_), "Expected " #a_ " != " #b_ ", but got " + print(a_) + " == " + print(b_) + "")

#define ASSERT_GTE(a_, b_) \
  ASSERT((a_) >= (b_), "Expected " #a_ " >= " #b_ ", but got " + print(a_) + " < " + print(b_) + "")

#define ASSERT_LTE(a_, b_) \
  ASSERT((a_) <= (b_), "Expected " #a_ " <= " #b_ ", but got " + print(a_) + " > " + print(b_) + "")

#define ASSERT_GT(a_, b_) \
  ASSERT((a_) > (b_), "Expected " #a_ " > " #b_ ", but got " + print(a_) + " <= " + print(b_) + "")

#define ASSERT_LT(a_, b_) \
  ASSERT((a_) < (b_), "Expected " #a_ " < " #b_ ", but got " + print(a_) + " >= " + print(b_) + "")

#define ASSERT_THROW(expr_, exception_) \
  TEST_I.reset_assert_context(); \
  TEST_I.context.assert.message = "'Expected " #expr_ " to throw " #exception_ "' at " __FILE__ ":" + print(__LINE__); \
  if (!TEST_I.context.assert.skip && !TEST_I.context.test.skip) \
  { \
    try \
    { \
      expr_; \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = true; \
      } \
      else \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
    } \
    catch (const exception_&) \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
      else \
      { \
        TEST_I.context.assert.success = true; \
      } \
    } \
    catch (...) \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = true; \
      } \
      else \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
    } \
  }

#define ASSERT_NOT_THROW(expr_, exception_) \
  TEST_I.reset_assert_context(); \
  TEST_I.context.assert.message = "'Expected " #expr_ " not to throw " #exception_ "' at " __FILE__ ":" + print(__LINE__); \
  if (!TEST_I.context.assert.skip && !TEST_I.context.test.skip) \
  { \
    try \
    { \
      expr_; \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
      else \
      { \
        TEST_I.context.assert.success = true; \
      } \
    } \
    catch (const exception_&) \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = true; \
      } \
      else \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
    } \
    catch (...) \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
      else \
      { \
        TEST_I.context.assert.success = true; \
      } \
    } \
  }

#define ASSERT_NOT_THROW_ANY(expr_) \
  TEST_I.reset_assert_context(); \
  TEST_I.context.assert.message = "'Expected " #expr_ " not to throw any exceptions' at " __FILE__ ":" + print(__LINE__); \
  if (!TEST_I.context.assert.skip && !TEST_I.context.test.skip) \
  { \
    try \
    { \
      expr_; \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
      else \
      { \
        TEST_I.context.assert.success = true; \
      } \
    } \
    catch (...) \
    { \
      if (TEST_I.context.assert.should_fail || TEST_I.context.test.should_fail) \
      { \
        TEST_I.context.assert.success = true; \
      } \
      else \
      { \
        TEST_I.context.assert.success = false; \
        TEST_I.return_code = FAILURE; \
      } \
    } \
  }

#define FLECTRON_INTERNAL_CATCH() \
  catch(const std::exception& e) \
  { \
    FLECTRON_LOG_CRITICAL("Unhandled exception: {}", e.what()); \
    TEST_I.return_code = UNHANDLED_EXCEPTION; \
  } \
  catch(...) \
  { \
    FLECTRON_LOG_CRITICAL("Unhandled exception"); \
    TEST_I.return_code = UNHANDLED_EXCEPTION; \
  }

class TestApplication : public flectron::Application
{
public:
  TestApplication();
  static std::string_view get_test_suite_name();
};

int main(void)
{
  flectron::Log::init();
  FLECTRON_LOG_TRACE("Running tests '{}'", TestApplication::get_test_suite_name());

  try 
  {
    auto app = flectron::createScope<TestApplication>();

    if (app->numLayers() > 0)
    {
      try 
      {
        app->run();
      }
      FLECTRON_INTERNAL_CATCH();
    }

  }
  FLECTRON_INTERNAL_CATCH();

  TEST_I.reset_context();
  for (const auto& result : TEST_I.results)
  {
    if (result.is_success())
      FLECTRON_LOG_INFO("Test '{}' succeeded", result.name);
    else if (result.is_skip())
      FLECTRON_LOG_INFO("Test '{}' skipped", result.name);
    else
      FLECTRON_LOG_ERROR("Test '{}' failed:\n{}", result.name, result.get_message());
  }

  FLECTRON_LOG_INFO(
    "Test suite finished with {} tests, {} passed, {} failed, {} skipped",
    TEST_I.num_total(), TEST_I.num_success(), TEST_I.num_fail(), TEST_I.num_skip()
  );
  FLECTRON_LOG_TRACE("Test suite finished with return code {}", TEST_I.return_code);

  return TEST_I.return_code;
}

#define TEST_SUITE_PARAMS(name_, ...) \
  std::string_view TestApplication::get_test_suite_name() \
  { \
    return name_; \
  } \
  TestApplication::TestApplication() : flectron::Application(flectron::ApplicationArguments(), flectron::WindowProperties( name_, ##__VA_ARGS__ ))

#define TEST_SUITE(name_) \
  TEST_SUITE_PARAMS(name_, 640, 480, flectron::VSYNC)