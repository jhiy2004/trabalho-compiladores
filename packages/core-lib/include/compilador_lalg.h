#include <cctype>
#include <string_view>
#include <string>
#include <unordered_map>
#include <optional>
#include <vector>

enum class TokenType {
    ProgramWord,
    ProcedureWord,
    VarWord,
    IntWord,
    BooleanWord,
    ReadWord,
    WriteWord,
    TrueWord,
    FalseWord,
    BeginWord,
    EndWord,
    IfWord,
    ThenWord,
    ElseWord,
    WhileWord,
    DoWord,
    OrWord,
    DivWord,
    AndWord,
    NotWord,
    SemiColonOp,
    ColonOp,
    CommaOp,
    DotOp,
    EqualOp,
    AssignOp,
    DiffOp,
    LessOp,
    LessEqualOp,
    GreaterEqualOp,
    GreaterOp,
    AddOp,
    SubOp,
    MulOp,
    OpenParOp,
    CloseParOp,
    SingleCommentOp,
    Id,
    Num,
    UNK,
};

struct Token {
    TokenType type;
    std::string lexeme;
    unsigned int line;
    unsigned int col; 

    Token(TokenType t, std::string_view le, unsigned int li, unsigned int c)
        : type(t), lexeme(le), line(li), col(c) {}

    bool operator==(const Token& rhs) const {
        return (type == rhs.type) && (lexeme == rhs.lexeme) && (line == rhs.line) && (col == rhs.col);
    }
};

class LexicalAnalysis {
public:
    virtual ~LexicalAnalysis() {}
    virtual std::optional<Token> get_token() = 0;
    virtual std::vector<Token> get_tokens() const = 0;
    virtual std::vector<Token> tokenize_all() = 0;
};

class LexicalAnalysisLALG : public LexicalAnalysis {
public:
    LexicalAnalysisLALG(std::string_view text) : _text(text){}
    std::optional<Token> get_token() override;
    std::vector<Token> get_tokens() const override;
    std::vector<Token> tokenize_all() override;
private:
    static bool is_digit(char c);
    static bool is_letter(char c);
    static bool is_op(char c);
    static bool is_newline(char c);
    static bool is_separator(char c);

    static const std::unordered_map<std::string, TokenType> reserved_words;
    static const std::unordered_map<std::string, TokenType> operators;

    static constexpr char start_multiline_comment = '{';
    static constexpr char end_multiline_comment = '}';

    std::string _text;
    std::size_t curr_pos{0};
    unsigned int curr_line{1};
    unsigned int curr_col{1};
    std::vector<Token> tokens;
};
