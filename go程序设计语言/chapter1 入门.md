## 入门
- go是编译型语言，他可以直接run：
```bash
go run helloworld.go
```
- 也可以先build出可执行文件：
```
go build helloworld.go
./helloworld
```
- go的组织方式与java类似，每个源文件前面都需要先声明一个package，作为自己的package名字，如果有需要import的包，都import。
- go无需用;结尾，但是go对代码的格式要求很严格，不能有多余的变量，不能有多余的包，所以每次写到一半保存都可能砍掉很多东西。保存时自动运行gofmt。
- go输入参数是os.Args，它是一个字符串类型的slice，slice可以理解成动态数组。go中变量声明是:=,这个符号意味着短变量声明。go中只支持后缀的++，也就是i++，它不支持++i。
- go中唯一的循环语句是for,一般来说有initialization,condition,post这三个，initialization是指初始化，condition是条件，post是往下递推。你也可以只有condition，此时就跟while的作用一样。
```go
for initialization;condition;post{

}

for condition{

}

for{

}
```
- go提供直接range的做法，用range可以直接遍历索引，和索引处元素的值。range的话一定是K/V的，但如果不需要使用key的时候，go又不允许无用变量，因此可以用_来代替这个。
```go
for _,arg:=range os.Args[1:]{

}
```
- go的变量命名有多种写法，以下都是正确的：
```
s:=""                   //短变量声明，通常只在一个函数内使用，不适合包级别的变量。
var s string            //初始化一个字符串，默认为""
var s = ""             
var s string = ""
```
- go创建hash表也有点意思,它是通过make来创建的
```
counts:=make(map[string]int)
```
- go的并发机制有goroutine与channel，goroutine可以启动一个线程，channel的话就是一个消息队列，用于RPC。
- go用type来命名类型：有意思的是，go的命名变量刚好是相反的，类型写在名字后面，很奇怪。
```go
    type Point struct{
        X,Y int
    }
    var p Point
```
- go中还提供了指针，它跟c指针差不多，可以取址，可以获取值，但是不能算数运算。