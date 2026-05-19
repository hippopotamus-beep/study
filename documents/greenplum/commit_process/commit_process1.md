## 版本号  
greemplum-6.12   

## sql  
sql1  

```
insert into foreign_table select gsid, gsid||'name', 'name'||gsid from generate_series(1,5) as gsid;
```

sql2  

```
begin;
insert into foreign_table select gsid, gsid||'name', 'name'||gsid from generate_series(1,5) as gsid;
end;
```

## sql1的处理  

### QD(Query Dispather)  

设置CurrentTransactionState->blockState状态为TBLOCK_STARTED  

发送QueryDesc, Plan等内容  

完成事务  

### QE(Query Executor)  

* 设置CurrentTransactionState->blockState状态为TBLOCK_STARTED  
* ```
  setupQEDtxContext
  ```

  设置DistributedTransactionContext为DTX_CONTEXT_QE_AUTO_COMMIT_IMPLICIT  
* 执行plan  
* 完成事务  

## sql2的处理  

### BEGIN处理  

#### QD(Query Dispather)  

* ```
  start_xact_command
  ```

  StartTransactionCommand  

```
设置 DistributedTransactionContext 状态为DTX_CONTEXT_QD_DISTRIBUTED_CAPABLE
设置CurrentTransactionState->blockState状态为 TBLOCK_STARTED
```

* standard_ProcessUtility  

```
调用BeginTransactionBlock,设置CurrentTransactionState->blockState为 TBLOCK_BEGIN
```

调用sendDtxExplicitBegin  

设置MyTmGxactLocal->state为DTX_STATE_ACTIVE_DISTRIBUTED  

设置MyTmGxactLocal->explicitBeginRemembered为true  

* ```
  finish_xact_command
  ```

```
设置CurrentTransactionState->blockState状态为TBLOCK_INPROGRESS
```

### insert into foreigntable  

#### QD(Query Dispather)  

* cdbdisp_dispatchX

调用cdbdisp_buildPlanQueryParms->mppTxnOptions，设置  txnOptions & GP_OPT_EXPLICT_BEGIN  
之后会将 txnOptions 存放于 DtxContextInfo->distributedTxnOptions，随QueryDesc, Plan一起发给QE  

#### QE(Query Executor)  

随QueryDesc, Plan收到  

* ```
  setupQEDtxContext
  ```

  设置DistributedTransactionContext为DTX_CONTEXT_QE_TWO_PHASE_EXPLICIT_WRITER  
  调用doQEDistributedExplicitBegin将CurrentTransactionState->blockState状态置为TBLOCK_INPROGRESS  

### END 处理  

#### QD(Query Dispather)  

* ```
  standard_ProcessUtility
  ```

  调用EndTransactionBlock，CurrentTransactionState->blockState状态变更为TBLOCK_END  
* ```
  finish_xact_command
  ```

  调用CommitTransaction->prepareDtxTransaction  
  prepareDtxTransaction发送Dtx到QE  

#### QE(Query Executor)  

收到Dtx相关信息  

* ```
  exec_mpp_dtx_protocol_command
  ```

  调用performDtxProtocolCommand->performDtxProtocolCommitOnePhase, 结束事务  

## 其他  

如果某个节点出错，QD会通过 signalQEs 通知其他节点。  
sql1中如果某个节点已提交，则会忽略信号通知  
