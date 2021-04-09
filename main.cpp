#include <cstddef>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <cstring>
#include <cmath>
#include <chrono>

/* ---------------- aliases ----------------- */
using u8_t = std::uint8_t;
using i8_t = std::int8_t;
using u16_t = std::uint16_t;
using i16_t = std::int16_t;
using u32_t = std::uint32_t;
using i32_t = std::int32_t;
using u64_t = std::uint64_t;
using i64_t = std::int64_t;
using nullptr_t = std::nullptr_t;
using string = std::string;

template <typename T>
using vector = std::vector<T>;

template <typename T, std::size_t N>
using array = std::array<T, N>;

template <typename T, typename S>
using umap = std::unordered_map<T, S>;


/* ------------------- fundamental cmponents ------------------- */
enum TokenKind {
    Integer, Double, Character,
    String,
    Plus, Minus, Star, Slash,
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    LessThan, LessEqual, GreaterThan,
    GreaterEqual, Equal, EqualEqual,
    LogicalAnd, LogicalOr,
    Bang, NotEqual, Semicolon,
    Modulus,
    Comma,
    Nil, True, False, Var,
    Print, If, Else,
    Elif,
    While, For,
    Return,
    Input,
    Get_C, Get_I,
    Get_S, Get_D, Get_B,
    Func,
    Identifier,
    FuncIdentifier,
    Eof, Error,
    Unrecognized
};

constexpr char const *tokens[] = {
    "Integer", "Double", "Character",
    "String",
    "+", "-", "*", "/",
    "(", ")",
    "{", "}",
    "<", "<=", ">",
    ">=", "=", "==",
    "&&", "||",
    "!", "!=", ";",
    "%",
    ",",
    "Nil", "True", "False",
    "var", "print", "if",
    "else", "elif",
    "while", "for",
    "return",
    "input", "get_c",
    "get_i", "get_s",
    "get_d", "get_b",
    "func",
    "identifier",
    "function name",
    "Eof", "Error"
};

struct Token {
    TokenKind _kind{ Eof };
    i32_t line;
};

enum OpCode {
    int_c,  /* integer constant */
    char_c,     /* character constant */
    double_c,   /* double constant */
    string_c,
    add,
    sub,
    mult,
    idiv,
    positive,
    neg,
    nil,
    true_l,
    false_l,
    lt,
    lte,
    gt,
    gte,
    eq,
    inot,
    neq,
    logical_and,
    logical_or,
    mod,
    jit,    /* jumpt if true */
    jif,    /* jump if false */
    jump,
    ipop,
    ipush_bp,
    ipop_bp,
    push_arg_addr,
    pop_arg_addr,
    set_arg_addr,
    ret_addr,
    print,
    local_get_c,
    local_get_i,
    local_get_s,
    local_get_d,
    get_c,
    get_i,
    get_s,
    get_d,
    define_global,
    define_local,
    set_global,
    get_global,
    set_local,
    get_local,
    store_ret_value,
    load_ret_value,
    ret,
    main_ret
};

constexpr char const *instructions[] = {
    "int_c",
    "char_c",
    "double_c",
    "string_c",
    "add",
    "sub",
    "mult",
    "idiv",
    "positive",
    "neg",
    "nil",
    "true_l",
    "false_l",
    "lt",
    "lte",
    "gt",
    "gte",
    "eq",
    "inot",
    "neq",
    "logical_and",
    "logical_or",
    "mod",
    "jit",
    "jif",
    "jump",
    "ipop",
    "ipush_bp",
    "ipop_bp",
    "push_arg_addr",
    "pop_arg_addr",
    "set_arg_addr",
    "ret_addr",
    "print",
    "local_get_c",
    "local_get_i",
    "local_get_s",
    "local_get_d",
    "get_c",
    "get_i",
    "get_s",
    "get_d",
    "define_global",
    "define_local",
    "set_global",
    "get_global",
    "set_local",
    "get_local",
    "store_ret_value",
    "load_ret_value",
    "ret",
    "main_ret"
};

enum ValueKind {
    Int_v,
    Char_v,
    Bool_v,
    Double_v,
    String_v,
    Nil_v
};

struct Fraction {
    double val;
    i8_t precision;
};

struct StringLiteral {
    char const *text;
    i32_t length;
};

char escape_character(char d);

struct Value {
    Value() = default;

    Value(nullptr_t val)
        : Value() { }

    Value(char val)
        : _kind{ Char_v }
    {
        _val.charcter = val;
    }

    Value(bool val)
        : _kind{ Bool_v }
    {
        _val.boolean = val;
    }

    Value(i64_t val)
        : _kind{ Int_v }
    {
        _val.integer = val;
    }

    Value(double val, i8_t precision = 6)
        : _kind{ Double_v }
    {
        _val.floats = {val, precision};
    }

    Value(Fraction val)
        : _kind{ Double_v }
    {
        _val.floats = val;
    }

    Value(StringLiteral val)
        : _kind{ String_v }
    {
        _val.strings = val;
    }

    Value(char const *text, i32_t length)
        : Value(StringLiteral{text, length}) { }

    Value &operator=(nullptr_t val) { _kind = Nil_v; _val.nil = nullptr; return *this; }
    Value &operator=(char val) { _kind = Char_v; _val.charcter = val; return *this; }
    Value &operator=(i64_t  val) { _kind = Int_v; _val.integer = val; return *this; }
    Value &operator=(Fraction val) { _kind = Double_v; _val.floats = val; return *this; }
    Value &operator=(double val) { _kind = Double_v; _val.floats = {val, 6}; return *this; }
    Value &operator=(bool val) { _kind = Bool_v; _val.boolean = val; return *this; }
    Value &operator=(StringLiteral val) { _kind = String_v; _val.strings = val; return *this; }

    bool is_nil() const { return _kind == Nil_v; }
    bool is_char() const { return _kind == Char_v; }
    bool is_int() const { return _kind == Int_v; }
    bool is_double() const { return _kind == Double_v; }
    bool is_bool() const { return _kind == Bool_v; }
    bool is_string() const { return _kind == String_v; }

    char as_char() { return _val.charcter; }
    i64_t as_int() { return _val.integer; }
    double as_double() { return _val.floats.val; }
    char const *as_cstring() { return _val.strings.text; }
    StringLiteral as_string() { return _val.strings; }
    bool as_bool() { 
        switch (_kind) {
            case Bool_v:
                return _val.boolean; 
            case Int_v:
                return (_val.integer ? true : false);
            case Double_v:
                return (_val.floats.val ? true : false);
            case Char_v:
                return (_val.charcter == '\0'? false : true);
            case String_v:
                return _val.strings.length > 0;
            case Nil_v:
                return false;
        }
    }

    static void escape_string(FILE *des, char const *str, i32_t length) {
        for (i32_t i = 0; i < length; ++i) {
            if (str[i] == '\\') {
                std::fputc(escape_character(str[i + 1]), des);
                ++i;
                continue;
            }
            std::fputc(str[i], des);
        }
    }

    void print(FILE *des = stdout, bool escape = true) {
        switch (_kind) {
            case Int_v:
                std::fprintf(des, "%lld", static_cast<long long>( _val.integer));
                break;
            case Char_v:
                std::fprintf(des, "%c", _val.charcter);
                break;
            case Double_v:
                std::fprintf(des, "%.*lF", _val.floats.precision, _val.floats.val);
                break;
            case Bool_v:
                std::fprintf(des, "%s", (_val.boolean ? "true" : "false"));
                break;
            case String_v:
                if (escape)
                    escape_string(des, _val.strings.text, _val.strings.length);
                else
                    std::fprintf(des, "%.*s", _val.strings.length, _val.strings.text);
                break;
            case Nil_v:
                std::fprintf(des, "nil");
                break;
        }
    }

    ValueKind _kind{ Nil_v };
    union Val {
        nullptr_t nil;
        char charcter;
        bool boolean;
        i64_t integer;
        Fraction floats;
        StringLiteral strings;
    } _val{ nullptr };
};


/* -------------- globals -------------- */
char *source = nullptr;
i32_t source_length = 0;
i32_t source_index = 0; 

bool show_opcodes = false;

char const *text = source; /* to store the current lexme */
i32_t text_len = 0;     /* current lexme's length */

i32_t line = 1;

Token cur_token = {Eof, line};

bool compile_error = false;
bool parse_error = false;
bool execution_error = false;

vector<u8_t> code;  /* this will be our vector of opcodes */
vector<u8_t>::iterator ip; /* our instruction pointer */

array<Value, INT16_MAX> stack;
auto sp = stack.begin();    /* stack pointer */
auto bp = stack.begin();    /* base pointer */
i32_t main_addr = -1;

