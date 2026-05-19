## 版本号  
greemplum-6.12  


## sql  

```
insert into local_table select gsid, gsid||'name' from generate_series(1,5) as gsid;
```

## 处理过程   

### QD  

#### 函数 StartTransactionCommand  

调用 setupRegularDtxContext 设置 DistributedTransactionContext 为 DTX_CONTEXT_QD_DISTRIBUTED_CAPABLE  
设置 CurrentTransactionState->blockState 为 TBLOCK_STARTED  

#### 函数 standard_ExecutorStart  

调用 setupDtxTransaction， 将 MyTmGxactLocal->state 设置为 DTX_STATE_ACTIVE_DISTRIBUTED  
调用 CdbDispatchPlan->cdbdisp_dispatchX->cdbdisp_buildPlanQueryParms->mppTxnOptions 设置 options |= GP_OPT_NEED_DTX，之后发送给 QE  

#### 函数doPrepareTransaction  

设置MyTmGxactLocal->state 状态为 DTX_STATE_PREPARING  
调用currentDtxDispatchProtocolCommand发送DTX_PROTOCOL_COMMAND_PREPARE给QE  
在QE处理DTX_PROTOCOL_COMMAND_PREPARE后，设置MyTmGxactLocal->state状态为DTX_STATE_PREPARED  

#### 函数RecordTransactionCommit  

调用insertingDistributedCommitted， 变更MyTmGxactLocal->state状态为DTX_STATE_INSERTING_COMMITTED  
调用insertedDistributedCommitted，变更MyTmGxactLocal->state状态为DTX_STATE_INSERTED_COMMITTED  

#### 函数doNotifyingCommitPrepared  

变更MyTmGxactLocal->state状态为DTX_STATE_NOTIFYING_COMMIT_PREPARED  
调用currentDtxDispatchProtocolCommand发送DTX_PROTOCOL_COMMAND_COMMIT_PREPARED给QE  
commit  

### QE  

#### 函数 setupQEDtxContext  

设置 DistributedTransactionContext 状态为 DTX_CONTEXT_QE_TWO_PHASE_IMPLICIT_WRITER  

#### 函数StartTransactionCommand  

设置 CurrentTransactionState->blockState 为 TBLOCK_INPROGRESS  

#### 函数performDtxProtocolPrepare  

调用PrepareTransactionBlock，CurrentTransactionState->blockState状态更改为TBLOCK_PREPARE  
调用CommitTransactionCommand，CurrentTransactionState->blockState状态更改为TBLOCK_DEFAULT  
设置DistributedTransactionContext状态为DTX_CONTEXT_QE_PREPARED  

#### 函数performDtxProtocolCommitPrepared  

在之前DistributedTransactionContext状态变更为DTX_CONTEXT_QE_FINISH_PREPARED  
commit  
DistributedTransactionContext变更为DTX_CONTEXT_LOCAL_ONLY  
