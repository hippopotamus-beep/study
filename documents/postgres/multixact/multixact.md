# multixact的创建与存储  
## 版本号  
postgres-13.3  

## 环境  
centos-7.9  

## 宏定义  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-08%2011-39-52.png)  
> #define MULTIXACT_OFFSETS_PER_PAGE		2048  
> #define MultiXactIdToOffsetPage(xid)			(xid/2048)  
> #define MultiXactIdToOffsetEntry(xid)			(xid%2048)  
> #define MultiXactIdToOffsetSegment(xid)		(xid/2048)/32  

![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-08%2011-13-17.png)  
> #define MXACT_MEMBER_BITS_PER_XACT			8  
> #define MXACT_MEMBER_FLAGS_PER_BYTE			1  
> #define MXACT_MEMBER_XACT_BITMASK   			0xff  
> #define MULTIXACT_FLAGBYTES_PER_GROUP			4  
> #define MULTIXACT_MEMBERS_PER_MEMBERGROUP  	4  
> #define MULTIXACT_MEMBERGROUP_SIZE			20  
> #define MULTIXACT_MEMBERGROUPS_PER_PAGE		409  
> #define MULTIXACT_MEMBERS_PER_PAGE			1636  

![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-08%2011-50-22.png)  
> #define MAX_MEMBERS_IN_LAST_MEMBERS_PAGE		1036  
> #define MXOffsetToMemberPage(xid)			((xid)/1636)  
> #define MXOffsetToMemberSegment(xid)		((xid)/1636/24)  
> #define MXOffsetToFlagsOffset(xid)			((((xid) / 4) % 409) * 20)  
> #define MXOffsetToFlagsBitShift(xid)			(((xid) % 4) * 8)  
> #define MXOffsetToMemberOffset(xid)			(((((xid) / 4) % 409) * 20) + 4 + ((xid) % 4) * 4)  
> #define MULTIXACT_MEMBER_SAFE_THRESHOLD		2147483647  
> #define MULTIXACT_MEMBER_DANGER_THRESHOLD	3221225472  

## multixact的创建  
### 函数  
#### 函数 MultiXactIdCreate  
该函数用于将xids组合成multixid  

#### 函数 RecordNewMultiXact  
该函数用于记录 multixact  

```
static void
RecordNewMultiXact(MultiXactId multi, MultiXactOffset offset,
				   int nmembers, MultiXactMember *members)
{
	pageno = MultiXactIdToOffsetPage(multi); // offset页面号
	entryno = MultiXactIdToOffsetEntry(multi); // offset入口号

	for (i = 0; i < nmembers; i++, offset++)
	{
		pageno = MXOffsetToMemberPage(offset); // member页面号
		memberoff = MXOffsetToMemberOffset(offset); // member偏移量
		flagsoff = MXOffsetToFlagsOffset(offset);  // flag偏移量
		bshift = MXOffsetToFlagsBitShift(offset);  // 设置flag的比特位偏移量
	}
}
```


### 测试  
进程1，先执行sql  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-07%2019-33-04.png)  
> t_infomask 402转换成16进制为0x192  
>     0x0100 表示HEAP_XMIN_COMMITTED  
>     0x0090 表示 HEAP_XMAX_LOCK_ONLY | HEAP_XMAX_KEYSHR_LOCK  
>     0x0002 表示 HEAP_HASVARWIDTH  
> t_xmax 3128表示进程3128获得了该锁  

进程2  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-07%2019-33-30.png)  
> t_infomask 402转换成16进制为11d2  
>     0x1000 表示HEAP_XMAX_IS_MULTI  
>     0x00d2表示 HEAP_XMAX_LOCK_ONLY | HEAP_XMAX_EXCL_LOCK | HEAP_XMAX_KEYSHR_LOCK  
> t_xmax 5表示 multixact 5 获得了该锁  

### 文件解读  
> 记录multixact的有两个文件，分别是member, offset  
>     member位于pg_multixact/members  
>     offset位于pg_multixact/offsets  

#### 文件offset解读  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-07%2017-38-58.png)  
> 该文件的最后一个offset是 0900 0000， 经过MXOffsetToMemberOffset计算，member偏移量48 bytes  
>     经过MXOffsetToFlagsOffset计算，flag偏移量40 bytes  
>     经过MXOffsetToFlagsBitShift计算, flag bit偏移 8 bit  

#### 文件member解读  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/multixact/Screenshot%20from%202023-02-07%2017-40-10.png)  
> 由offset可知 0000 0100 2e0c 0000 380c 0000 398c 0000 为member内容  
>     2e0c 0000是上一条的事务号  
>     380c 0000是事务号3128  
>     398c 0000是事务号3129  
>     00 01分别对应3128、3129锁类型 MultiXactStatusForKeyShare、MultiXactStatusForShare  

## multixact的扩展  
### 函数MultiXactIdExpand  
该函数并不覆盖之前创建的multixactid，而是会组合之前的xid组合成一个新的multixactid  

## 参考资料  
[PG的multixact是做什么的](https://www.modb.pro/db/14939)  
