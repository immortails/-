## 方法
方法是go中的oop编程中最主要的一块，**方法是针对于某种特定类型的函数**，在go中面对对象主要提到的思想是组合与封装。它不具备继承与多态性。可能是嫌弃继承的耦合性太强了。
```go
type Point struct{
    X,Y float64
}
//普通的函数
func Distance(p, q Point) float64 {
    return math.Hypot(q.X - p.X, q.Y - p.Y)
}
//Point类型的方法
func(p Point) Distance(q Point) float64 {
    return math.Hypot(q.X - p.X, q.Y - p.Y)
}
```
- 我们一般称p为接收者receiver, p.Distance为selector，表示p选择了Distance这个方法；不像cpp必须用this来表示，在go中可以自定义receiver的名字。相同类型的方法必须唯一，不同类型可以拥有相同名字的方法。
- receiver可以传值也可以传指针，传值就会拷贝一遍receiver，如果想避免复制，就传指针。从本质上说，receiver也是一个参数。
- 不管是传值还是传指针，想要调用的时候都可以直接p.Distance(q),不管p是值还是指针，go的编译器会进行隐式转换。
- 方法也可以内嵌到其他结构体中，但注意，这里**调用这些方法时候的参数必须和方法定义时候一致**,因为go不具备多态性，所以方法里写什么就是什么。
- 方法变量：方法本质上也是一个变量，可以赋值然后单独拿出来用。
```go
p := Point{1, 2}
distanceFromP := p.Distance     //方法变量
distanceFromP(q)
```
- 方法表达式:方法也可以不用receiver赋值出来，这样就是方法表达式,后面第一个参数就是原来的receiver。
```go
distance := Point.Distance
distance(p, q)
```
### 封装
go中的访问权限是根据变量名首字母大小写决定的，对于一些内部的变量，完全可以小写不开放，这样有三个好处:
- 使用者无需关心内部的实现。
- 只要接口一致，设计者就可以直接升级API而不影响使用者。
- 使用者不会破坏API内部的实现，做到了保护。