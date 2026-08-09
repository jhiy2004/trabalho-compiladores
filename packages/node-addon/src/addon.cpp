#include <napi.h>
#include "compilador.h"
#include "compilador_lalg.h"
#include "analisador_sintatico_procedimento.h"
#include "tabela_simbolos.h"
#include "analisador_semantico.h"

#include "util.h"
#include "mepa.h"

static Napi::Object stack_elem_to_js(Napi::Env env, const StackElem& s) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("terminal", Napi::Boolean::New(env, s.terminal));
    obj.Set("name", Napi::String::New(env, s.name));

    return obj;
}

static Napi::Object syntactic_error_to_js(Napi::Env env, const SyntacticError& e) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("error", Napi::String::New(env, e.error));
    obj.Set("line", Napi::Number::New(env, e.line));
    obj.Set("col", Napi::Number::New(env, e.col));

    return obj;
}

static Napi::Object simbolo_entry_to_js(Napi::Env env, const SimboloEntry& s) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("cadeia", Napi::String::New(env, s.cadeia));
    obj.Set("token", Napi::String::New(env, s.token));
    obj.Set("categoria", Napi::String::New(env, s.categoria_str()));
    obj.Set("tipo", Napi::String::New(env, s.tipo));
    obj.Set("valor", Napi::String::New(env, s.valor));
    obj.Set("escopo", Napi::String::New(env, s.escopo));
    obj.Set("utilizada", Napi::Boolean::New(env, s.utilizada));
    obj.Set("linha", Napi::Number::New(env, s.linha));

    Napi::Array params_arr = Napi::Array::New(env, s.parametros.size());
    for (size_t i = 0; i < s.parametros.size(); ++i) {
        Napi::Object p_obj = Napi::Object::New(env);
        p_obj.Set("cadeia", Napi::String::New(env, s.parametros[i].cadeia));
        p_obj.Set("tipo", Napi::String::New(env, s.parametros[i].tipo));
        p_obj.Set("por_referencia", Napi::Boolean::New(env, s.parametros[i].por_referencia));
        params_arr.Set(i, p_obj);
    }
    obj.Set("parametros", params_arr);

    return obj;
}

static Napi::Object semantic_error_to_js(Napi::Env env, const SemanticError& e) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("mensagem", Napi::String::New(env, e.mensagem));
    obj.Set("linha", Napi::Number::New(env, e.linha));
    obj.Set("col", Napi::Number::New(env, e.col));
    obj.Set("tipo_erro", Napi::Number::New(env, static_cast<int>(e.tipo_erro)));

    return obj;
}

static Napi::Object token_to_js(Napi::Env env, const Token& t) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("type", static_cast<int>(t.type));
    obj.Set("lexeme", t.lexeme);
    obj.Set("line", t.line);
    obj.Set("col", t.col);

    return obj;
}

static Napi::Object token_to_js(Napi::Env env, const TokenCalc& t) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("type", static_cast<int>(t.type));
    obj.Set("lexeme", t.lexeme);
    obj.Set("line", t.line);
    obj.Set("col", t.col);

    return obj;
}

static Napi::Object snapshot_to_js(Napi::Env env, const Snapshot& s) {
    Napi::Object obj = Napi::Object::New(env);

    if (s.curr_token.has_value()) {
        obj.Set("curr_token", token_to_js(env, s.curr_token.value()));
    } else {
        obj.Set("curr_token", env.Null());
    }
    
    Napi::Array symbols_arr = Napi::Array::New(env, s.curr_symbols.size());
    std::stack<StackElem> temp_stack = s.curr_symbols; 
    
    uint32_t stack_idx = temp_stack.size();
    while (!temp_stack.empty()) {
        stack_idx--;
        symbols_arr.Set(stack_idx, stack_elem_to_js(env, temp_stack.top()));
        temp_stack.pop();
    }
    obj.Set("curr_symbols", symbols_arr);

    Napi::Array errors_arr = Napi::Array::New(env, s.curr_errors.size());
    std::queue<SyntacticError> temp_queue = s.curr_errors;
    
    uint32_t queue_idx = 0;
    while (!temp_queue.empty()) {
        errors_arr.Set(queue_idx++, syntactic_error_to_js(env, temp_queue.front()));
        temp_queue.pop();
    }
    obj.Set("curr_errors", errors_arr);

    obj.Set("action", Napi::String::New(env, s.action));

    return obj;
}

