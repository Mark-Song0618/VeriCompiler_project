#pragma once
#include <string>
#include <vector>
#include <regex>

class Token {
public:
    enum TKTYPE {
        INVALID_TOKEN,
        // key words
        K_FOR,
        K_IF,
        K_ELSE,
        K_ASSIGN,
        K_MODULE,
        K_ENDMODULE,
        K_INPUT,
        K_OUTPUT,
        K_BEGIN,
        K_END,
        K_WIRE,
        K_REG,
        K_ALWAYS,
        K_AT,
        // ignore
        SPACE,
        /*NEXT_LINE_WIN,
        NEXT_LINE_QT,*/
        COMMENT1,
        COMMENT2,
        // identifiers
        ID,
        // punctuations
        P_SEMICOLON,
        P_PARENTHESIS_L,
        P_PARENTHESIS_R,
        P_COMMAN,
        // operators
        O_ASSIGN_BLOCKING,
        O_ASSIGN_NONBLOCKING,
        O_ADD,
        O_MIN,
        O_MUL,
        O_DIV,
    };
    Token() = default;
    Token(std::string value, TKTYPE type) : m_type(type), m_val(value) {}
    Token& operator =(const Token& rh);

    std::string getValue() { return m_val; }
    TKTYPE getType() { return m_type; }
private:
    TKTYPE m_type = INVALID_TOKEN;
    std::string m_val;
};

class Tokenizer {
public:
    Tokenizer() = default;
    Tokenizer(std::string str) : m_context(str), m_cursor(0) {}
    void init(std::string str) {
        m_context = str;
        m_cursor = 0;
    }

    void reset();

    bool hasMoretoken() {
        return m_cursor < m_context.size() - 1;    //point to the position which behind the last char
    }

    Token getNextToken(bool skipEmpty = false);

private:
    void acceptToken(std::string value, Token::TKTYPE type);

private:
    std::string         m_context;
    int                 m_cursor = -1;
    std::vector<Token>  m_tokens;
};