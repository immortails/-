## 函数
go中的函数是按如下的格式来声明的。
```go
func name(parameter-list) (result-list) {
    body
}
```
- go中的函数继承了python的优点可以有多个返回值
- go的函数变量要比c方便的多，函数本身就是个变量对于go来说，不用函数指针就可以表示。
- go中的匿名函数自动就可以捕捉到整个词法环境，相当于cpp中捕获自带了&，但是因为go中bug级别的垃圾回收器的存在；以为这如果你用一个函数来返回一个匿名函数，那么这个函数中的所有局部变量不会被销毁，因为匿名函数可以访问。
```go
func squares() func() int {
    var x int
    return func() int {
        x++
        return x * x
    }
}

func main() {
    f := squares()
    fmt.Println(f()) // 1
    fmt.Println(f()) // 4
    fmt.Println(f()) // 9
    fmt.Println(f()) // 16
}
```
- 上面这波操作因为f这个匿名函数只对应了一个x，所以底下每次调用就会对x这个变量自增。
- 因此用**匿名函数可以很轻松的对对象进行构造与析构**，每个对象构造完后返回一个匿名的析构函数，然后保存起来，当需要析构的时候就调用这些析构函数就可以。
- 匿名函数**递归**之前，必须先声明一个函数变量然后再将这个匿名函数赋给相应的函数变量，这样才能递归调用。
- 注意匿名函数**迭代**调用的时候，循环条件里的变量必须先取出来，创建一个新的局部变量，然后再操作，不然如果你想把这些匿名函数保存起来后面操作的时候就会操作最后一个变量，因为这些匿名函数拿到的是同一个变量的引用。
```go
var rmdirs []func()
for _, d := range tempDirs() {
    dir := d    //这一行必不可少
    os.MkdirAll(dir, 0755)
    rmdirs = append(rmdirs, func() {
        os.RemoveAll(dir)
    })
}
···
for _, rmdir := range rmdirs {
    rmdir() 
}
```
- 变长函数是指参数用 ...来省略，实际传过来的将会是一个数组，变长参数往往用于格式化字符串。
```go
func sum(vals ...int) int {
    total := 0
    for _, val := range vals {
        total += val
    }
}
```
### defer
- 延迟函数调用defer，这是一个很好用的功能。defer是用于延迟一个函数调用，在外层函数返回或者宕机后才执行，就是最后执行的意思，多个defer按逆序执行。
- defer语句往往用于成对的打开关闭操作，对于有些关闭后还需要返回东西的，那么可以defer这个关闭操作，这样返回了之后再close，是个很棒的技巧。
```go
var mu sync.Mutex
var m = make(map[string]int)
func lookup(key string) int {
    mu.lock()
    defer mu.unlock()
    return m[key]   //返回了m[key]之后再lock
}
```
- 在多个执行路径都需要返回并且返回都需要做相同操作，如关闭某个资源的情况下，用defer就可以直接复用。
### 宕机
- 宕机是指运行时发生的一些错误，如数组越界等。当宕机发生时，一般程序就会终止，并且会按倒序执行defer中的内容。
- 对于有些不影响全局的错误，我们希望宕机了后还能恢复，go提供了这个机制，**recover**，recover函数可以从宕机状态正常返回，并输出返回值，若不是宕机状态，则会返回nil。这意味着**这个程序还会正常工作**，只是这个函数直接从recover处返回了。