//////////////////////////////////////////////////////////////
// Add Example
//////////////////////////////////////////////////////////////

Napi::Number AddWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected 2 arguments")
            .ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }

    int a = info[0].As<Napi::Number>().Int32Value();
    int b = info[1].As<Napi::Number>().Int32Value();

    return Napi::Number::New(env, add(a, b));
}

//////////////////////////////////////////////////////////////
// CALC LEXER WRAPPER
//////////////////////////////////////////////////////////////

class LexicalAnalysisCalcWrapper : public Napi::ObjectWrap<LexicalAnalysisCalcWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    LexicalAnalysisCalcWrapper(const Napi::CallbackInfo& info);

private:
    Napi::Value Analyze(const Napi::CallbackInfo& info);
    Napi::Value GetTokens(const Napi::CallbackInfo& info);

    std::unique_ptr<LexicalAnalysisCalc> lexer;
};

Napi::Object LexicalAnalysisCalcWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LexicalAnalysisCalc", {
        InstanceMethod("analyze", &LexicalAnalysisCalcWrapper::Analyze),
        InstanceMethod("get_tokens", &LexicalAnalysisCalcWrapper::GetTokens),
    });

    exports.Set("LexicalAnalysisCalc", func);
    return exports;
}

LexicalAnalysisCalcWrapper::LexicalAnalysisCalcWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LexicalAnalysisCalcWrapper>(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string")
            .ThrowAsJavaScriptException();
        return;
    }

    std::string input = info[0].As<Napi::String>();
    lexer = std::make_unique<LexicalAnalysisCalc>(input);
}

Napi::Value LexicalAnalysisCalcWrapper::Analyze(const Napi::CallbackInfo& info) {
    bool result = lexer->analyze();
    return Napi::Boolean::New(info.Env(), result);
}

Napi::Value LexicalAnalysisCalcWrapper::GetTokens(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    const auto& tokens = lexer->get_tokens();
    Napi::Array arr = Napi::Array::New(env, tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        arr.Set(i, token_to_js(env, tokens[i]));
    }

    return arr;
}

//////////////////////////////////////////////////////////////
// LALG LEXER WRAPPER
//////////////////////////////////////////////////////////////

class LexicalAnalysisLALGWrapper : public Napi::ObjectWrap<LexicalAnalysisLALGWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    LexicalAnalysisLALGWrapper(const Napi::CallbackInfo& info);

private:
    Napi::Value TokenizeAll(const Napi::CallbackInfo& info);
    Napi::Value GetToken(const Napi::CallbackInfo& info);
    Napi::Value getTokens(const Napi::CallbackInfo& info);

    std::unique_ptr<LexicalAnalysisLALG> lexer;
};

Napi::Object LexicalAnalysisLALGWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LexicalAnalysisLALG", {
        InstanceMethod("tokenizeAll", &LexicalAnalysisLALGWrapper::TokenizeAll),
        InstanceMethod("getToken", &LexicalAnalysisLALGWrapper::GetToken),
        InstanceMethod("getTokens", &LexicalAnalysisLALGWrapper::getTokens)
    });

    exports.Set("LexicalAnalysisLALG", func);
    return exports;
}

LexicalAnalysisLALGWrapper::LexicalAnalysisLALGWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LexicalAnalysisLALGWrapper>(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Expected source code string")
            .ThrowAsJavaScriptException();
        return;
    }

    std::string text = info[0].As<Napi::String>();
    lexer = std::make_unique<LexicalAnalysisLALG>(text);
}

Napi::Value LexicalAnalysisLALGWrapper::TokenizeAll(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto tokens = lexer->tokenize_all();
    Napi::Array arr = Napi::Array::New(env, tokens.size());

    for (size_t i = 0; i < tokens.size(); i++) {
        arr.Set(i, token_to_js(env, tokens[i]));
    }

    return arr;
}

Napi::Value LexicalAnalysisLALGWrapper::GetToken(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto tok = lexer->get_token();

    if (!tok.has_value())
        return env.Null();

    return token_to_js(env, tok.value());
}

Napi::Value LexicalAnalysisLALGWrapper::getTokens(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    const auto& tokens = lexer->get_tokens();

    Napi::Array arr = Napi::Array::New(env, tokens.size());

    for (size_t i = 0; i < tokens.size(); i++) {
        arr.Set(i, token_to_js(env, tokens[i]));
    }

    return arr;
}

