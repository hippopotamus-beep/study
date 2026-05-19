## 说明  
以下安装测试在同一台机器上  
postgres9.6  

## 配置  
### 主从配置  
1. 目录  
    ```
    cd  /home/hgm/Software/postgresql/pg_basebackup_test/async
    ```
2. 初始化主库  
    ```
    initdb -D master
    ```
3. 配置主库的master/postgresql.conf  
    ```
    listen_addresses = '*'
    port = 5432
    max_connections = 3000
    logging_collector = on
    log_directory = 'log'

    max_files_per_process = 5000
    wal_level = replica
    max_wal_senders = 10
    wal_keep_segments = 128000000
    synchronous_commit = off

    # These settings are ignored on a master server.
    hot_standby = on
    hot_standby_feedback = on
    
    # These settings are ignored on a standby server.
    synchronous_standby_names = 'slavenode'
    
    # need to change
    archive_mode = on
    archive_command = 'rsync %p hgm@192.168.2.101:/home/hgm/Software/postgresql/pg_basebackup_test/async/archivedir_master/%f'
    ```
4. 配置主库的master/pg_hba.conf  
    ```
    host    all             all         192.168.2.1/24          trust
    host    replication     hgm         192.168.2.1/24          trust
    ```
5. 启动主库  
    ```
    pg_ctl start -D master
    ```
6. 使用pg_basebackup生成备库  
    ```
    pg_basebackup -D slave -F p -X fetch -R -P -h 192.168.2.101 -p 5432 -U hgm
    ```
7. 修改备库的slave/postgresql.conf  
    ```
    # need to change
    port = 5433
    archive_mode = off
    archive_command = ''
    ```
8. 修改备库slave/recovery.conf  
    ```
    standby_mode = on
    primary_conninfo = 'user=hgm host=192.168.2.101 port=5432 sslmode=prefer sslcompression=1 application_name=slavenode'
    recovery_target_timeline = 'latest'
    ```
9. 启动备库  
    ```
    pg_ctl start -D slave
    ```

### 主备切换  
1. 停止主库  
    ```
    pg_ctl stop -D master
    ```
2. 提升备库为主库  
    ```
    pg_ctl promote -D slave
    ```
3. 修改现主库slave/postgresql.conf  
    ```
    # need to change
    port = 5433
    archive_mode = on
    archive_command = 'rsync %p hgm@192.168.2.101:/home/hgm/Software/postgresql/pg_basebackup_test/async/archivedir_slave/%f'
    ```
4. 重读现主库  
    ```
    pg_ctl restart -D slave
    ```
5. 修改现备库master/postgresql.conf  
    ```
    # need to change
    port = 5432
    archive_mode = off
    archive_command = ''
    ```
6. 配置现备库master/recovery.conf  
    ```
    standby_mode = on
    primary_conninfo = 'user=hgm host=192.168.2.101 port=5433 sslmode=prefer sslcompression=1 application_name=masternode'
    recovery_target_timeline = 'latest'
    ```
7. 启动现备库  
    ```
    pg_ctl start -D master
    ```

## 函数  
pg_is_in_recovery()  
true表示备库，false表示主库  

## 表  
pg_stat_replication  

## 插件  
pg_controldata  

## 参考资料  
[使用pg_basebackup工具搭建流复制环境以及主备切换方法](https://blog.csdn.net/prettyshuang/article/details/50898363)  
[postgresql 主从复制并切换](https://www.cnblogs.com/yhq1314/p/10119556.html)  

