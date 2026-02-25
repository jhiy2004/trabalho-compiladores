#include <napi.h>
#include "compilador.h"

Napi::Number AddWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected 2 arguments")
            .ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }

    int a = info[0].As<Napi::Number>().Int32Value();
    int b = info[1].As<Napi::Number>().Int32Value();

    int result = add(a, b);

    return Napi::Number::New(env, result);
}

class LexicalAnalysisWrapper : public Napi::ObjectWrap<LexicalAnalysisWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LexicalAnalysisWrapper(const Napi::CallbackInfo& info);

private:
    Napi::Value Analyze(const Napi::CallbackInfo& info);
    Napi::Value GetTokens(const Napi::CallbackInfo& info);

    std::unique_ptr<LexicalAnalysis> lexer;
};

Napi::Object LexicalAnalysisWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LexicalAnalysis", {
        InstanceMethod("analyze", &LexicalAnalysisWrapper::Analyze),
        InstanceMethod("get_tokens", &LexicalAnalysisWrapper::GetTokens),
    });

    exports.Set("LexicalAnalysis", func);
    return exports;
}

LexicalAnalysisWrapper::LexicalAnalysisWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LexicalAnalysisWrapper>(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string").ThrowAsJavaScriptException();
        return;
    }

    std::string input = info[0].As<Napi::String>();
    lexer = std::make_unique<LexicalAnalysis>(input);
}

Napi::Value LexicalAnalysisWrapper::Analyze(const Napi::CallbackInfo& info) {
    bool result = lexer->analyze();
    return Napi::Boolean::New(info.Env(), result);
}

Napi::Value LexicalAnalysisWrapper::GetTokens(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    const auto& tokens = lexer->get_tokens();
    Napi::Array arr = Napi::Array::New(env, tokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        Napi::Object obj = Napi::Object::New(env);

        obj.Set("type", static_cast<int>(tokens[i].type));
        obj.Set("lexeme", tokens[i].lexeme);

        arr.Set(i, obj);
    }

    return arr;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("add", Napi::Function::New(env, AddWrapped));
   
    LexicalAnalysisWrapper::Init(env, exports);
    
    Napi::Object tokenType = Napi::Object::New(env);

    tokenType.Set("IntId", (int)TokenType::IntId);
    tokenType.Set("RealId", (int)TokenType::RealId);
    tokenType.Set("OpAdd", (int)TokenType::OpAdd);
    tokenType.Set("OpSub", (int)TokenType::OpSub);
    tokenType.Set("OpMul", (int)TokenType::OpMul);
    tokenType.Set("OpDiv", (int)TokenType::OpDiv);
    tokenType.Set("OpenPar", (int)TokenType::OpenPar);
    tokenType.Set("ClosePar", (int)TokenType::ClosePar);

    exports.Set("TokenType", tokenType);

    return exports;
}

NODE_API_MODULE(addon, Init)