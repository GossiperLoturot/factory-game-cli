#include <gtest/gtest.h>

TEST(test, add_function) {
  EXPECT_EQ(2 + 3, 5);
  EXPECT_EQ(-1 + 1, 0);
  EXPECT_EQ(0 + 0, 0);
  EXPECT_EQ(-5 + -5, -10);
}
