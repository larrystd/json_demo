#### 转义

JSON 的字符串语法和 C 语言很相似，都是以双引号把字符括起来，如 "Hello"。但字符串采用了双引号作分隔，那么怎样可以在字符串中插入一个双引号？ 把 a"b 写成 "a"b" 肯定不行，都不知道那里是字符串的结束了。因此，我们需要引入转义字符（escape character），C 语言和 JSON 都使用 \（反斜线）作为转义字符，那么 " 在字符串中就表示为 \"，a"b 的 JSON 字符串则写成 "a\"b"。如以下的字符串语法所示，JSON 共支持 9 种转义序列：

string = quotation-mark *char quotation-mark
char = unescaped /
   escape (
       %x22 /          ; "    quotation mark  U+0022
       %x5C /          ; \    reverse solidus U+005C
       %x2F /          ; /    solidus         U+002F
       %x62 /          ; b    backspace       U+0008
       %x66 /          ; f    form feed       U+000C
       %x6E /          ; n    line feed       U+000A
       %x72 /          ; r    carriage return U+000D
       %x74 /          ; t    tab             U+0009
       %x75 4HEXDIG )  ; uXXXX                U+XXXX
escape = %x5C          ; \
quotation-mark = %x22  ; "
unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
JSON 字符串是由前后两个双引号夹着零至多个字符。字符分为无转义字符或转义序列。转义序列有 9 种，都是以反斜线开始，如常见的 \n 代表换行符。比较特殊的是 \uXXXX，当中 XXXX 为 16 进位的 UTF-16 编码，本单元将不处理这种转义序列，留待下回分解。

无转义字符就是普通的字符，语法中列出了合法的码点范围（码点还是在下单元才介绍）。要注意的是，该范围不包括 0 至 31、双引号和反斜线，这些码点都必须要使用转义方式表示。


#### 缓冲区

我们解析字符串（以及之后的数组、对象）时，需要把解析的结果先储存在一个临时的缓冲区，最后再用 lept_set_string() 把缓冲区的结果设进值之中。在完成解析一个字符串之前，这个缓冲区的大小是不能预知的。因此，我们可以采用动态数组（dynamic array）这种数据结构，即数组空间不足时，能自动扩展。C++ 标准库的 std::vector 也是一种动态数组。缓冲区可以用堆栈模拟

```cpp
typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}lept_context;
```