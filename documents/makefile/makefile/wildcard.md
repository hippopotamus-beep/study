[Makefile中的wildcard用法](https://blog.csdn.net/liangkaiming/article/details/6267357)

$(wildcard PATTERN)
	被展开为已经存在的、使用空格分隔的、匹配此模式的所有文件列表。如果不存在，则返回空。
	$(wildcard *.c)用于获取工作目录下的所有.c文件

## sample
objects := $(patsubst %.c,%.o,$(wildcard *.c))
foo: $(objects)
	cc -o foo $(objects)


wildcard:扩展通配符
notdir:去处路径
patsubst: 替换通配符