//////////////////////////////////////////////////////////////
// TokenTypeCalc
//////////////////////////////////////////////////////////////
Napi::Object createTokenTypeCalcObject(Napi::Env env) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("IntId", (int)TokenTypeCalc::IntId);
    obj.Set("RealId", (int)TokenTypeCalc::RealId);
    obj.Set("OpAdd", (int)TokenTypeCalc::OpAdd);
    obj.Set("OpSub", (int)TokenTypeCalc::OpSub);
    obj.Set("OpMul", (int)TokenTypeCalc::OpMul);
    obj.Set("OpDiv", (int)TokenTypeCalc::OpDiv);
    obj.Set("OpenPar", (int)TokenTypeCalc::OpenPar);
    obj.Set("ClosePar", (int)TokenTypeCalc::ClosePar);
    obj.Set("Unk", (int)TokenTypeCalc::Unk);

    return obj;
}

//////////////////////////////////////////////////////////////
// TokenType
//////////////////////////////////////////////////////////////
Napi::Object createTokenTypeObject(Napi::Env env) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("ProgramWord", (int)TokenType::ProgramWord);
    obj.Set("ProcedureWord", (int)TokenType::ProcedureWord);
    obj.Set("VarWord", (int)TokenType::VarWord);
    obj.Set("IntWord", (int)TokenType::IntWord);
    obj.Set("BooleanWord", (int)TokenType::BooleanWord);
    obj.Set("RealWord", (int)TokenType::RealWord);
    obj.Set("ReadWord", (int)TokenType::ReadWord);
    obj.Set("WriteWord", (int)TokenType::WriteWord);
    obj.Set("TrueWord", (int)TokenType::TrueWord);
    obj.Set("FalseWord", (int)TokenType::FalseWord);
    obj.Set("BeginWord", (int)TokenType::BeginWord);
    obj.Set("EndWord", (int)TokenType::EndWord);
    obj.Set("IfWord", (int)TokenType::IfWord);
    obj.Set("ThenWord", (int)TokenType::ThenWord);
    obj.Set("ElseWord", (int)TokenType::ElseWord);
    obj.Set("WhileWord", (int)TokenType::WhileWord);
    obj.Set("DoWord", (int)TokenType::DoWord);
    obj.Set("OrWord", (int)TokenType::OrWord);
    obj.Set("DivWord", (int)TokenType::DivWord);
    obj.Set("AndWord", (int)TokenType::AndWord);
    obj.Set("NotWord", (int)TokenType::NotWord);
    obj.Set("SemiColonOp", (int)TokenType::SemiColonOp);
    obj.Set("ColonOp", (int)TokenType::ColonOp);
    obj.Set("CommaOp", (int)TokenType::CommaOp);
    obj.Set("DotOp", (int)TokenType::DotOp);
    obj.Set("EqualOp", (int)TokenType::EqualOp);
    obj.Set("AssignOp", (int)TokenType::AssignOp);
    obj.Set("DiffOp", (int)TokenType::DiffOp);
    obj.Set("LessOp", (int)TokenType::LessOp);
    obj.Set("LessEqualOp", (int)TokenType::LessEqualOp);
    obj.Set("GreaterEqualOp", (int)TokenType::GreaterEqualOp);
    obj.Set("GreaterOp", (int)TokenType::GreaterOp);
    obj.Set("AddOp", (int)TokenType::AddOp);
    obj.Set("SubOp", (int)TokenType::SubOp);
    obj.Set("MulOp", (int)TokenType::MulOp);
    obj.Set("OpenParOp", (int)TokenType::OpenParOp);
    obj.Set("CloseParOp", (int)TokenType::CloseParOp);
    obj.Set("SingleCommentOp", (int)TokenType::SingleCommentOp);
    obj.Set("Id", (int)TokenType::Id);
    obj.Set("Num", (int)TokenType::Num);
    obj.Set("UNK", (int)TokenType::UNK);

    return obj;
}

//
// Command Wrapper
//