vector<Value> values;
vector<i32_t> lines;

struct SourceCode {
    char const *text;
    i32_t length;
};

vector<SourceCode> sourcecode;
char const *cur_line = source;
i32_t cur_line_length = 0;

u8_t print_arguments = 0;


/*umap<string, Value> globals;*/
/*vector<string> globals_names;*/


struct GlobalSymbolTable {
    GlobalSymbolTable() = default;

    bool contains(StringLiteral literal, i32_t &index) {
        for (i32_t i = 0; i < i32_t(objects.size()); ++i) {
            if (objects[i].length == literal.length &&
                    std::strncmp(objects[i].text, literal.text, objects[i].length) == 0) {
                index = i;
                return true;
            }
        }
        return false;
    }

    bool contains(StringLiteral literal) {
        i32_t index;
        return contains(literal, index);
    }

    i32_t push(StringLiteral literal, Value val) {
        i32_t index = objects.size();;
        objects.push_back({literal.text, literal.length});
        vals.push_back(val);
        return index;
    }

    Value &operator[](i32_t index) {
        return vals.at(index);
    }
    
    vector<StringLiteral> objects;
    vector<Value> vals;
};

GlobalSymbolTable globals2;

struct Variable {
    char const *name;
    i32_t length;
    i32_t index;
    i32_t scope;
};

i32_t cur_scope_depth = 0;
i32_t cur_local_index = 0;
struct SymbolTable {
    SymbolTable() = default;

    void push(i32_t scope = cur_scope_depth, 
            char const *name = text, i32_t length = text_len) {
        i32_t index = cur_local_index++;
        variables.push_back(Variable{name, length, index, scope});
    }

    void pop() {
        variables.pop_back();
    }

    bool contains(char const *name, i32_t length, i32_t scope, i32_t &index) {
        for (auto var = variables.rbegin(); var != variables.rend(); ++var) {
            if (var->scope < cur_scope_depth)
                break;

            if (var->scope == scope && var->length == length 
                    && std::strncmp(var->name, name, length) == 0) {
                index = var->index;
                return true;
            }
        }

        push(cur_scope_depth, name, length);
        index = cur_local_index - 1;
        return false;
    }

    bool contains(char const *name, i32_t length, i32_t &index) {
        for (auto var = variables.rbegin(); var != variables.rend(); ++var) {
            if (var->length == length && std::strncmp(var->name, name, length) == 0) {
                index = var->index;
                return true;
            }
        }
        return false;
    }

    Variable &back() {
        return variables.back();
    }

    Variable &operator[](i32_t offset) {
        return variables.at(variables.size() - 1 - offset);
    }

    vector<Variable> variables;
};

SymbolTable locals;


struct Function {
    char const *name;
    i32_t length;
    i32_t address;
    i8_t arguments;
};

struct Functions {
    Functions() = default;

    bool defined(char const *name, i32_t length, i32_t &address, i8_t &arguments) {
        for (auto func = functions.begin(); func != functions.end(); ++func) {
            if (func->length == length &&
                    std::strncmp(func->name, name, length) == 0) {
                address = func->address;
                arguments = func->arguments;
                return true;
            }
        }

        return false;
    }

    bool defined(char const *name, i32_t length) {
        i32_t address;
        i8_t arguments;
        return defined(name, length, address, arguments);
    }

    bool declare(char const *name, i32_t length, i32_t address, i8_t arguments) {
        if (defined(name, length))
            return false;
        functions.push_back({name, length, address, arguments});
        return true;
    }

    vector<Function> functions;
};


Functions functions;
vector<u32_t> argument_indexes(INT16_MAX, 0);
Value function_return_value;
vector<i32_t> exit_addrs;
bool return_found = false;






/* -------------- helper functions ---------------- */





template <typename T, typename N>
inline T as_t(N as) {
    return static_cast<T>(as);
}

template <typename T, typename N>
inline T *as_ptr(N *as) {
    return static_cast<T*>(as);
}

void read_file(char const *argv) {
    std::ifstream file(argv, std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);

    auto fsize = as_t<long long>(file.tellg());
    file.seekg(0, std::ios::beg);

    source = new char[fsize + 1];
    file.read(source, fsize);
    source[fsize] = '\0';

    while (source[fsize - 1] == '\n')
        source[--fsize] = '\0';
    source_length = fsize;
    cur_line = source;
}

void save_all_lines() {
    for (i32_t i = 0; i <= source_length; ++i) {
        if (source[i] == '\n' || source[i] == '\0') {
            sourcecode.push_back({cur_line, cur_line_length});
            cur_line = cur_line + cur_line_length + 1;
            cur_line_length = 0;
            continue;
        }
        ++cur_line_length;
    }
}

i16_t get_double_byte_index(i32_t offset) {
    auto byte1 = code.at(offset);
    auto byte2 = code.at(offset + 1);

    return as_t<i16_t>(as_t<i16_t>(byte1 << 8) | as_t<i16_t>(byte2));
}

void single_byte_instruction(OpCode opcode) {
    std::fprintf(stderr,"%15s\n", instructions[opcode]);
}

void double_byte_instruction(OpCode opcode, i32_t offset) {    
    std::fprintf(stderr, "%15s\t%4d\n", instructions[opcode], code.at(offset));
}

void three_byte_instruction(OpCode opcode, i32_t &offset) {
    auto index = get_double_byte_index(offset);
    std::fprintf(stderr, "%15s\t%4d\t", instructions[opcode], index);
    values.at(index).print(stderr, false);
    std::fprintf(stderr, "\n");
    offset += 1;
}

void jump_true_false_instruction(OpCode opcode, i32_t &offset) {
    auto index = get_double_byte_index(offset);
    std::fprintf(stderr, "%15s\t%4d\t%15s\n", instructions[opcode], index, instructions[code.at(index)]);
    offset += 1;
}

void get_globals(OpCode opcode, i32_t &offset) {
    auto index = get_double_byte_index(offset);
    auto val = globals2.objects[index];
    std::fprintf(stderr, "%15s\t%4d\t%.*s\n", instructions[opcode], index, val.length, val.text);
    offset += 1;
}

void get_locals(OpCode opcode, i32_t &offset) {
    auto index = get_double_byte_index(offset);
    std::fprintf(stderr, "%15s\t%4d\n", instructions[opcode], index);
    offset += 1;
}

void disassemble_instruction(i32_t &offset) {
    std::fprintf(stderr, "%04d\t%4d\t", offset, lines.at(offset));
    switch (code.at(offset)) {
        case int_c:
            three_byte_instruction(int_c, ++offset);
            break;
        case char_c:
            three_byte_instruction(char_c, ++offset);
            break;
        case double_c:
            three_byte_instruction(double_c, ++offset);
            break;
        case string_c:
            three_byte_instruction(string_c, ++offset);
            break;
        case add:
            single_byte_instruction(add);
            break;
        case sub:
            single_byte_instruction(sub);
            break;
        case mult:
            single_byte_instruction(mult);
            break;
        case idiv:
            single_byte_instruction(idiv);
            break;
        case positive:
            single_byte_instruction(positive);
            break;
        case neg:
            single_byte_instruction(neg);
            break;
        case nil:
            single_byte_instruction(nil);
            break;
        case true_l:
            single_byte_instruction(true_l);
            break;
        case false_l:
            single_byte_instruction(false_l);
            break;
        case lt:
            single_byte_instruction(lt);
            break;
        case lte:
            single_byte_instruction(lte);
            break;
        case gt:
            single_byte_instruction(gt);
            break;
        case gte:
            single_byte_instruction(gte);
            break;
        case eq:
            single_byte_instruction(eq);
            break;
        case inot:
            single_byte_instruction(inot);
            break;
        case neq:
            single_byte_instruction(neq);
            break;
        case logical_and:
            single_byte_instruction(logical_and);
            break;
        case logical_or:
            single_byte_instruction(logical_or);
            break;
        case mod:
            single_byte_instruction(mod);
            break;
        case jit:
            jump_true_false_instruction(jit, ++offset);
            break;
        case jif:
            jump_true_false_instruction(jif, ++offset);
            break;
        case jump:
            jump_true_false_instruction(jump, ++offset);
            break;
        case ipop:
            single_byte_instruction(ipop);
            break;
        case ipush_bp:
            single_byte_instruction(ipush_bp);
            break;
        case ipop_bp:
            single_byte_instruction(ipop_bp);
            break;
        case ret_addr:
            jump_true_false_instruction(ret_addr, ++offset);
            break;
        case push_arg_addr:
            std::fprintf(stderr,"%15s\t%4d\n", instructions[push_arg_addr],
                    argument_indexes.at(get_double_byte_index(++offset)));
            offset += 1;
            break;
        case pop_arg_addr:
            std::fprintf(stderr,"%15s\t%4d\n", instructions[pop_arg_addr], get_double_byte_index(++offset));
            offset += 1;
            break;
        case set_arg_addr:
            std::fprintf(stderr,"%15s\t%4d\n", instructions[set_arg_addr], get_double_byte_index(++offset));
            offset += 1;
            break;
        case print:
            double_byte_instruction(print, ++offset);
            break;
        case get_c:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[get_c], get_double_byte_index(++offset));
            offset += 1;
            break;
        case get_i:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[get_i], get_double_byte_index(++offset));
            offset += 1;
            break;
        case get_s:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[get_s], get_double_byte_index(++offset));
            offset += 1;
            break;
        case get_d:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[get_d], get_double_byte_index(++offset));
            offset += 1;
            break;
        case local_get_c:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[local_get_c], get_double_byte_index(++offset));
            offset += 1;
            break;
        case local_get_i:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[local_get_i], get_double_byte_index(++offset));
            offset += 1;
            break;
        case local_get_s:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[local_get_s], get_double_byte_index(++offset));
            offset += 1;
            break;
        case local_get_d:
            std::fprintf(stderr, "%15s\t%4d\n", instructions[local_get_d], get_double_byte_index(++offset));
            offset += 1;
            break;
        case define_global:
            get_globals(define_global, ++offset);
            break;
        case get_global:
            get_globals(get_global, ++offset);
            break;
        case set_global:
            get_globals(set_global, ++offset);
            break;
        case define_local:
            get_locals(define_local, ++offset);
            break;
        case get_local:
            get_locals(get_local, ++offset);
            break;
        case set_local:
            get_locals(set_local, ++offset);
            break;
        case store_ret_value:
            single_byte_instruction(store_ret_value);
            break;
        case load_ret_value:
            single_byte_instruction(load_ret_value);
            break;
        case ret:
            single_byte_instruction(ret);
            break;
        case main_ret:
            single_byte_instruction(main_ret);
            break;
    }
}

