## 基本数据
Go的数据类型主要分为4种：基础类型，聚合类型，引用类型，接口类型。基础就是数字，字符串，布尔；聚合就是结构体数组；引用就是指针，slice，map，函数，通道。
### 整数
- rune类型是int32类型的同义词；byte类型是uint8类型同义词。
### 浮点数
- 浮点数float64有效数字是15位，float32是6位， 绝大多数情况下，用float64，float32会累计误差。
### 复数
- Go中内置了复数，可以通过real和imag获取实部虚部，也可以直接用i来表示。
### 字符串
- Go中的字符串是**不可变**的序列，不可变意味着他是一个安全的序列，也就是说字符串和其子串可以公用一段内存，用两个指针就可以代表一个子串，无需额外去拷贝或者占用内存空间。Go中子串的获取方式与python中的切片类似，左闭右开。
### 基本类型的操作
及基本类型的操作有4个标准包，strings，bytes，strconv，unicode。
- strings提供了一些字符串基本操作，如替换查找拼接等。
- bytes主要是有个bytes.Buffer,一个字节流的存在，功能上就是可以任意的去读写字节流。其实cpp的string自带这个功能。
- strconv包用于数据转换，如bool，int，float转为string等操作，就是cpp中的stoi()和to_string()
- unicode用于判别文字符号值特性的函数。
### 常量
- Go中的常量很特别，他可以是任何的基本数据类型，如int，float，string，bool。它的精度不受基本数据类型的限制，可以达到256位。
- Go的常量可以用来枚举，用iota这个关键字,iota从第一条记录开始指代0，依次累加，同时iota还可以做一些算数运算。
- 只有常量是无类型的，若将无类型声明成变量，必须要能够转换。不然就会编译错误。
```go
type Weekday int
const (
    Sunday Weekday = iota
    Monday 
    Tuesday
    Wednesday
    Thursday
    Friday
    Saturday
)
```