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
        Napi::TypeError::New(env, "Expected string").ThrowAsJavaScriptException();
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
        Napi::Object obj = Napi::Object::New(env);

        obj.Set("type", static_cast<int>(tokens[i].type));
        obj.Set("lexeme", tokens[i].lexeme);
        obj.Set("line", tokens[i].line);
        obj.Set("col", tokens[i].col);

        arr.Set(i, obj);
    }

    return arr;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("add", Napi::Function::New(env, AddWrapped));
   
    LexicalAnalysisCalcWrapper::Init(env, exports);
    
    Napi::Object tokenType = Napi::Object::New(env);

    tokenType.Set("IntId", (int)TokenTypeCalc::IntId);
    tokenType.Set("RealId", (int)TokenTypeCalc::RealId);
    tokenType.Set("OpAdd", (int)TokenTypeCalc::OpAdd);
    tokenType.Set("OpSub", (int)TokenTypeCalc::OpSub);
    tokenType.Set("OpMul", (int)TokenTypeCalc::OpMul);
    tokenType.Set("OpDiv", (int)TokenTypeCalc::OpDiv);
    tokenType.Set("OpenPar", (int)TokenTypeCalc::OpenPar);
    tokenType.Set("ClosePar", (int)TokenTypeCalc::ClosePar);

    exports.Set("TokenTypeCalc", tokenType);

    return exports;
}

NODE_API_MODULE(addon, Init)
