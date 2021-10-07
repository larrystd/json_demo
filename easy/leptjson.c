#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

/*
判断字符元素匹配, 利用assert
如果匹配, c->json++
*/

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

/*
lept_context是一个 typedef struct
*/
typedef struct {
    const char* json;
}lept_context;

/* parse whitespace */
static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    /*解析 whitespace*/
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    /* 解析完毕, 指针赋给c->json*/
    c->json = p;
}

/* 解析 null */
static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    
    return *c->json == '\0' ? LEPT_PARSE_OK : LEPT_PARSE_ROOT_NOT_SINGULAR;
}

/* 解析 true */
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    
    return *c->json == '\0' ? LEPT_PARSE_OK : LEPT_PARSE_ROOT_NOT_SINGULAR;
}

/// 解析数字, 实际是调用strtod
static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);   /// 字符串向double进行转换, 返回double, 传入&end为二重指针, 储存不能继续转换的下一个字符
    if (c->json == end) /// end 指向c->json
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

/* 解析 false */
static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    
    return *c->json == '\0' ? LEPT_PARSE_OK : LEPT_PARSE_ROOT_NOT_SINGULAR;
}


static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        /*空 null, 第一个元素是n, 调用lept_parse_null */
        case 'n':  return lept_parse_null(c, v);
        // 解析 true;
        case 't': return lept_parse_true(c,v);
        // 解析false;
        case 'f': return lept_parse_false(c,v);
        /* 解析到期待的数据, 也就是解析成功 */
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default:   return lept_parse_number(c, v);
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    /* 设置v->type为LEPT_NULL, 因此目标是解析NULL*/
    v->type = LEPT_NULL;

    lept_parse_whitespace(&c);  /* 解析空格 */
    return lept_parse_value(&c, v);
}

/// 得到数据类型
lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}