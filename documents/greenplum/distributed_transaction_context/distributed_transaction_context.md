## 环境和版本号  

### 操作系统  
> LSB Version:	:core-4.1-amd64:core-4.1-noarch  
> Distributor ID:	CentOS  
> Description:	CentOS Linux release 7.9.2009 (Core)  
> Release:	7.9.2009  
> Codename:	Core  

### 数据库版本号  
> greenplum-6.12  

## 处理过程  

### sql示例1  

#### 表结构  

#### sql语句  
> insert into tbl_test1 select * from tbl_test2;  

### QD  
函数StartTransactionCommand  
```
void
StartTransactionCommand(void)
{
	if (Gp_role == GP_ROLE_DISPATCH)
		// DistributedTransactionContext 由 DTX_CONTEXT_LOCAL_ONLY 变更为 DTX_CONTEXT_QD_DISTRIBUTED_CAPABLE
		setupRegularDtxContext();

        TransactionState s = CurrentTransactionState;

        switch (s->blockState)
        {
                case TBLOCK_DEFAULT:
                        StartTransaction();
                        s->blockState = TBLOCK_STARTED;
                        break;
        }
}
```

函数 cdbdisp_dispatchX  
```
static void
cdbdisp_dispatchX(QueryDesc* queryDesc,
					bool planRequiresTxn,
					bool cancelOnError)
{
	for (iSlice = 0; iSlice < nSlices; iSlice++)
	{
		// 分别发送query给 QE reader, QE writer
		cdbdisp_dispatchToGang(ds, primaryGang, si);
	}
}
```

函数 CommitTransactionCommand  
```
void
CommitTransactionCommand(void)
{
	TransactionState s = CurrentTransactionState;

	switch (s->blockState)
	{
		case TBLOCK_STARTED:
			CommitTransaction();
			s->blockState = TBLOCK_DEFAULT;
			break;
	}
}
```

函数 CommitTransaction  
```
static void
CommitTransaction(void)
{
	// 发送 DTX_PROTOCOL_COMMAND_PREPARE 给 QE writer
	prepareDtxTransaction();

	// 发送 DTX_PROTOCOL_COMMAND_COMMIT_PREPARED 给 QE writer
	notifyCommittedDtxTransaction();

	// DistributedTransactionContext 由 DTX_CONTEXT_QD_DISTRIBUTED_CAPABLE 变更为 DTX_CONTEXT_LOCAL_ONLY
	finishDistributedTransactionContext("CommitTransaction", false);
}
```

### QE reader  
#### 处理 query 
函数 setupQEDtxContext  
```
void
setupQEDtxContext(DtxContextInfo *dtxContextInfo)
{
        // DistributedTransactionContext 由 DTX_CONTEXT_LOCAL_ONLY 变更为 DTX_CONTEXT_QE_READER
        setDistributedTransactionContext(DTX_CONTEXT_QE_TWO_PHASE_IMPLICIT_WRITER);
}
```

函数StartTransactionCommand  
```
void
StartTransactionCommand(void)
{

	TransactionState s = CurrentTransactionState;

	switch (s->blockState)
	{
		case TBLOCK_DEFAULT:
			StartTransaction();
			s->blockState = TBLOCK_STARTED;
			break;
	}
}
```

函数 CommitTransactionCommand  
```
void
CommitTransactionCommand(void)
{
	TransactionState s = CurrentTransactionState;

	switch (s->blockState)
	{
		case TBLOCK_STARTED:
			// DistributedTransactionContext由DTX_CONTEXT_QE_READER 变更为 DTX_CONTEXT_LOCAL_ONLY
			CommitTransaction();
			s->blockState = TBLOCK_DEFAULT;
			break;
	}
}
```

### QE writer  
#### 处理 query   
函数 setupQEDtxContext  
```
void
setupQEDtxContext(DtxContextInfo *dtxContextInfo)
{
	// DistributedTransactionContext由DTX_CONTEXT_LOCAL_ONLY 变更为 DTX_CONTEXT_QE_TWO_PHASE_IMPLICIT_WRITER
	setDistributedTransactionContext(DTX_CONTEXT_QE_TWO_PHASE_IMPLICIT_WRITER);
}
```
函数StartTransactionCommand
```
void
StartTransactionCommand(void)
{

        TransactionState s = CurrentTransactionState;

        switch (s->blockState)
        {
                case TBLOCK_DEFAULT:
                        StartTransaction();
                        s->blockState = TBLOCK_INPROGRESS;
                        break;
        }
}
```

#### 处理 DTX_PROTOCOL_COMMAND_PREPARE  
函数performDtxProtocolPrepare  
```
static void
performDtxProtocolPrepare(const char *gid)
{
	StartTransactionCommand();

	// CurrentTransactionState->blockState 由TBLOCK_INPROGRESS变更为TBLOCK_END再变更为 TBLOCK_PREPARE
	PrepareTransactionBlock();

	// CurrentTransactionState->blockState 由TBLOCK_PREPARE变更为 TBLOCK_DEFAULT
	CommitTransactionCommand();
	// DistributedTransactionContext由DTX_CONTEXT_QE_TWO_PHASE_IMPLICIT_WRITER 变更为 DTX_CONTEXT_QE_PREPARED
	setDistributedTransactionContext(DTX_CONTEXT_QE_PREPARED);
}
```

#### 处理 DTX_PROTOCOL_COMMAND_COMMIT_PREPARED  
函数 performDtxProtocolCommand  
```
void
performDtxProtocolCommand(DtxProtocolCommand dtxProtocolCommand,
						  const char *gid,
						  DtxContextInfo *contextInfo)
{
	switch (dtxProtocolCommand)
	{
	case DTX_PROTOCOL_COMMAND_COMMIT_PREPARED:
		// DistributedTransactionContext 由DTX_CONTEXT_QE_PREPARED变更为 DTX_CONTEXT_QE_FINISH_PREPARED
		setDistributedTransactionContext(DTX_CONTEXT_QE_FINISH_PREPARED);
		performDtxProtocolCommitPrepared(gid, true);
		break;
	}
}
```

函数 performDtxProtocolCommitPrepared  
```
static void
performDtxProtocolCommitPrepared(const char *gid, bool raiseErrorIfNotFound)
{
	// CurrentTransactionState->blockState由TBLOCK_DEFAULT 变更为 TBLOCK_STARTED
	StartTransactionCommand();
	FinishPreparedTransaction((char *) gid, true, raiseErrorIfNotFound);
	
	// 提交事务
	// DistributedTransactionContext由DTX_CONTEXT_QE_FINISH_PREPARED 为 DTX_CONTEXT_LOCAL_ONLY
	// CurrentTransactionState->blockState由TBLOCK_STARTED 变更为 TBLOCK_DEFAULT
	CommitTransactionCommand();
}

```