void disassemble_code(char const *part) {
    std::fprintf(stderr, "======== %s =========\n", part);

    for (i32_t offset = 0; offset < as_t<i32_t>(code.size()); ++offset)
        disassemble_instruction(offset);
}

void emit_single_byte(u8_t byte, i32_t _line = cur_token.line) {
    code.push_back(byte);
    lines.push_back(_line);
}

void emit_double_byte(u8_t byte1, u8_t byte2, i32_t _line = cur_token.line) {
    emit_single_byte(byte1);
    emit_single_byte(byte2);
}

void emit_value(OpCode op, Value val, i32_t _line = cur_token.line) {
    emit_single_byte(op, _line);
    values.push_back(val);

    auto index = as_t<u16_t>(values.size() - 1);
    emit_single_byte(as_t<u8_t>(index >> 8), _line);
    emit_single_byte(as_t<u8_t>(index), _line);
}

void emit_three_bytes(OpCode op, i16_t index, i32_t _line = cur_token.line) {
    emit_single_byte(op, _line);
    emit_single_byte(as_t<u8_t>(index >> 8), _line);
    emit_single_byte(as_t<u8_t>(index), _line);
}

void emit_jump(OpCode op, i32_t _line = cur_token.line) {
    emit_single_byte(op, _line);
    emit_single_byte(0xff, _line);
    emit_single_byte(0xff, _line);
}

void set_correct_code_address(i16_t index, i32_t offset) {
    code.at(offset - 2) &= as_t<u8_t>(index >> 8);
    code.at(offset - 1) &= as_t<u8_t>(index);
}

i64_t to_i64(char const *text, i32_t length) {
    i64_t ret = 0;
    for (i32_t i = 0; i < length; ++i) {
        ret = ret * 10 + (text[i] - '0');
    }

    return ret;
}

Fraction to_double(char const *text, i32_t length) {
    double ret = 0;
    i8_t fraction_count = 0;
    bool flag = false;
    for (i32_t i = 0; i < length; ++i) {
        if (text[i] == '.') {
            flag = true;
            continue;
        }

        if (fraction_count >= 10)
            break;

        if (flag)
            ++fraction_count;
        ret = ret * 10 + (text[i] - '0');
    }

    while (fraction_count--)
        ret /= 10;

    return {ret, fraction_count};
}

#ifdef __linux
#define BOLD_RED "\x1b[1;31m"
#define BOLD_GREEN "\x1b[1;32m"
#define BOLD_PURBLE "\x1b[1;35m"
#define NORMAL "\x1b[m"
#endif

void print_error_line(int offset, char const *_text = text) {
    auto &error_line = sourcecode.at(offset);
    auto len = _text - error_line.text;
    std::fprintf(stderr, BOLD_GREEN "\t%4d" NORMAL "| ", offset + 1);
    for (i32_t i = 0; i < error_line.length; ++i) {
        if (i == len)
            std::fprintf(stderr, BOLD_RED);
        if (error_line.text[i] == '\0')
            std::fprintf(stderr, "(eof)");
        else
            std::fputc(error_line.text[i], stderr);
        std::fprintf(stderr, NORMAL);
    }
    std::fprintf(stderr, "\n\t");
    for (i32_t i = 0; i < len; ++i)
        std::fputc(' ', stderr);
    std::fprintf(stderr, "      ^\n\n");
}

void error_header(i32_t line) {
    std::fprintf(stderr, "[line:%d] " BOLD_RED "error" NORMAL ": ", line);
}

void erroneous_token(char const *tok, i32_t length) {
    if (source_index >= source_length) {
        std::fprintf(stderr, "'" BOLD_RED "%.*s'eof'" NORMAL "'\n", length, tok);
        return;
    }

    if (length == 0)
        length = 1;
    if (*tok == '\0') {
        tok = "(eof)";
        length = 5;
    }
    bool flag = false;
    if ((flag = length > 10))
        length = 10;
    std::fprintf(stderr, "'" BOLD_RED); 
    for (i32_t i = 0; i < length; ++i) {
        switch (tok[i]) {
            case '\n':
                std::fprintf(stderr, "\\n");
                break;
            case '\t':
                std::fprintf(stderr, "\\t");
                break;
            default:
                std::fputc(tok[i], stderr);
                break;
        }
    }
    std::fprintf(stderr, "%s" NORMAL "'\n", (flag ? "...": ""));
}



/* ------------------- functions that will be used throgh out the program ----------------- */

/* lexer start */

inline bool is_eof() {
    return source_index >= source_length; 
}

char eat_c() {
    if (is_eof())
        return '\0';

    ++text_len;
    return source[source_index++];
}

char peek_c() {
    if (is_eof())
        return '\0';
    return source[source_index];
}

char peek_next_c() {
    if (is_eof())
        return '\0';
    return source[source_index + 1];
}


void error_token(char const *message, char const *_text = text, i32_t _line = line) {
    compile_error = true;
    error_header(line);
    std::fprintf(stderr, "%s: ", message);
    erroneous_token(_text, text_len);
    print_error_line(_line - 1, _text);
}

void unterminated_string(char const *text, i32_t _line = line) {
    error_token("unterminated string", text, _line);
    std::fprintf(stderr, BOLD_PURBLE "NOTE" NORMAL ": expected '" BOLD_GREEN "\"" NORMAL 
                        "' at the end of the string\n\n");
}

void unterminated_print_argument() {
    error_token("unterminated print argument");
    std::fprintf(stderr, BOLD_PURBLE "NOTE" NORMAL ": expected '" BOLD_GREEN "}" NORMAL 
                        "' at the end of expression\n\n");
}

void empty_print_argument(char const *text, i32_t _line = line) {
    error_token("empty print argument", text, _line);
    std::fprintf(stderr, BOLD_PURBLE "NOTE" NORMAL ": expected expression after '{'\n\n");
}


void skip_whitespace(bool save_line = true) {
    while (true) {
        switch (peek_c()) {
            case '\n':
                ++line;
            case ' ':
            case '\t':
            case '\v':
                break;
            case '/':
                if (peek_next_c() == '/') {
                    while (peek_c() != '\n' && !is_eof())
                        eat_c();
                    continue;
                }
                return;
            default:
                return;
        }
        eat_c();
    }
}

TokenKind number_token() {
    while (std::isdigit(peek_c()) && !is_eof()) {
        eat_c();
    }

    if (peek_c() == '.') {
        eat_c();
        while (std::isdigit(peek_c()) && !is_eof()) {
            eat_c();
        }

        cur_token._kind = Double;
        cur_token.line = line;
        return Double;
    }

    cur_token._kind = Integer;
    cur_token.line = line;
    return Integer;
}

