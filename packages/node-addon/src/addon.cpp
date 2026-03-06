#include <napi.h>
#include "compilador.h"
#include "compilador_lalg.h"

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

//////////////////////////////////////////////////////////////
// MODULE INIT
//////////////////////////////////////////////////////////////

Napi::Object Init(Napi::Env env, Napi::Object exports) {

    exports.Set("add", Napi::Function::New(env, AddWrapped));

    LexicalAnalysisCalcWrapper::Init(env, exports);
    LexicalAnalysisLALGWrapper::Init(env, exports);

    exports.Set("TokenTypeCalc", createTokenTypeCalcObject(env));
    exports.Set("TokenType", createTokenTypeObject(env));

    return exports;
}

NODE_API_MODULE(addon, Init)
