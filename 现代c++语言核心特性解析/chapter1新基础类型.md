## 新基础类型
### 整型 long long:
- long long是一个至少大于64位的整数类型。提供3个宏定义：LLONG_MAX,LLONG_MIN,ULLONG_MAX；考虑到c++中应该尽可能减少宏的使用，可以通过numeric_limits这个模板来实现,它定义了两个静态函数max与min
```cpp
#include<limits>
std::numeric_limits<long long>::max();
std::numeric_limits<long long>::min();
```
### char16_t与char32_t
char16_t与char32_t主要是用于对应utf-16,utf-32。
utf-32采用32位的内存空间来存储一个字符编码，由于unicode最多也就4字节，所以其可以容纳所有的字符编码。utf-32使用起来非常简单，但是太占用空间了。
utf-16从32位缩减到16位，由于存储空间缩小，它采用了一种特殊的编码方式来映射。
utf-8用的最多，由于他只有8位，所以它采用了前缀编码的方法，最开头告诉你一个字符的字节数，可以从1-4字节不等，因此它是一种变长的编码。
utf-8由于其可变长，一般直接用char去存，utf-16与utf-32是定长的，在c11中专门为其定制了两种字符类型，char16_t,char32_t。
