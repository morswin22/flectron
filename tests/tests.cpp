#include <cstdlib>
#include <vector>
#include <string>
#include <future>
#include <iostream>

#include <tests_list.hpp>

void run_test(const std::string& test, std::promise<int> promise)
{
  promise.set_value(std::system(test.c_str()));
}

// TODO parse arguments
// int main(int argc, char** argv)
int main(void)
{
  std::vector<std::thread> threads;

  std::vector<std::future<int>> results;
  for (const auto& test : tests_list)
  { 
    std::promise<int> promise;
    results.push_back(promise.get_future());
    threads.emplace_back(run_test, std::string(test), std::move(promise));
  }

  if (results.empty())
  {
    std::cerr << "No tests found" << std::endl;
    return -1;
  }

  int return_code = 0;
  for (auto& result : results)
  {
    int code = result.get();
    if (code != 0)
    {
      std::cerr << "Test failed with code " << code << std::endl;
      return_code = code;
    }
  }

  for (auto& thread : threads)
    thread.join();

  if (return_code == 0)
  {
    std::cout << "All tests passed" << std::endl;
    return 0;
  }
  else
  {
    std::cerr << "Not all tests passed" << std::endl;
    return 1;
  }
}