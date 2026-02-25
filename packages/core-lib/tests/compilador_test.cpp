#include <gtest/gtest.h>
#include "compilador.h"

TEST(CompiladorTest, SomaSimples) {
    EXPECT_EQ(add(2, 3), 5);
}

TEST(CompiladorTest, SomaNegativo) {
    EXPECT_EQ(add(2, -3), -1);
}