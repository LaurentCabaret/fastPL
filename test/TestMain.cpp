#include "TestCriterion.cpp"
#include "TestCategory.cpp"
#include "gtest/gtest.h"
#include <sstream>
#include <utility>

int AtomicMCDAObject::nb_instances_{0};

int main(int argc, char *argv[]) {
  
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}