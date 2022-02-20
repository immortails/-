#IO复用
IO复用使得程序能同时监听多个文件描述符，这对提高程序性能非常重要。
## select系统调用
### select API
- select系统调用提供的原型如下：
```cpp
#include<sys/select.h>
int select(int nfds,readfds,fd_set* writefds,fd_set* exceptdfs,struct timeval* timeout);
struct timeval{
    long tv_sec;    //秒数
    long tv_usec;   //微秒数
}
```
- nfds是指定被监听的文件描述符的总数，通常被设置为所有文件描述符最大值+1，fd_set支持最大可放1024个文件描述符。最好不要改这个宏定义，timeout是用来设置select函数的超时时间，采用指针是因为内核将修改它以告诉应用程序timeout等待了多久。
- readfds、writefds、exceptfds分别指向可读可写和异常事件对应的文件描述符集合。应用程序调用时，通过这三个参数传入自己感兴趣的fd_set指针。当select函数返回时，内核会修改他们来通知哪些文件描述符已经就绪。fd_set包含一个整形数组，该数组的每个元素的每一位标记一个文件描述符，其能容纳的数量由FD_SETSIZE决定，就是1024。
- 由于位操作繁琐且容易出错，linux提供了一系列API来访问修改fd_set结构体中的位：
```cpp
#include<sys/select.h>
FD_ZERO(fd_set* fdset);                 //清除fdset的所有位
FD_SET(int fd, fd_set* fdset)           //设置fdset的位fd
FD_CLR(int fd, fd_set* fdset);          //清除fdset的位fd
int FD_ISSET(int fd,fd_set* fdset);     //测试fdset的位fd是否被设置
```
- select成功时返回就绪(可读、可写、异常)的文件描述符总数。如果超时没有任何文件描述符就绪，返回0。select失败时返回-1并设置errno。
## poll API
- poll与select比较相似，也是在一定时间内轮询一定数量的文件描述符，以测试其中是否有就绪者。poll的原型定义如下：
```cpp
#include<poll.h>
int poll(pollfd* fds,nfds_t nfds,int timeout);
struct pollfd{
    int fd          //文件描述符
    short events    //注册的事件
    short revents   //实际发生的事件，由内核填充
}
```
- 其中fds参数是一个pollfd结构的数组，它将文件描述符与相应的我们感兴趣的事件(可读、可写、可异常)绑定。pollfd只需要传入一次fd与events，每次内核只会更改revents剩余两项不会更改，所以它不需要像select那样每次都重置fdset。nfds是指被监听事件集合fds的大小，这个值可以达到系统所能允许的最大文件描述符个数：65535。
- 一般比较常用的事件类型有POLLIN:数据可读，POLLPRI：高优先级数据可读，如TCP带外数据；POLLOUT：数据可写；POLLERR：错误；POLLRDHUP：TCP连接被关闭，或对方关闭了写操作等。
- 可写主要是当你需要向其他socket写数据时，不能直接往指定的socket里写，**应该作为一个事件重新载入到事件集合中**，然后由poll来重新分发，这样避免你两个线程同时往一个socket里写数据，聊天室代码就是这么做的。
## epoll API
### 内核事件表
- epoll是Linux中特有的I/O复用函数。它在实现上与select与poll有很大差异。epoll需要使用一组函数来完成工作，epoll将用户关心的文件描述符的事件都放在内核里的一个事件表中，这个表叫epoll内核事件表。epoll使用一个额外的文件描述符来唯一的标识内核中的这个事件表，这个文件描述符用epoll_create函数来创建。
```cpp
#include<sys/epoll.h>
int epoll_create(int size);
int epoll_ctl(int epfd,int op, int fd,struct epoll_event* event);
```
- epoll_create中的size用来提示内核，告诉他事件表需要多大。epoll_ctl用来操作内核事件表，epfd就是之前创建的epoll的文件描述符。op则指定了操作类型，操作类型一般有三种：

