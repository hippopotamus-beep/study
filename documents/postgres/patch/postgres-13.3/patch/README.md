### 1 制作补丁  
1.1 将postgresql-13.3和postgresql-13.3-new放置于同一目录  
1.2 生成临时补丁  
```
diff -ar postgresql-13.3 postgresql-13.3-new > pg-13.3.tmp.patch
```
1.3 删除多余文件  
```
cat pg-13.3.tmp.patch | grep "Only in" | sed -e "s/Only in //g" -e "s/: /\//g" | xargs rm -rf
```
1.4 生成补丁  
```
diff -Naur postgresql-13.3 postgresql-13.3-new > pg-13.3.patch
```
### 2 打补丁  
2.1 进入源码目录  
2.2 将patch放入源码目录  
2.3 打补丁  
```
patch -p1 < pg-13.3.patch
```
