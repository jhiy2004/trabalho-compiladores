#include <gtest/gtest.h>
#include "compilador.h"

TEST(LexerTest, AddLexer) {
    LexicalAnalysis analysis("12+3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12" },
        { TokenType::OpAdd, "+" },
        { TokenType::IntId, "3" }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, SubLexer) {
    LexicalAnalysis analysis("12-3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12" },
        { TokenType::OpSub, "-"},
        { TokenType::IntId, "3" }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, DivLexer) {
    LexicalAnalysis analysis("12/3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12" },
        { TokenType::OpDiv, "/"},
        { TokenType::IntId, "3" }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, MulLexer) {
    LexicalAnalysis analysis("12*3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12" },
        { TokenType::OpMul, "*"},
        { TokenType::IntId, "3" }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, ParenthesisLexer) {
    LexicalAnalysis analysis("(12)");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::OpenPar, "(" },
        { TokenType::IntId, "12"},
        { TokenType::ClosePar, ")" }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, ExpressionLexer) {
    LexicalAnalysis analysis("(12 + 3) / 4.5");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::OpenPar, "(" },
        { TokenType::IntId, "12" },
        { TokenType::OpAdd, "+"},
        { TokenType::IntId, "3" },
        { TokenType::ClosePar, ")" },
        { TokenType::OpDiv, "/" },
        { TokenType::RealId, "4.5" },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}