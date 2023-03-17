/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "PikaParser.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

/* local head */
typedef QueueObj AST;
char* Lexer_getTokenStream(Args* outBuffs, char* stmt);
char* AST_genAsm(AST* ast, Args* outBuffs);
int32_t AST_deinit(AST* ast);

uint8_t TokenStream_isContain(char* tokenStream,
                              enum TokenType token_type,
                              char* pyload);
char* TokenStream_pop(Args* buffs_p, char** tokenStream);

/* Cursor preivilage */
void _Cursor_init(struct Cursor* cs);
void _Cursor_parse(struct Cursor* cs, char* stmt);
void _Cursor_beforeIter(struct Cursor* cs);

/* Cursor iter api */
void Cursor_iterStart(struct Cursor* cs);
void Cursor_iterEnd(struct Cursor* cs);
void Cursor_deinit(struct Cursor* cs);

/* Cursor high level api */
char* Cursor_popToken(Args* buffs, char** pStmt, char* devide);
PIKA_BOOL Cursor_isContain(char* stmt, TokenType type, char* pyload);
char* Cursor_splitCollect(Args* buffs, char* stmt, char* devide, int index);

char* Parser_linesToAsm(Args* outBuffs, char* multiLine);
uint16_t TokenStream_getSize(char* tokenStream) {
    if (strEqu("", tokenStream)) {
        return 0;
    }
    return strCountSign(tokenStream, 0x1F) + 1;
}

char* Cursor_popLastToken(Args* outBuffs, char** pStmt, char* str) {
    char* stmts = *pStmt;
    uint8_t divider_index = 0;
    Arg* keeped_arg = arg_newStr("");
    Arg* poped_arg = arg_newStr("");
    Cursor_forEach(cs, stmts) {
        Cursor_iterStart(&cs);
        if (cs.branket_deepth == 0) {
            if (strEqu(str, cs.token1.pyload)) {
                divider_index = cs.iter_index;
            }
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    Cursor_forEachExistPs(cs, stmts) {
        Cursor_iterStart(&cs);
        if (cs.iter_index < divider_index) {
            poped_arg = arg_strAppend(poped_arg, cs.token1.pyload);
        }
        if (cs.iter_index > divider_index) {
            keeped_arg = arg_strAppend(keeped_arg, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    char* keeped = strsCopy(outBuffs, arg_getStr(keeped_arg));
    char* poped = arg_getStr(poped_arg);
    pika_platform_memcpy(stmts, poped, strGetSize(poped) + 1);
    arg_deinit(poped_arg);
    arg_deinit(keeped_arg);
    return keeped;
}

char* Cursor_getCleanStmt(Args* outBuffs, char* cmd) {
    pika_assert(cmd != NULL);
    int32_t size = strGetSize(cmd);
    /* lexer may generate more chars than input */
    char* strOut = args_getBuff(outBuffs, size * 2);
    int32_t iOut = 0;
    Cursor_forEach(cs, cmd) {
        Cursor_iterStart(&cs);
        for (uint16_t k = 0; k < strGetSize(cs.token1.pyload); k++) {
            strOut[iOut] = cs.token1.pyload[k];
            iOut++;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* add \0 */
    strOut[iOut] = 0;
    return strOut;
}

static uint8_t Lexer_isError(char* line) {
    Args buffs = {0};
    uint8_t res = 0; /* not error */
    char* tokenStream = Lexer_getTokenStream(&buffs, line);
    if (NULL == tokenStream) {
        res = 1; /* lex error */
        goto exit;
    }
    goto exit;
exit:
    strsDeinit(&buffs);
    return res;
}

static char* Cursor_removeTokensBetween(Args* outBuffs,
                                        char* input,
                                        char* token_pyload1,
                                        char* token_pyload2) {
    Args buffs = {0};
    uint8_t block_deepth = 0;
    char* output = "";
    Cursor_forEach(cs, input) {
        Cursor_iterStart(&cs);
        if (strEqu(token_pyload1, cs.token1.pyload)) {
            if (block_deepth == 0) {
                output = strsAppend(&buffs, output, cs.token1.pyload);
            }
            block_deepth++;
        }
        if (strEqu(token_pyload2, cs.token1.pyload)) {
            block_deepth--;
        }
        if (block_deepth == 0) {
            output = strsAppend(&buffs, output, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    output = strsCopy(outBuffs, output);
    strsDeinit(&buffs);
    return output;
}

char* _remove_sub_stmt(Args* outBuffs, char* stmt) {
    Args buffs = {0};
    stmt = strsCopy(&buffs, stmt);
    stmt = Cursor_removeTokensBetween(&buffs, stmt, "(", ")");
    stmt = Cursor_removeTokensBetween(&buffs, stmt, "[", "]");
    stmt = Cursor_removeTokensBetween(&buffs, stmt, "{", "}");
    char* res = args_cacheStr(outBuffs, stmt);
    strsDeinit(&buffs);
    return res;
}

static enum StmtType Lexer_matchStmtType(char* right) {
    Args buffs = {0};
    enum StmtType stmtType = STMT_none;
    char* rightWithoutSubStmt = _remove_sub_stmt(&buffs, right);

    PIKA_BOOL is_get_operator = PIKA_FALSE;
    PIKA_BOOL is_get_method = PIKA_FALSE;
    PIKA_BOOL is_get_string = PIKA_FALSE;
    PIKA_BOOL is_get_bytes = PIKA_FALSE;
    PIKA_BOOL is_get_number = PIKA_FALSE;
    PIKA_BOOL is_get_symbol = PIKA_FALSE;
    PIKA_BOOL is_get_list = PIKA_FALSE;
    PIKA_BOOL is_get_slice = PIKA_FALSE;
    PIKA_BOOL is_get_dict = PIKA_FALSE;
    PIKA_BOOL is_get_import = PIKA_FALSE;
    PIKA_BOOL is_get_chain = PIKA_FALSE;
    Cursor_forEach(cs, rightWithoutSubStmt) {
        Cursor_iterStart(&cs);
        /* collect type */
        if (strEqu(cs.token1.pyload, " import ")) {
            is_get_import = PIKA_TRUE;
            goto iter_continue;
        }
        if (strEqu(cs.token2.pyload, "[")) {
            /* (symble | iteral | <]> | <)>) + <[> */
            if (TOKEN_symbol == cs.token1.type ||
                TOKEN_literal == cs.token1.type ||
                strEqu(cs.token1.pyload, "]") ||
                strEqu(cs.token1.pyload, ")")) {
                is_get_slice = PIKA_TRUE;
                goto iter_continue;
            }
            /* ( <,> | <=> ) + <[> */
            is_get_list = PIKA_TRUE;
        }
        if (strEqu(cs.token1.pyload, "[") && cs.iter_index == 1) {
            /* VOID + <[> */
            is_get_list = PIKA_TRUE;
            goto iter_continue;
        }
        if (strEqu(cs.token1.pyload, "...")) {
            goto iter_continue;
        }

        if (strEqu(cs.token1.pyload, "pass")) {
            goto iter_continue;
        }

        if (strIsStartWith(cs.token1.pyload, ".")) {
            if (cs.iter_index != 1) {
                is_get_chain = PIKA_TRUE;
                goto iter_continue;
            }
        }
        if (strEqu(cs.token1.pyload, "{")) {
            is_get_dict = PIKA_TRUE;
            goto iter_continue;
        }
        if (cs.token1.type == TOKEN_operator) {
            is_get_operator = PIKA_TRUE;
            goto iter_continue;
        }
        /* <(> */
        if (strEqu(cs.token1.pyload, "(")) {
            is_get_method = PIKA_TRUE;
            goto iter_continue;
        }
        if (cs.token1.type == TOKEN_literal) {
            if (cs.token1.pyload[0] == '\'' || cs.token1.pyload[0] == '"') {
                is_get_string = PIKA_TRUE;
                goto iter_continue;
            }
            if (cs.token1.pyload[1] == '\'' || cs.token1.pyload[1] == '"') {
                if (cs.token1.pyload[0] == 'b') {
                    is_get_bytes = PIKA_TRUE;
                    goto iter_continue;
                }
            }
            is_get_number = PIKA_TRUE;
            goto iter_continue;
        }
        if (cs.token1.type == TOKEN_symbol) {
            is_get_symbol = PIKA_TRUE;
            goto iter_continue;
        }
    iter_continue:
        Cursor_iterEnd(&cs);
    }
    if (is_get_import) {
        stmtType = STMT_import;
        goto exit;
    }
    if (is_get_operator) {
        stmtType = STMT_operator;
        goto exit;
    }
    if (is_get_chain) {
        stmtType = STMT_chain;
        goto exit;
    }
    if (is_get_slice) {
        stmtType = STMT_slice;
        goto exit;
    }
    if (is_get_list) {
        stmtType = STMT_list;
        goto exit;
    }
    if (is_get_dict) {
        stmtType = STMT_dict;
        goto exit;
    }
    if (is_get_method) {
        stmtType = STMT_method;
        goto exit;
    }
    if (is_get_string) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            stmtType = STMT_tuple;
            goto exit;
        }
        stmtType = STMT_string;
        goto exit;
    }
    if (is_get_bytes) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            stmtType = STMT_tuple;
            goto exit;
        }
        stmtType = STMT_bytes;
        goto exit;
    }
    if (is_get_number) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            stmtType = STMT_tuple;
            goto exit;
        }
        stmtType = STMT_number;
        goto exit;
    }
    if (is_get_symbol) {
        /* support multi assign */
        if (Cursor_isContain(right, TOKEN_devider, ",")) {
            stmtType = STMT_tuple;
            goto exit;
        }
        stmtType = STMT_reference;
        goto exit;
    }
exit:
    Cursor_deinit(&cs);
    strsDeinit(&buffs);
    return stmtType;
}

char* Lexer_printTokenStream(Args* outBuffs, char* tokenStream) {
    pika_assert(tokenStream);
    /* init */
    Args buffs = {0};
    char* printOut = strsCopy(&buffs, "");

    /* process */
    uint16_t token_size = TokenStream_getSize(tokenStream);
    for (uint16_t i = 0; i < token_size; i++) {
        char* token = TokenStream_pop(&buffs, &tokenStream);
        if (token[0] == TOKEN_operator) {
            printOut = strsAppend(&buffs, printOut, "{opt}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_devider) {
            printOut = strsAppend(&buffs, printOut, "{dvd}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_symbol) {
            printOut = strsAppend(&buffs, printOut, "{sym}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_literal) {
            printOut = strsAppend(&buffs, printOut, "{lit}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
    }
    /* out put */
    printOut = strsCopy(outBuffs, printOut);
    strsDeinit(&buffs);
    return printOut;
}

uint8_t Parser_checkIsDirect(char* str) {
    Args buffs = {0};
    uint8_t res = 0;
    pika_assert(NULL != str);
    char* left = Cursor_splitCollect(&buffs, str, "=", 1);
    if (!strEqu(left, str)) {
        res = 1;
        goto exit;
    }
exit:
    strsDeinit(&buffs);
    return res;
}

Arg* Lexer_setToken(Arg* tokenStream_arg,
                    enum TokenType token_type,
                    char*
                    operator) {
    Args buffs = {0};
    char token_type_buff[3] = {0};
    token_type_buff[0] = 0x1F;
    token_type_buff[1] = token_type;
    char* tokenStream = arg_getStr(tokenStream_arg);
    tokenStream = strsAppend(&buffs, tokenStream, token_type_buff);
    tokenStream = strsAppend(&buffs, tokenStream, operator);
    Arg* new_tokenStream_arg = arg_newStr(tokenStream);
    arg_deinit(tokenStream_arg);
    strsDeinit(&buffs);
    return new_tokenStream_arg;
}

Arg* Lexer_setSymbel(Arg* tokenStream_arg,
                     char* stmt,
                     int32_t i,
                     int32_t* symbol_start_index) {
    Args buffs = {0};
    char* symbol_buff = NULL;
    if (-1 == *symbol_start_index) {
        /* no found symbol start index */
        goto exit;
    }
    /* nothing to add symbel */
    if (i == *symbol_start_index) {
        goto exit;
    }
    symbol_buff = args_getBuff(&buffs, i - *symbol_start_index);
    pika_platform_memcpy(symbol_buff, stmt + *symbol_start_index,
                         i - *symbol_start_index);
    /* literal */
    if ((symbol_buff[0] == '\'') || (symbol_buff[0] == '"')) {
        /* "" or '' */
        tokenStream_arg =
            Lexer_setToken(tokenStream_arg, TOKEN_literal, symbol_buff);
        goto exit;
    }

    if ((symbol_buff[0] >= '0') && (symbol_buff[0] <= '9')) {
        /* number */
        tokenStream_arg =
            Lexer_setToken(tokenStream_arg, TOKEN_literal, symbol_buff);
        goto exit;
    }

    if ((symbol_buff[0] == 'b') &&
        ((symbol_buff[1] == '\'') || (symbol_buff[1] == '"'))) {
        /* b"" or b'' */
        tokenStream_arg =
            Lexer_setToken(tokenStream_arg, TOKEN_literal, symbol_buff);
        goto exit;
    }

    /* symbol */
    tokenStream_arg =
        Lexer_setToken(tokenStream_arg, TOKEN_symbol, symbol_buff);
    goto exit;
exit:
    *symbol_start_index = -1;
    strsDeinit(&buffs);
    return tokenStream_arg;
}

/* tokenStream is devided by space */
/* a token is [TOKENTYPE|(CONTENT)] */
char* Lexer_getTokenStream(Args* outBuffs, char* stmt) {
    /* init */
    Arg* tokenStream_arg = New_arg(NULL);
    tokenStream_arg = arg_setStr(tokenStream_arg, "", "");
    int32_t size = strGetSize(stmt);
    uint8_t bracket_deepth = 0;
    uint8_t cn2 = 0;
    uint8_t cn1 = 0;
    uint8_t c0 = 0;
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
    uint8_t c4 = 0;
    uint8_t c5 = 0;
    uint8_t c6 = 0;
    int32_t symbol_start_index = -1;
    int is_in_string = 0;
    int is_number = 0;
    char* tokenStream;

    /* process */
    for (int32_t i = 0; i < size; i++) {
        /* update char */
        cn2 = 0;
        cn1 = 0;
        c0 = stmt[i];
        c1 = 0;
        c2 = 0;
        c3 = 0;
        c4 = 0;
        c5 = 0;
        c6 = 0;
        if (i - 2 >= 0) {
            cn2 = stmt[i - 2];
        }
        if (i - 1 >= 0) {
            cn1 = stmt[i - 1];
        }
        if (i + 1 < size) {
            c1 = stmt[i + 1];
        }
        if (i + 2 < size) {
            c2 = stmt[i + 2];
        }
        if (i + 3 < size) {
            c3 = stmt[i + 3];
        }
        if (i + 4 < size) {
            c4 = stmt[i + 4];
        }
        if (i + 5 < size) {
            c5 = stmt[i + 5];
        }
        if (i + 6 < size) {
            c6 = stmt[i + 6];
        }
        if (-1 == symbol_start_index) {
            is_number = 0;
            if ((c0 >= '0') && (c0 <= '9')) {
                is_number = 1;
            }
            symbol_start_index = i;
        }

        /* solve string */
        if (0 == is_in_string) {
            if ('\'' == c0) {
                if ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2)) {
                    /* in ' */
                    is_in_string = 1;
                    continue;
                }
            }
            if ('"' == c0) {
                if ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2)) {
                    /* in "" */
                    is_in_string = 2;
                    continue;
                }
            }
        }

        if (1 == is_in_string) {
            if ('\'' == c0 && ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2))) {
                is_in_string = 0;
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i + 1,
                                                  &symbol_start_index);
            }
            continue;
        }
        if (2 == is_in_string) {
            if ('"' == c0 && ('\\' != cn1 || ('\\' == cn1 && '\\' == cn2))) {
                is_in_string = 0;
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i + 1,
                                                  &symbol_start_index);
            }
            continue;
        }

        /* match annotation */
        if ('#' == c0) {
            break;
        }

        /* match devider*/
        if (('(' == c0) || (')' == c0) || (',' == c0) || ('[' == c0) ||
            (']' == c0) || (':' == c0) || ('{' == c0) || ('}' == c0)) {
            tokenStream_arg =
                Lexer_setSymbel(tokenStream_arg, stmt, i, &symbol_start_index);
            char content[2] = {0};
            content[0] = c0;
            tokenStream_arg =
                Lexer_setToken(tokenStream_arg, TOKEN_devider, content);
            if (c0 == '(') {
                bracket_deepth++;
            }
            if (c0 == ')') {
                bracket_deepth--;
            }
            continue;
        }
        /* match operator */
        if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
            ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
            ('%' == c0) || ('&' == c0) || ('|' == c0) || ('^' == c0) ||
            ('~' == c0)) {
            if ('-' == c0 && is_number) {
                if ((cn1 == 'e') || (cn1 == 'E')) {
                    continue;
                }
            }
            if (('*' == c0) || ('/' == c0)) {
                /*
                    =, **=, //
                */
                if ((c0 == c1) && ('=' == c2)) {
                    char content[4] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    content[2] = '=';
                    tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                      &symbol_start_index);
                    tokenStream_arg = Lexer_setToken(tokenStream_arg,
                                                     TOKEN_operator, content);
                    i = i + 2;
                    continue;
                }
            }
            /*
                >>, <<, **, //
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0)) {
                if (c0 == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                      &symbol_start_index);
                    tokenStream_arg = Lexer_setToken(tokenStream_arg,
                                                     TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }
            /*
                >=, <=, *=, /=, +=, -=, !=, ==, %=
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
                ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
                ('%' == c0)) {
                if ('=' == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                      &symbol_start_index);
                    tokenStream_arg = Lexer_setToken(tokenStream_arg,
                                                     TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }

            /* single operator */
            /*
                +, -, *, ... /
            */
            char content[2] = {0};
            content[0] = c0;
            tokenStream_arg =
                Lexer_setSymbel(tokenStream_arg, stmt, i, &symbol_start_index);
            tokenStream_arg =
                Lexer_setToken(tokenStream_arg, TOKEN_operator, content);
            continue;
        }

        // not the string operator
        if ((cn1 >= 'a' && cn1 <= 'z') || (cn1 >= 'A' && cn1 <= 'Z') ||
            (cn1 >= '0' && cn1 <= '9') || cn1 == '_' || cn1 == '.') {
            goto after_match_string_operator;
        }
        /* not */
        if ('n' == c0) {
            if (('o' == c1) && ('t' == c2) && (' ' == c3)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " not ");
                i = i + 3;
                continue;
            }
        }
        /* and */
        if ('a' == c0) {
            if (('n' == c1) && ('d' == c2) && (' ' == c3)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " and ");
                i = i + 3;
                continue;
            }
        }
        /* or */
        if ('o' == c0) {
            if (('r' == c1) && (' ' == c2)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " or ");
                i = i + 2;
                continue;
            }
        }
        /* is */
        if ('i' == c0) {
            if (('s' == c1) && (' ' == c2)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " is ");
                i = i + 2;
                continue;
            }
        }
        /* in */
        if ('i' == c0) {
            if (('n' == c1) && (' ' == c2)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " in ");
                i = i + 2;
                continue;
            }
        }
        /* as */
        if ('a' == c0) {
            if (('s' == c1) && (' ' == c2)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " as ");
                i = i + 2;
                continue;
            }
        }
        /* import */
        if ('i' == c0) {
            if (('m' == c1) && ('p' == c2) && ('o' == c3) && ('r' == c4) &&
                ('t' == c5) && (' ' == c6)) {
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
                tokenStream_arg =
                    Lexer_setToken(tokenStream_arg, TOKEN_operator, " import ");
                i = i + 5;
                continue;
            }
        }
    after_match_string_operator:

        /* skip spaces */
        if (' ' == c0) {
            /* not get symbal */
            if (i == symbol_start_index) {
                symbol_start_index = -1;
            } else {
                /* already get symbal */
                tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, i,
                                                  &symbol_start_index);
            }
        }
        if (i == size - 1) {
            /* last check symbel */
            // if('\n' == c0){
            //     continue;
            // }
            tokenStream_arg = Lexer_setSymbel(tokenStream_arg, stmt, size,
                                              &symbol_start_index);
        }
    }
    /* output */
    tokenStream = arg_getStr(tokenStream_arg);
    tokenStream = strsCopy(outBuffs, tokenStream);
    arg_deinit(tokenStream_arg);
    return tokenStream;
}