class CommandWrapper : public Napi::ObjectWrap<CommandWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    CommandWrapper(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(
        Napi::Env env,
        const Command& command
    );

    const Command& GetCommand() const {
        return command;
    }
private:
    static Napi::FunctionReference constructor;

    Command command;

    // Properties
    Napi::Value GetType(const Napi::CallbackInfo& info);
    void SetType(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetArg(const Napi::CallbackInfo& info);
    void SetArg(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetDst(const Napi::CallbackInfo& info);
    void SetDst(const Napi::CallbackInfo& info, const Napi::Value& value);

    // Methods
    Napi::Value GetNumberArgs(const Napi::CallbackInfo& info);
    Napi::Value IsJump(const Napi::CallbackInfo& info);
    Napi::Value ToString(const Napi::CallbackInfo& info);

    static Command::Type ParseType(const Napi::Value& value);
    static Napi::String TypeToJS(Napi::Env env, Command::Type type);
};

Napi::FunctionReference CommandWrapper::constructor;

Napi::Object CommandWrapper::Init(
    Napi::Env env,
    Napi::Object exports
) {
    Napi::Function func = DefineClass(
        env,
        "Command",
        {
            InstanceAccessor(
                "type",
                &CommandWrapper::GetType,
                &CommandWrapper::SetType
            ),

            InstanceAccessor(
                "arg",
                &CommandWrapper::GetArg,
                &CommandWrapper::SetArg
            ),

            InstanceAccessor(
                "dst",
                &CommandWrapper::GetDst,
                &CommandWrapper::SetDst
            ),

            InstanceMethod(
                "getNumberArgs",
                &CommandWrapper::GetNumberArgs
            ),

            InstanceMethod(
                "isJump",
                &CommandWrapper::IsJump
            ),

            InstanceMethod(
                "toString",
                &CommandWrapper::ToString
            ),
        }
    );

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Command", func);

    // Export enum values.
    Napi::Object types = Napi::Object::New(env);

    types.Set("CRCT", Napi::Number::New(env, Command::CRCT));
    types.Set("CRVL", Napi::Number::New(env, Command::CRVL));
    types.Set("ARMZ", Napi::Number::New(env, Command::ARMZ));

    types.Set("SOMA", Napi::Number::New(env, Command::SOMA));
    types.Set("SUBT", Napi::Number::New(env, Command::SUBT));
    types.Set("MULT", Napi::Number::New(env, Command::MULT));
    types.Set("DIVI", Napi::Number::New(env, Command::DIVI));
    types.Set("MODI", Napi::Number::New(env, Command::MODI));

    types.Set("INVR", Napi::Number::New(env, Command::INVR));
    types.Set("CONJ", Napi::Number::New(env, Command::CONJ));
    types.Set("DISJ", Napi::Number::New(env, Command::DISJ));
    types.Set("NEGA", Napi::Number::New(env, Command::NEGA));
    types.Set("CMME", Napi::Number::New(env, Command::CMME));
    types.Set("CMMA", Napi::Number::New(env, Command::CMMA));
    types.Set("CMIG", Napi::Number::New(env, Command::CMIG));
    types.Set("CMDG", Napi::Number::New(env, Command::CMDG));
    types.Set("CMAG", Napi::Number::New(env, Command::CMAG));
    types.Set("CMEG", Napi::Number::New(env, Command::CMEG));

    types.Set("DSVS", Napi::Number::New(env, Command::DSVS));
    types.Set("DSVF", Napi::Number::New(env, Command::DSVF));
    types.Set("NADA", Napi::Number::New(env, Command::NADA));

    types.Set("LEIT", Napi::Number::New(env, Command::LEIT));
    types.Set("LECH", Napi::Number::New(env, Command::LECH));
    types.Set("IMPR", Napi::Number::New(env, Command::IMPR));
    types.Set("IMPC", Napi::Number::New(env, Command::IMPC));
    types.Set("IMPE", Napi::Number::New(env, Command::IMPE));

    types.Set("INPP", Napi::Number::New(env, Command::INPP));
    types.Set("AMEM", Napi::Number::New(env, Command::AMEM));
    types.Set("DMEM", Napi::Number::New(env, Command::DMEM));
    types.Set("PARA", Napi::Number::New(env, Command::PARA));

    exports.Set("CommandType", types);

    return exports;
}


CommandWrapper::CommandWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<CommandWrapper>(info),
      command{Command::NADA, std::nullopt, std::nullopt} {

    Napi::Env env = info.Env();

    if (info.Length() == 1 && info[0].IsExternal()) {
        Command* command =
            info[0].As<Napi::External<Command>>().Data();

        this->command = *command;
        return;
    }

    try {
        command.type = ParseType(info[0]);
    }
    catch (const std::exception& e) {
        Napi::TypeError::New(
            env,
            e.what()
        ).ThrowAsJavaScriptException();

        return;
    }

    // arg
    if (info.Length() >= 2 && !info[1].IsNull() && !info[1].IsUndefined()) {
        if (!info[1].IsNumber()) {
            Napi::TypeError::New(
                env,
                "arg must be a number or null"
            ).ThrowAsJavaScriptException();

            return;
        }

        command.arg = info[1].As<Napi::Number>().Int32Value();
    }

    // dst
    if (info.Length() >= 3 && !info[2].IsNull() && !info[2].IsUndefined()) {
        if (!info[2].IsString()) {
            Napi::TypeError::New(
                env,
                "dst must be a string or null"
            ).ThrowAsJavaScriptException();

            return;
        }

        command.dst = info[2].As<Napi::String>().Utf8Value();
    }
}


// ---------------------------------------------------------
// type
// ---------------------------------------------------------

Napi::Value CommandWrapper::GetType(
    const Napi::CallbackInfo& info
) {
    return TypeToJS(
        info.Env(),
        command.type
    );
}


void CommandWrapper::SetType(
    const Napi::CallbackInfo& info,
    const Napi::Value& value
) {
    try {
        command.type = ParseType(value);
    }
    catch (const std::exception& e) {
        Napi::TypeError::New(
            info.Env(),
            e.what()
        ).ThrowAsJavaScriptException();
    }
}


// ---------------------------------------------------------
// arg
// ---------------------------------------------------------

Napi::Value CommandWrapper::GetArg(
    const Napi::CallbackInfo& info
) {
    if (!command.arg) {
        return info.Env().Null();
    }

    return Napi::Number::New(
        info.Env(),
        *command.arg
    );
}


void CommandWrapper::SetArg(
    const Napi::CallbackInfo& info,
    const Napi::Value& value
) {
    if (value.IsNull() || value.IsUndefined()) {
        command.arg.reset();
        return;
    }

    if (!value.IsNumber()) {
        Napi::TypeError::New(
            info.Env(),
            "arg must be a number or null"
        ).ThrowAsJavaScriptException();

        return;
    }

    command.arg = value.As<Napi::Number>().Int32Value();
}


// ---------------------------------------------------------
// dst
// ---------------------------------------------------------

Napi::Value CommandWrapper::GetDst(
    const Napi::CallbackInfo& info
) {
    if (!command.dst) {
        return info.Env().Null();
    }

    return Napi::String::New(
        info.Env(),
        *command.dst
    );
}


void CommandWrapper::SetDst(
    const Napi::CallbackInfo& info,
    const Napi::Value& value
) {
    if (value.IsNull() || value.IsUndefined()) {
        command.dst.reset();
        return;
    }

    if (!value.IsString()) {
        Napi::TypeError::New(
            info.Env(),
            "dst must be a string or null"
        ).ThrowAsJavaScriptException();

        return;
    }

    command.dst = value.As<Napi::String>().Utf8Value();
}


// ---------------------------------------------------------
// Methods
// ---------------------------------------------------------

Napi::Value CommandWrapper::GetNumberArgs(
    const Napi::CallbackInfo& info
) {
    return Napi::Number::New(
        info.Env(),
        command.get_number_args()
    );
}


Napi::Value CommandWrapper::IsJump(
    const Napi::CallbackInfo& info
) {
    return Napi::Boolean::New(
        info.Env(),
        command.is_jump()
    );
}


Napi::Value CommandWrapper::ToString(
    const Napi::CallbackInfo& info
) {
    std::string result = command.to_string();

    if (command.arg) {
        result += " ";
        result += std::to_string(*command.arg);
    }

    return Napi::String::New(
        info.Env(),
        result
    );
}


// ---------------------------------------------------------
// Conversion
// ---------------------------------------------------------

Command::Type CommandWrapper::ParseType(
    const Napi::Value& value
) {
    if (value.IsNumber()) {
        auto n = value.As<Napi::Number>().Int32Value();

        if (n < 0 || n > static_cast<int>(Command::PARA)) {
            throw std::invalid_argument(
                "Invalid Command type"
            );
        }

        return static_cast<Command::Type>(n);
    }

    if (!value.IsString()) {
        throw std::invalid_argument(
            "Command type must be a string or number"
        );
    }

    const std::string type =
        value.As<Napi::String>().Utf8Value();

#define COMMAND_TYPE(name) \
    if (type == #name) return Command::name;

    COMMAND_TYPE(CRCT)
    COMMAND_TYPE(CRVL)
    COMMAND_TYPE(ARMZ)

    COMMAND_TYPE(SOMA)
    COMMAND_TYPE(SUBT)
    COMMAND_TYPE(MULT)
    COMMAND_TYPE(DIVI)
    COMMAND_TYPE(MODI)

    COMMAND_TYPE(INVR)
    COMMAND_TYPE(CONJ)
    COMMAND_TYPE(DISJ)
    COMMAND_TYPE(NEGA)
    COMMAND_TYPE(CMME)
    COMMAND_TYPE(CMMA)
    COMMAND_TYPE(CMIG)
    COMMAND_TYPE(CMDG)
    COMMAND_TYPE(CMAG)
    COMMAND_TYPE(CMEG)

    COMMAND_TYPE(DSVS)
    COMMAND_TYPE(DSVF)
    COMMAND_TYPE(NADA)

    COMMAND_TYPE(LEIT)
    COMMAND_TYPE(LECH)
    COMMAND_TYPE(IMPR)
    COMMAND_TYPE(IMPC)
    COMMAND_TYPE(IMPE)

    COMMAND_TYPE(INPP)
    COMMAND_TYPE(AMEM)
    COMMAND_TYPE(DMEM)
    COMMAND_TYPE(PARA)

#undef COMMAND_TYPE

    throw std::invalid_argument(
        "Unknown Command type: " + type
    );
}


Napi::String CommandWrapper::TypeToJS(
    Napi::Env env,
    Command::Type type
) {
    return Napi::String::New(
        env,
        [&]() -> const char* {
            switch (type) {
                case Command::CRCT: return "CRCT";
                case Command::CRVL: return "CRVL";
                case Command::ARMZ: return "ARMZ";

                case Command::SOMA: return "SOMA";
                case Command::SUBT: return "SUBT";
                case Command::MULT: return "MULT";
                case Command::DIVI: return "DIVI";
                case Command::MODI: return "MODI";

                case Command::INVR: return "INVR";
                case Command::CONJ: return "CONJ";
                case Command::DISJ: return "DISJ";
                case Command::NEGA: return "NEGA";
                case Command::CMME: return "CMME";
                case Command::CMMA: return "CMMA";
                case Command::CMIG: return "CMIG";
                case Command::CMDG: return "CMDG";
                case Command::CMAG: return "CMAG";
                case Command::CMEG: return "CMEG";

                case Command::DSVS: return "DSVS";
                case Command::DSVF: return "DSVF";
                case Command::NADA: return "NADA";

                case Command::LEIT: return "LEIT";
                case Command::LECH: return "LECH";
                case Command::IMPR: return "IMPR";
                case Command::IMPC: return "IMPC";
                case Command::IMPE: return "IMPE";

                case Command::INPP: return "INPP";
                case Command::AMEM: return "AMEM";
                case Command::DMEM: return "DMEM";
                case Command::PARA: return "PARA";

                default: return "UNK";
            }
        }()
    );
}

Napi::Object CommandWrapper::NewInstance(
    Napi::Env env,
    const Command& command
) {
    Napi::External<Command> external =
        Napi::External<Command>::New(
            env,
            const_cast<Command*>(&command)
        );

    return constructor.New({ external });
}

//////////////////////////////////////////////////////////////
// Syntactic Analyzer
//////////////////////////////////////////////////////////////
class SyntacticAnalyzerProceduresWrapper : public Napi::ObjectWrap<SyntacticAnalyzerProceduresWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    SyntacticAnalyzerProceduresWrapper(const Napi::CallbackInfo& info);

    Napi::Value Run(const Napi::CallbackInfo& info);
    Napi::Value GetErrors(const Napi::CallbackInfo& info);
    Napi::Value GetSymbols(const Napi::CallbackInfo& info);
    Napi::Value GetSnapshots(const Napi::CallbackInfo& info);
    Napi::Value GetSymbolTable(const Napi::CallbackInfo& info);
    Napi::Value GetSemanticErrors(const Napi::CallbackInfo& info);
    Napi::Value GetCommands(const Napi::CallbackInfo& info);
private:
    std::unique_ptr<SyntacticAnalyzerProcedures> analyzer;
};

Napi::Object SyntacticAnalyzerProceduresWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SyntacticAnalyzerProcedures", {
        InstanceMethod("run", &SyntacticAnalyzerProceduresWrapper::Run),
        InstanceMethod("get_errors", &SyntacticAnalyzerProceduresWrapper::GetErrors),
        InstanceMethod("get_symbols", &SyntacticAnalyzerProceduresWrapper::GetSymbols),
        InstanceMethod("get_snapshots", &SyntacticAnalyzerProceduresWrapper::GetSnapshots),
        InstanceMethod("get_tabela_simbolos", &SyntacticAnalyzerProceduresWrapper::GetSymbolTable),
        InstanceMethod("get_erros_semanticos", &SyntacticAnalyzerProceduresWrapper::GetSemanticErrors),
        InstanceMethod("get_commands", &SyntacticAnalyzerProceduresWrapper::GetCommands)
    });

    exports.Set("SyntacticAnalyzerProcedures", func);
    return exports;
}