操作类型|意义
---|---
EPOLL_CTL_ADD|往事件表中注册事件
EPOLL_CTL_MOD|修改epfd上的注册事件
EPOLL_CTL_DEL|删除epfd上的注册事件
- event参数用来指定事件，它是一个epoll_event结构指针类型，其中events指示事件，他们就是把poll中的事件前面加一个e就行。data一般用来指明文件描述符，它所占4个字节，是一个union。
```cpp
struct epoll_event{
    _uint32_t events;       //epoll事件
    epoll_data_t data;      //用户数据
}
```
### epoll_wait函数
- epoll_wait函数是epoll事件调用的接口，它如果检测到有epfd中注册的事件发生，它就会将就绪的事件写入第二个参数events所指向的数组中。这个参数用于保存epoll_wait检测到的就绪的事件。这样它就不用像select与poll一样轮询所有文件描述符，epoll直接就可以获得已经就绪的事件，这样大大提高了索引的效率。
- maxevents指最多监听多少个事件，timeout是指超时时间。
```cpp
#include<sys/epoll.h>
int epoll_wait(int epfd,struct epoll_event* events,int maxevents,int timeout);
```
- 如下是epoll与poll调用方式的不同：
```cpp
/*如何索引poll中就绪的文件描述符*/
int ret=poll(fds,MAX_EVENT_NUMBER,-1);
for(int i=0;i<MAX_EVENT_NUMBER;i++){
    if(fds[i].revents&POLLIN){      //需要判断第i个文件描述符是否就绪
        int sockfd=fds[i].fd;
        /* 处理socket*/
    }
}

/*如何索引epoll中就绪的文件描述符*/
int ret=epoll_wait(fds,events,MAX_EVENT_NUMBER,-1);
for(int i=0;i<ret;i++){
    int sockfd=fds[i].fd;
    /* 肯定就绪，直接处理socket*/
}
```
### LT与ET模式
- epoll对文件描述符的操作有两种触发方式，分别是LT:条件触发，ET：边沿触发。一般LT是默认的工作模式，这种模式是指只要有事件就绪就会通知应用程序，不管这个事件之前通知过没有；而边沿触发ET是指对于一个事件就绪只会通知一次，之后不会再对相同事件就绪通知。从某种程度上讲，边沿触发会降低同一个epoll事件触发的次数，其效率要比条件触发要高。
- 程序使用边沿触发的方式在注册事件时opt选项或一个EPOLLET。每个使用EPOLLET的文件描述符应该是非阻塞的，因为一个就绪的事件只会通知一次，所以一旦通知了程序就会不停的read直到数据截止，如果读到空了一旦文件描述符阻塞，那么就会卡住不动。
### EPOLLONESHOT事件
- 即使使用了ET模式，一个socket上还是有可能触发多次事件，这时候可能存在两个线程操作一个socket的并发局面，这应该要被避免的。对此我们可以用EPOLLONESHOT事件来实现。
- 对于注册了EPOLLONESHOT事件的描述符，内核最多触发一个可读可写或者异常事件，且只触发一次，除非我们再用epoll_ctl来重置该文件描述符上注册的EPOLLONESHOT事件。这样当一个线程操作一个socket时，内核就不会触发该socket直到该线程重置了EPOLLONESHOT事件。因此如果注册了这个事件，每次线程处理事务结束后，都需要重置对应的文件描述符上的EPOLLONESHOT事件，以保证后序事件可以即时通知到。
## 三组IO函数比较
- 前面的select,poll,epoll三组IO复用系统调用，都可以同时监听多个文件描述符。他们将等待由timeout参数指定的时间，直到一个或多个文件描述符上有事件返回，返回值就是就绪的文件描述符数量，为0就代表没有事件发生。
- select的参数模型没有将文件描述符与事件绑定，fdset仅仅是一个文件描述符集合，因此select需要提供三个这种类型的参数来区分传入、传出与异常事件；另一方面，应用程序下一次调用select前不得不重置fdset。
- poll比select聪明一些，他把文件描述符与事件绑定，从而使编程接口变得简洁。由于每次内核只修改pollfd中的revents成员，events和fd保持不变，所以下次调用poll时无需重置pollfd的事件集参数。
- 对于poll和select，其查询是否有事件就绪都是轮询的方式，也就是O(n)的时间复杂度。而epoll采用与其完全不同的方式来管理用户注册的事件。它在内核中维护一个事件表，并提供一个独立的系统调用epoll_ctl来添加修改删除事件，epoll_wait只用来获取已经就绪的事件，所以其索引就绪文件描述符的复杂度为O(1)。
- 一般来说，epoll的效率是要比select和poll高的，但是对于活动连接较多的时候，由于回调函数触发的很频繁，其效率不一定比select和poll高。所以epoll在连接数量很多，但活动连接较小的情况性能体现的比较明显。

系统调用|select|poll|epoll
---|---|---|---
事件集合|用户通过三个参数分别传入感兴趣的可读可写以及异常等事件，内核通过对这些参数的在线修改来反馈其中就绪的事件。如果用户需要的话需要创建三个fdset以监听不同类型的事件。|统一处理所有的事件类型，因此只需要一个事件集参数。用户通过pollfd.events来传入感兴趣的事件，内核通过修改pollfd.revents反馈其中就绪的事件。|内核通过一个事件表直接管理用户感兴趣的所有事件。每次调用epoll_wait，内核直接在调用参数的events中注册就绪事件。
应用程序索引效率|采用轮询方式，O(n)|采用轮询方式，O(n)|采用回调方式，O(1)
最大支持文件描述符数|一般1024|65535|65535
工作模式|条件触发|条件触发|条件触发或边缘触发



- poll与epoll分别用nfds和maxevents来表示最多能监听多少个文件描述符，他能达到系统所允许打开的最大文件描述符数65535。而select允许监听的最多文件描述符只有1024个。
