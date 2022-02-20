## pipe函数
- pipe函数的功能是创建一个管道，以实现进程间通信。
```cpp
#include<unistd.h>
int pipe(int fd[2]);
```
- pipe 输入2个文件描述符，该函数设置成功时返回0，失败返回-1并设置errno。
- fd[1]是写，fd[0]是读，fd[0]只能用来读，如果想要双向传输，则应该定义两个管道。默认情况下，这两个文件描述符都是阻塞的。如果管道fd[1]端的引用计数降为0，对fd[0]read的操作会返回0，即读到了EOF。反之如果fd[0]的引用计数降为0，那么对fd[1]的写就失败，并引发SIGPIPE信号。
- 管道内部传输的数据是字节流，这和TCP的概念相同，但是TCP可以通过接收窗口实现流量控制，而管道本身有一个容量限制，即应用程序不被读走的话，该管道最多能写如多少数据，Linux2.6.11起，这个值是65536.、
- 管道是单向的，如果想要创建双向管道需要用sockpairAPI,有点像个socket。这里传的第一个协议簇一般是PF_UNIX，指的是系统内部通信。
```cpp
#include<sys/types.h>
#include<sys/socket.h>
int socketpair(int domain,int type,int protocol, int fd[2]);
```
## dup与dup2函数
- dup函数是用来创建一个新的文件描述符，该文件描述符指向原有文件描述符即输入的文件描述符相同的对象，dup选取的一般是系统当前可用的最小文件描述符。dup2与dup相似，但dup2可以自行设定文件描述符的值，它设定的是第一个不小于file_descriptor_two的值，dup2设定失败时返回-1并设置errno。
- dup与dup2设置时并不继承原有文件描述符的属性。它没有拷贝这一步。
```cpp
#include<unistd.h>
int dup(int file_descriptor);
int dup2(int file_descriptor_one,int file_descriptor_two);
```
- dup2可以用来实现一个CGI服务器，所谓CGI服务器就是将标准输出的文件描述符1所指的对象改成clientSocket。这样标准输出的所有内容都会发给client。
### readv和writev函数
- readv函数是将数据从文件描述符读取到分散的内存块上，而read函数是读到一个整块的内存块上。writev同理，从多个分散的内存块上写入数据到对应文件描述符上。它们所需的参数是iovec结构体数组，iovec结构体指向一块内存块。
```cpp
struct iovec{
    void* iov_base; //内存起始地址
    size_t iov_len; //内存的长度
}
#include<sys/uio.h>
ssize_t readv(int fd,const iovec* vector,int count);
ssize_t writev(int fd,const iovec* vector,int count);
```
- 这个函数应该还是能被很多地方用到的，比如http中，HTTP头部内容可能写在一个内存块中，而内容可能是在另一个内存块中，不需要提前将两个拼接到一起，用writev就行。
## sendfile函数
sendfile函数直接在两个文件描述符之间传递数据，不用拷贝到用户空间，直接在内核空间就可以完成工作，其效率很高，也被叫做零拷贝。
```cpp
#include<sys/sendfile.h>
ssize_t sendfile(int out_fd,int in_fd,off_t* offset,size_t count);
```
- in_fd是往进写的文件描述符，in_fd必须是真实的文件，不能是socket或者管道，而out_fd是读的文件描述符，也就是in往out里写。offset是指从out_fd的哪个位置开始读入，如果为NULL就是默认位置，count是读的字节数。返回是传输的字节数。
## mmap函数与munmap函数
- mmap是将给定的虚拟空间地址映射到实际的物理内存中，之后访问这段虚拟内存地址就是在访问对应的物理内存地址。munmap函数则是释放由mmap创建的这段空间。
```cpp
#include<sys/mman.h>
void* mmap(void* start,size_t length,int prot,int flags,int fd, off_t offset);
int munmap(void* start,size_t length);
```
- start参数指明这段虚拟内存的开始地址，length是指内存端的长度，prot是用来设置内存的访问权限：

字段 | 意义
---|---
PROT_READ | 内存段可读
PROT_WRITE | 内存段可写
PROT_EXEC | 内存段可执行
PROT_NONE | 内存段不能被访问
- flags参数控制内存段被修改后程序的行为，这些字段和上面的prot中的字段都可以按位或来组合，除了MAP_SHARED和MAP_PRIVATE，这两个是互斥的。

常用值 | 含义
---|---
MAP_SHARED | 在进程间共享这段内存，对该内存段修改将会反映到被映射的文件中。
MAP_PRIVATE | 内存段为调用进程私有，对其修改不会反映到被映射的文件中。
MAP_ANONYMOUS | 这段内存不是从文件映射过来的，其内容全部被初始化为0，这种情况下，最后两个参数被忽略。
MAP_FIXED | 内存段必须位于start参数指定的地址出，start必须是内存页面大小的整数倍(4096KB)。
MAP_HUGETLB | 按照大内存页面来分配内存空间，大内存页面大小可设置。

## splice函数
- splice是在两个文件描述符之间移动数据，也是零拷贝操作。从fd_in到fd_out,其中一个必须是管道文件描述符，所以splice函数是管道和一般文件描述符之间的粘合剂，之前的sendfile是必须写入端是真实file，不能是管道和socket所以这个函数就是在专门粘合这个。off_in是指写入的偏移位置，如果fd_in是管道的话，那么off_in必须是NULL。len表示移动数据的长度，flags指明数据如何移动，当splice函数执行成功时。
```cpp
#include<fcntl.h>
ssize_t splice(int fd_in, loff_t* off_in , int fd_out, loff_t* off_out, ssize_t len, unsigned int flags);
```
- splice的flags参数的常用值与含义

常用值|含义
---|---
SPLICE_F_MOVE|本来是如果合适的话整页移动数据，但因为有bug从内核2.6.21后无任何效果。
SPLICE_F_NONBLOCK|非阻塞的splice操作，实际还与文件描述符本身阻塞状态有关。
SPLICE_F_MORE|给内核一个提示：后续的splice调用将读取更多的数据。
SPLICE_F_GIFT|对splice无效果。
- splice函数可能产生的errno的含义：

错误 |含义
---|---
EBADF |参数所指文件描述符有错
EINVAL |目标文件系统不支持splice，或者两个文件描述符都不是管道文件描述符，或者某个offset参数被用于不支持随机访问设备。
ENOMEM | 内存空间不够。
ESPIPE | fd_in 或 fd_out 是管道，但是对应的off不是null。
- 与splice函数一起的还有tee函数，这个函数是将fd_in的写入复制到fd_out上，注意这里是复制，fd_in 不是只能复制到fd_out,还可以复制到fd_out1,fd_out2等。比如同时输出到终端与文件上。
## fcntl函数
- fcntl是用来控制文件描述符，还有一个ioctl不过那个函数执行起来比较麻烦，一般人们都用fcntl函数：
```cpp
#include<fcntl.h>
int fcntl(int fd,int cmd,...);
```
- cmd操作很多，有时还需要第三个参数，比如获取旧的状态，设置新的状态：
```
int setnonblocking(int fd){
    int old_option=fcntl(fd,F_GETFL);       \\获取文件描述符旧的状态
    int new_option = old_option|O_NONBLOCK; \\设置非阻塞
    fcntl(fd,F_SETFL,new_option)            \\设置新的文件描述符状态，new_option算是第三个参数
    return old_option;
}
```