SyntacticAnalyzerProceduresWrapper::SyntacticAnalyzerProceduresWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<SyntacticAnalyzerProceduresWrapper>(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string")
            .ThrowAsJavaScriptException();
        return;
    }

    std::string input = info[0].As<Napi::String>();
    analyzer = std::make_unique<SyntacticAnalyzerProcedures>(input);
}

Napi::Value SyntacticAnalyzerProceduresWrapper::Run(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    analyzer->run();

    return env.Undefined();
}

Napi::Value SyntacticAnalyzerProceduresWrapper::GetErrors(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto errors_queue = analyzer->get_errors(); 
    
    Napi::Array arr = Napi::Array::New(env, errors_queue.size());

    uint32_t i = 0;
    while (!errors_queue.empty()) {
        arr.Set(i, syntactic_error_to_js(env, errors_queue.front()));
        errors_queue.pop();
        i++;
    }

    return arr;
}

Napi::Value SyntacticAnalyzerProceduresWrapper::GetSymbols(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto symbols_stack = analyzer->get_symbols(); 
    
    uint32_t size = symbols_stack.size();
    Napi::Array arr = Napi::Array::New(env, size);

    uint32_t i = size; 
    
    while (!symbols_stack.empty()) {
        i--;
        arr.Set(i, stack_elem_to_js(env, symbols_stack.top()));
        symbols_stack.pop();
    }

    return arr;
}

