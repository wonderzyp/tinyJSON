JSON(JavaScript Object Notation)
---
json：一种用于数据交换的文本格式
JSON为树状结构，仅包含六种数据类型
- null
- boolean
- number
- string
- array
- object

JSON库的三个需求：
- 将JSON文本解析为树状数据结构(parse)
- 提供接口访问该数据结构
- 将数据结构转化为JSON文本

头文件与API设计
---
避免`#include`引入重复声明，利用宏加入include防范`include guard`

单元测试
---
一般软件开发以周期进行，加入功能再写该功能的单元测试
另一种软件开发方法论：测试驱动开发(TDD, test-driven development)，先写测试再编写实现代码，直到通过所有测试


宏编写技巧：
当宏中存在多于一个语句，使用`do{/*...*/} while(0)`包裹成单个语句

解析器
---
传入的JSON文本是一个C字符串，不会对其进行操作，声明为`const char*`类型
为减少解析函数之间传递多个参数，将数据均放进一个结构体

断言(assertion)
---
C语言中常见的防御式编程方式，可减少错误
- 函数开始阶段：检测所有参数
- 调用函数后：检查上下文是否正确

C语言包含`assert()`宏，提供断言功能
release模式编译时，由于定义了`NDEBUG`，不会对`assert`进行检测
debug模式则会检测条件是否为真
> 注意release和debug模式行为可能不一致，使用断言需避免副作用

错误由程序员错误编码引起，使用断言
运行环境引起（打开文件失败等），处理运行时错误（抛出异常等）


TDD--重构(refactoring)
---
重构：在不改变程序的外在功能前提下，改变程序的内部结构

TDD目标引导性强，可能导致代码品质下降
在通过测试后，应审视代码并判断是否有可改进的地方
- DRY(don't repeat yourself)原则


Union
---
节省空间，但调用时可能显得很长
C11引入匿名Union

使用`#ifndef X #define X ... #endif`：使用者可在编译选项中自设置宏值，未设置则使用缺省值



解析数组
---
解析数组的关键是内存管理

#### 数据结构的选择
数组：
- 优：O(1)访问任意元素，内存紧凑且高速缓存一致性(cache coherence)
- 缺：无法快速插入元素，不确定分配多大内存

链表：
- 优：可快速插入，但索引元素O(n)
- 缺：额外内存开销（指向下一个元素的指针），内存不连续导致缓存不命中(cache miss)

#### 动态内存管理
解析JSON字符串时，借助临时缓冲区存储解析后结果，实现动态压入字符，最后一次性将整个字符串弹出
动态的堆栈(stack)数据结构
```c
typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}lept_context;
```
`size`：当前的堆栈容量
`top`： 栈顶位置，由于会拓展stack，因此`top`不可使用指针形式存储
创建`lept_context`时初始化`stack`并释放内存

#### 堆栈的压入压出
按字节存储，每次可压入任意大小数据，返回数据的**起始指针**






指向void*的指针
---
一个指向任何对象类型的指针均可赋值给类型为`void*`的变量
用途：
1. 从函数返回一个无类型对象，使用时需进行显式的类型转换
2. 需向函数传递指针，且不能对对象类型做假设

对于数据的元素释放，需将数组内的元素递归调用`lept_free()`，再释放本身的`v->u.a.e`


ps：
前向声明
使用自身类型的指针，因此需前向声明
// lept_parse_array与lept_parse_value之间会相互引用，需加入函数前向声明


返回指针的生命周期
---
```c
for (;;) {
    /* bug! */
    lept_value* e = lept_context_push(c, sizeof(lept_value));
    lept_init(e);
    size++;
    if ((ret = lept_parse_value(c, e)) != LEPT_PARSE_OK)
        return ret;
    /* ... */
}
```
`lept_parse_value`及以下的函数均会调用`lept_context_push`函数，可能调用`realloc()`导致最上层的指针`lept_value* e`失效，变为悬挂指针(dangling pointer)
> 悬挂指针指向非法的内存地址，无法正常使用的指针

同理，使用C++容器时，取得迭代器后，若改动容器内容，会导致之前的迭代器失效

此类bug不易察觉，一般在堆栈满时发生，因此需要压力测试

因此需谨慎考虑变量周期（尤其是指针）