char* TokenStream_pop(Args* buffs_p, char** tokenStream) {
    return strsPopToken(buffs_p, tokenStream, 0x1F);
}

enum TokenType Token_getType(char* token) {
    return (enum TokenType)token[0];
}

char* Token_getPyload(char* token) {
    return (char*)((uintptr_t)token + 1);
}

uint8_t TokenStream_count(char* tokenStream,
                          enum TokenType token_type,
                          char* pyload) {
    Args buffs = {0};
    char* tokenStream_buff = strsCopy(&buffs, tokenStream);
    uint8_t res = 0;
    uint16_t token_size = TokenStream_getSize(tokenStream);
    for (int i = 0; i < token_size; i++) {
        char* token = TokenStream_pop(&buffs, &tokenStream_buff);
        if (token_type == Token_getType(token)) {
            if (strEqu(Token_getPyload(token), pyload)) {
                res++;
            }
        }
    }
    strsDeinit(&buffs);
    return res;
}

uint8_t TokenStream_isContain(char* tokenStream,
                              enum TokenType token_type,
                              char* pyload) {
    if (TokenStream_count(tokenStream, token_type, pyload) > 0) {
        return 1;
    }
    return 0;
}

static char* _solveEqualLevelOperator(Args* buffs,
                                      char*
                                      operator,
                                      char * op1,
                                      char* op2,
                                      char* stmt) {
    if ((strEqu(operator, op1)) || (strEqu(operator, op2))) {
        Cursor_forEach(cs, stmt) {
            Cursor_iterStart(&cs);
            if (strEqu(cs.token1.pyload, op1)) {
                operator= strsCopy(buffs, op1);
            }
            if (strEqu(cs.token1.pyload, op2)) {
                operator= strsCopy(buffs, op2);
            }
            Cursor_iterEnd(&cs);
        }
        Cursor_deinit(&cs);
    }
    return operator;
}

static const char operators[][9] = {
    "**", "~",  "*",  "/",    "%",     "//",    "+",    "-",
    ">>", "<<", "&",  "^",    "|",     "<",     "<=",   ">",
    ">=", "!=", "==", " is ", " in ",  "%=",    "/=",   "//=",
    "-=", "+=", "*=", "**=",  " not ", " and ", " or ", " import "};

char* Lexer_getOperator(Args* outBuffs, char* stmt) {
    Args buffs = {0};
    char* operator= NULL;
    char* tokenStream = Lexer_getTokenStream(&buffs, stmt);

    // use parse state foreach to get operator
    for (uint32_t i = 0; i < sizeof(operators) / 9; i++) {
        Cursor_forEach(cs, tokenStream) {
            Cursor_iterStart(&cs);
            // get operator
            if (strEqu(cs.token2.pyload, (char*)operators[i])) {
                // solve the iuuse of "~-1"
                operator= strsCopy(&buffs, (char*)operators[i]);
                Cursor_iterEnd(&cs);
                break;
            }
            Cursor_iterEnd(&cs);
        };
        Cursor_deinit(&cs);
    }

    /* solve the iuuse of "~-1" */
    if (strEqu(operator, "-")) {
        Cursor_forEach(cs, stmt) {
            Cursor_iterStart(&cs);
            if (strEqu(cs.token2.pyload, "-")) {
                if (cs.token1.type == TOKEN_operator) {
                    operator= strsCopy(&buffs, cs.token1.pyload);
                    Cursor_iterEnd(&cs);
                    break;
                }
            }
            Cursor_iterEnd(&cs);
        };
        Cursor_deinit(&cs);
    }

    /* match the last operator in equal level */
    operator= _solveEqualLevelOperator(&buffs, operator, "+", "-", stmt);
    operator= _solveEqualLevelOperator(&buffs, operator, "*", "/", stmt);
    /* out put */
    if (NULL == operator) {
        return NULL;
    }
    operator= strsCopy(outBuffs, operator);
    strsDeinit(&buffs);
    return operator;
}

const char void_str[] = "";

void LexToken_update(struct LexToken* lex_token) {
    lex_token->type = Token_getType(lex_token->tokenStream);
    if (lex_token->type == TOKEN_strEnd) {
        lex_token->pyload = (char*)void_str;
    } else {
        lex_token->pyload = Token_getPyload(lex_token->tokenStream);
    }
}

void Cursor_iterStart(struct Cursor* cs) {
    cs->iter_index++;
    cs->iter_buffs = New_strBuff();
    /* token1 is the last token */
    cs->token1.tokenStream =
        strsCopy(cs->iter_buffs, arg_getStr(cs->last_token));
    /* token2 is the next token */
    cs->token2.tokenStream = TokenStream_pop(cs->iter_buffs, &cs->tokenStream);
    /* store last token */
    arg_deinit(cs->last_token);
    cs->last_token = arg_newStr(cs->token2.tokenStream);

    LexToken_update(&cs->token1);
    LexToken_update(&cs->token2);
    if (strEqu(cs->token1.pyload, "(")) {
        cs->branket_deepth++;
    }
    if (strEqu(cs->token1.pyload, ")")) {
        cs->branket_deepth--;
    }
    if (strEqu(cs->token1.pyload, "[")) {
        cs->branket_deepth++;
    }
    if (strEqu(cs->token1.pyload, "]")) {
        cs->branket_deepth--;
    }
    if (strEqu(cs->token1.pyload, "{")) {
        cs->branket_deepth++;
    }
    if (strEqu(cs->token1.pyload, "}")) {
        cs->branket_deepth--;
    }
}

void LexToken_init(struct LexToken* lt) {
    lt->pyload = NULL;
    lt->tokenStream = NULL;
    lt->type = TOKEN_strEnd;
}