bool is_escape_char(char c) {
    return !(c != 'a' && c != 'b' && c != 'n' && c != 'r' && c != 't' && c != '\\' && c != '\'' && c != '"');
}

TokenKind char_token(bool print_error = true) {
    TokenKind kind = Character;
    if (peek_c() == '\\') {
        eat_c();

        if (!is_escape_char(peek_c())) {
            auto save_text = text + text_len;
            auto save_line = line;
            auto save_text_len = text_len;
            text_len -= 2;
            eat_c();
            if (print_error)
                error_token("unrecognized escape secuence", save_text, save_line);
            text_len = save_text_len;
            kind = Error;
        } else {
            eat_c();
        }
    } else {
        eat_c();
    }

    if (peek_c() != '\'') {
        skip_whitespace();
        eat_c();
        if (print_error)
            error_token("multibyte character");
        
        if (!is_eof()) {
            while (peek_c() != '\'' && !is_eof()) {
                if (eat_c() == '\n')
                    ++line;
            }
        }

        kind = Error;
        cur_token._kind = kind;
        cur_token.line = line;
        return kind;
    }
    
    eat_c(); /* eat the last single quote */

    cur_token._kind = kind;
    cur_token.line = line;
    return kind;
}

TokenKind string_token(bool print_error = true) {
    auto save_text = text;
    auto save_line = line;
    TokenKind kind = String;
    while (peek_c() != '"' && !is_eof()) {
        if (peek_c() == '\n') {
            auto save_text2 = text + text_len - 1;
            auto save_line2 = line;
            ++line;
            eat_c();
            if (print_error) {
                error_token("expected expression", save_text2, save_line2);
                unterminated_string(save_text, save_line);
            }
            
            cur_token._kind = Error;
            cur_token.line = line;
            return Error;
        }

        if (peek_c() == '{') {
            auto save_text_len = text_len;
            eat_c();
            auto save_text = text;
            auto save_line = line;
            skip_whitespace();
            if (peek_c() == '}') {
                if (print_error)
                    empty_print_argument(save_text + save_text_len, save_line);
                eat_c();
                continue;
            }

            while (peek_c() != '}' && !is_eof()) {
                if (eat_c() == '\n')
                    ++line;
            }
            if (is_eof()) {
                if (print_error) {
                    text = text + save_text_len;
                    text_len -= save_text_len;
                    
                    unterminated_print_argument();

                    text = text - save_text_len;
                    text_len += save_text_len;
                }
                cur_token._kind = Error;
                cur_token.line = line;
                return Error;
            }
        }

        if (peek_c() == '\\') {
            eat_c();
            char c = peek_c();
            if (!is_escape_char(c) && c != '{') {
                eat_c();
                auto save_text_len = text_len;
                text_len = 1;
                if (print_error)
                    error_token("unrecognized escape secuence", text + save_text_len - 1);
                text_len = save_text_len;
                kind = Error;
                continue;
            }
        }
        eat_c();
    }

    if (peek_c() == '\0' && print_error) {
        if (print_error) {
            unterminated_string(save_text, save_line);
        }
        cur_token._kind = Error;
        cur_token.line = line;
        return Error;
    }

    eat_c();
    cur_token._kind = kind;
    cur_token.line = line;
    return String;
}

TokenKind identifier_token() {
    while (std::isalnum(peek_c()) || peek_c() == '_' && !is_eof())
        eat_c();
    TokenKind kind = Identifier;
    switch (*text) {
        case 'e':
            if (text_len == 4 && std::strncmp(text+1, "lse", 3) == 0)
                kind = Else;
            else if (text_len == 4 && std::strncmp(text+1, "lif", 3) == 0)
                kind = Elif;
            break;
        case 'n':
            if (text_len == 3 && std::strncmp(text+1, "il", 2) == 0)
                kind = Nil;
            break;
        case 't':
            if (text_len == 4 && std::strncmp(text+1, "rue", 3) == 0)
                kind = True;
            break;
        case 'f':
            if (text_len == 4 && std::strncmp(text+1, "unc", 3) == 0)
                kind = Func;
            else if (text_len == 5 && std::strncmp(text+1, "alse", 4) == 0)
                kind = False;
            else if (text_len == 3 && std::strncmp(text+1, "or", 2) == 0)
                kind = For;
            break;
        case 'p':
            if (text_len == 5 && std::strncmp(text+1, "rint", 4) == 0)
                kind = Print;
            break;
        case 'v':
            if (text_len == 3 && std::strncmp(text+1, "ar", 2) == 0)
                kind = Var;
            break;
        case 'i':
            if (text_len == 2 && *(text+1) == 'f')
                kind = If;
            else if (text_len == 5 && std::strncmp(text+1, "nput", 4) == 0)
                kind = Input;
            break;
        case 'w':
            if (text_len == 5 && std::strncmp(text+1, "hile", 4) == 0)
                kind = While;
            break;
        case 'r':
            if (text_len == 6 && std::strncmp(text+1, "eturn", 5) == 0)
                kind = Return;
            break;
        case 'g':
            if (text_len == 4 && std::strncmp(text+1, "etc", 3) == 0)
                kind = Get_C;
            else if (text_len == 4 && std::strncmp(text+1, "eti", 3) == 0)
                kind = Get_I;
            else if (text_len == 4 && std::strncmp(text+1, "ets", 3) == 0)
                kind = Get_S;
            else if (text_len == 4 && std::strncmp(text+1, "etd", 3) == 0)
                kind = Get_D;
            break;
        default:
            break;
    }

    if (kind == Identifier && peek_c() == '(')
        kind = FuncIdentifier;
    
    cur_token._kind = kind;
    cur_token.line = line;
    return kind;
}

TokenKind gettoken(bool save_line = true) {
    skip_whitespace(save_line); 
    text = source + source_index;
    text_len = 0;
    
    auto c = eat_c();
    TokenKind kind;
    switch (c) {
        case '+': kind = Plus; break;
        case '-': kind = Minus; break;
        case '*': kind = Star; break;
        case '/': kind = Slash; break;
        case '(': kind = LeftParen; break;
        case ')': kind = RightParen; break;
        case ';': kind = Semicolon; break;
        case '{': kind = LeftBrace; break;
        case '}': kind = RightBrace; break;
        case '"': return string_token(save_line);
        case ',': kind = Comma; break;
        case '%': kind = Modulus; break;
        case '<':
            if (peek_c() == '=') {
                eat_c();
                kind = LessEqual;
            } else {
                kind = LessThan;
            }
            break;
        case '>':
            if (peek_c() == '=') {
                eat_c();
                kind = GreaterEqual;
            } else {
                kind = GreaterThan;
            }
            break;
        case '!':
            if (peek_c() == '=') {
                eat_c();
                kind = NotEqual;
            }
            else
                kind = Bang;
            break;
        case '=':
            if (peek_c() == '=') {
                eat_c();
                kind = EqualEqual;
            }
            else
                kind = Equal;
            break;
        case '&':
            if (peek_c() == '&') {
                eat_c();
                kind = LogicalAnd;
            } else {
                kind = Error;
            }
            break;
        case '|':
            if (peek_c() == '|') {
                eat_c();
                kind = LogicalOr;
            } else {
                kind = Error;
            }
            break;
        case '\0': 
          kind =  Eof; 
          break;
        case '\'': return char_token(save_line);
        default:
           if (std::isdigit(c)) {
               return number_token();
           } else if (std::isalnum(c) || c == '_') {
               return identifier_token();
           }
           kind = Unrecognized;
           if (save_line)
               error_token("unrecognised token");
           break;
    }

    cur_token._kind = kind;
    cur_token.line = line;
    return kind;
}

TokenKind peek_token(bool save_cur = true, int count = 0) {
    if (is_eof())
        return Eof;
    auto save_text = text;
    auto save_source_index = source_index;
    auto save_text_len = text_len;
    auto save_line = line;
    Token save_token = cur_token;
    TokenKind ret;
    
    for ( ; count >= 0; --count)
        ret = gettoken(false);

    source_index = save_source_index;
    if (save_cur) {
        text = save_text;
        text_len = save_text_len;
        cur_token = save_token;
    }
    line = save_line;

    return ret;
}

bool match_token(TokenKind kind) {
    if (peek_token() == kind) {
        return gettoken() == kind;
    }
    return false;
}

/* lexer end */

/* parser start */

void unexpected_token(char const *expected, Token &tok) {
    parse_error = true;
    error_header(tok.line);
    std::fprintf(stderr, "expected '" BOLD_GREEN "%s" NORMAL "', found ", expected);
    erroneous_token(text, text_len);
    print_error_line(tok.line - 1);
}

