## 版本号  
greemplum-6.12 

## 情形1  

### 样例sql  

```
create table test1(id int, name varchar) distributed by(id);
create table test4(id int, id2 int, name varchar) distributed by(id);
```

### 执行  

#### 函数doSendTuple  

ExecMotion->execMotionSender->doSendTuple  
调用 evalHashKey计算需要路由到的segid  
调用SendTuple发送数据  
