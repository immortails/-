## 内联与嵌套命名空间
### 内联命名空间
- 为了避免开发大工程而引入命名冲突的问题，cpp提出来了命名空间的说法，程序员可以将函数与类型纳入命名空间中，这样就不至于产生冲突。
```cpp
    namespace S1{
        void foo(){}
    }
    namespace S2{
        void foo(){}
    }
    int main(){
        using namespace S1;
        foo();
        S2::foo();
    }
```
c11 在此基础上提出了内联命名空间：
```cpp
    namespace parent{
        namespace Child1{
            void foo() {std::cout<<"child1"<<std::endl;}
        }
        inline namespace Child2{
            void foo() {std::cout<<"child2"<<std::endl;}
        }
    }
    int main(){
        parent::foo();      //实际调用的是child2
    }
```
- inline namespace与inline函数一样，直接会把代码复制到相应位置上，这样parent无需再::child2就可以访问child2中的成员。
- 这样的好处在于：函数的版本升级很容易，新的版本直接inline，这样也无需改动其他使用这个命名空间的代码，对于想要使用旧版本的可以自行去引用。
### 嵌套命名空间
```cpp
namespace A::B::C{
    int foo(){ return 5; }
}

namespace A{
    namespace B{
        namespace C{
            int foo(){};
        }
    }
}
```



