#include <gtest/gtest.h>
#include <filesystem>
#include "compilador_lalg.h"
#include "util.h"

void compare_tokens(
    const std::vector<Token>& tokens,
    const std::vector<Token>& expected)
{
    ASSERT_EQ(tokens.size(), expected.size());

    for (size_t i = 0; i < tokens.size(); i++) {
        EXPECT_EQ(tokens[i], expected[i])
            << "Erro no token " << i
            << "\nObtido: " << tokens[i]
            << "\nEsperado: " << expected[i];
    }
}

TEST(LALGTest, TestComTxt) {
    std::string content = parse_file_to_string(std::filesystem::path(EXAMPLES_DIR) / "com.txt");

    LexicalAnalysisLALG analise(content);
    std::vector<Token> tokens = analise.tokenize_all();

    std::vector<Token> expected = {
        {TokenType::ProgramWord,"program",1,1},
        {TokenType::Id,"correto",1,9},
        {TokenType::SemiColonOp,";",1,16},

        {TokenType::IntWord,"int",2,1},
        {TokenType::Id,"a",2,5},
        {TokenType::CommaOp,",",2,6},
        {TokenType::Id,"b",2,8},
        {TokenType::CommaOp,",",2,9},
        {TokenType::Id,"c",2,11},
        {TokenType::SemiColonOp,";",2,12},

        {TokenType::BooleanWord,"boolean",3,1},
        {TokenType::Id,"d",3,9},
        {TokenType::CommaOp,",",3,10},
        {TokenType::Id,"e",3,12},
        {TokenType::CommaOp,",",3,13},
        {TokenType::Id,"f",3,15},
        {TokenType::SemiColonOp,";",3,16},

        {TokenType::ProcedureWord,"procedure",7,1},
        {TokenType::Id,"proc",7,11},
        {TokenType::OpenParOp,"(",7,15},
        {TokenType::VarWord,"var",7,16},
        {TokenType::Id,"a1",7,20},
        {TokenType::ColonOp,":",7,23},
        {TokenType::IntWord,"int",7,25},
        {TokenType::CloseParOp,")",7,28},
        {TokenType::SemiColonOp,";",7,29},

        {TokenType::IntWord,"int",8,1},
        {TokenType::Id,"a",8,5},
        {TokenType::CommaOp,",",8,6},
        {TokenType::Id,"b",8,8},
        {TokenType::CommaOp,",",8,9},
        {TokenType::Id,"c",8,11},
        {TokenType::SemiColonOp,";",8,12},

        {TokenType::BooleanWord,"boolean",9,1},
        {TokenType::Id,"d",9,9},
        {TokenType::CommaOp,",",9,10},
        {TokenType::Id,"e",9,12},
        {TokenType::CommaOp,",",9,13},
        {TokenType::Id,"f",9,15},
        {TokenType::SemiColonOp,";",9,16},

        {TokenType::BeginWord,"begin",10,1},

        {TokenType::Id,"a",11,2},
        {TokenType::AssignOp,":=",11,3},
        {TokenType::Num,"1",11,5},
        {TokenType::SemiColonOp,";",11,6},

        {TokenType::IfWord,"if",12,2},
        {TokenType::OpenParOp,"(",12,5},
        {TokenType::Id,"a",12,6},
        {TokenType::LessOp,"<",12,7},
        {TokenType::Num,"1",12,8},
        {TokenType::CloseParOp,")",12,9},

        {TokenType::Id,"a",13,3},
        {TokenType::AssignOp,":=",13,4},
        {TokenType::Num,"12",13,6},

        {TokenType::EndWord,"end",14,1},
        {TokenType::SemiColonOp,";",14,4},

        {TokenType::BeginWord,"begin",16,1},

        {TokenType::Id,"a",17,2},
        {TokenType::AssignOp,":=",17,3},
        {TokenType::Num,"2",17,5},
        {TokenType::SemiColonOp,";",17,6},

        {TokenType::Id,"b",18,2},
        {TokenType::AssignOp,":=",18,3},
        {TokenType::Num,"10",18,5},
        {TokenType::SemiColonOp,";",18,7},

        {TokenType::Id,"c",19,2},
        {TokenType::AssignOp,":=",19,3},
        {TokenType::Num,"11",19,5},
        {TokenType::SemiColonOp,";",19,7},

        {TokenType::Id,"a",20,2},
        {TokenType::AssignOp,":=",20,3},
        {TokenType::Id,"b",20,5},
        {TokenType::AddOp,"+",20,6},
        {TokenType::Id,"c",20,7},
        {TokenType::SemiColonOp,";",20,8},

        {TokenType::Id,"d",21,2},
        {TokenType::AssignOp,":=",21,3},
        {TokenType::TrueWord,"true",21,5},
        {TokenType::SemiColonOp,";",21,9},

        {TokenType::Id,"e",22,2},
        {TokenType::AssignOp,":=",22,3},
        {TokenType::FalseWord,"false",22,5},
        {TokenType::SemiColonOp,";",22,10},

        {TokenType::Id,"f",23,2},
        {TokenType::AssignOp,":=",23,3},
        {TokenType::TrueWord,"true",23,5},
        {TokenType::SemiColonOp,";",23,9},

        {TokenType::IfWord,"if",25,2},
        {TokenType::OpenParOp,"(",25,5},
        {TokenType::Id,"d",25,6},
        {TokenType::CloseParOp,")",25,7},

        {TokenType::BeginWord,"begin",26,2},

        {TokenType::Id,"a",27,3},
        {TokenType::AssignOp,":=",27,4},
        {TokenType::Num,"20",27,6},
        {TokenType::SemiColonOp,";",27,8},

        {TokenType::Id,"b",28,3},
        {TokenType::AssignOp,":=",28,4},
        {TokenType::Num,"10",28,6},
        {TokenType::MulOp,"*",28,8},
        {TokenType::Id,"c",28,9},
        {TokenType::SemiColonOp,";",28,10},

        {TokenType::Id,"c",29,3},
        {TokenType::AssignOp,":=",29,4},
        {TokenType::Id,"a",29,6},
        {TokenType::DivWord,"div",29,8},
        {TokenType::Id,"b",29,12},

        {TokenType::EndWord,"end",30,2},
        {TokenType::SemiColonOp,";",30,5},

        {TokenType::WhileWord,"while",31,2},
        {TokenType::OpenParOp,"(",31,8},
        {TokenType::Id,"a",31,9},
        {TokenType::GreaterOp,">",31,10},
        {TokenType::Num,"1",31,11},
        {TokenType::CloseParOp,")",31,12},

        {TokenType::BeginWord,"begin",32,2},

        {TokenType::IfWord,"if",33,3},
        {TokenType::OpenParOp,"(",33,6},
        {TokenType::Id,"b",33,7},
        {TokenType::GreaterOp,">",33,8},
        {TokenType::Num,"10",33,9},
        {TokenType::CloseParOp,")",33,11},

        {TokenType::Id,"b",34,4},
        {TokenType::AssignOp,":=",34,5},
        {TokenType::Num,"2",34,7},
        {TokenType::SemiColonOp,";",34,8},

        {TokenType::Id,"a",35,3},
        {TokenType::AssignOp,":=",35,4},
        {TokenType::Id,"a",35,6},
        {TokenType::SubOp,"-",35,7},
        {TokenType::Num,"1",35,8},

        {TokenType::EndWord,"end",36,2},

        {TokenType::EndWord,"end",37,1},
        {TokenType::DotOp,".",37,4},
    };

    compare_tokens(tokens, expected);
}
