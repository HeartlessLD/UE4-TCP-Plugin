# UE4-TCP-Plugin
简单的UE4 tcp通信插件
注意 这个插件目前只支持简单的字符串传送，没有做粘包处理（接收 发送进行了+1操作），如果频繁发送字节流数据的话可能会错误

+ 实现了简单的TCPServer TCPClient 相互连接 发送数据
[github地址](https://github.com/HeartlessLD/UE4-TCP-Plugin)
#### 用法
+ 服务器创建
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200506170549404.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L21heGlhb3NoZW5nNTIx,size_16,color_FFFFFF,t_70)

+ 客户端创建并连接
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200506170629572.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L21heGlhb3NoZW5nNTIx,size_16,color_FFFFFF,t_70)
连接回调 ConnectedServerResult
