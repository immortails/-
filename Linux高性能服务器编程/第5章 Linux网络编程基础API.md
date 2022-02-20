# Socket 地址API
## 主机字节序与网络字节序
- 字节序分为大端序和小端序。如0X0102，若01被存在内存的低位，02被存在高位，就是大端序，不然就是小端序。或者说 char a[2]=(char)0x0102; 
如果a[0]=是1，a[1]=2，那么就是大端序，反之就是小端序。左边是端，左边谁存的大就是谁。
- 现代PC大多采用小端序，所以小端序又被称为主机字节序；在网络中传递时，为了统一一般都把各个主机的字节序变为大端序，所以大端序也叫作网络字节序。
- Linux提供了4个函数来完成主机字节序和网络字节序的转换问题：都十分好记。
```
unsigned long int htonl(unsigned long int hostlong);
unsigned short int htons(unsigned long int hostshort);
unsigned long int ntohl(unsigned long int netlong);
unsigned short int ntohs(unsigned long int netshort);
```
## 通用socket地址
- Socket网络编程接口表示socket地址的是sockaddr：
```cpp
#include<sys/socket.h>
    struct sockaddr{
    sa_family_t sa_family;
    char sa_data[14];
}
```
- 这个格式通常比较复杂，通常程序员都是用sockaddr_in/sockaddr_in6这两个结构体：一般来说这个用的比较多一些
```cpp
struct  sockaddr_in{
    sa_family_t sin_family;
    u_int16_t sin_port;
    in_addr sin_addr;
}

struct  in_addr{
    u_int32_t s_addr;
}
```
## IP地址转换函数
- 一般人们输入IP地址时，都是使用点分十进制的字符串来表示IP地址，但是计算机并不能理解这个意思，所以需要将其转换为用整数表示的IP地址才能使用，日志记录时正好相反，需要把二进制数转换成点分十进制的字符串来表示IP地址：linux系统提供了3个函数来实现以下功能：
```cpp
#include<arpa/inet.h>
int_addr_t inet_addr(const char* strptr);
int inet_aton(const char* cp, in_addr* inp);
char* inet_ntoa(in_addr* in)
```
- inet_addr与inet_aton所实现的是相同的一件事，只不过inet_aton是把ip地址直接加载到inp中。
inet_ntoa注意该函数内部用一个静态变量存储转化的字符串，所以其是不可重入的。如果用个指针指向该返回结果。
- 对于IPv6则是如下：af表示的是地址簇，可以指定AF_INET还是AF_INET6。
```cpp
#include<arpa/inet.h>
int inet_pton(int af,const char* src, void* dat);
const char* inet_ntop(int af,const void* src,char* dst,socklen_t cnt);
```
# SOCKET创建、绑定、监听、接收连接、发起连接、关闭连接
## socket创建
- Unix/Linux哲学就是：everything is file。socket就是一个可读可写、可控制可关闭的文件描述符。
domain参数指出使用哪个底层协议簇，如IPV4就是PF_INET,type是指服务类型，一般有SOCK_STREAM和SOCK_UGRAM服务，一个是基于流的对应着TCP，一个是基于数据报的对应着UDP。，最后一个protocol是指具体的协议，一般选0默认。
```cpp
#include<sys/socket.h>
int socket(int domain,int type,int protocol);
```
## 绑定socket
- socket绑定是给socket分配一个主机上具体的IP与端口号，在服务端我们需要这么做；但在客户端我们不需要，因为客户端是自动分配IP与端口号。绑定用的是bind：
```cpp
#include<sys/socket.h>
int bind(int sockfd,const sockaddr* my_addr,socklen_t addrlen);
```
- my_addr是指socket所绑定的地址，然后addrlen是指地址的长度。bind绑定成功时，会返回0，失败时会返回-1并设置errno，其中两种常见的是EACCES和EADDRINUSE。
- - EACCES是指绑定的地址受保护，只有sudo才能访问。
- - EADDRINUSE是指被绑定的端口正在被使用，eg：socket绑定到一个处于TIME_WAIT的状态的socket地址。
## 监听socket
- socket和地址绑定后，我们还不能立刻接收客户连接，需要使用listen，来创建监听队列来存放待处理的客户连接。sockfd即被监听的socket，backlog是指监听队列的长度，如果超过，服务器将不受理新的连接，客户端收到ECONNREFUSE的错误信息。在Linux2.2版本以前，backlog是指所有的半连接状态(SYN_RCVD)与完全连接状态(ESTABLISHED)之和；在2.2版本之后，它只表示完全连接的状态，也就是说只包括连接建立之后还没有accept的连接。半连接状态的数则是由内核参数定义。
- listen成功时返回0，失败返回-1，并设置errno。
```cpp
#include<sys/socket.h>
int listen(int sockfd,int backlog);
```
## 接受连接
- 建立起的连接要被应用程序使用需要accept，accept成功时会返回一个建立连接的socket。通过该socket可以与对应的客户端进行通信。accept失败时会返回-1并设置errno。这时这个连接会从监听队列移动到应用程序的手中，监听队列就会空出来一个位置。
```cpp
#include<sys/socket.h>
int accept(int sockfd,sockaddr* addr,socklen_t* addrlen);
```
## 发起连接
- 发起时指客户端主动与服务器建立连接。sockfd是建立连接所用socket，serv_addr是指服务端的ip地址与端口号，addrlen是指地址的长度。connect成功时返回0，一旦建立了连接，sockfd就唯一的标识了这个连接，客户端就可以通过sockfd来与服务器通信。
```cpp
int connect(int sockfd,const sockaddr* serv_addr,socklen_t addrlen);
```
## 关闭连接
- 因为之前说linux的设计哲学是everything is file。所以可以直接用close关闭这个连接，不过close并不是真正的关闭连接，而是将fd的引用计数-1，只有fd的引用计数为0时，才算真正的关闭连接。尤其是多进程的程序中，这点尤为重要。
- 如果无论如何终止连接，可以用shutdown。shutdown提供3个功能，SHUT_RD、SHUT_WR、SHUT_RDWR，在参数howto中标识。shutdown成功时返回0，失败时返回-1并设置errno。
```cpp
#include<sys/socket.h>
int shutdown(int sockfd,int howto);
```
## 数据读写
### TCP数据读写
- 因为everything is file 所以直接write和read就能用，但是socket也提供了TCP的专用读写函数。
```cpp
#include<sys/socket.h>
ssize_t recv(int sockfd,void* buf,size_t len, int flags);
ssize_t send(int sockfd,const void* buf, size_t len, int flags);
```
- recv中buf与len是缓冲区的位置与大小，flags提供一些接收选项，如MSG_OOB是带外紧急传输，send同理。
- 带外数据的位置会在TCP段头中的偏移量中指明，读取数据时被标为紧急的数据是单独用recv的MSG_OOB选项才能读出来，默认选项是读不出来的。
### UDP数据读写
- UDP是无连接的，所以每次都需要在发送与接收时指明对方的IP与端口号。
```cpp
#include<sys/socket.h>
ssize_t recvfrom(int sockfd,void* buf,size_t len,int flags,sockaddr* src_addr,socklen_t* addrlen);
ssize_t sendto(int sockfd,const void* buf,size_t len,int flags,const sockaddr* dest_addr,socklen_t addrlen);
```
## socket 选项
- socket文件描述符的属性也可以读取和设置。level一般选项是SOL_SOCKET，可以用来设置socket选项，option_name是具体的属性选项，像SO_RCVBUF,SO_SNDBUF是设置接收的缓冲区大小与发送的缓冲区大小。
- SO_REUSEADDR是指强制使用处于TIME_WAIT状态的连接占用的地址，通过setsockopt设置后，即使sock处于TIME_WAIT状态，也可以使用这个端口号，但这样无法可靠的终止连接，万一对面没收到ack;或者有个历史报文段经迟到了，没有这个时间直接发到新的连接上，就会出现问题。
```cpp
#include<sys/socket.h>
int getsockopt(int sockfd,int level, int option_name, void* option_value,socklen_t* restrict option_len);
int setsockopt(int sockfd, int level, int option_name, const void* option_value,socklen_t option_len);
```
## 复位报文段（第三章的部分写错地方了）
- 在某些情况下，TCP连接的一端会向另一端发送RST标志的报文段，以通知对方关闭连接或者重新建立连接。主要有3个情况：
- - 访问不存在的端口，告诉收到复位报文段的一端应该关闭连接或者重新连接，而不能回应这个报文段。当客户端向一个被占用的端口发送连接请求时，也会收到复位报文段。
-  - 异常终止连接，连接正常终止是需要通过4次握手来关闭连接，但TCP提供了一个异常终止连接的方法，就是给对方发送一个复位报文段，一旦发送，发送端的所有排队等待的数据都会被丢弃。
-  - 处理半打开连接，就是说一方关闭了连接，而对方没有收到结束报文段，比如直接拔网线这种，对方此时并不知道你的连接已经关闭了，如果网络恢复后，对方继续发送数据的话，你就会给对方发送一个复位报文段。

## 网络信息API
socket的两个要素IP与端口号都是用数字来标识的，但这两个并不好记。所以一般人们用域名和服务名称来代替ip与端口号。
### 域名与IP
- 通过域名来获取IP是通过gethostbyname函数，gethostbyaddr函数是来获取域名,里面的type跟以前一样是指所用IP地址的类型，如AF_INET与AF_INET6。
```cpp
#include<netdb.h>
hostent* gethostbyname(const char*name);
hostent* gethostbyaddr(const void* addr,size_t len,int type);
struct hostent{
    char* h_name;           //主机名
    char** h_aliases;       //主机别名列表，可能多个
    int h_addrtype;         //地址类型，地址簇
    int h_length;           //地址长度
    char** h_addr_list;     //按网络字节序给出的IP列表
};
```
### 服务名与端口号
服务名与端口号的转换是getservbyname与getservbyport。
```cpp
#include<netdb.h>
servent* getservbyname(const char* name,const* proto);
servent* getservbyport(int port,const char* proto);
struct servent{
    char* s_name;           //服务名称
    char** s_aliases;       //服务别名
    int s_port;             //端口号
    char* s_proto;          //服务类型，TCP还是UDP
}
```
