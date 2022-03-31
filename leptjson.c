#include "leptjson.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h> //HUGE_VAL
#include <errno.h>


#define EXPECT(c, ch)  do { assert(*c->json == (ch)); c->json++; } while(0)

#define ISDIGIT(ch) ((ch) >= '0' && (ch)<='9')
#define ISDIGIT1TO9(ch) ((ch) >='1' && (ch) <='9')

#define lept_init(v) do { (v)->type = LEPT_NULL; } while(0)

typedef struct {
  const char* json;

  char* stack;  // 动态堆栈的数据
  size_t size, top;
}lept_context;


/**
* @brief 解析器
* ****************************************************************************************/

// 解析空白：空白由零或多个空格符、制表符、换行符、回车符组成
static void lept_parse_whitespace(lept_context* c){
  const char *p = c->json;
  while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    p++;
  c->json = p;
}

static int lept_parse_literal(lept_context* c, const char* literal, lept_value* v, lept_type type){
  size_t i;
  EXPECT(c, literal[0]);
  for (i=0; literal[i+1]; ++i){ // 注意循环条件的设计
    if (c->json[i] != literal[i+1]) // 为什么？？？？
    // 之前写的c->json[i+1]为什么错了？
      return LEPT_PARSE_INVALID_VALUE; 
  }

  c->json += i;
  v->type = type;
  return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context*c, lept_value* v){

  const char* p = c->json;

  if (*p == '-') ++p;

  if (*p == '0') ++p;
  else {
    if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
    // 提前给p加一位，是因为上一位已检测
    for(++p; ISDIGIT(*p); ++p);
  } 

  if (*p == '.') {
    ++p;
    if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
    for(++p; ISDIGIT(*p); ++p);
  }

  if (*p == 'e' || *p == 'E'){
    ++p;
    if (*p == '+' || *p == '-') ++p;
    if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
    for(++p; ISDIGIT(*p); ++p);
  }
  
  v->n = strtod(c->json, NULL);

  if ((v->n == HUGE_VAL || v->n == -HUGE_VAL) && errno==ERANGE){
    return LEPT_PARSE_NUMBER_TOO_BIG;
  }

  v->type = LEPT_NUMBER;
  c->json = p;
  return LEPT_PARSE_OK;
}

// 现实值可以是null, false或true
static int lept_parse_value(lept_context* c, lept_value* v){
  switch (*c->json){
    case 'n':  return lept_parse_literal(c, "null", v, LEPT_NULL);
    case 't':  return lept_parse_literal(c, "true", v, LEPT_TRUE);
    case 'f':  return lept_parse_literal(c, "false", v, LEPT_FALSE);
    case '\0': return LEPT_PARSE_EXPECT_VALUE;  // json只有空白
    default:   return lept_parse_number(c,v); //
  };
}


int lept_parse(lept_value* v, const char* json){
  lept_context c;
  int ret;
  assert(v != NULL); // why?
  c.json = json;
  c.stack = NULL;
  c.size = c.top = 0;
  lept_init(v);

  lept_parse_whitespace(&c);
  
  // value后的空白之后不可存在值
  // 对解析值后的json调用lept_parse_whitespace()解析
  if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
    lept_parse_whitespace(&c);
    if (*c.json != '\0')
      ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
  }
  assert(c.top == 0); // 确保所有数据均被弹出
  free(c.stack);

  return ret;
}

/**
* @brief 获取类型
* ****************************************************************************************/
lept_type lept_get_type(const lept_value* v){
  assert(v != NULL);
  return v->type;
}


// 使用断言，确保用户调用时类型满足: type==LEPT_NUMBER
double lept_get_number(const lept_value* v){
  assert(v != NULL && v->type == LEPT_NUMBER);
  return v->n;
}

void lept_free(lept_value* v){
  assert(v != NULL);
  if (v->type == LEPT_STRING){
    free(v->s.s);
    // 释放内存后将类型置为NULL，避免重复释放
    v->type = NULL; // 这里教程是否有误?
  }
}

void lept_set_string(lept_value* v, const char* s, size_t len) {
  assert(v != NULL && (s != NULL || len==0));
  lept_free(v);
  v->s.s = (char*)malloc(len+1);
  memcpy(v->s.s, s, len); // 拷贝一份参数的字符串
  v->s.s[len]='\0'; // 补上结尾的空字符
  v->s.len = len;
  v->type = LEPT_STRING;
}