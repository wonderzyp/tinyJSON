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


