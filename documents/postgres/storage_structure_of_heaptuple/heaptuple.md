## 环境  
ubuntu16.04 little-endian  

## pg版本号  
postgres-13.3  

## 准备  
建表  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/761dd704_10017097.png)

插入数据  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/9b5e815a_10017097.png)

## heaptuple结构  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/Screenshot%20from%202023-02-03%2018-08-11.png)  
上图表示了tuple的数据存储结构，位于 include/storage/bufpage.h

## 从文件中解读  
数据文件位于base/${dbid}/${relfilenode}  
### 头部解读  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/538a34f3_10017097.png)
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/b81d961f_10017097.png)  
到PageHeaderData->pd_linp 总共24个字节，其中  
&nbsp;&nbsp;&nbsp;&nbsp;0300 0000 e058 872e 是 pd_lsn  
&nbsp;&nbsp;&nbsp;&nbsp;0000 是 pd_checksum  
&nbsp;&nbsp;&nbsp;&nbsp;0000 是 pd_flags  
&nbsp;&nbsp;&nbsp;&nbsp;2000 是 pd_lower, 转换后为 0020, 正好与':'左边 00000020 对上  
&nbsp;&nbsp;&nbsp;&nbsp;501f 是 pd_upper, 转换后为 1f50  
&nbsp;&nbsp;&nbsp;&nbsp;0020 是 pd_special，转换后为 2000, 即为8k，表示不存在special space  
&nbsp;&nbsp;&nbsp;&nbsp;0420 是 pd_pagesize_version, 转换后为 2004, 04表示 page layout version, 2000表示 block size 为8k  
&nbsp;&nbsp;&nbsp;&nbsp;0000 0000 是 pd_prune_xid  
24字节之后的 a89f a200, 509f a800 分别指向第一行，第二行数据.  
以a89f a200为例: a89f a200 转换后为 00000000 10100010 10011111 10101000 -> 000000001010001 01 001111110101000,  
&nbsp;&nbsp;&nbsp;&nbsp;其中000000001010001 表示数据长度为81， 01表示used,  001111110101000表示偏移量8104, 16进制为1fa8

### 数据解读  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/26dd9ecc_10017097.png)
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/1c601743_10017097.png)  
502行':'左边的 00001f50 正好与 pd_upper 对上  
507行':'右边的 be2f 的位置(1fa8)正好与pd_linp的指向的第一行数据的偏移量1fa8对上  
以bbbb这行数据为例,前23个字节为头部  
&nbsp;&nbsp;&nbsp;&nbsp;bf2c 0000是t_xmin  
&nbsp;&nbsp;&nbsp;&nbsp;0000 0000是t_xmax  
&nbsp;&nbsp;&nbsp;&nbsp;0000 0000是t_cid  
&nbsp;&nbsp;&nbsp;&nbsp;0000 0000 0200是t_ctid 
&nbsp;&nbsp;&nbsp;&nbsp;0a00是t_infomask2，0a表示含有10个字段  
&nbsp;&nbsp;&nbsp;&nbsp;0308是t_infomask，08表示HEAP_XMAX_INVALID，03表示 HEAP_HASNULL | HEAP_HASVARWIDTH  
&nbsp;&nbsp;&nbsp;&nbsp;20是t_hoff，表示数据起始偏移量为32  
&nbsp;&nbsp;&nbsp;&nbsp;ff01是t_bits，表示第10个字段为空  
&nbsp;&nbsp;&nbsp;&nbsp;00 0000 0000 0000为padding  
&nbsp;&nbsp;&nbsp;&nbsp;0200 0000表示数值2  
&nbsp;&nbsp;&nbsp;&nbsp;0d62 6262 6262： 0d -> 00001101 表示长度占1个字节，总共长度为6  
&nbsp;&nbsp;&nbsp;&nbsp;0000为padding  

## 修改  
可以通过编辑文件修改数据内容,
修改后需要等待一定时间或者查询其他数据或者重启，因为数据可能存在缓存导致查出来并没有改变  
![image.png](https://github.com/hanguanmiao/study/blob/main/pictures/postgres/storage_structure_of_heaptuple/357cbe2d_10017097.png)