void expected_expression(Token &tok) {
    parse_error = true;
    error_header(tok.line);
    std::fprintf(stderr, "expected expression, found ");
    erroneous_token(text, text_len);
    print_error_line(tok.line - 1);
}

void redefining_variable(char const *text, i32_t length, i32_t _line = line) {
    parse_error = true;
    error_header(_line);
    std::fprintf(stderr, "redefining variable in the same scope ");
    erroneous_token(text, length);
    print_error_line(_line, text);
}

void redefining_function(char const *text, i32_t length, i32_t _line = line) {
    parse_error = true;
    error_header(_line);
    std::fprintf(stderr, "redefining function ");
    erroneous_token(text, length);
    print_error_line(_line, text);
}

void undefined_reference(char const *_text = text, i32_t _length = text_len, i32_t _line = line) {
    parse_error = true;
    error_header(_line);
    std::fprintf(stderr, "undefined reference to ");
    erroneous_token(_text, _length);
    print_error_line(_line - 1);
}

void consume(TokenKind kind) {
    auto ret = gettoken() == kind;
    if (!ret) {
        unexpected_token(tokens[kind], cur_token);
        return;
    }
}

void synchronize() {
    while (true) {
        auto tok = peek_token(false);
        switch (tok) {
            case Eof:
            case Func:
                return;
            default:
                break;
        }
        gettoken(false);
    }
}

i8_t unary_operator_precedence(TokenKind kind) {
    switch (kind) {
        case Plus:
        case Minus:
        case Bang:
            return 7;
        case Integer:
        case Double:
        case Character:
        case Identifier:
        case FuncIdentifier:
        case String:
        case True:
        case False:
        case Nil:
        case LeftParen:
        case Eof:
        case Semicolon:
        case Comma:
            return 0;
        default:
            return -1;
    }
}

i8_t binary_operator_precedence(TokenKind kind) {
    switch (kind) {
        case Star:
        case Slash:
        case Modulus:
            return 6;
        case Plus:
        case Minus:
            return 5;
        case LessThan:
        case LessEqual:
        case GreaterThan:
        case GreaterEqual:
            return 4;
        case EqualEqual:
        case NotEqual:
            return 3;
        case LogicalAnd:
            return 2;
        case LogicalOr:
            return 1;
        case Eof:
        case RightParen:
        case RightBrace:
        case Semicolon:
        case Comma:
            return 0;
        default:
            return -1;
    }
}

void parse_assignment(i8_t parentPrecedence = 0);
void parse_expression(i8_t parentPrecedence = 0);

char escape_character(char d) {
    switch (d) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'v': return '\v';
        case 't': return '\t';
        case '\\': return '\\';
        case '\'': return '\'';
        case 'n': return '\n';
        case '"': return '"';
        case 'r': return '\r';
        case 'f': return '\f';
        case '0': return '\0';
    }

    error_token("invalid escape sequence");
    return '\0';
}

i16_t index_of(char const *text, i32_t length, bool &is_global) {
    i32_t index;
    is_global = false;
    if (locals.contains(text, length, index)) {
        return index;
    }

    if (!globals2.contains({text, length}, index))
        return -1;
    
    is_global = true;
    return index;
}


void function_call() {
    i32_t address;
    i8_t arguments;
    if (!functions.defined(text, text_len, address, arguments)) {
        undefined_reference();
        TokenKind tok;
        while ((tok = peek_token()) != RightParen && tok != Eof)
            gettoken();
        if (tok == RightParen)
            gettoken();
        return;
    }

    auto func_name = text;
    auto func_name_len = text_len;
    auto save_line = line;

    gettoken(); /* eat the '(' */
    auto tok = peek_token();
    
    i8_t argument_count = 0;
    while (tok != RightParen && tok != Eof) {
        parse_expression();
        ++argument_count;
        tok = peek_token();
        if (tok == Comma) {
            gettoken();
            tok = peek_token();
        }
    }
    
    consume(RightParen);

    if (argument_count != arguments) {
        parse_error = true;
        error_header(save_line);
        std::fprintf(stderr, "undefined reference to function: ");
        erroneous_token(func_name, func_name_len);
        print_error_line(save_line - 1, func_name);
        std::fprintf(stderr, BOLD_PURBLE "NOTE" NORMAL ": function '" BOLD_GREEN "%.*s" NORMAL "' expects %d arguments\n\n", func_name_len,
                func_name, arguments);
        TokenKind tok;
        while ((tok = peek_token()) != RightParen && tok != Eof)
            gettoken(false);
        if (tok == RightParen)
            gettoken(false);

        return;
    }

    emit_jump(ret_addr);
    auto return_addr = code.size();
    emit_three_bytes(jump, as_t<i16_t>(address));
    set_correct_code_address(code.size(), return_addr);

    for (i8_t i = 0; i < arguments; ++i)
        emit_single_byte(ipop);
    emit_single_byte(load_ret_value);
}

void parse_primary_expression() {
    switch (gettoken()) {
        case Integer:
            emit_value(int_c, to_i64(text, text_len));
            break;
        case Character:
            {
                char c = text[1];
                if (c == '\\')
                    c = escape_character(text[2]);
                emit_value(char_c, c);
            }
            break;
        case Double:
            emit_value(double_c, to_double(text, text_len));
            break;
        case String:
            emit_value(string_c, Value(text+1, text_len - 2));
            break;
        case LeftParen:
            parse_assignment();
            consume(RightParen);
            break;
        case Identifier:
            {
                bool is_global = false;
                auto index = index_of(text, text_len, is_global);
                if (index == -1) {
                    undefined_reference();
                    return;
                }

                OpCode op = (is_global) ? get_global : get_local;
                emit_three_bytes(op, index);
            }
            break;
        case Nil:
            emit_single_byte(nil);
            break;
        case True:
            emit_single_byte(true_l);
            break;
        case False:
            emit_single_byte(false_l);
            break;
        case FuncIdentifier:
            function_call();
            break;
        case Eof:
            return;
        case Semicolon:
            return;
        case Comma:
            return;
        default:
            break;
    }
}

void unary_expression(i8_t parentPrecedence) {
    gettoken();
    auto op = cur_token;
    parse_expression(parentPrecedence);
    switch (op._kind) {
        case Plus:
            emit_single_byte(positive, op.line);
            break;
        case Minus:
            emit_single_byte(neg, op.line);
            break;
        case Bang:
            emit_single_byte(inot, op.line);
            break;
        default:
            break;
    }
}

void binary_expression(i8_t parentPrecedence) {
    gettoken();
    auto op = cur_token;
    parse_expression(parentPrecedence);
    switch (op._kind) {
        case Plus:
            emit_single_byte(add, op.line);
            break;
        case Minus:
            emit_single_byte(sub, op.line);
            break;
        case Star:
            emit_single_byte(mult, op.line);
            break;
        case Slash:
            emit_single_byte(idiv, op.line);
            break;
        case Modulus:
            emit_single_byte(mod, op.line);
            break;
        case LessThan:
            emit_single_byte(lt, op.line);
            break;
        case LessEqual:
            emit_single_byte(lte, op.line);
            break;
        case GreaterThan:
            emit_single_byte(gt, op.line);
            break;
        case GreaterEqual:
            emit_single_byte(gte, op.line);
            break;
        case EqualEqual:
            emit_single_byte(eq, op.line);
            break;
        case NotEqual:
            emit_single_byte(neq, op.line);
            break;
        case LogicalAnd:
            emit_single_byte(logical_and, op.line);
            break;
        case LogicalOr:
            emit_single_byte(logical_or, op.line);
            break;
        default:
            break;
    }   
}

void parse_expression(i8_t parentPrecedence) {
    auto tok = peek_token();
    auto precedence = unary_operator_precedence(tok);
    if (precedence == -1) {
        if ((tok = gettoken()) == Unrecognized || tok == Error)
            return;
        unexpected_token("; or expression", cur_token);
        return;
    } else if (!precedence || precedence < parentPrecedence) {
        parse_primary_expression();
    } else {
        unary_expression(precedence);
    }

    while (true) {
        tok = peek_token();
        auto precedence = binary_operator_precedence(tok);

        if (precedence == -1) {
            if ((tok = gettoken()) == Unrecognized || tok == Error)
                return;
            unexpected_token("; or expression", cur_token);
            return;
        } else if (!precedence || precedence <= parentPrecedence) {
            break;
        }

        if (tok == LogicalAnd) {
            emit_jump(jif);
            auto prev_index = code.size();
            binary_expression(precedence);
            set_correct_code_address(code.size(), prev_index);
        } else if (tok == LogicalOr) {
            emit_jump(jit);
            auto prev_index = code.size();
            binary_expression(precedence);
            set_correct_code_address(code.size(), prev_index);
        } else {
            binary_expression(precedence);
        }
    }
}

