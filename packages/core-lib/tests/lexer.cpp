#include <gtest/gtest.h>
#include "compilador.h"

TEST(LexerTest, AddLexer) {
    LexicalAnalysisCalc analysis("12+3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "12", 1, 1 },
        { TokenTypeCalc::OpAdd, "+", 1, 3 },
        { TokenTypeCalc::IntId, "3", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, SubLexer) {
    LexicalAnalysisCalc analysis("12-3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "12", 1, 1 },
        { TokenTypeCalc::OpSub, "-", 1, 3 },
        { TokenTypeCalc::IntId, "3", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, DivLexer) {
    LexicalAnalysisCalc analysis("12/3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "12", 1, 1 },
        { TokenTypeCalc::OpDiv, "/", 1, 3 },
        { TokenTypeCalc::IntId, "3" ,1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, MulLexer) {
    LexicalAnalysisCalc analysis("12*3");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "12", 1, 1 },
        { TokenTypeCalc::OpMul, "*", 1, 3 },
        { TokenTypeCalc::IntId, "3", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, ParenthesisLexer) {
    LexicalAnalysisCalc analysis("(12)");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::OpenPar, "(", 1, 1 },
        { TokenTypeCalc::IntId, "12", 1, 2},
        { TokenTypeCalc::ClosePar, ")", 1, 4 }
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, ExpressionLexer) {
    LexicalAnalysisCalc analysis("(12 + 3) / 4.5");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::OpenPar, "(", 1, 1 },
        { TokenTypeCalc::IntId, "12", 1, 2 },
        { TokenTypeCalc::OpAdd, "+", 1, 5},
        { TokenTypeCalc::IntId, "3", 1, 7},
        { TokenTypeCalc::ClosePar, ")", 1, 8 },
        { TokenTypeCalc::OpDiv, "/", 1, 10 },
        { TokenTypeCalc::RealId, "4.5", 1, 12 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, MultilineCode) {
    LexicalAnalysisCalc analysis("10+2\n5.2-3.1");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "10", 1, 1 },
        { TokenTypeCalc::OpAdd, "+", 1, 3 },
        { TokenTypeCalc::IntId, "2", 1, 4 },
        { TokenTypeCalc::RealId, "5.2", 2, 1 },
        { TokenTypeCalc::OpSub, "-", 2, 4 },
        { TokenTypeCalc::RealId, "3.1", 2, 5 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, SpaceOneCol) {
    LexicalAnalysisCalc analysis(" 10+2");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "10", 1, 2 },
        { TokenTypeCalc::OpAdd, "+", 1, 4 },
        { TokenTypeCalc::IntId, "2", 1, 5 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}

TEST(LexerTest, TabOneCol) {
    LexicalAnalysisCalc analysis("\t10+2");
    bool success = analysis.analyze();

    ASSERT_TRUE(success);

    std::vector<TokenCalc> expected = {
        { TokenTypeCalc::IntId, "10", 1, 2 },
        { TokenTypeCalc::OpAdd, "+", 1, 4 },
        { TokenTypeCalc::IntId, "2", 1, 5 },
    };

    EXPECT_EQ(analysis.get_tokens(), expected);
}
