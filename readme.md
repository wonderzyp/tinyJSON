通过实现JSON库，学习软件开发整体流程
---
- 动态内存管理
- 测试驱动开发(test driven development, TDD)
- 单元测试

**学习教程**：[从零开始的JSON库](https://github.com/miloyip/json-tutorial)

JSON相关知识
---
JSON, JavaScript Object Notation, 一种用于数据交换的文本格式。
JSON为树状结构，仅包含六种数据类型
- null
- boolean
- number
- string
- array
- object

JSON库的实现目标
---
- 将JSON文本**解析**为树状数据结构
- 提供接口**访问**该数据结构
- 将数据结构转化为JSON文本

大体思路
---
#### 数据结构设计：
##### lept_context
缓存传入的JSON文本。
结构体内部包含栈，可实现动态内存管理。
```cpp
typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}lept_context;
```
##### lept_value
存放解析后JSON节点内容及类型
```cpp
struct lept_value {
  union {
    struct { lept_member* m; size_t size; }o; // 对象
    struct { lept_value* e; size_t size; }a;  // 数组
    struct { char* s; size_t len; }s;         // 字符串
    double n;                                 // 数字
  }u;
  lept_type type;
};
```
其中`lept_member`存储对象类，包含键(`string`类型)及与对应的值
```cpp
struct lept_member {
  char* k; size_t klen;   /* member key string, key string length */
  lept_value v;           /* member value */
};
```

收获
---
#### 1. include防范`include guard`
避免头文件重复include导致的重复声明
```cpp
#ifndef AFILENAME_H__
#define AFILENAME_H__
/*...*/
#endif
```

#### 2. 宏编写技巧
当宏中存在多于一个语句，使用`do{/*...*/} while(0)`包裹成单个语句



#### 3. 动态内存管理
以数组为例，由于数组难以确定事先分配多大内存，因此借助临时缓冲区存储解析后的结果，实现动态压入字符，最后**一次性**将整个字符串弹出。
```c
typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}lept_context;
```
其中：
- `size`：当前的堆栈容量
- `top`： 栈顶位置，由于会拓展stack，因此`top`不可使用指针形式存储

##### 堆栈的压入弹出
```c
static void* lept_context_push(lept_context* c, size_t size) {
  void* ret;
  assert(size > 0);
  if (c->top + size >= c->size) {
    if (c->size == 0)
      c->size = LEPT_PARSE_STACK_INIT_SIZE;
    while (c->top + size >= c->size)
      c->size += c->size >> 1;  /* c->size * 1.5 */
    c->stack = (char*)realloc(c->stack, c->size);
  }
  ret = c->stack + c->top;
  c->top += size;
  return ret;
}
```
> `void*`: 




#### 4. 单元测试
一般先加入功能，再写对应功能的单元测试
测试驱动开发(TDD, test-driven development)，先写测试再编写实现代码，直至通过所有测试。
TDD方便对代码进行重构。

#### 5. 断言 assertion
通过`assert()`宏实现；一般在函数开始前检查所有参数，函数结束后检查上下文是否正确。
> release模式定义`NOEBUG`，不会检测`assert`
> debug模式则会检测条件是否为真

#### 6. 指向void的指针

#### 7. 悬挂指针