void parse_assignment(i8_t parentPrecedence) {
    auto tok1 = peek_token();
    auto tok2 = peek_token(true, 1);
    if (tok1 == Identifier && tok2 == Equal) {
        gettoken();
        auto identifier = text;
        auto identifier_len = text_len;
        auto save_line = line;
        gettoken();
        parse_assignment(parentPrecedence);

        bool is_global = false;
        auto index = index_of(identifier, identifier_len, is_global);
        if (index == -1) {
            undefined_reference(identifier, identifier_len, save_line);
            while ((tok1 = peek_token()) != Semicolon && tok1 != RightParen && tok1 != RightBrace && 
                    tok1 != Comma && tok1 != Eof)
                gettoken(false);
            gettoken(false);
            return;
        }

        OpCode op = (is_global ? set_global : set_local);
        emit_three_bytes(op, index, save_line);
    } else {
        parse_expression(parentPrecedence);
    }
}

void parse_print_arguments() {
    skip_whitespace();

    if (peek_c() != '"') {
        gettoken();
        unexpected_token("\"", cur_token);
        return;
    }

    text = source + source_index;
    text_len = 0;
    auto save_source_index = source_index;
    eat_c(); // eat '"'
    string_token();     // parse the whole string
    if (compile_error)
        return;
    source_index = save_source_index;

    if (text_len > 2) {
        i32_t prev_index = 0;
        i32_t i = 1;
        ++source_index;
        for ( ; i < text_len - 1; ) {
            if (text[i] != '}' && text[i] != '{')
                ++prev_index;
            if (text[i] == '{') {
                ++source_index;
                if (i > 1 && text[i - 1] != '}') {
                    emit_value(string_c, StringLiteral{text + i - prev_index, prev_index});
                    ++print_arguments;
                    prev_index = 0;
                }

                auto save_text = text;
                auto save_text_len = text_len;

                /* changing text and text_len so that errors are printed correctly
                 * if occurs */
                text = text + i + 1;
                text_len -= i + 1;

                /* parse th expression in the print argument */
                /* print("hello {"world"}\n"); */
                /* parse_assignment will parse the '"world"' expression */
                parse_assignment();

                if (parse_error || compile_error) {
                    auto tok = peek_token();
                    while (tok != Eof && tok != Semicolon) {
                        gettoken(false);
                        tok = peek_token();
                    }
                    gettoken(false);
                    return;
                }
                
                text = save_text;
                text_len = save_text_len;
                
                ++print_arguments;

                while (text[i] != '}' && i < text_len - 1 && text[i] != '\0')
                    ++i;
            }
            if (source_index < source_length)
                ++source_index;
            ++i;
        }

        /* if prev_index is not zero, that means their is a string left */
        if (prev_index) {
            ++print_arguments;
            emit_value(string_c, StringLiteral{text + i - prev_index, prev_index});
        }
        if (source_index < source_length)
            ++source_index;
    } else {
        emit_value(string_c, StringLiteral{"", 0});
        ++print_arguments;
        source_index += 2;
    }
}

void parse_print_statement() {
    gettoken();
    auto tok = cur_token;
    consume(LeftParen);

    if (peek_token(false) == RightParen) {
        expected_expression(cur_token);
    } else {
        parse_print_arguments();
    }
    consume(RightParen);
    consume(Semicolon);
    emit_double_byte(print, print_arguments, tok.line);
    print_arguments = 0;
}

void parse_expression_statement() {
    if (peek_token() == Semicolon) {
        gettoken();
        return;
    }
    parse_assignment();
    consume(Semicolon);
    emit_single_byte(ipop);
}

void start_new_scope() {
    cur_local_index = 0;
    ++cur_scope_depth;
}

void end_new_scope() {
    while (locals.variables.size() > 0 && locals.back().scope == cur_scope_depth) {
        emit_single_byte(ipop);
        locals.pop();
    }
    --cur_scope_depth;
}

void parse_declaration(TokenKind kind);

void parse_block_statement() {
    start_new_scope();
    gettoken(); /* eat '{' */

    auto tok = peek_token();
    while (tok != RightBrace && tok != Eof) {
        parse_declaration(tok);
        tok = peek_token();
    }
    consume(RightBrace);
    end_new_scope();
}

void parse_if_statement() {
    gettoken();
    consume(LeftParen);
    parse_expression();
    consume(RightParen);
    
    emit_jump(jif);
    auto prev_index = code.size();
    emit_single_byte(ipop);

    if (peek_token() != LeftBrace) {
        gettoken();
        unexpected_token("{", cur_token);
        return;
    }

    parse_block_statement();
    emit_jump(jump);
    auto prev_index2 = code.size();
    set_correct_code_address(code.size(), prev_index);
    emit_single_byte(ipop);

    auto tok = peek_token();
    if (tok == Elif) {
        parse_if_statement();
    } 
    
    tok = peek_token();
    if (tok == Else) {
        gettoken();
        if (peek_token() != LeftBrace) {
            gettoken();
            unexpected_token("{", cur_token);
            return;
        }
        parse_block_statement();
    }
    set_correct_code_address(code.size(), prev_index2);
}

void parse_while_statement() {
    gettoken();
    consume(LeftParen);
    auto loop_start = code.size();
    parse_expression();
    consume(RightParen);

    emit_jump(jif);
    auto exit_loop = code.size();
    emit_single_byte(ipop);

    if (peek_token() != LeftBrace) {
        gettoken();
        unexpected_token("{", cur_token);
        return;
    }
    
    parse_block_statement();
    emit_three_bytes(jump, as_t<u16_t>(loop_start));
    set_correct_code_address(code.size(), exit_loop);
    emit_single_byte(ipop);
}

void parse_declaration(TokenKind kind);
void parse_variable_declaration(bool consume_semicolon = true);

/* inefficient for loop, takes more time to execute than while loop */
void parse_for_statement() {
    start_new_scope();
    gettoken();
    consume(LeftParen);
    
    if (peek_token() == Var) {
        parse_variable_declaration();
    } else {
        consume(Semicolon);
    }

    auto loop_start = code.size();
    bool has_expression = false;
    i32_t exit_loop;
    if (peek_token() != Semicolon) {
        parse_expression();
        has_expression = true;
        emit_jump(jif);
        exit_loop = code.size();
        emit_single_byte(ipop);
    }
    consume(Semicolon);
    emit_jump(jump);
    auto body = code.size();

    if (peek_token() != RightParen) {
        auto check_expression = loop_start;
        loop_start = code.size();
        parse_assignment();
        emit_single_byte(ipop);
        emit_three_bytes(jump, as_t<i16_t>(check_expression));
    }
    consume(RightParen);

    if (peek_token() != LeftBrace) {
        gettoken();
        unexpected_token("{", cur_token);
        return;
    }

    set_correct_code_address(code.size(), body);
    parse_block_statement();

    emit_three_bytes(jump, as_t<u16_t>(loop_start));
    if (has_expression) {
        set_correct_code_address(code.size(), exit_loop);
        emit_single_byte(ipop);
    }
    end_new_scope();
}

/* more efficient for loop */
void parse_for_loop_effeciently() {
    start_new_scope();
    gettoken();
    consume(LeftParen);

    auto tok = peek_token();
    if (tok != Semicolon) {
        parse_declaration(tok);
    } else {
        consume(Semicolon);
    }

    auto loop_start = code.size();
    bool has_expression = false;
    i32_t exit_loop;
    if (peek_token() != Semicolon) {
        parse_expression();
        has_expression = true;
        emit_jump(jif);
        exit_loop = code.size();
        emit_single_byte(ipop);
    }

    consume(Semicolon);
    auto save_source_index = source_index;
    bool has_increment = false;
    tok = peek_token();
    if (tok != RightParen) {
        has_increment = true;
        while (tok != RightParen) {
            gettoken(false);
            tok = peek_token();
        }
    }

    consume(RightParen);
    if (peek_token() != LeftBrace) {
        gettoken();
        unexpected_token("{", cur_token);
        return;
    }
    
    parse_block_statement();
    if (has_increment) {
        auto save_source_index2 = source_index;
        source_index = save_source_index;
        parse_assignment();
        emit_single_byte(ipop);
        source_index = save_source_index2;
    }
    emit_three_bytes(jump, as_t<u16_t>(loop_start));
    if (has_expression) {
        set_correct_code_address(code.size(), exit_loop);
        emit_single_byte(ipop);
    }
    end_new_scope();
}

