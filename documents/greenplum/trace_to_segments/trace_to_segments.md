## 版本号  
Linux version 3.10.0-862.3.2.el7.x86_64 (gcc version 4.8.5 20150623 (Red Hat 4.8.5-28) (GCC) )  
PostgreSQL 8.2.15 (Greenplum Database 4.3.99.00 build dev)  

---
## [方法](#fangfa)  

## [原理](#yuanli)  
### [1. psql启动](#psql_qidong)  
- [PostmasterMain](#PostmasterMain)  
    - [StreamServerPort](#StreamServerPort)  
    - [ServerLoop](#ServerLoop)  
        - [BackendStartup](#BackendStartup)  
            - [BackendRun](#BackendRun)  
                - [PostgresMain](#PostgresMain)  

### [2. master与segment通信](#master_segment_tongxin)  
- [ExecutorStart](#ExecutorStart)  
    - [ExecInitMotion](#ExecInitMotion)  
    - [createGang](#createGang)  
    - [cdbdisp_dispatchToGang](#cdbdisp_dispatchToGang)  


---

## <div id="fangfa">方法</div>  

- psql进入master  
```
psql -d database -p 2345
```
- 获取master上的后端进程号  
```
ps -ef --forest | grep postgres
```
![enter image description here](https://github.com/hanguanmiao/study/blob/main/pictures/greenplum/trace_to_segments/b2a0a66e98ce9b9b87bf7345393c2c28.png)  

- gdb联系master上的后端进程号  
```
gdb
attach 34046
b cdbdisp_dispatchPlan
c
```
```
// gp6.0
gdb
attach 34046
b CdbDispatchPlan->cdbdisp_dispatchX->AssignGangs 之后
c
```

- psql查询  
```
SELECT * from test order by id;
```

- 获取segment上的后端进程号  
```
ps -ef --forest | grep postgres
```
![enter image description here](https://github.com/hanguanmiao/study/blob/main/pictures/greenplum/trace_to_segments/266a8bfeecb1437c3a8cebf2f200c9fa.png)  

- gdb联系segment上的后端进程号  
```
gdb
attach 35905
b exec_mpp_query
c
```

- 在master的gdb上  
```
c
```

接下来就可以对segment进行代码跟踪调式了  

---
## <div id="yuanli">原理(代码并非完全复制，请自行参照源码)</div>  
  

### <div id="psql_qidong">1. psql启动</div>  
 

#### <div id="PostmasterMain">函数PostmasterMain</div>  
 
```
/*
 * Postmaster main entry point
 */
int
PostmasterMain(int argc, char *argv[])
{
    // ListenAddresses是postgresql.conf配置的参数listen_addresses
    if (ListenAddresses)
	{
		char	   *rawstring;
		List	   *elemlist;
		ListCell   *l;
		int			success = 0;

		/* Need a modifiable copy of ListenAddresses */
		rawstring = pstrdup(ListenAddresses);

		/* Parse string into list of identifiers */
		if (!SplitIdentifierString(rawstring, ',', &elemlist))
		{
			/* syntax error in list */
			ereport(FATAL,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("invalid list syntax for \"listen_addresses\"")));
		}

		foreach(l, elemlist)
		{
			char	   *curhost = (char *) lfirst(l);

            // StreamServerPort创建和绑定socket,并进行监听
			if (strcmp(curhost, "*") == 0)
				status = StreamServerPort(AF_UNSPEC, NULL,
										  (unsigned short) PostPortNumber,
										  UnixSocketDir,
										  ListenSocket, MAXLISTEN);
			else
				status = StreamServerPort(AF_UNSPEC, curhost,
										  (unsigned short) PostPortNumber,
										  UnixSocketDir,
										  ListenSocket, MAXLISTEN);
			if (status == STATUS_OK)
				success++;
			else
				ereport(WARNING,
						(errmsg("could not create listen socket for \"%s\"",
								curhost)));
		}
	}
	
	// 利用函数ServerLoop和全局静态变量ListenSocket进行循环接受连接请求
	status = ServerLoop();
}
```
#### <div id="StreamServerPort">函数StreamServerPort</div>  
 
```
/*
 * StreamServerPort -- open a "listening" port to accept connections.
 *
 * Successfully opened sockets are added to the ListenSocket[] array,
 * at the first position that isn't PGINVALID_SOCKET.
 *
 * RETURNS: STATUS_OK or STATUS_ERROR
 */

int
StreamServerPort(int family, char *hostName, unsigned short portNumber,
				 char *unixSocketName,
				 pgsocket ListenSocket[], int MaxListen)
{
    for (addr = addrs; addr; addr = addr->ai_next)
	{
	    // 创建socket
	    if ((fd = socket(addr->ai_family, SOCK_STREAM, 0)) < 0)
		{
			ereport(LOG,
					(errcode_for_socket_access(),
			/* translator: %s is IPv4, IPv6, or Unix */
					 errmsg("could not create %s socket: %m",
							familyDesc)));
			continue;
		}
		
		// 绑定socket
		err = bind(fd, addr->ai_addr, addr->ai_addrlen);
		
		// 监听
		err = listen(fd, maxconn);
	}
}
```
#### <div id="ServerLoop">函数ServerLoop</div>  

```
/*
 * Main idle loop of postmaster
 */
static int
ServerLoop(void)
{
    for (;;)
	{
        int			selres;

        if (acceptNewConnections)
        {
            // select进行I/O监视，有新的连接请求返回大于0，
            selres = select(nSockets, &rmask, NULL, NULL, &timeout);
        }
        
        /*
		 * New connection pending on any of our sockets? If so, fork a child
		 * process to deal with it.
		 */
		if (selres > 0)
		{
            int			i;

			for (i = 0; i < MAXLISTEN; i++)
			{
			    if (ListenSocket[i] == -1)
					break;
				if (FD_ISSET(ListenSocket[i], &rmask))
				{
				    Port	   *port;

                    // ConnCreate->StreamConnection->accept，接受连接
					port = ConnCreate(ListenSocket[i]);
					if (port)
					{
					    // 启动后端
					    BackendStartup(port);

						/*
						 * We no longer need the open socket or port structure
						 * in this process
						 */
						StreamClose(port->sock);
						ConnFree(port);
					}
				}
			}
        }
    }
}
```

#### <div id="BackendStartup">函数BackendStartup</div>  

```
/*
 * BackendStartup -- start backend process
 *
 * returns: STATUS_ERROR if the fork failed, STATUS_OK otherwise.
 *
 * Note: if you change this code, also consider StartAutovacuumWorker.
 */
static int
BackendStartup(Port *port)
{
    pid = fork_process();

    if (pid == 0)				/* child */
	{ // fork新的进程与psql进行交互
	    /* Close the postmaster's sockets */
		ClosePostmasterPorts(false);
		
		/* And run the backend */
		proc_exit(BackendRun(port));
	}
}
```

#### <div id="BackendRun">函数BackendRun</div>  

```
/*
 * BackendRun -- set up the backend's argument list and invoke PostgresMain()
 *
 * returns:
 *		Shouldn't return at all.
 *		If PostgresMain() fails, return status.
 */
static int
BackendRun(Port *port)
{
    return PostgresMain(ac, av, port->database_name, port->user_name);
}
```

#### <div id="PostgresMain">函数PostgresMain</div>  

```
/* ----------------------------------------------------------------
 * PostgresMain
 *	   postgres main loop -- all backends, interactive or otherwise start here
 *
 * argc/argv are the command line arguments to be used.  (When being forked
 * by the postmaster, these are not the original argv array of the process.)
 * username is the (possibly authenticated) PostgreSQL user name to be used
 * for the session.
 * ----------------------------------------------------------------
 */
int
PostgresMain(int argc, char *argv[],
			 const char *dbname, const char *username)
{
    /*
	 * Non-error queries loop here.
	 */

	for (;;)
	{
		if (send_ready_for_query)
		{
		    /* 
		     * 1. ReadyForQuery->pq_endmessage->pq_putmessage->internal_putbytes,
		     * internal_putbytes把准备完毕的信息存放在全局静态变量PqSendBufferSize中
		     * 2. ReadyForQuery->pq_flush->internal_flush->secure_write->send，
		     * send通知psql后端准备完毕，至此psql启动完毕
		     */
			if (Gp_role == GP_ROLE_EXECUTE && Gp_is_writer)
			{
				ReadyForQuery_QEWriter(whereToSendOutput);
			}
			else
				ReadyForQuery(whereToSendOutput);
			send_ready_for_query = false;
		}
		
		/* 
		 * 此处接受psql查询
		 * ReadCommand->SocketBackend->pq_getbyte->pq_recvbuf->secure_read->recv,
		 * recv获取psql发送的信息(包括信息类型，长度，sql)，
		 * 并且SocketBackend调用pq_getmessage将sql存放到input_message中。
		 * ReadCommand返回消息类型
		 */
		firstchar = ReadCommand(&input_message);
	}
}
```

---

### <div id="master_segment_tongxin">2. master与segment通信</div>  


#### <div id='ExecutorStart'>函数ExecutorStart</div>  

调用关系: exec_simple_query->PortalStart->ExecutorStart  
```
void
ExecutorStart(QueryDesc *queryDesc, int eflags)
{
    START_MEMORY_ACCOUNT(plannedStmt->memoryAccount);
	{
	    /* If the interconnect has been set up; we need to catch any
		 * errors to shut it down -- so we have to wrap InitPlan in a PG_TRY() block. */
		PG_TRY();
		{
			// InitPlan->ExecInitNode->ExecInitMotion
		    InitPlan(queryDesc, eflags);
		
		    if (Gp_role == GP_ROLE_DISPATCH &&
				queryDesc->plannedstmt->planTree->dispatch == DISPATCH_PARALLEL)
			{
			    /* Assign gang descriptions to the root slices of the slice forest. */
			    // 非常简单的select语句，这里并未做任何改变
				InitRootSlices(queryDesc);
				
				if (!(eflags & EXEC_FLAG_EXPLAIN_ONLY))
				{
					// AssignGangs->allocateWriterGang(primaryWriterGang==NULL)->createGang
					AssignGangs(queryDesc, gp_singleton_segindex);
				}
			}
			
			if (shouldDispatch)
			{
			    // cdbdisp_dispatchPlan->cdbdisp_dispatchX->cdbdisp_dispatchToGang
			    cdbdisp_dispatchPlan(queryDesc, needDtxTwoPhase, true, estate->dispatcherState);
			}
		}
	}
}
```

#### <div id="ExecInitMotion">函数ExecInitMotion</div>  

```
MotionState *
ExecInitMotion(Motion * node, EState *estate, int eflags)
{
    MotionState *motionstate = NULL;
	TupleDesc	tupDesc;
	Slice	   *sendSlice = NULL;
    Slice      *recvSlice = NULL;
    SliceTable *sliceTable = estate->es_sliceTable;

	int			parentSliceIndex = estate->currentSliceIdInPlan;
	
	/* Look up the sending gang's slice table entry. */
    sendSlice = (Slice *)list_nth(sliceTable->slices, node->motionID);
    
    /* QD must fill in the global slice table. */
	if (Gp_role == GP_ROLE_DISPATCH)
	{   // 主要作用是将sendSlice置为recvSlice的child，并且设置sendSlice
	    /* Look up the receiving (parent) gang's slice table entry. */
		recvSlice = (Slice *)list_nth(sliceTable->slices, parentSliceIndex);
		
		/* Sending slice become a children of recv slice */
		recvSlice->children = lappend_int(recvSlice->children, sendSlice->sliceIndex);
		sendSlice->parentIndex = parentSliceIndex;
        sendSlice->rootIndex = recvSlice->rootIndex;

		/* The gang beneath a Motion will be a reader. */
		sendSlice->gangType = GANGTYPE_PRIMARY_READER;
	}
}
```


#### <div id="createGang">函数createGang</div>  

```
/*
 * creates a new gang by logging on a session to each segDB involved
 *
 */
static Gang *
createGang(GangType type, int gang_id, int size, int content, char *portal_name)
{
    for (i = 0; i < threadCount; i++)
	{
	    /* 
	     * thread_DoConnect->cdbconn_doConnect->PQconnectdbParams连接segment数据库，
	     * 类似于psql连接postgres数据库
	     */
	    pthread_err = gp_pthread_create(&pParms->thread, thread_DoConnect, pParms, "createGang");
	}
	
	/*
	 * wait for all of the DoConnect threads to complete.
	 */
	for (i = 0; i < threadCount; i++)
	{
		DoConnectParms *pParms = &doConnectParmsAr[i];

		if (0 != pthread_join(pParms->thread, NULL))
		{
			elog(FATAL, "could not create segworker group");
		}
	}
}
```

#### <span id="cdbdisp_dispatchToGang">函数cdbdisp_dispatchToGang</span>  

```
void
cdbdisp_dispatchToGang(struct CdbDispatcherState *ds,
					   GpDispatchCommandType		mppDispatchCommandType,
					   void						   *commandTypeParms,
                       struct Gang                 *gp,
                       int                          sliceIndex,
                       unsigned int                 maxSlices,
                       CdbDispatchDirectDesc		*disp_direct)
{
    /*
	 * Create the threads. (which also starts the dispatching).
	 */
	for (i = 0; i < newThreads; i++)
	{
	    DispatchCommandParms *pParms = &(ds->dispatchThreads->dispatchCommandParmsAr + ds->dispatchThreads->threadCount)[i];
		
	    if (gp_connections_per_thread==0)
	    {
			Assert(newThreads <= 1);
			thread_DispatchOut(pParms);
	    }
	    else
	    {
			int		pthread_err = 0;

			pParms->thread_valid = true;
			/*
			 * thread_DispatchCommand->thread_DispatchOut->dispatchCommandQuery->PQsendGpQuery_shared，
			 * 类似于psql发送查询信息
			 */
			pthread_err = gp_pthread_create(&pParms->thread, thread_DispatchCommand, pParms, "dispatchToGang");
	    }
	}
}
```


