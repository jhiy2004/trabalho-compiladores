#include <napi.h>
#include "compilador.h"
#include "compilador_lalg.h"
#include "analisador_sintatico_procedimento.h"
#include "tabela_simbolos.h"
#include "analisador_semantico.h"

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
        InstanceMethod("get_erros_semanticos", &SyntacticAnalyzerProceduresWrapper::GetSemanticErrors)
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

//////////////////////////////////////////////////////////////
// MODULE INIT
//////////////////////////////////////////////////////////////

Napi::Object Init(Napi::Env env, Napi::Object exports) {

    exports.Set("add", Napi::Function::New(env, AddWrapped));

    LexicalAnalysisCalcWrapper::Init(env, exports);
    LexicalAnalysisLALGWrapper::Init(env, exports);
    SyntacticAnalyzerProceduresWrapper::Init(env, exports);

    exports.Set("TokenTypeCalc", createTokenTypeCalcObject(env));
    exports.Set("TokenType", createTokenTypeObject(env));

    return exports;
}

NODE_API_MODULE(addon, Init)