void parse_return_statement() {
    return_found = true;
    gettoken();
    if (peek_token() != Semicolon) {
        parse_assignment();
    } else {
        emit_value(int_c, as_t<i64_t>(0));
    }
    consume(Semicolon);
    emit_single_byte(store_ret_value);
    emit_jump(jump);
    exit_addrs.push_back(code.size());
}

void parse_input_statement(OpCode op1, OpCode op2) {
    gettoken();
    consume(LeftParen);
    consume(Identifier);

    auto ident_name = text;
    auto ident_len = text_len;
    auto save_line = line;
    consume(RightParen);
    consume(Semicolon);

    bool is_global = false;
    i16_t index = index_of(ident_name, ident_len, is_global);
    if (index == -1) {
        undefined_reference(ident_name, ident_len, save_line);
        return; 
    }

    op1 = (is_global ? op1 : op2);
    emit_three_bytes(op1, index, save_line);
}

void parse_get_c() {
    parse_input_statement(get_c, local_get_c);
}

void parse_get_i() {
    parse_input_statement(get_i, local_get_i);
}

void parse_get_d() {
    parse_input_statement(get_d, local_get_d);
}

void parse_statement(TokenKind kind) {
    if (kind == Print) {
        parse_print_statement();
    } else if (kind == If) {
        parse_if_statement();
    } else if (kind == While) {
        parse_while_statement();
    } else if (kind == For) {
        parse_for_loop_effeciently();
    } else if (kind == Return) {
        parse_return_statement();
    } else if (kind == Get_C) {
        parse_get_c();
    } else if (kind == Get_I) {
        parse_get_i();
    } else if (kind == Get_D) {
        parse_get_d();
    } else if (kind == LeftBrace) {
        parse_block_statement();
    } else {
        parse_expression_statement();
    }
}

void define_variable(char const *identifier, i32_t identifier_len, i32_t _line) {
    if (cur_scope_depth == 0) {
        StringLiteral name = {identifier, identifier_len};
        if (globals2.contains(name)) {
            redefining_variable(identifier, identifier_len, _line);
            return;
        }

        auto index = globals2.push(name, nullptr);
        emit_three_bytes(define_global, index, _line);
    } else {
        i32_t index;
        if (locals.contains(identifier, identifier_len, cur_scope_depth, index)) {
            redefining_variable(identifier, identifier_len, _line);
            return;
        }
        emit_three_bytes(define_local, as_t<i16_t>(index), _line);
    }
}

void parse_variable_declaration(bool consume_semicolon) {
    gettoken(); /* eat var */
    consume(Identifier);
    if (parse_error)
        return;
    auto identifier = text;
    auto identifier_len = text_len;
    auto save_line = line;
    
    if (peek_token() == Equal) {
        gettoken();
        parse_assignment();
    } else {
        emit_single_byte(nil);
    }
    
    define_variable(identifier, identifier_len, save_line);
    if (consume_semicolon)
        consume(Semicolon);
}

void parse_function_body() {
    consume(LeftBrace); /* eat '{' */
    auto tok = peek_token();
    while (tok != RightBrace && tok != Eof) {
        parse_declaration(tok);

        if (parse_error || compile_error)
            return;

        tok = peek_token();
    }  

    consume(RightBrace);
}

void parse_function_declaration() {
    start_new_scope();
    gettoken(); /* eat 'func' */
    consume(FuncIdentifier);
    auto func_name = text;
    auto func_name_len = text_len;


    if (functions.defined(func_name, func_name_len)) {
        redefining_function(func_name, func_name_len);
        return;
    }

    consume(LeftParen);

    i8_t arguments = 0;
    auto tok = peek_token();
    while (tok != RightParen && tok != Eof) {
        consume(Identifier);
        locals.push();
        ++arguments;
        tok = peek_token();
        if (tok != RightParen) {
            consume(Comma);
            tok = peek_token();
        }
    }
    consume(RightParen);

    if (arguments > 0) {
        for (i8_t i = 0; i < arguments; ++i) {
            auto &local = locals[arguments - 1 - i];
            local.index = -(2 + arguments - i);
        }
    }

    if (peek_token() != LeftBrace) {
        gettoken();
        unexpected_token("{", cur_token);
        return;
    }

    consume(LeftBrace);
    tok = peek_token();

    if (tok == RightBrace) {
        gettoken();
        expected_expression(cur_token);
        return;
    }

    i32_t address = code.size();
    functions.declare(func_name, func_name_len, address, arguments);
    emit_single_byte(ipush_bp);
    OpCode return_value = ret;
    if (func_name_len == 4 && std::strncmp(func_name, "main", 4) == 0) {
        main_addr = address;
        return_value = main_ret;
    }

    while (tok != RightBrace && tok != Eof) {
        parse_declaration(tok);
        tok = peek_token();
    }
    consume(RightBrace);
    for (i8_t i = 0; i < arguments; ++i)
        locals.pop();

    if (!exit_addrs.empty()) {
        for (auto &exit_function: exit_addrs)
            set_correct_code_address(code.size(), exit_function);
        exit_addrs.clear();
    }
    if (!return_found) {
        emit_value(int_c, as_t<i64_t>(0));
        emit_single_byte(store_ret_value);
    }
    end_new_scope();
    emit_single_byte(ipop_bp);
    emit_single_byte(return_value); 
}

void parse_declaration(TokenKind kind) {
    return_found = false;
    if (kind == Var) {
        parse_variable_declaration();
    } else {
        parse_statement(kind);
    }
    
    if (parse_error || compile_error)
        synchronize();
}

void parse_functions(TokenKind kind) {
    if (kind == Func) {
        parse_function_declaration();
    } else {
        parse_declaration(kind);
    }
}

/* parser end */

/* compiler start */

bool compile() {
    auto kind = peek_token();
    while (kind != Eof) {
        if (kind == Var || kind == Func)
            parse_functions(kind);
        else {
            parse_error = true;
            gettoken(false);
            error_header(line);
            std::fprintf(stderr, "unqualified statement in global scope\n");
            print_error_line(line - 1);
            break;
        }
        kind = peek_token();
    }
    
    if (show_opcodes) {
        disassemble_code("compiler");
        std::fprintf(stderr, "\n");
    }
    return !(compile_error || parse_error);
}

/* compiler end */


/* runtime start */

void runtime_error(char const *message, int offset) {
    auto lineNo = lines.at(offset);
    error_header(lineNo);
    std::fprintf(stderr, "%s\n\t", message);

    auto &error_line = sourcecode.at(lineNo - 1);
    std::fprintf(stderr, BOLD_GREEN "%d" NORMAL "| %.*s\n\n", lineNo, error_line.length, error_line.text);
}

void push(i64_t val) {
    *sp = val;
    ++sp;
}

void push(char val) {
    *sp = val;
    ++sp;
}

void push(double val) {
    *sp = val;
    ++sp;
}

void push(nullptr_t val) {
    *sp = val;
    ++sp;
}

void push(bool val) {
    *sp = val;
    ++sp;
}

void push(StringLiteral val) {
    *sp = val;
    ++sp;
}

void push(char const *text, i32_t length) {
    *sp = StringLiteral{text, length};
    ++sp;
}

void push(Value &val) {
    if (val.is_nil())
        push(nullptr);
    else if (val.is_bool())
        push(val.as_bool());
    else if (val.is_char())
        push(val.as_char());
    else if (val.is_int())
        push(val.as_int());
    else if (val.is_double())
        push(val.as_double());
    else if (val.is_string()) {
        *sp = val.as_string();
        ++sp;
    }
}

Value nil_value{};
Value &pop() {
    if (sp == stack.begin())
        return nil_value;
    sp -= 1;
    return *sp;
}

Value &peek(i32_t offset = 0) {
    return *(sp - 1 - offset);
}


void print_function() {
    auto print_args = *ip++;
    auto pop_n = print_args;
    while (print_args--) {
        peek(print_args).print();
    }

    for (i8_t i = 0; i < pop_n; ++i)
        pop();
}