Napi::Value SyntacticAnalyzerProceduresWrapper::GetSnapshots(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto snapshots = analyzer->get_snapshots();
    Napi::Array arr = Napi::Array::New(env, snapshots.size());

    for (size_t i = 0; i < snapshots.size(); i++) {
        arr.Set(i, snapshot_to_js(env, snapshots[i]));
    }

    return arr;
}

Napi::Value SyntacticAnalyzerProceduresWrapper::GetSymbolTable(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    const auto& tabela = analyzer->get_tabela_simbolos().get_entradas();
    Napi::Array arr = Napi::Array::New(env, tabela.size());

    for (size_t i = 0; i < tabela.size(); ++i) {
        arr.Set(i, simbolo_entry_to_js(env, tabela[i]));
    }

    return arr;
}

Napi::Value SyntacticAnalyzerProceduresWrapper::GetSemanticErrors(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    const auto& erros = analyzer->get_erros_semanticos();
    Napi::Array arr = Napi::Array::New(env, erros.size());

    for (size_t i = 0; i < erros.size(); ++i) {
        arr.Set(i, semantic_error_to_js(env, erros[i]));
    }

    return arr;
}

Napi::Value SyntacticAnalyzerProceduresWrapper::GetCommands(
    const Napi::CallbackInfo& info
) {
    Napi::Env env = info.Env();

    const std::vector<Command> commands =
        analyzer->get_commands();

    Napi::Array result =
        Napi::Array::New(env, commands.size());

    for (std::size_t i = 0; i < commands.size(); ++i) {
        result.Set(
            i,
            CommandWrapper::NewInstance(env, commands[i])
        );
    }

    return result;
}