void _Cursor_init(struct Cursor* cs) {
    cs->tokenStream = NULL;
    cs->length = 0;
    cs->iter_index = 0;
    cs->branket_deepth = 0;
    cs->last_token = NULL;
    cs->iter_buffs = NULL;
    cs->buffs_p = New_strBuff();
    cs->result = PIKA_RES_OK;
    LexToken_init(&cs->token1);
    LexToken_init(&cs->token2);
}

void Cursor_iterEnd(struct Cursor* cs) {
    args_deinit(cs->iter_buffs);
}

void Cursor_deinit(struct Cursor* cs) {
    if (NULL != cs->last_token) {
        arg_deinit(cs->last_token);
    }
    args_deinit(cs->buffs_p);
}

void _Cursor_parse(struct Cursor* cs, char* stmt) {
    if (NULL == stmt) {
        cs->result = PIKA_RES_ERR_SYNTAX_ERROR;
        return;
    }
    cs->tokenStream = Lexer_getTokenStream(cs->buffs_p, stmt);
    if (NULL == cs->tokenStream) {
        cs->result = PIKA_RES_ERR_SYNTAX_ERROR;
        return;
    }
    cs->length = TokenStream_getSize(cs->tokenStream);
}

void _Cursor_beforeIter(struct Cursor* cs) {
    /* clear first token */
    if (cs->result != PIKA_RES_OK) {
        return;
    }
    TokenStream_pop(cs->buffs_p, &cs->tokenStream);
    cs->last_token = arg_newStr(TokenStream_pop(cs->buffs_p, &cs->tokenStream));
}

uint8_t Cursor_count(char* stmt, TokenType type, char* pyload) {
    /* fast return */
    if (!strstr(stmt, pyload)) {
        return PIKA_FALSE;
    }
    Args buffs = {0};
    char* tokenStream = Lexer_getTokenStream(&buffs, stmt);
    uint8_t res = TokenStream_count(tokenStream, type, pyload);
    strsDeinit(&buffs);
    return res;
}

PIKA_BOOL Cursor_isContain(char* stmt, TokenType type, char* pyload) {
    if (Cursor_count(stmt, type, pyload) > 0) {
        return PIKA_TRUE;
    }
    return PIKA_FALSE;
}