bool run_vm() {
#define arithmatic_type_check() \
    if (peek()._kind != peek(1)._kind || \
            (!peek().is_int() && !peek().is_double())) {\
        runtime_error("both operands have to be <integer> or <double>", offset);\
        return false;\
    }

#define addition_type_check() \
    if (peek()._kind != peek(1)._kind || \
            (!peek().is_int() && !peek().is_double())) {\
        runtime_error("both operands have to be <integer> or <double>", offset);\
        return false;\
    }

#define relational_type_check() \
    if (peek()._kind != peek(1)._kind || \
            (!peek().is_int() && !peek().is_double() && !peek().is_char())) {\
        runtime_error("both operands have to be <integer> or <double> or <character>", offset);\
        return false;\
    }

#define arithmatic_operation(op) \
    val2 = pop();\
    val1 = pop();\
    if (val1.is_int())\
        push(val1.as_int() op val2.as_int());\
    else if (val1.is_double())\
        push(val1.as_double() op val2.as_double());

#define relational_operation(op) \
    val2 = pop();\
    val1 = pop();\
    if (val1.is_int())\
        push(val1.as_int() op val2.as_int());\
    else if (val1.is_char())\
        push(val1.as_char() op val2.as_char());

#define equality_operation(op) \
   if (peek()._kind != peek(1)._kind) {\
        runtime_error("operands have to be of same type", offset);\
        return false;\
   }\
   val2 = pop();\
   val1 = pop();\
   if (val1.is_int())\
       push(val1.as_int() op val2.as_int());\
   else if (val1.is_char())\
       push(val2.as_char() op val1.as_char());\
   else if (val1.is_double())\
       push(std::fabs(val1.as_double() - val2.as_double()) op 0.0);\
   else if (val1.is_bool())\
        push(val1.as_bool() op val2.as_bool());



    while (true) {
        auto offset = as_t<i32_t>(ip - code.begin());
        if (show_opcodes) {
            std::fprintf(stderr, "\t\t\t\t\t\t\t\tstack = [ ");
            for (auto i = stack.begin(); i != sp; ++i) {
                (*i).print(stderr, false);
                std::fprintf(stderr, " ");
            }
            std::fprintf(stderr, "]\n");
            disassemble_instruction(offset);
        }
        
        auto instruction = as_t<OpCode>(*ip++);
        Value val1;
        Value val2;
        switch (instruction) {
            case int_c:
                push(values.at(get_double_byte_index(ip - code.begin())).as_int());
                ip += 2;
                break;
            case char_c:
                push(values.at(get_double_byte_index(ip - code.begin())).as_char());
                ip += 2;
                break;
            case double_c:
                push(values.at(get_double_byte_index(ip - code.begin())).as_double());
                ip += 2;
                break;
            case string_c:
                push(values.at(get_double_byte_index(ip - code.begin())).as_string());
                ip += 2;
                break;
            case add:
                addition_type_check();

                val2 = pop();
                val1 = pop();
                if (val1.is_int())
                    push(val1.as_int() + val2.as_int());
                else if (val1.is_double())
                    push(val1.as_double() + val2.as_double());
                break;
            case sub:
                arithmatic_type_check();

                arithmatic_operation(-);
                break;
            case mult:
                arithmatic_type_check();

                arithmatic_operation(*);
                break;
            case idiv:
                arithmatic_type_check();

                arithmatic_operation(/);
                break;
            case positive:
                if (!peek().is_int())
                    return false;
                break;
            case neg:
                if (!peek().is_int())
                    return false;
                *(sp - 1) = -(*(sp - 1)).as_int();
                break;
            case nil:
                push(nullptr);
                break;
            case true_l:
                push(true);
                break;
            case false_l:
                push(false);
                break;
            case lt:
                relational_type_check();

                relational_operation(<)
                else if (val1.is_double())
                    push(std::isless(val1.as_double(), val2.as_double()));
               break; 
            case lte:
               relational_type_check();

               relational_operation(<=)
               else if (val1.is_double())
                   push(std::islessequal(val1.as_double(), val2.as_double()));
               break;
            case gt:
               relational_type_check();

               relational_operation(>)
               else if (val1.is_double())
                   push(std::isgreater(val1.as_double(), val2.as_double()));
               break;
            case gte:
               relational_type_check();

               relational_operation(>=)
               else if (val1.is_double())
                   push(std::isgreaterequal(val1.as_double(), val2.as_double()));
               break;
            case eq:
               equality_operation(==);
               break;
            case inot:
               push(!pop().as_bool());
               break;
            case neq:
               equality_operation(!=);
               break;
            case logical_and:
               val2 = pop();
               val1 = pop();
               push(val1.as_bool() && val2.as_bool());
               break;
            case logical_or:
               val2 = pop();
               val1 = pop();
               push(val1.as_bool() || val2.as_bool());
               break;
            case mod:
                arithmatic_type_check();

                val2 = pop();
                val1 = pop();
                if (val1.is_int())
                    push(val1.as_int() % val2.as_int());
                else if (val1.is_double())
                    push(std::fmod(val1.as_double(), val2.as_double()));
                break;
            case jit:
               {
                   val1 = peek();
                   if (val1.as_bool()) {
                        auto index = get_double_byte_index(ip - code.begin());
                        ip = code.begin() + index;
                   } else {
                        ip += 2;
                   }
               }
               break;
            case jif:
               {
                   val1 = peek();
                   if (!val1.as_bool()) {
                        auto index = get_double_byte_index(ip - code.begin());
                        ip = code.begin() + index;
                   } else {
                        ip += 2;
                   }
               }
               break;
            case jump:
               {
                    ip = code.begin() + get_double_byte_index(ip - code.begin());
               }
               break;
            case ipop:
               pop();
               break;
            case ipush_bp:
               push(bp - stack.begin());
               bp = sp;
               break;
            case ipop_bp:
               {
                   auto index = (*(sp - 1)).as_int();
                   bp = stack.begin() + index;
                   --sp;
               }
               break;
            case ret_addr:
               {
                    auto addr = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    push(as_t<i64_t>(addr));
               }
               break;
            case push_arg_addr:
               push(as_t<i64_t>(argument_indexes.at(get_double_byte_index(ip - code.begin()))));
               ip += 2;
               break;
            case pop_arg_addr:
               argument_indexes.at(get_double_byte_index(ip - code.begin())) = pop().as_int();
               ip += 2;
               break;
            case set_arg_addr:
                argument_indexes.at(get_double_byte_index(ip - code.begin())) = sp - stack.begin();
                ip += 2;
                break;             
            case print:
               print_function();
               break;
            case get_c:
               {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    globals2[index] = as_t<char>(std::getchar());
               }
               break;
            case get_i:
               {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    i64_t in;
                    std::scanf("%ld", &in);
                    globals2[index] = in;
               }
               break;
            case get_d:
               {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    double in;
                    std::scanf("%lF", &in);
                    globals2[index] = in;
               }
               break;
            case local_get_c:
               {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    *(bp + index) = as_t<char>(std::getchar());
               }
               break;
            case local_get_i:
               {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    i64_t in;
                    std::scanf("%ld", &in);
                    *(bp + index) = in;
               }
               break;
            case local_get_d:
               {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    double in;
                    std::scanf("%lF", &in);
                    *(bp + index) = in;
               }
               break;
            case define_global:
                {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    globals2[index] = pop();
                }
                break;
            case define_local:
                {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    *(bp + index) = peek();
                }
                break;
            case get_global:
                {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    push(globals2[index]);
                }
                break;
            case get_local:
                {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    push(*(bp + index));
                }
                break;
            case set_global:
                {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    globals2[index] = peek();
                }
                break;
            case set_local:
                {
                    auto index = get_double_byte_index(ip - code.begin());
                    ip += 2;
                    *(bp + index) = peek();
                }
                break;
            case store_ret_value:
                function_return_value = pop();
                break;
            case load_ret_value:
                push(function_return_value);
                break;
            case ret:
                {
                    auto ret_addr = pop().as_int();
                    ip = code.begin() + ret_addr;
                }
                break;
            case main_ret:
                return true;
            default:
                return true;
        }
    }

    return true;
}


bool interpret() {
    auto start = std::chrono::system_clock::now();
    if (!compile()) {
        return false;
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "compile time: " << duration_cast<std::chrono::seconds>(end - start).count() << "s\n";
   

    if (main_addr == -1) {
        std::fprintf(stderr, "could not find main function\n");
        return false;
    }

    ip = code.begin() + main_addr;
    if (show_opcodes) {
        std::fprintf(stderr, "main function starts at:\n");
        disassemble_instruction(main_addr);
    }
    return run_vm();
}

/* runtime end */

int main(int argc, char **argv) {
    if (argc > 1) {
        read_file(argv[1]);
        save_all_lines();

        if (argc > 2) {
            if (std::strlen(argv[2]) == 2 && std::strncmp(argv[2], "-d", 2) == 0)
                show_opcodes = true;
        }
    } else {
        std::fprintf(stderr, "usage: ncc FILE\n");
        return EXIT_FAILURE;
    }

    if (interpret()) {
        delete[] source;
        return EXIT_SUCCESS;
    }

    delete[] source;
	return EXIT_FAILURE;
}