//
// Mepa
//

class MepaWrapper : public Napi::ObjectWrap<MepaWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    MepaWrapper(const Napi::CallbackInfo& info);

private:
    Napi::Value PopOutput(const Napi::CallbackInfo& info);
    Napi::Value PushInput(const Napi::CallbackInfo& info);
    Napi::Value NextInputType(const Napi::CallbackInfo& info);
    Napi::Value Run(const Napi::CallbackInfo& info);

    std::unique_ptr<Mepa> mepa;
};

Napi::Object MepaWrapper::Init(
    Napi::Env env,
    Napi::Object exports
) {
    Napi::Function func = DefineClass(
        env,
        "Mepa",
        {
            InstanceMethod(
                "popOutput",
                &MepaWrapper::PopOutput
            ),

            InstanceMethod(
                "pushInput",
                &MepaWrapper::PushInput
            ),

            InstanceMethod(
                "nextInputType",
                &MepaWrapper::NextInputType
            ),

            InstanceMethod(
                "run",
                &MepaWrapper::Run
            ),
        }
    );

    exports.Set("Mepa", func);

    return exports;
}

MepaWrapper::MepaWrapper(
    const Napi::CallbackInfo& info
)
    : Napi::ObjectWrap<MepaWrapper>(info) {

    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(
            env,
            "Expected an array of commands"
        ).ThrowAsJavaScriptException();

        return;
    }

    if (!info[0].IsArray()) {
        Napi::TypeError::New(
            env,
            "Expected an array of commands"
        ).ThrowAsJavaScriptException();

        return;
    }

    Napi::Array js_commands =
        info[0].As<Napi::Array>();

    std::vector<Command> commands;
    commands.reserve(js_commands.Length());

    for (std::size_t i = 0; i < js_commands.Length(); ++i) {
        Napi::Value value = js_commands.Get(i);

        if (!value.IsObject()) {
            Napi::TypeError::New(
                env,
                "Each command must be a Command object"
            ).ThrowAsJavaScriptException();

            return;
        }

        Napi::Object object = value.As<Napi::Object>();

        CommandWrapper* wrapper = Napi::ObjectWrap<CommandWrapper>::Unwrap(object);

        commands.push_back(wrapper->GetCommand());
    }

    mepa = std::make_unique<Mepa>(commands);
}