char* Cursor_popToken(Args* buffs, char** pStmt, char* devide) {
    Arg* out_item = arg_newStr("");
    Arg* tokenStream_after = arg_newStr("");
    PIKA_BOOL is_find_devide = PIKA_FALSE;
    Cursor_forEach(cs, *pStmt) {
        Cursor_iterStart(&cs);
        if (!is_find_devide) {
            if ((cs.branket_deepth == 0 && strEqu(cs.token1.pyload, devide)) ||
                cs.iter_index == cs.length) {
                is_find_devide = PIKA_TRUE;
                Cursor_iterEnd(&cs);
                continue;
            }
        }
        if (!is_find_devide) {
            out_item = arg_strAppend(out_item, cs.token1.pyload);
        } else {
            tokenStream_after =
                arg_strAppend(tokenStream_after, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* cache out item */
    char* out_item_str = strsCopy(buffs, arg_getStr(out_item));
    arg_deinit(out_item);
    /* cache tokenStream after */
    char* token_after_str = strsCopy(buffs, arg_getStr(tokenStream_after));
    arg_deinit(tokenStream_after);
    /* update tokenStream */
    *pStmt = token_after_str;
    return out_item_str;
}

char* Cursor_splitCollect(Args* buffs, char* stmt, char* devide, int index) {
    Arg* aOut = arg_newStr("");
    int expect_branket = 0;
    if (devide[0] == '(' || devide[0] == '[' || devide[0] == '{') {
        expect_branket = 1;
    }
    int i = 0;
    Cursor_forEach(cs, stmt) {
        Cursor_iterStart(&cs);
        if (cs.branket_deepth == expect_branket &&
            strEqu(cs.token1.pyload, devide)) {
            i++;
            Cursor_iterEnd(&cs);
            continue;
        }
        if (i == index) {
            aOut = arg_strAppend(aOut, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* if not found, return origin string */
    if (i == 0) {
        arg_deinit(aOut);
        aOut = arg_newStr(stmt);
    }
    return strsCacheArg(buffs, aOut);
}

static void Slice_getPars(Args* outBuffs,
                          char* inner,
                          char** pStart,
                          char** pEnd,
                          char** pStep) {
#if PIKA_NANO_ENABLE
    return;
#endif
    Args buffs = {0};
    *pStart = "";
    *pEnd = "";
    *pStep = "";

    /* slice */
    uint8_t colon_i = 0;
    Cursor_forEach(cs, inner) {
        Cursor_iterStart(&cs);
        if (strEqu(cs.token1.pyload, ":") && cs.branket_deepth == 0) {
            colon_i++;
            goto iter_continue1;
        }
        if (colon_i == 0) {
            *pStart = strsAppend(&buffs, *pStart, cs.token1.pyload);
        }
        if (colon_i == 1) {
            *pEnd = strsAppend(&buffs, *pEnd, cs.token1.pyload);
        }
        if (colon_i == 2) {
            *pStep = strsAppend(&buffs, *pStep, cs.token1.pyload);
        }
    iter_continue1:
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (colon_i == 1) {
        *pStep = "1";
        if (strEqu(*pStart, "")) {
            *pStart = "0";
        }
        if (strEqu(*pEnd, "")) {
            *pEnd = "-1";
        }
    }
    if (colon_i == 0) {
        *pEnd = strsAppend(&buffs, *pStart, " + 1");
        *pStep = "1";
    }

    /* slice with step */

    /* output */
    *pStart = strsCopy(outBuffs, *pStart);
    *pEnd = strsCopy(outBuffs, *pEnd);
    *pStep = strsCopy(outBuffs, *pStep);
    /* clean */
    strsDeinit(&buffs);
}

char* Suger_leftSlice(Args* outBuffs, char* right, char** left_p) {
#if !PIKA_SYNTAX_SLICE_ENABLE
    return right;
#endif
    /* init objects */
    Args buffs = {0};
    Arg* right_arg = arg_newStr("");
    char* left = *left_p;
    uint8_t is_in_brancket = 0;
    args_setStr(&buffs, "inner", "");
    uint8_t matched = 0;
    char* right_res = NULL;
    /* exit when NULL */
    if (NULL == left) {
        arg_deinit(right_arg);
        right_arg = arg_newStr(right);
        goto exit;
    }
    /* exit when not match
         (symble|iteral)'['
    */
    Cursor_forEach(cs, left) {
        Cursor_iterStart(&cs);
        if (strEqu(cs.token2.pyload, "[")) {
            if (TOKEN_symbol == cs.token1.type ||
                TOKEN_literal == cs.token1.type) {
                matched = 1;
                Cursor_iterEnd(&cs);
                break;
            }
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (!matched) {
        /* not contain '[', return origin */
        arg_deinit(right_arg);
        right_arg = arg_newStr(right);
        goto exit;
    }

    /* matched [] */
    Cursor_forEachExistPs(cs, left) {
        Cursor_iterStart(&cs);
        /* found '[' */
        if ((TOKEN_devider == cs.token2.type) &&
            (strEqu(cs.token2.pyload, "["))) {
            /* get 'obj' from obj[] */
            args_setStr(&buffs, "obj", cs.token1.pyload);
            is_in_brancket = 1;
            /* fond ']' */
        } else if ((TOKEN_devider == cs.token2.type) &&
                   (strEqu(cs.token2.pyload, "]"))) {
            is_in_brancket = 0;
            char* inner = args_getStr(&buffs, "inner");
            Arg* inner_arg = arg_newStr(inner);
            inner_arg = arg_strAppend(inner_arg, cs.token1.pyload);
            args_setStr(&buffs, "inner", arg_getStr(inner_arg));
            arg_deinit(inner_arg);
            /* update inner pointer */
            inner = args_getStr(&buffs, "inner");
            char* start = NULL;
            char* end = NULL;
            char* step = NULL;
            Slice_getPars(&buffs, inner, &start, &end, &step);
            /* obj = __setitem__(obj, key, val) */
            right_arg = arg_strAppend(right_arg, "__setitem__(");
            right_arg = arg_strAppend(right_arg, args_getStr(&buffs, "obj"));
            right_arg = arg_strAppend(right_arg, ",");
            right_arg = arg_strAppend(right_arg, start);
            right_arg = arg_strAppend(right_arg, ",");
            right_arg = arg_strAppend(right_arg, right);
            right_arg = arg_strAppend(right_arg, ")");
            /* clean the inner */
            args_setStr(&buffs, "inner", "");
            /* in brancket and found '[' */
        } else if (is_in_brancket && (!strEqu(cs.token1.pyload, "["))) {
            char* inner = args_getStr(&buffs, "inner");
            Arg* index_arg = arg_newStr(inner);
            index_arg = arg_strAppend(index_arg, cs.token1.pyload);
            args_setStr(&buffs, "inner", arg_getStr(index_arg));
            arg_deinit(index_arg);
            /* out of brancket and not found ']' */
        } else if (!is_in_brancket && (!strEqu(cs.token1.pyload, "]"))) {
            if (TOKEN_strEnd != cs.token1.type) {
                right_arg = arg_strAppend(right_arg, cs.token1.pyload);
            }
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* clean the left */
    for (size_t i = 0; i < strGetSize(left); i++) {
        if (left[i] == '[') {
            left[i] = '\0';
            break;
        }
    }
exit:
    /* clean and return */
    right_res = strsCopy(outBuffs, arg_getStr(right_arg));
    arg_deinit(right_arg);
    strsDeinit(&buffs);
    return right_res;
}

char* Suger_format(Args* outBuffs, char* right) {
#if !PIKA_SYNTAX_FORMAT_ENABLE
    return right;
#endif
    /* quick skip */
    if (!strIsContain(right, '%')) {
        return right;
    }

    PIKA_BOOL is_format = PIKA_FALSE;
    Cursor_forEach(ps1, right) {
        Cursor_iterStart(&ps1);
        if (ps1.branket_deepth == 0 && strEqu(ps1.token1.pyload, "%")) {
            is_format = PIKA_TRUE;
        }
        Cursor_iterEnd(&ps1);
    }
    Cursor_deinit(&ps1);
    if (PIKA_FALSE == is_format) {
        return right;
    }

    char* res = right;
    Arg* str_buf = arg_newStr("");
    Arg* var_buf = arg_newStr("");
    PIKA_BOOL is_in_format = PIKA_FALSE;
    PIKA_BOOL is_tuple = PIKA_FALSE;
    PIKA_BOOL is_out_vars = PIKA_FALSE;
    Args buffs = {0};
    char* fmt = NULL;
    Cursor_forEach(cs, right) {
        char* item = "";
        Cursor_iterStart(&cs);
        if (PIKA_FALSE == is_in_format) {
            if (cs.token1.type != TOKEN_literal) {
                item = cs.token1.pyload;
                goto iter_continue;
            }
            if (cs.token1.pyload[0] != '\'' && cs.token1.pyload[0] != '"') {
                item = cs.token1.pyload;
                goto iter_continue;
            }
            if (!strEqu(cs.token2.pyload, "%")) {
                item = cs.token1.pyload;
                goto iter_continue;
            }
            /* found the format stmt */
            is_in_format = PIKA_TRUE;
            fmt = strsCopy(&buffs, cs.token1.pyload);
            goto iter_continue;
        }
        if (PIKA_TRUE == is_in_format) {
            /* check the format vars */
            if (strEqu(cs.token1.pyload, "%")) {
                /* is a tuple */
                if (strEqu(cs.token2.pyload, "(")) {
                    is_tuple = PIKA_TRUE;
                } else {
                    var_buf = arg_strAppend(var_buf, cs.token2.pyload);
                }
                goto iter_continue;
            }
            /* found the end of tuple */
            if (cs.iter_index == cs.length) {
                is_out_vars = PIKA_TRUE;
                is_in_format = PIKA_FALSE;
            } else {
                /* push the vars inner the tuple */
                var_buf = arg_strAppend(var_buf, cs.token2.pyload);
            }
            if (is_out_vars) {
                if (is_tuple) {
                    str_buf = arg_strAppend(str_buf, "cformat(");
                    str_buf = arg_strAppend(str_buf, fmt);
                    str_buf = arg_strAppend(str_buf, ",");
                    str_buf = arg_strAppend(str_buf, arg_getStr(var_buf));
                } else {
                    str_buf = arg_strAppend(str_buf, "cformat(");
                    str_buf = arg_strAppend(str_buf, fmt);
                    str_buf = arg_strAppend(str_buf, ",");
                    str_buf = arg_strAppend(str_buf, arg_getStr(var_buf));
                    str_buf = arg_strAppend(str_buf, ")");
                }
            }
        }
    iter_continue:
        if (!is_in_format) {
            str_buf = arg_strAppend(str_buf, item);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    res = strsCopy(outBuffs, arg_getStr(str_buf));
    arg_deinit(str_buf);
    arg_deinit(var_buf);
    strsDeinit(&buffs);
    return res;
}

uint8_t Suger_selfOperator(Args* outbuffs,
                           char* stmt,
                           char** right_p,
                           char** left_p) {
    char* left_new = NULL;
    char* right_new = NULL;
    Arg* left_arg = arg_newStr("");
    Arg* right_arg = arg_newStr("");
    Arg* right_arg_new = arg_newStr("");
    uint8_t is_left_exist = 0;

    Args buffs = {0};
    char _operator[2] = {0};
    char* operator=(char*) _operator;
    uint8_t is_right = 0;
    if (Cursor_isContain(stmt, TOKEN_operator, "+=")) {
        operator[0] = '+';
    }
    if (Cursor_isContain(stmt, TOKEN_operator, "-=")) {
        operator[0] = '-';
    }
    if (Cursor_isContain(stmt, TOKEN_operator, "*=")) {
        operator[0] = '*';
    }
    if (Cursor_isContain(stmt, TOKEN_operator, "/=")) {
        operator[0] = '/';
    }
    /* not found self operator */
    if (operator[0] == 0) {
        goto exit;
    }
    /* found self operator */
    is_left_exist = 1;
    Cursor_forEach(cs, stmt) {
        Cursor_iterStart(&cs);
        if ((strEqu(cs.token1.pyload, "*=")) ||
            (strEqu(cs.token1.pyload, "/=")) ||
            (strEqu(cs.token1.pyload, "+=")) ||
            (strEqu(cs.token1.pyload, "-="))) {
            is_right = 1;
            goto iter_continue;
        }
        if (!is_right) {
            left_arg = arg_strAppend(left_arg, cs.token1.pyload);
        } else {
            right_arg = arg_strAppend(right_arg, cs.token1.pyload);
        }
    iter_continue:
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    /* connect right */
    right_arg_new = arg_strAppend(right_arg_new, arg_getStr(left_arg));
    right_arg_new = arg_strAppend(right_arg_new, operator);
    right_arg_new = arg_strAppend(right_arg_new, "(");
    right_arg_new = arg_strAppend(right_arg_new, arg_getStr(right_arg));
    right_arg_new = arg_strAppend(right_arg_new, ")");

    /* collect left_new and right_new */
    left_new = arg_getStr(left_arg);
    right_new = arg_getStr(right_arg_new);

exit:
    strsDeinit(&buffs);
    if (NULL != right_new) {
        *(right_p) = strsCopy(outbuffs, right_new);
        ;
    }
    if (NULL != left_new) {
        *(left_p) = strsCopy(outbuffs, left_new);
    }
    arg_deinit(right_arg);
    arg_deinit(left_arg);
    arg_deinit(right_arg_new);
    return is_left_exist;
}

PIKA_RES AST_setNodeAttr(AST* ast, char* attr_name, char* attr_val) {
    return obj_setStr(ast, attr_name, attr_val);
}

char* AST_getNodeAttr(AST* ast, char* attr_name) {
    return obj_getStr(ast, attr_name);
}

PIKA_RES AST_setNodeBlock(AST* ast, char* node_content) {
    return AST_setNodeAttr(ast, "block", node_content);
}

char* AST_getThisBlock(AST* ast) {
    return obj_getStr(ast, "block");
}

AST* AST_parseStmt(AST* ast, char* stmt);
PIKA_RES AST_parseSubStmt(AST* ast, char* node_content) {
    queueObj_pushObj(ast, (char*)"stmt");
    AST_parseStmt(queueObj_getCurrentObj(ast), node_content);
    return PIKA_RES_OK;
}

char* Parser_popSubStmt(Args* outbuffs, char** stmt_p, char* delimiter) {
    Arg* substmt_arg = arg_newStr("");
    Arg* newstmt_arg = arg_newStr("");
    char* stmt = *stmt_p;
    PIKA_BOOL is_get_substmt = PIKA_FALSE;
    Args buffs = {0};
    Cursor_forEach(cs, stmt) {
        Cursor_iterStart(&cs);
        if (is_get_substmt) {
            /* get new stmt */
            newstmt_arg = arg_strAppend(newstmt_arg, cs.token1.pyload);
            Cursor_iterEnd(&cs);
            continue;
        }
        if (cs.branket_deepth > 0) {
            /* ignore */
            substmt_arg = arg_strAppend(substmt_arg, cs.token1.pyload);
            Cursor_iterEnd(&cs);
            continue;
        }
        if (strEqu(cs.token1.pyload, delimiter)) {
            /* found delimiter */
            is_get_substmt = PIKA_TRUE;
            Cursor_iterEnd(&cs);
            continue;
        }
        /* collect substmt */
        substmt_arg = arg_strAppend(substmt_arg, cs.token1.pyload);
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    strsDeinit(&buffs);

    char* substmt = strsCacheArg(outbuffs, substmt_arg);
    char* newstmt = strsCacheArg(outbuffs, newstmt_arg);
    *stmt_p = newstmt;
    return substmt;
}

char* Parser_popLastSubStmt(Args* outbuffs, char** stmt_p, char* delimiter) {
    uint8_t last_stmt_i = 0;
    char* stmt = *stmt_p;
    Cursor_forEach(cs, stmt) {
        Cursor_iterStart(&cs);
        if (strIsStartWith(cs.token1.pyload, delimiter)) {
            /* found delimiter */
            if (!strEqu(delimiter, "[") && cs.branket_deepth > 0) {
                /* ignore */
                Cursor_iterEnd(&cs);
                continue;
            }

            /* for "[" */
            if (cs.branket_deepth > 1) {
                /* ignore */
                Cursor_iterEnd(&cs);
                continue;
            }

            last_stmt_i = cs.iter_index;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);

    Arg* mainStmt = arg_newStr("");
    Arg* lastStmt = arg_newStr("");
    {
        Cursor_forEach(cs, stmt) {
            Cursor_iterStart(&cs);
            if (cs.iter_index < last_stmt_i) {
                mainStmt = arg_strAppend(mainStmt, cs.token1.pyload);
            }
            if (cs.iter_index >= last_stmt_i) {
                lastStmt = arg_strAppend(lastStmt, cs.token1.pyload);
            }
            Cursor_iterEnd(&cs);
        }
        Cursor_deinit(&cs);
    }

    *stmt_p = strsCacheArg(outbuffs, mainStmt);
    return strsCacheArg(outbuffs, lastStmt);
}

static void _AST_parse_list(AST* ast, Args* buffs, char* stmt) {
#if !PIKA_BUILTIN_STRUCT_ENABLE
    return;
#endif
    AST_setNodeAttr(ast, (char*)"list", "list");
    char* subStmts = strsCut(buffs, stmt, '[', ']');
    subStmts = strsAppend(buffs, subStmts, ",");
    while (1) {
        char* subStmt = Parser_popSubStmt(buffs, &subStmts, ",");
        AST_parseSubStmt(ast, subStmt);
        if (strEqu(subStmts, "")) {
            break;
        }
    }
    return;
}

static void _AST_parse_dict(AST* ast, Args* buffs, char* stmt) {
#if !PIKA_BUILTIN_STRUCT_ENABLE
    return;
#endif
    AST_setNodeAttr(ast, (char*)"dict", "dict");
    char* subStmts = strsCut(buffs, stmt, '{', '}');
    subStmts = strsAppend(buffs, subStmts, ",");
    while (1) {
        char* subStmt = Parser_popSubStmt(buffs, &subStmts, ",");
        char* key = Parser_popSubStmt(buffs, &subStmt, ":");
        char* value = subStmt;
        AST_parseSubStmt(ast, key);
        AST_parseSubStmt(ast, value);
        if (strEqu(subStmts, "")) {
            break;
        }
    }
}

static void _AST_parse_slice(AST* ast, Args* buffs, char* stmt) {
#if !PIKA_SYNTAX_SLICE_ENABLE
    return;
#endif
    AST_setNodeAttr(ast, (char*)"slice", "slice");
    stmt = strsCopy(buffs, stmt);
    char* laststmt = Parser_popLastSubStmt(buffs, &stmt, "[");
    AST_parseSubStmt(ast, stmt);
    char* slice_list = strsCut(buffs, laststmt, '[', ']');
    pika_assert(slice_list != NULL);
    slice_list = strsAppend(buffs, slice_list, ":");
    int index = 0;
    while (1) {
        char* slice_str = Parser_popSubStmt(buffs, &slice_list, ":");
        if (index == 0 && strEqu(slice_str, "")) {
            AST_parseSubStmt(ast, "0");
        } else if (index == 1 && strEqu(slice_str, "")) {
            AST_parseSubStmt(ast, "-99999");
        } else {
            AST_parseSubStmt(ast, slice_str);
        }
        index++;
        if (strEqu("", slice_list)) {
            break;
        }
    }
}

char* Suger_not_in(Args* out_buffs, char* line) {
#if PIKA_NANO_ENABLE
    return line;
#endif
    char* ret = line;
    char* stmt1 = "";
    char* stmt2 = "";
    PIKA_BOOL got_not_in = PIKA_FALSE;
    PIKA_BOOL skip = PIKA_FALSE;
    Args buffs = {0};
    if (1 != Cursor_count(line, TOKEN_operator, " not ")) {
        ret = line;
        goto __exit;
    }
    if (1 != Cursor_count(line, TOKEN_operator, " in ")) {
        ret = line;
        goto __exit;
    }

    /* stmt1 not in stmt2 => not stmt1 in stmt2 */
    Cursor_forEach(cs, line) {
        Cursor_iterStart(&cs);
        if (!got_not_in) {
            if (strEqu(cs.token1.pyload, " not ") &&
                strEqu(cs.token2.pyload, " in ")) {
                got_not_in = PIKA_TRUE;
                Cursor_iterEnd(&cs);
                continue;
            }
            stmt1 = strsAppend(&buffs, stmt1, cs.token1.pyload);
        } else {
            if (!skip) {
                skip = PIKA_TRUE;
                Cursor_iterEnd(&cs);
                continue;
            }
            stmt2 = strsAppend(&buffs, stmt2, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (!got_not_in) {
        ret = line;
        goto __exit;
    }
    ret = strsFormat(out_buffs, strGetSize(line) + 3, " not %s in %s", stmt1,
                     stmt2);
    goto __exit;
__exit:
    strsDeinit(&buffs);
    return ret;
}

AST* AST_parseStmt(AST* ast, char* stmt) {
    Args buffs = {0};
    char* assignment = Cursor_splitCollect(&buffs, stmt, "(", 0);
    char* method = NULL;
    char* ref = NULL;
    char* str = NULL;
    char* num = NULL;
    char* left = NULL;
    char* right = NULL;
    char* import = NULL;
    PIKA_RES result = PIKA_RES_OK;

    right = stmt;
    /* solve check direct */
    uint8_t isLeftExist = 0;
    if (Parser_checkIsDirect(assignment)) {
        isLeftExist = 1;
        left = strsCopy(&buffs, "");
        right = strsCopy(&buffs, "");
        uint8_t is_meet_equ = 0;
        Cursor_forEach(cs, stmt) {
            Cursor_iterStart(&cs);
            if (!is_meet_equ && strEqu(cs.token1.pyload, "=") &&
                cs.token1.type == TOKEN_operator) {
                is_meet_equ = 1;
                Cursor_iterEnd(&cs);
                continue;
            }
            if (0 == is_meet_equ) {
                left = strsAppend(&buffs, left, cs.token1.pyload);
            }
            if (1 == is_meet_equ) {
                right = strsAppend(&buffs, right, cs.token1.pyload);
            }
            Cursor_iterEnd(&cs);
        }
        Cursor_deinit(&cs);
    }
    /* solve the += -= /= *= stmt */
    if (!isLeftExist) {
        isLeftExist = Suger_selfOperator(&buffs, stmt, &right, &left);
    }

    /* remove hint */
    if (isLeftExist) {
        left = Cursor_splitCollect(&buffs, left, ":", 0);
    }

    /* solve the [] stmt */
    right = Suger_leftSlice(&buffs, right, &left);
    right = Suger_format(&buffs, right);

    /* set left */
    if (isLeftExist) {
        AST_setNodeAttr(ast, (char*)"left", left);
    }
    /* match statment type */
    enum StmtType stmtType = Lexer_matchStmtType(right);
    if (STMT_tuple == stmtType) {
        right = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "(%s)", right);
        stmtType = STMT_method;
    }
    /* solve operator stmt */
    if (STMT_operator == stmtType) {
        right = Suger_not_in(&buffs, right);
        char* rightWithoutSubStmt = _remove_sub_stmt(&buffs, right);
        char* operator= Lexer_getOperator(&buffs, rightWithoutSubStmt);
        if (NULL == operator) {
            result = PIKA_RES_ERR_SYNTAX_ERROR;
            goto exit;
        }
        AST_setNodeAttr(ast, (char*)"operator", operator);
        char* rightBuff = strsCopy(&buffs, right);
        char* subStmt2 = Cursor_popLastToken(&buffs, &rightBuff, operator);
        char* subStmt1 = rightBuff;
        AST_parseSubStmt(ast, subStmt1);
        AST_parseSubStmt(ast, subStmt2);
        goto exit;
    }

    /* solve list stmt */
    if (STMT_list == stmtType) {
        _AST_parse_list(ast, &buffs, right);
        goto exit;
    }

    /* solve dict stmt */
    if (STMT_dict == stmtType) {
        _AST_parse_dict(ast, &buffs, right);
        goto exit;
    }

    /* solve method chain */
    if (STMT_chain == stmtType) {
        char* stmt = strsCopy(&buffs, right);
        char* lastStmt = Parser_popLastSubStmt(&buffs, &stmt, ".");
        AST_parseSubStmt(ast, stmt);
        AST_parseStmt(ast, lastStmt);
        goto exit;
    }

    if (STMT_slice == stmtType) {
        /* solve slice stmt */
        _AST_parse_slice(ast, &buffs, right);
        goto exit;
    }

    /* solve method stmt */
    if (STMT_method == stmtType) {
        method = strsGetFirstToken(&buffs, right, '(');
        AST_setNodeAttr(ast, (char*)"method", method);
        char* subStmts = strsCut(&buffs, right, '(', ')');
        if (NULL == subStmts) {
            result = PIKA_RES_ERR_SYNTAX_ERROR;
            goto exit;
        }
        /* add ',' at the end */
        subStmts = strsAppend(&buffs, subStmts, ",");
        while (1) {
            char* substmt = Parser_popSubStmt(&buffs, &subStmts, ",");
            AST_parseSubStmt(ast, substmt);
            if (strEqu("", subStmts)) {
                break;
            }
        }
        goto exit;
    }
    /* solve reference stmt */
    if (STMT_reference == stmtType) {
        ref = right;
        AST_setNodeAttr(ast, (char*)"ref", ref);
        goto exit;
    }
    /* solve import stmt */
    if (STMT_import == stmtType) {
        import = strsGetLastToken(&buffs, right, ' ');
        AST_setNodeAttr(ast, (char*)"import", import);
        goto exit;
    }
    /* solve str stmt */
    if (STMT_string == stmtType) {
        str = strsCopy(&buffs, right);
        /* remove the first char */
        str = str + 1;
        /* remove the last char */
        str[strGetSize(str) - 1] = '\0';
        /* replace */
        if (strIsContain(str, '\\')) {
            str = strsReplace(&buffs, str, "\\\"", "\"");
            str = strsReplace(&buffs, str, "\\'", "'");
        }
        AST_setNodeAttr(ast, (char*)"string", str);
        goto exit;
    }
    /* solve bytes stmt */
    if (STMT_bytes == stmtType) {
        str = right + 1;
        str = strsDeleteChar(&buffs, str, '\'');
        str = strsDeleteChar(&buffs, str, '\"');
        AST_setNodeAttr(ast, (char*)"bytes", str);
        goto exit;
    }
    /* solve number stmt */
    if (STMT_number == stmtType) {
        num = right;
        AST_setNodeAttr(ast, (char*)"num", num);
        goto exit;
    }
exit:
    strsDeinit(&buffs);
    if (result != PIKA_RES_OK) {
        AST_deinit(ast);
        return NULL;
    }
    return ast;
}

static int32_t _getSpaceNum(char* line) {
    uint32_t size = strGetSize(line);
    for (uint32_t i = 0; i < size; i++) {
        if (line[i] != ' ') {
            return i;
        }
    }
    return 0;
}

static int32_t Parser_getPyLineBlockDeepth(char* line) {
    int32_t spaceNum = _getSpaceNum(line);
    if (0 == spaceNum % 4) {
        return spaceNum / 4;
    }
    /* space Num is not 4N, error*/
    return -1;
}

char* Parser_removeComment(char* line) {
    uint8_t is_annotation_exit = 0;
    uint8_t is_in_single_quotes = 0;
    uint8_t is_in_pika_float_quotes_deepth = 0;
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        if ('\'' == line[i]) {
            is_in_single_quotes = !is_in_single_quotes;
            continue;
        }
        if ('"' == line[i]) {
            is_in_pika_float_quotes_deepth = !is_in_pika_float_quotes_deepth;
            continue;
        }
        if (!(is_in_single_quotes == 0 &&
              is_in_pika_float_quotes_deepth == 0)) {
            continue;
        }
        if ('#' == line[i]) {
            /* end the line */
            line[i] = 0;
            is_annotation_exit = 1;
            break;
        }
    }
    /* no annotation, exit */
    if (!is_annotation_exit) {
        return line;
    }
    /* check empty line */
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        if (' ' != line[i]) {
            return line;
        }
    }
    /* is an emply line */
    line = "@annontation";
    return line;
}

char* _defGetDefault(Args* outBuffs, char** psDeclearOut) {
#if PIKA_NANO_ENABLE
    return "";
#endif
    Args buffs = {0};
    char* sDeclear = strsCopy(&buffs, *psDeclearOut);
    char* sFnName = strsGetFirstToken(&buffs, sDeclear, '(');
    Arg* aDeclear = arg_strAppend(arg_newStr(sFnName), "(");
    Arg* aDefault = arg_newStr("");
    char* sArgList = strsCut(&buffs, sDeclear, '(', ')');
    char* sDefaultOut = NULL;
    pika_assert(NULL != sArgList);
    int argNum = Cursor_count(sArgList, TOKEN_devider, ",") + 1;
    for (int i = 0; i < argNum; i++) {
        char* sItem = Cursor_popToken(&buffs, &sArgList, ",");
        char* sDefaultVal = NULL;
        char* sDefaultKey = NULL;
        int is_default = 0;
        if (Cursor_isContain(sItem, TOKEN_operator, "=")) {
            /* has default value */
            sDefaultVal = Cursor_splitCollect(&buffs, sItem, "=", 1);
            sDefaultKey = Cursor_splitCollect(&buffs, sItem, "=", 0);
            sDefaultKey = Cursor_splitCollect(&buffs, sDefaultKey, ":", 0);
            aDefault = arg_strAppend(aDefault, sDefaultKey);
            aDefault = arg_strAppend(aDefault, "=");
            aDefault = arg_strAppend(aDefault, sDefaultVal);
            aDefault = arg_strAppend(aDefault, ",");
            is_default = 1;
        } else {
            sDefaultKey = sItem;
        }
        aDeclear = arg_strAppend(aDeclear, sDefaultKey);
        if (is_default) {
            aDeclear = arg_strAppend(aDeclear, "=");
        }
        aDeclear = arg_strAppend(aDeclear, ",");
    }
    strPopLastToken(arg_getStr(aDeclear), ',');
    aDeclear = arg_strAppend(aDeclear, ")");
    *psDeclearOut = strsCopy(outBuffs, arg_getStr(aDeclear));
    sDefaultOut = strsCopy(outBuffs, arg_getStr(aDefault));
    strPopLastToken(sDefaultOut, ',');
    arg_deinit(aDeclear);
    arg_deinit(aDefault);
    strsDeinit(&buffs);
    return sDefaultOut;
}

static char* Suger_multiReturn(Args* out_buffs, char* line) {
#if PIKA_NANO_ENABLE
    return line;
#endif
    Cursor_forEach(cs, line) {
        Cursor_iterStart(&cs);
        if (cs.branket_deepth == 0 && strEqu(cs.token1.pyload, ",")) {
            line = strsFormat(out_buffs, strGetSize(line) + 3, "(%s)", line);
            Cursor_iterEnd(&cs);
            break;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    return line;
}

/* match block start keywords */
const char control_keywords[][9] = {"break", "continue"};

/* normal keyward */
const char normal_keywords[][7] = {"while", "if", "elif"};

AST* AST_parseLine_withBlockStack_withBlockDeepth(char* line,
                                                  Stack* tBlockStack,
                                                  int block_deepth) {
    Stack s;
    stack_init(&s);
    if (tBlockStack == NULL) {
        tBlockStack = &s;
    }
    /* line is not exist */
    if (line == NULL) {
        return NULL;
    }
    /* init data */
    AST* ast = New_queueObj();
    Args buffs = {0};
    int8_t block_deepth_now, block_deepth_last = -1;
    char *line_start, *stmt;
    /* get block deepth */
    block_deepth_now = Parser_getPyLineBlockDeepth(line);
    /* set block deepth */
    if (block_deepth_now == -1) {
        /* get block_deepth error */
        pika_platform_printf(
            "IndentationError: unexpected indent, only support 4 "
            "spaces\r\n");
        obj_deinit(ast);
        ast = NULL;
        goto exit;
    }
    block_deepth_now += block_deepth;
    obj_setInt(ast, "blockDeepth", block_deepth_now);

    /* check if exit block */
    block_deepth_last = stack_getTop(tBlockStack) + block_deepth;
    /* exit each block */
    for (int i = 0; i < block_deepth_last - block_deepth_now; i++) {
        QueueObj* exit_block_queue = obj_getObj(ast, "exitBlock");
        /* create an exit_block queue */
        if (NULL == exit_block_queue) {
            obj_newObj(ast, "exitBlock", "", New_TinyObj);
            exit_block_queue = obj_getObj(ast, "exitBlock");
            queueObj_init(exit_block_queue);
        }
        char buff[10] = {0};
        char* block_type = stack_popStr(tBlockStack, buff);
        /* push exit block type to exit_block queue */
        queueObj_pushStr(exit_block_queue, block_type);
    }

    line_start = line + (block_deepth_now - block_deepth) * 4;
    stmt = line_start;

    // "while" "if" "elif"
    for (uint32_t i = 0; i < sizeof(normal_keywords) / 7; i++) {
        char* keyword = (char*)normal_keywords[i];
        uint8_t keyword_len = strGetSize(keyword);
        if (strIsStartWith(line_start, keyword) &&
            (line_start[keyword_len] == ' ')) {
            stmt = strsCut(&buffs, line_start, ' ', ':');
            AST_setNodeBlock(ast, keyword);
            stack_pushStr(tBlockStack, keyword);
            goto block_matched;
        }
    }

    /* contral keyward */
    /* "break", "continue" */
    for (uint32_t i = 0; i < sizeof(control_keywords) / 8; i++) {
        char* keyward = (char*)control_keywords[i];
        uint8_t keyward_size = strGetSize(keyward);
        if ((strIsStartWith(line_start, keyward)) &&
            ((line_start[keyward_size] == ' ') ||
             (line_start[keyward_size] == 0))) {
            AST_setNodeAttr(ast, keyward, "");
            stmt = "";
            goto block_matched;
        }
    }

    /* for */
    if (strIsStartWith(line_start, "for ")) {
        Args* list_buffs = New_strBuff();
        char* line_buff = strsCopy(list_buffs, line_start + 4);
        line_buff = Cursor_getCleanStmt(list_buffs, line_buff);
        if (strCountSign(line_buff, ':') < 1) {
            args_deinit(list_buffs);
            obj_deinit(ast);
            ast = NULL;
            goto exit;
        }
        char* arg_in = strsPopToken(list_buffs, &line_buff, ' ');
        AST_setNodeAttr(ast, "arg_in", arg_in);
        strsPopToken(list_buffs, &line_buff, ' ');
        char* list_in = Cursor_splitCollect(list_buffs, line_buff, ":", 0);
        list_in = strsAppend(list_buffs, "iter(", list_in);
        list_in = strsAppend(list_buffs, list_in, ")");
        list_in = strsCopy(&buffs, list_in);
        args_deinit(list_buffs);
        AST_setNodeBlock(ast, "for");
        AST_setNodeAttr(ast, "list_in", list_in);
        stack_pushStr(tBlockStack, "for");
        stmt = list_in;
        goto block_matched;
    }

    /* else */
    if (strIsStartWith(line_start, "else")) {
        if ((line_start[4] == ' ') || (line_start[4] == ':')) {
            stmt = "";
            AST_setNodeBlock(ast, "else");
            stack_pushStr(tBlockStack, "else");
        }
        goto block_matched;
    }

#if PIKA_SYNTAX_EXCEPTION_ENABLE
    /* try */
    if (strIsStartWith(line_start, "try")) {
        if ((line_start[3] == ' ') || (line_start[3] == ':')) {
            stmt = "";
            AST_setNodeBlock(ast, "try");
            stack_pushStr(tBlockStack, "try");
        }
        goto block_matched;
    }

    /* except */
    if (strIsStartWith(line_start, "except")) {
        if ((line_start[6] == ' ') || (line_start[6] == ':')) {
            stmt = "";
            AST_setNodeBlock(ast, "except");
            stack_pushStr(tBlockStack, "except");
        }
        goto block_matched;
    }
#endif

    if (strEqu(line_start, "return")) {
        AST_setNodeAttr(ast, "return", "");
        stmt = "";
        goto block_matched;
    }
    if (strIsStartWith(line_start, "return ")) {
        char* lineBuff = strsCopy(&buffs, line_start);
        strsPopToken(&buffs, &lineBuff, ' ');
        stmt = lineBuff;
        stmt = Suger_multiReturn(&buffs, stmt);
        AST_setNodeAttr(ast, "return", "");
        goto block_matched;
    }

#if PIKA_SYNTAX_EXCEPTION_ENABLE
    if (strEqu(line_start, "raise")) {
        AST_setNodeAttr(ast, "raise", "");
        stmt = "RuntimeError";
        goto block_matched;
    }
    if (strIsStartWith(line_start, "raise ")) {
        AST_setNodeAttr(ast, "raise", "");
        char* lineBuff = strsCopy(&buffs, line_start);
        strsPopToken(&buffs, &lineBuff, ' ');
        stmt = lineBuff;
        if (strEqu("", stmt)) {
            stmt = "RuntimeError";
        }
        goto block_matched;
    }
    /* assert */
    if (strIsStartWith(line_start, "assert ")) {
        stmt = "";
        AST_setNodeAttr(ast, "assert", "");
        char* lineBuff = strsCopy(&buffs, line_start + 7);
        /* assert expr [, msg] */
        while (1) {
            char* subStmt = Parser_popSubStmt(&buffs, &lineBuff, ",");
            AST_parseSubStmt(ast, subStmt);
            if (strEqu(lineBuff, "")) {
                break;
            }
        }
        goto block_matched;
    }
#endif

    if (strIsStartWith(line_start, "global ")) {
        stmt = "";
        char* global_list = line_start + 7;
        global_list = Cursor_getCleanStmt(&buffs, global_list);
        AST_setNodeAttr(ast, "global", global_list);
        goto block_matched;
    }
    if (strIsStartWith(line_start, "del ") ||
        strIsStartWith(line_start, "del(")) {
        stmt = "";
        char* del_dir = NULL;
        if (line_start[3] == '(') {
            del_dir = strsCut(&buffs, line_start, '(', ')');
        } else {
            del_dir = line_start + sizeof("del ") - 1;
        }
        del_dir = Cursor_getCleanStmt(&buffs, del_dir);
        AST_setNodeAttr(ast, "del", del_dir);
        goto block_matched;
    }
    if (strIsStartWith(line_start, (char*)"def ")) {
        stmt = "";
        char* sDeclare = strsCut(&buffs, line_start, ' ', ':');
        if (NULL == sDeclare) {
            obj_deinit(ast);
            ast = NULL;
            goto exit;
        }
        sDeclare = Cursor_getCleanStmt(&buffs, sDeclare);
        if (!strIsContain(sDeclare, '(') || !strIsContain(sDeclare, ')')) {
            obj_deinit(ast);
            ast = NULL;
            goto exit;
        }
        char* sDefaultStmt = _defGetDefault(&buffs, &sDeclare);
        AST_setNodeBlock(ast, "def");
        AST_setNodeAttr(ast, "declare", sDeclare);
        if (sDefaultStmt[0] != '\0') {
            AST_setNodeAttr(ast, "default", sDefaultStmt);
        }
        stack_pushStr(tBlockStack, "def");
        goto block_matched;
    }
    if (strIsStartWith(line_start, (char*)"class ")) {
        stmt = "";
        char* declare = strsCut(&buffs, line_start, ' ', ':');
        if (NULL == declare) {
            obj_deinit(ast);
            ast = NULL;
            goto exit;
        }
        declare = Cursor_getCleanStmt(&buffs, declare);
        AST_setNodeBlock(ast, "class");
        AST_setNodeAttr(ast, "declare", declare);
        stack_pushStr(tBlockStack, "class");
        goto block_matched;
    }

block_matched:
    if (NULL == stmt) {
        AST_deinit(ast);
        ast = NULL;
        goto exit;
    }
    stmt = Cursor_getCleanStmt(&buffs, stmt);
    ast = AST_parseStmt(ast, stmt);
    goto exit;
exit:
    stack_deinit(&s);
    strsDeinit(&buffs);
    return ast;
}

static AST* AST_parseLine_withBlockStack(char* line, Stack* block_stack) {
    return AST_parseLine_withBlockStack_withBlockDeepth(line, block_stack, 0);
}

static AST* AST_parseLine_withBlockDeepth(char* line, int block_deepth) {
    return AST_parseLine_withBlockStack_withBlockDeepth(line, NULL,
                                                        block_deepth);
}

int AST_getBlockDeepthNow(AST* ast) {
    return obj_getInt(ast, "blockDeepth");
}

AST* AST_parseLine(char* line) {
    return AST_parseLine_withBlockStack(line, NULL);
}

static char* Suger_import_as(Args* out_buffs, char* line) {
#if !PIKA_SYNTAX_IMPORT_EX_ENABLE
    return line;
#endif
    Args buffs = {0};
    char* line_out = line;
    char* alias = NULL;
    char* origin = NULL;
    char* stmt = line + 7;

    /* not import, exit */
    if (!strIsStartWith(line, "import ")) {
        line_out = line;
        goto exit;
    }

    if (!Cursor_isContain(stmt, TOKEN_operator, " as ")) {
        line_out = line;
        goto exit;
    }

    /* {origin} as {alias} */
    origin = Cursor_popToken(&buffs, &stmt, " as ");
    alias = stmt;

    /* 'import' and 'as' */
    line_out = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "import %s\n%s = %s",
                          origin, alias, origin);
exit:
    return strsReturnOut(&buffs, out_buffs, line_out);
}

static PIKA_BOOL _check_is_multi_assign(char* arg_list) {
#if PIKA_NANO_ENABLE
    return PIKA_FALSE;
#endif
    PIKA_BOOL res = PIKA_FALSE;
    Cursor_forEach(cs, arg_list) {
        Cursor_iterStart(&cs);
        if ((cs.branket_deepth == 0 && strEqu(cs.token1.pyload, ","))) {
            res = PIKA_TRUE;
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    return res;
}

static char* Suger_multiAssign(Args* out_buffs, char* line) {
#if PIKA_NANO_ENABLE
    return line;
#endif
    Args buffs = {0};
    char* line_out = line;
    int space_num = _getSpaceNum(line);
    PIKA_BOOL is_assign = PIKA_FALSE;
    Arg* stmt = arg_newStr("");
    Arg* out_list = arg_newStr("");
    Arg* out_item = arg_newStr("");
    Arg* line_out_arg = arg_newStr("");
    char* line_item = NULL;
    char* out_list_str = NULL;
    int out_num = 0;
    Cursor_forEach(cs, line) {
        Cursor_iterStart(&cs);
        if (cs.branket_deepth == 0 && strEqu(cs.token1.pyload, "=")) {
            is_assign = PIKA_TRUE;
            Cursor_iterEnd(&cs);
            continue;
        }
        if (is_assign) {
            stmt = arg_strAppend(stmt, cs.token1.pyload);
        }
        if (!is_assign) {
            out_list = arg_strAppend(out_list, cs.token1.pyload);
        }
        Cursor_iterEnd(&cs);
    }
    Cursor_deinit(&cs);
    if (!is_assign) {
        line_out = line;
        goto exit;
    }

    if (!_check_is_multi_assign(arg_getStr(out_list))) {
        line_out = line;
        goto exit;
    }

    line_item =
        strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "$tmp= %s\n", arg_getStr(stmt));

    /* add space */
    for (int i = 0; i < space_num; i++) {
        line_out_arg = arg_strAppend(line_out_arg, " ");
    }
    line_out_arg = arg_strAppend(line_out_arg, line_item);

    out_list_str = arg_getStr(out_list);
    while (1) {
        char* item = Cursor_popToken(&buffs, &out_list_str, ",");
        if (item[0] == '\0') {
            break;
        }
        char* line_item = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE,
                                     "%s = $tmp[%d]\n", item, out_num);
        /* add space */
        for (int i = 0; i < space_num; i++) {
            line_out_arg = arg_strAppend(line_out_arg, " ");
        }
        line_out_arg = arg_strAppend(line_out_arg, line_item);
        out_num++;
    }
    /* add space */
    for (int i = 0; i < space_num; i++) {
        line_out_arg = arg_strAppend(line_out_arg, " ");
    }
    line_out_arg = arg_strAppend(line_out_arg, "del $tmp");

    line_out = strsCopy(out_buffs, arg_getStr(line_out_arg));
exit:
    arg_deinit(stmt);
    arg_deinit(out_list);
    arg_deinit(out_item);
    arg_deinit(line_out_arg);
    strsDeinit(&buffs);
    return line_out;
}

static char* Suger_from_import_as(Args* buffs_p, char* line) {
#if !PIKA_SYNTAX_IMPORT_EX_ENABLE
    return line;
#endif
    Args buffs = {0};
    char* line_out = line;
    char* class = NULL;
    char* module = NULL;
    char* alias = NULL;
    char* stmt = line + 5;
    char* class_after = "";

    if (!strIsStartWith(line, "from ")) {
        line_out = line;
        goto exit;
    }

    module = Cursor_popToken(&buffs, &stmt, " import ");
    if (!Cursor_isContain(stmt, TOKEN_operator, " as ")) {
        class = stmt;
    } else {
        class = Cursor_popToken(&buffs, &stmt, " as ");
        alias = stmt;
    }

    if (NULL == alias) {
        alias = class;
    }

    /* skip PikaObj */
    if (strEqu(module, "PikaObj")) {
        line_out = strsCopy(buffs_p, "");
        goto exit;
    }

    while (1) {
        char* class_item = Cursor_popToken(&buffs, &class, ",");
        if (class_item[0] == '\0') {
            break;
        }
        class_item = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "%s.%s,", module,
                                class_item);
        class_after = strsAppend(&buffs, class_after, class_item);
    }
    class_after[strlen(class_after) - 1] = '\0';
    class = class_after;

    line_out = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE, "import %s\n%s = %s",
                          module, alias, class);
    line_out = strsCopy(buffs_p, line_out);
exit:
    strsDeinit(&buffs);
    return line_out;
}

static char* Suger_import(Args* outbuffs, char* line) {
#if !PIKA_SYNTAX_IMPORT_EX_ENABLE
    return line;
#endif
    line = Suger_import_as(outbuffs, line);
    line = Suger_from_import_as(outbuffs, line);
    Arg* line_buff = arg_newStr("");
    while (1) {
        char* single_line = strPopFirstToken(&line, '\n');
        if (single_line[0] == '\0') {
            break;
        }
        if (strIsStartWith(single_line, "import ")) {
            if (strIsContain(single_line, ',')) {
                single_line = single_line + 7;
                while (1) {
                    char* single_import = strPopFirstToken(&single_line, ',');
                    if (single_import[0] == '\0') {
                        break;
                    }
                    line_buff = arg_strAppend(line_buff, "import ");
                    line_buff = arg_strAppend(line_buff, single_import);
                    line_buff = arg_strAppend(line_buff, "\n");
                }
                char* line_after = arg_getStr(line_buff);
                line_after[strlen(line_after) - 1] = '\0';
            }
        }
        line_buff = arg_strAppend(line_buff, single_line);
        line_buff = arg_strAppend(line_buff, "\n");
    }
    char* line_after = arg_getStr(line_buff);
    line_after[strlen(line_after) - 1] = '\0';
    line_after = strsCopy(outbuffs, line_after);
    arg_deinit(line_buff);
    return line_after;
}

static char* Parser_linePreProcess(Args* outbuffs, char* line) {
    line = Parser_removeComment(line);
    Arg* line_buff = NULL;
    int line_num = 0;
    /* check syntex error */
    if (Lexer_isError(line)) {
        line = NULL;
        goto exit;
    }
    /* process EOL */
    line = strsDeleteChar(outbuffs, line, '\r');
    /* process import */
    line = Suger_import(outbuffs, line);

    /* process multi assign */
    line_num = strCountSign(line, '\n') + 1;
    line_buff = arg_newStr("");
    for (int i = 0; i < line_num; i++) {
        if (i > 0) {
            line_buff = arg_strAppend(line_buff, "\n");
        }
        char* single_line = strsPopToken(outbuffs, &line, '\n');
        single_line = Suger_multiAssign(outbuffs, single_line);
        line_buff = arg_strAppend(line_buff, single_line);
    }
    line = strsCopy(outbuffs, arg_getStr(line_buff));
exit:
    if (NULL != line_buff) {
        arg_deinit(line_buff);
    }
    return line;
}

char* Parser_LineToAsm(Args* buffs_p, char* line, Stack* blockStack) {
    char* ASM = NULL;
    AST* ast = NULL;
    uint8_t line_num = 0;
    /* pre process */
    line = Parser_linePreProcess(buffs_p, line);
    if (NULL == line) {
        /* preprocess error */
        goto exit;
    }
    if (strEqu("@annontation", line)) {
        ASM = "";
        goto exit;
    }
    /*
        solve more lines
        preprocess may generate more lines
    */
    line_num = strCountSign(line, '\n') + 1;
    for (int i = 0; i < line_num; i++) {
        char* single_line = strsPopToken(buffs_p, &line, '\n');
        /* parse line to AST */
        ast = AST_parseLine_withBlockStack(single_line, blockStack);
        /* gen ASM from AST */
        if (ASM == NULL) {
            ASM = AST_genAsm(ast, buffs_p);
        } else {
            ASM = strsAppend(buffs_p, ASM, AST_genAsm(ast, buffs_p));
        }
        if (NULL != ast) {
            AST_deinit(ast);
        }
    }
exit:
    return ASM;
}

static int Parser_isVoidLine(char* line) {
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        if (line[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

static uint8_t Parser_checkIsMultiComment(char* line, uint8_t* pbIsOneLine) {
    uint8_t bIsMultiComment = 0;
    uint32_t i = 0;
    uint32_t uLineSize = strGetSize(line);
    while (i + 2 < uLineSize) {
        /* not match ' or " */
        if ((line[i] != '\'') && (line[i] != '"')) {
            i++;
            continue;
        }
        /* not match ''' or """ */
        if (!((line[i + 1] == line[i]) && (line[i + 2] == line[i]))) {
            i++;
            continue;
        }
        if (bIsMultiComment) {
            *pbIsOneLine = 1;
        }
        bIsMultiComment = 1;
        i += 3;
    }
    return bIsMultiComment;
}

static char* _Parser_linesToBytesOrAsm(Args* outBuffs,
                                       ByteCodeFrame* bytecode_frame,
                                       char* sPyLines) {
    Stack tBlockStack;
    stack_init(&tBlockStack);
    Arg* aAsm = arg_newStr("");
    uint32_t uLinesOffset = 0;
    uint16_t uLinesNum = strCountSign(sPyLines, '\n') + 1;
    uint16_t uLinesIndex = 0;
    uint8_t bIsInMultiComment = 0;
    uint8_t bIsOneLineMultiComment = 0;
    Arg* aLineConnection = arg_newStr("");
    uint8_t bIsLineConnection = 0;
    char* sOutASM = NULL;
    char* sSingleASM = NULL;
    uint32_t uLineSize = 0;
    /* parse each line */
    while (1) {
        uLinesIndex++;
        Args buffs = {0};
        char* sLineOrigin = NULL;
        char* sLine = NULL;

        /* add void line to the end */
        if (uLinesIndex >= uLinesNum + 1) {
            sLine = "";
            goto parse_line;
        }

        /* get single line by pop multiline */
        sLineOrigin = strsGetFirstToken(&buffs, sPyLines + uLinesOffset, '\n');

        sLine = strsCopy(&buffs, sLineOrigin);

        /* line connection */
        if (bIsLineConnection) {
            bIsLineConnection = 0;
            aLineConnection = arg_strAppend(aLineConnection, sLine);
            sLine = strsCopy(&buffs, arg_getStr(aLineConnection));
            /* reflash the line_connection_arg */
            arg_deinit(aLineConnection);
            aLineConnection = arg_newStr("");
        }

        /* check connection */
        if ('\\' == sLine[strGetSize(sLine) - 1]) {
            /* remove the '\\' */
            sLine[strGetSize(sLine) - 1] = '\0';
            bIsLineConnection = 1;
            aLineConnection = arg_strAppend(aLineConnection, sLine);
            goto next_line;
        }

        /* filter for not end \n */
        if (Parser_isVoidLine(sLine)) {
            goto next_line;
        }

        /* filter for multiline comment ''' or """ */
        if (Parser_checkIsMultiComment(sLine, &bIsOneLineMultiComment)) {
            bIsInMultiComment = ~bIsInMultiComment;
            /* skip one line multiline comment */
            if (bIsOneLineMultiComment) {
                bIsInMultiComment = 0;
                bIsOneLineMultiComment = 0;
            }
            goto next_line;
        }

        /* skip multiline comment */
        if (bIsInMultiComment) {
            goto next_line;
        }

        /* support Tab */
        sLine = strsReplace(&buffs, sLine, "\t", "    ");
        /* remove \r */
        sLine = strsReplace(&buffs, sLine, "\r", "");

        /* check auto connection */
        Cursor_forEach(c, sLine) {
            Cursor_iterStart(&c);
            Cursor_iterEnd(&c);
        }
        Cursor_deinit(&c);
        /* auto connection */
        if (uLinesIndex < uLinesNum) {
            if (c.branket_deepth > 0) {
                aLineConnection = arg_strAppend(aLineConnection, sLine);
                bIsLineConnection = 1;
                goto next_line;
            }
        }

        /* branket match failed */
        if (c.branket_deepth != 0) {
            sSingleASM = NULL;
            goto parse_after;
        }

    parse_line:
        /* parse single Line to Asm */
        sSingleASM = Parser_LineToAsm(&buffs, sLine, &tBlockStack);
    parse_after:
        if (NULL == sSingleASM) {
            sOutASM = NULL;
            pika_platform_printf(
                "----------[%d]----------\r\n%s\r\n-------------------------"
                "\r\n",
                uLinesIndex, sLine);
            strsDeinit(&buffs);
            goto exit;
        }

        if (NULL == bytecode_frame) {
            /* store ASM */
            aAsm = arg_strAppend(aAsm, sSingleASM);
        } else if (NULL == outBuffs) {
            /* store ByteCode */
            byteCodeFrame_appendFromAsm(bytecode_frame, sSingleASM);
        }

    next_line:
        if (uLinesIndex < uLinesNum) {
            uLineSize = strGetSize(sLineOrigin);
            uLinesOffset = uLinesOffset + uLineSize + 1;
        }
        strsDeinit(&buffs);

        /* exit when finished */
        if (uLinesIndex >= uLinesNum + 1) {
            break;
        }
    }
    if (NULL != outBuffs) {
        /* load stored ASM */
        sOutASM = strsCopy(outBuffs, arg_getStr(aAsm));
    } else {
        sOutASM = (char*)1;
    }
    goto exit;
exit:
    if (NULL != aAsm) {
        arg_deinit(aAsm);
    }
    if (NULL != aLineConnection) {
        arg_deinit(aLineConnection);
    }
    stack_deinit(&tBlockStack);
    return sOutASM;
};

PIKA_RES Parser_linesToBytes(ByteCodeFrame* bf, char* py_lines) {
#if PIKA_BYTECODE_ONLY_ENABLE
    pika_platform_printf(
        "Error: In bytecode-only mode, can not parse python script.\r\n");
    pika_platform_printf(
        " Note: Please check PIKA_BYTECODE_ONLY_ENABLE config.\r\n");
    return PIKA_RES_ERR_SYNTAX_ERROR;
#else
    if (1 == (uintptr_t)_Parser_linesToBytesOrAsm(NULL, bf, py_lines)) {
        return PIKA_RES_OK;
    }
    return PIKA_RES_ERR_SYNTAX_ERROR;
#endif
}

char* Parser_linesToAsm(Args* outBuffs, char* multi_line) {
    return _Parser_linesToBytesOrAsm(outBuffs, NULL, multi_line);
}

char* Parser_fileToAsm(Args* outBuffs, char* filename) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, filename);
    pika_assert(NULL != file_arg);
    if (NULL == file_arg) {
        return NULL;
    }
    char* lines = (char*)arg_getBytes(file_arg);
    /* replace the "\r\n" to "\n" */
    lines = strsReplace(&buffs, lines, "\r\n", "\n");
    /* clear the void line */
    lines = strsReplace(&buffs, lines, "\n\n", "\n");
    /* add '\n' at the end */
    lines = strsAppend(&buffs, lines, "\n\n");
    char* res = Parser_linesToAsm(&buffs, lines);
    if (NULL == res) {
        goto __exit;
    }
    res = strsCopy(outBuffs, res);
__exit:
    arg_deinit(file_arg);
    strsDeinit(&buffs);
    return res;
}

char* AST_genAsm_sub(AST* ast, AST* subAst, Args* outBuffs, char* pikaAsm) {
    int deepth = obj_getInt(ast, "deepth");
    Args buffs = {0};
    /* append each queue item */
    while (1) {
        QueueObj* subStmt = queueObj_popObj(subAst);
        if (NULL == subStmt) {
            break;
        }
        obj_setInt(ast, "deepth", deepth + 1);
        pikaAsm = AST_genAsm_sub(ast, subStmt, &buffs, pikaAsm);
    }

    /* Byte code generate rules */
    const GenRule rules_subAst[] = {
        {.ins = "RUN", .type = VAL_DYNAMIC, .ast = "method"},
        {.ins = "OPT", .type = VAL_DYNAMIC, .ast = "operator"},
        {.ins = "BYT", .type = VAL_DYNAMIC, .ast = "bytes"},
        {.ins = "NUM", .type = VAL_DYNAMIC, .ast = "num"},
        {.ins = "IMP", .type = VAL_DYNAMIC, .ast = "import"},
        {.ins = "REF", .type = VAL_DYNAMIC, .ast = "ref"},
        {.ins = "STR", .type = VAL_DYNAMIC, .ast = "string"},
        {.ins = "SLC", .type = VAL_NONEVAL, .ast = "slice"},
        {.ins = "DCT", .type = VAL_NONEVAL, .ast = "dict"},
        {.ins = "LST", .type = VAL_NONEVAL, .ast = "list"},
        {.ins = "OUT", .type = VAL_DYNAMIC, .ast = "left"}};

    char* buff = args_getBuff(&buffs, PIKA_SPRINTF_BUFF_SIZE);

    /* append the syntax item */
    for (size_t i = 0; i < sizeof(rules_subAst) / sizeof(GenRule); i++) {
        GenRule rule = rules_subAst[i];
        char* astNodeVal = obj_getStr(subAst, rule.ast);
        if (NULL != astNodeVal) {
            /* e.g. "0 RUN print \n" */
            pika_platform_sprintf(buff, "%d %s ", deepth, rule.ins);
            Arg* abuff = arg_newStr(buff);
            if (rule.type == VAL_DYNAMIC) {
                abuff = arg_strAppend(abuff, astNodeVal);
            }
            abuff = arg_strAppend(abuff, "\n");
            pikaAsm = strsAppend(&buffs, pikaAsm, arg_getStr(abuff));
            arg_deinit(abuff);
        }
    }

    obj_setInt(ast, "deepth", deepth - 1);
    goto exit;
exit:
    pikaAsm = strsCopy(outBuffs, pikaAsm);
    strsDeinit(&buffs);
    return pikaAsm;
}

char* ASM_addBlockDeepth(AST* ast,
                         Args* buffs_p,
                         char* pikaAsm,
                         uint8_t deepthOffset) {
    pikaAsm = strsAppend(buffs_p, pikaAsm, (char*)"B");
    char buff[11];
    pikaAsm =
        strsAppend(buffs_p, pikaAsm,
                   fast_itoa(buff, AST_getBlockDeepthNow(ast) + deepthOffset));
    pikaAsm = strsAppend(buffs_p, pikaAsm, (char*)"\n");
    return pikaAsm;
}

char* GenRule_toAsm(GenRule rule,
                    Args* buffs,
                    AST* ast,
                    char* pikaAsm,
                    int deepth) {
    char* buff = args_getBuff(buffs, PIKA_SPRINTF_BUFF_SIZE);
    /* parse stmt ast */
    pikaAsm = AST_genAsm_sub(ast, ast, buffs, pikaAsm);
    /* e.g. "0 CTN \n" */
    pika_platform_sprintf(buff, "%d %s ", deepth, rule.ins);
    Arg* abuff = arg_newStr(buff);
    if (rule.type == VAL_DYNAMIC) {
        abuff = arg_strAppend(abuff, obj_getStr(ast, rule.ast));
    }
    if (rule.type == VAL_STATIC_) {
        abuff = arg_strAppend(abuff, rule.val);
    }
    abuff = arg_strAppend(abuff, "\n");
    pikaAsm = strsAppend(buffs, pikaAsm, arg_getStr(abuff));
    arg_deinit(abuff);
    return pikaAsm;
}

char* AST_genAsm(AST* oAST, Args* outBuffs) {
    const GenRule rules_topAst[] = {
        {.ins = "CTN", .type = VAL_NONEVAL, .ast = "continue"},
        {.ins = "BRK", .type = VAL_NONEVAL, .ast = "break"},
        {.ins = "DEL", .type = VAL_DYNAMIC, .ast = "del"},
        {.ins = "GLB", .type = VAL_DYNAMIC, .ast = "global"},
        {.ins = "RIS", .type = VAL_DYNAMIC, .ast = "raise"},
        {.ins = "ASS", .type = VAL_NONEVAL, .ast = "assert"},
        {.ins = "RET", .type = VAL_NONEVAL, .ast = "return"}};

    /* generate code for block ast */
    const GenRule rules_block[] = {
        {.ins = "TRY", .type = VAL_NONEVAL, .ast = "try"},
        {.ins = "EXP", .type = VAL_NONEVAL, .ast = "except"},
        {.ins = "NEL", .type = VAL_STATIC_, .ast = "else", .val = "1"},
        {.ins = "JEZ", .type = VAL_STATIC_, .ast = "if", .val = "1"},
        {.ins = "JEZ", .type = VAL_STATIC_, .ast = "while", .val = "2"},
    };

    Args buffs = {0};
    char* sPikaAsm = strsCopy(&buffs, "");
    QueueObj* exitBlock;
    uint8_t is_block_matched;
    if (NULL == oAST) {
        sPikaAsm = NULL;
        goto exit;
    }
    exitBlock = obj_getObj(oAST, "exitBlock");
    /* exiting from block */
    if (exitBlock != NULL) {
        while (1) {
            uint8_t block_type_num = obj_getInt(exitBlock, "top") -
                                     obj_getInt(exitBlock, "bottom") - 1;
            char* block_type = queueObj_popStr(exitBlock);
            if (NULL == block_type) {
                break;
            }
            /* goto the while start when exit while block */
            if (strEqu(block_type, "while")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              block_type_num);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 JMP -1\n");
            }
#if PIKA_SYNTAX_EXCEPTION_ENABLE
            /* goto the while start when exit while block */
            if (strEqu(block_type, "try")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              block_type_num);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 NTR \n");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 GER \n");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 JEZ 2\n");
            }
#endif
            /* goto the while start when exit while block */
            if (strEqu(block_type, "for")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              block_type_num);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 JMP -1\n");
                /* garbage collect for the list */
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              block_type_num);
                char _l_x[] = "$lx";
                char block_deepth_char =
                    AST_getBlockDeepthNow(oAST) + block_type_num + '0';
                _l_x[sizeof(_l_x) - 2] = block_deepth_char;
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 DEL ");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)_l_x);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"\n");
            }
            /* return when exit method */
            if (strEqu(block_type, "def")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              block_type_num + 1);
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 RET \n");
            }
            /* return when exit class */
            if (strEqu(block_type, "class")) {
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm,
                                              block_type_num + 1);
                sPikaAsm =
                    strsAppend(outBuffs, sPikaAsm, (char*)"0 RAS $origin\n");
                sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 1);
                sPikaAsm =
                    strsAppend(outBuffs, sPikaAsm, (char*)"0 NEW self\n");
                sPikaAsm = strsAppend(outBuffs, sPikaAsm, (char*)"0 RET \n");
            }
        }
    }
    /* add block deepth */
    /* example: B0 */
    sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 0);

    /* "deepth" is invoke deepth, not the blockDeepth */
    obj_setInt(oAST, "deepth", 0);

    /* match block */
    is_block_matched = 0;
    if (strEqu(AST_getThisBlock(oAST), "for")) {
        /* for "for" iter */
        char* arg_in = obj_getStr(oAST, "arg_in");
#if !PIKA_NANO_ENABLE
        char* arg_in_kv = NULL;
#endif
        Arg* newAsm_arg = arg_newStr("");
        char _l_x[] = "$lx";
        char block_deepth_char = '0';
        block_deepth_char += AST_getBlockDeepthNow(oAST);
        _l_x[sizeof(_l_x) - 2] = block_deepth_char;
        /* init iter */
        /*     get the iter(_l<x>) */
        sPikaAsm = AST_genAsm_sub(oAST, oAST, &buffs, sPikaAsm);
        newAsm_arg = arg_strAppend(newAsm_arg, "0 OUT ");
        newAsm_arg = arg_strAppend(newAsm_arg, _l_x);
        newAsm_arg = arg_strAppend(newAsm_arg, "\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, arg_getStr(newAsm_arg));
        arg_deinit(newAsm_arg);
        newAsm_arg = arg_newStr("");
        /* get next */
        /*     run next(_l<x>) */
        /*     check item is exist */
        /*
            $n = $lx.next()
            EST $n
            k, v = $n
            DEL $n
        */

#if !PIKA_NANO_ENABLE
        if (_check_is_multi_assign(arg_in)) {
            arg_in_kv = arg_in;
            arg_in = "$tmp";
        }
#endif

        sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 0);
        newAsm_arg = arg_strAppend(newAsm_arg, "0 RUN ");
        newAsm_arg = arg_strAppend(newAsm_arg, _l_x);
        newAsm_arg = arg_strAppend(newAsm_arg,
                                   ".__next__\n"
                                   "0 OUT ");
        newAsm_arg = arg_strAppend(newAsm_arg, arg_in);
        newAsm_arg = arg_strAppend(newAsm_arg,
                                   "\n"
                                   "0 EST ");
        newAsm_arg = arg_strAppend(newAsm_arg, arg_in);
        newAsm_arg = arg_strAppend(newAsm_arg, "\n0 JEZ 2\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, arg_getStr(newAsm_arg));
        arg_deinit(newAsm_arg);

#if !PIKA_NANO_ENABLE
        if (NULL != arg_in_kv) {
            int out_num = 0;
            while (1) {
                char* item = Cursor_popToken(&buffs, &arg_in_kv, ",");
                if (item[0] == '\0') {
                    break;
                }
                char* stmt = strsFormat(&buffs, PIKA_LINE_BUFF_SIZE,
                                        "%s = $tmp[%d]\n", item, out_num);

                AST* ast_this = AST_parseLine_withBlockDeepth(
                    stmt, AST_getBlockDeepthNow(oAST) + 1);
                sPikaAsm =
                    strsAppend(&buffs, sPikaAsm, AST_genAsm(ast_this, &buffs));
                AST_deinit(ast_this);
                out_num++;
            }
            sPikaAsm = ASM_addBlockDeepth(oAST, outBuffs, sPikaAsm, 1);
            sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 DEL $tmp\n");
        }
#endif

        is_block_matched = 1;
        goto exit;
    }
    if (strEqu(AST_getThisBlock(oAST), "elif")) {
        /* skip if __else is 0 */
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 NEL 1\n");
        /* parse stmt ast */
        sPikaAsm = AST_genAsm_sub(oAST, oAST, &buffs, sPikaAsm);
        /* skip if stmt is 0 */
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 JEZ 1\n");
        is_block_matched = 1;
        goto exit;
    }
    if (strEqu(AST_getThisBlock(oAST), "def")) {
#if !PIKA_NANO_ENABLE
        char* sDefaultStmts = AST_getNodeAttr(oAST, "default");
#endif
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 DEF ");
        sPikaAsm =
            strsAppend(&buffs, sPikaAsm, AST_getNodeAttr(oAST, "declare"));
        sPikaAsm = strsAppend(&buffs, sPikaAsm,
                              "\n"
                              "0 JMP 1\n");

#if !PIKA_NANO_ENABLE
        if (NULL != sDefaultStmts) {
            int iStmtNum = strGetTokenNum(sDefaultStmts, ',');
            for (int i = 0; i < iStmtNum; i++) {
                char* sStmt = Cursor_popToken(&buffs, &sDefaultStmts, ",");
                char* sArgName = strsGetFirstToken(&buffs, sStmt, '=');
                sPikaAsm = ASM_addBlockDeepth(oAST, &buffs, sPikaAsm, 1);
                sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 EST ");
                sPikaAsm = strsAppend(&buffs, sPikaAsm, sArgName);
                sPikaAsm = strsAppend(&buffs, sPikaAsm, "\n");
                sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 JNZ 2\n");
                AST* ast_this = AST_parseLine_withBlockDeepth(
                    sStmt, AST_getBlockDeepthNow(oAST) + 1);
                sPikaAsm =
                    strsAppend(&buffs, sPikaAsm, AST_genAsm(ast_this, &buffs));
                AST_deinit(ast_this);
            }
        }
#endif

        is_block_matched = 1;
        goto exit;
    }

    if (strEqu(AST_getThisBlock(oAST), "class")) {
        char* declare = obj_getStr(oAST, "declare");
        char* thisClass = NULL;
        char* superClass = NULL;
        if (strIsContain(declare, '(')) {
            thisClass = strsGetFirstToken(&buffs, declare, '(');
            superClass = strsCut(&buffs, declare, '(', ')');
        } else {
            thisClass = declare;
            superClass = "";
        }
        if (strEqu("", superClass)) {
            /* default superClass */
            superClass = "TinyObj";
        }
        if (strEqu("TinyObj", superClass)) {
            /* default superClass */
            superClass = "TinyObj";
        }
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 CLS ");
        sPikaAsm = strsAppend(&buffs, sPikaAsm,
                              strsAppend(&buffs, thisClass,
                                         "()\n"
                                         "0 JMP 1\n"));
        char block_deepth_str[] = "B0\n";
        /* goto deeper block */
        block_deepth_str[1] += AST_getBlockDeepthNow(oAST) + 1;
        sPikaAsm = strsAppend(&buffs, sPikaAsm, block_deepth_str);
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 RUN ");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, superClass);
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 OUT self\n");
        sPikaAsm = strsAppend(&buffs, sPikaAsm, block_deepth_str);
        sPikaAsm = strsAppend(&buffs, sPikaAsm, "0 RAS self\n");
        is_block_matched = 1;
        goto exit;
    }

    for (size_t i = 0; i < sizeof(rules_block) / sizeof(GenRule); i++) {
        GenRule rule = rules_block[i];
        if (strEqu(AST_getThisBlock(oAST), rule.ast)) {
            sPikaAsm = GenRule_toAsm(rule, &buffs, oAST, sPikaAsm, 0);
            is_block_matched = 1;
            goto exit;
        }
    }

    /* generate code for top level ast */
    for (size_t i = 0; i < sizeof(rules_topAst) / sizeof(rules_topAst[0]);
         i++) {
        GenRule item = rules_topAst[i];
        if (obj_isArgExist(oAST, item.ast)) {
            sPikaAsm = GenRule_toAsm(item, &buffs, oAST, sPikaAsm, 0);
            is_block_matched = 1;
            goto exit;
        }
    }
exit:
    if (NULL == sPikaAsm) {
        strsDeinit(&buffs);
        return NULL;
    }
    if (!is_block_matched) {
        /* parse stmt ast */
        sPikaAsm = AST_genAsm_sub(oAST, oAST, &buffs, sPikaAsm);
    }

    /* output pikaAsm */
    sPikaAsm = strsCopy(outBuffs, sPikaAsm);
    strsDeinit(&buffs);
    return sPikaAsm;
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}

ByteCodeFrame* byteCodeFrame_appendFromAsm(ByteCodeFrame* self, char* pikaAsm) {
    Asmer asmer = {
        .asm_code = pikaAsm,
        .block_deepth_now = 0,
        .is_new_line = 0,
        .line_pointer = pikaAsm,
    };
    uint16_t const_pool_offset;
    uint16_t exist_offset;
    int invoke_deepth_int = 0;
    for (int i = 0; i < strCountSign(pikaAsm, '\n'); i++) {
        Args buffs = {0};
        char* line = strsGetLine(&buffs, asmer.line_pointer);
        char* data = NULL;
        char ins_str[4] = "";
        char invoke_deepth[3] = "";
        uint8_t space_num = 0;
        uint8_t invoke_deepth_i = 0;
        uint8_t ins_str_i = 0;
        Arg* line_buff = arg_newStr(line);
        strsDeinit(&buffs);
        line = arg_getStr(line_buff);
        InstructUnit ins_unit = {0};
        /* remove '\r' */
        if (line[strGetSize(line) - 1] == '\r') {
            line[strGetSize(line) - 1] = 0;
        }
        /* process block deepth flag*/
        if ('B' == line[0]) {
            asmer.block_deepth_now = fast_atoi(line + 1);
            asmer.is_new_line = 1;
            goto next_line;
        }

        /* process each ins */

        /* get constPool offset */
        const_pool_offset = 0;

        for (int i = 0; i < (int)strGetSize(line); i++) {
            if (space_num < 2) {
                if (line[i] == ' ') {
                    space_num++;
                    if (space_num == 2) {
                        data = line + i + 1;
                        break;
                    }
                    continue;
                }
            }
            if (space_num == 0) {
                invoke_deepth[invoke_deepth_i++] = line[i];
                continue;
            }
            if (space_num == 1) {
                ins_str[ins_str_i++] = line[i];
                continue;
            }
        }

        exist_offset = constPool_getOffsetByData(&(self->const_pool), data);

        /* get const offset */
        if (strEqu(data, "")) {
            /* not need const value */
            const_pool_offset = 0;
        } else if (65535 == exist_offset) {
            /* push new const value */
            const_pool_offset = constPool_getLastOffset(&(self->const_pool));
            /* load const to const pool buff */
            constPool_append(&(self->const_pool), data);
        } else {
            /* use exist const value */
            const_pool_offset = exist_offset;
        }

        invoke_deepth_int = fast_atoi(invoke_deepth);
        /* load Asm to byte code unit */
        instructUnit_setBlockDeepth(&ins_unit, asmer.block_deepth_now);
        instructUnit_setInvokeDeepth(&ins_unit, invoke_deepth_int);
        instructUnit_setConstPoolIndex(&ins_unit, const_pool_offset);
        instructUnit_setInstruct(&ins_unit, pikaVM_getInstructFromAsm(ins_str));
        if (asmer.is_new_line) {
            instructUnit_setIsNewLine(&ins_unit, 1);
            asmer.is_new_line = 0;
        }

        /* append instructUnit to instructArray */
        instructArray_append(&(self->instruct_array), &ins_unit);

    next_line:
        /* point to next line */
        asmer.line_pointer += strGetLineSize(asmer.line_pointer) + 1;
        arg_deinit(line_buff);
    }
    return self;
}

char* Parser_linesToArray(char* lines) {
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    Parser_linesToBytes(&bytecode_frame, lines);
    /* do something */
    byteCodeFrame_print(&bytecode_frame);

    pika_platform_printf("\n\n/* clang-format off */\n");
    pika_platform_printf("PIKA_PYTHON(\n");
    pika_platform_printf("%s\n", lines);
    pika_platform_printf(")\n");
    pika_platform_printf("/* clang-format on */\n");
    byteCodeFrame_printAsArray(&bytecode_frame);
    /* deinit */
    byteCodeFrame_deinit(&bytecode_frame);
    pika_platform_printf("\n\n");
    return NULL;
}
