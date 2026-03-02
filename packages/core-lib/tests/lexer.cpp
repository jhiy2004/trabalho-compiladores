#include <gtest/gtest.h>
#include "compilador.h"

TEST(LexerTest, AddLexer) {
    LexicalAnalysis analysis("12+3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12", 1, 1 },
        { TokenType::OpAdd, "+", 1, 3 },
        { TokenType::IntId, "3", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, SubLexer) {
    LexicalAnalysis analysis("12-3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12", 1, 1 },
        { TokenType::OpSub, "-", 1, 3 },
        { TokenType::IntId, "3", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, DivLexer) {
    LexicalAnalysis analysis("12/3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12", 1, 1 },
        { TokenType::OpDiv, "/", 1, 3 },
        { TokenType::IntId, "3" ,1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, MulLexer) {
    LexicalAnalysis analysis("12*3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "12", 1, 1 },
        { TokenType::OpMul, "*", 1, 3 },
        { TokenType::IntId, "3", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, ParenthesisLexer) {
    LexicalAnalysis analysis("(12)");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::OpenPar, "(", 1, 1 },
        { TokenType::IntId, "12", 1, 2},
        { TokenType::ClosePar, ")", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, ExpressionLexer) {
    LexicalAnalysis analysis("(12 + 3) / 4.5");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::OpenPar, "(", 1, 1 },
        { TokenType::IntId, "12", 1, 2 },
        { TokenType::OpAdd, "+", 1, 5},
        { TokenType::IntId, "3", 1, 7},
        { TokenType::ClosePar, ")", 1, 8 },
        { TokenType::OpDiv, "/", 1, 10 },
        { TokenType::RealId, "4.5", 1, 12 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, MultilineCode) {
    LexicalAnalysis analysis("10+2\n5.2-3.1");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "10", 1, 1 },
        { TokenType::OpAdd, "+", 1, 3 },
        { TokenType::IntId, "2", 1, 4 },
        { TokenType::RealId, "5.2", 2, 1 },
        { TokenType::OpSub, "-", 2, 4 },
        { TokenType::RealId, "3.1", 2, 5 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, SpaceOneCol) {
    LexicalAnalysis analysis(" 10+2");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "10", 1, 2 },
        { TokenType::OpAdd, "+", 1, 4 },
        { TokenType::IntId, "2", 1, 5 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, TabOneCol) {
    LexicalAnalysis analysis("\t10+2");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<Token> expected = {
        { TokenType::IntId, "10", 1, 2 },
        { TokenType::OpAdd, "+", 1, 4 },
        { TokenType::IntId, "2", 1, 5 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}