Napi::Value MepaWrapper::PopOutput(
    const Napi::CallbackInfo& info
) {
    Napi::Env env = info.Env();

    return Napi::String::New(
        env,
        mepa->pop_output()
    );
}

Napi::Value MepaWrapper::PushInput(
    const Napi::CallbackInfo& info
) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(
            env,
            "Expected one argument"
        ).ThrowAsJavaScriptException();

        return env.Undefined();
    }

    Napi::Value value = info[0];

    if (value.IsNumber()) {
        mepa->push_input(
            value.As<Napi::Number>().Int32Value()
        );

        return env.Undefined();
    }

    if (value.IsString()) {
        std::string str =
            value.As<Napi::String>().Utf8Value();

        if (str.size() != 1) {
            Napi::TypeError::New(
                env,
                "Expected a single character"
            ).ThrowAsJavaScriptException();

            return env.Undefined();
        }

        mepa->push_input(str[0]);

        return env.Undefined();
    }

    Napi::TypeError::New(
        env,
        "Expected a number or a character"
    ).ThrowAsJavaScriptException();

    return env.Undefined();
}

Napi::Value MepaWrapper::NextInputType(
    const Napi::CallbackInfo& info
) {
    Napi::Env env = info.Env();

    switch (mepa->next_input_type()) {
        case Mepa::InputType::Int:
            return Napi::String::New(env, "int");

        case Mepa::InputType::Char:
            return Napi::String::New(env, "char");

        case Mepa::InputType::None:
            return Napi::String::New(env, "none");

        case Mepa::InputType::End:
            return Napi::String::New(env, "end");
    }

    return env.Null();
}

Napi::Value MepaWrapper::Run(
    const Napi::CallbackInfo& info
) {
    Napi::Env env = info.Env();

    mepa->run();

    return env.Undefined();
}

//////////////////////////////////////////////////////////////
// MODULE INIT
//////////////////////////////////////////////////////////////

Napi::Object Init(Napi::Env env, Napi::Object exports) {

    exports.Set("add", Napi::Function::New(env, AddWrapped));

    LexicalAnalysisCalcWrapper::Init(env, exports);
    LexicalAnalysisLALGWrapper::Init(env, exports);
    SyntacticAnalyzerProceduresWrapper::Init(env, exports);
    CommandWrapper::Init(env, exports);
    MepaWrapper::Init(env, exports);

    exports.Set("TokenTypeCalc", createTokenTypeCalcObject(env));
    exports.Set("TokenType", createTokenTypeObject(env));

    return exports;
}

NODE_API_MODULE(addon, Init)
