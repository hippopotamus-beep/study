## 版本号  
greemplum-6.12  

## sql  

```
insert into foreign_table select gsid, gsid||'name', 'name'||gsid from generate_series(1,5) as gsid;
```

本篇记录将外地表做类似本地表处理后，后续对sql的状态变化  

## 处理过程  

### QD  

#### 函数 StartTransactionCommand  

调用 setupRegularDtxContext 设置 DistributedTransactionContext 为 DTX_CONTEXT_QD_DISTRIBUTED_CAPABLE  
设置 CurrentTransactionState->blockState 为 TBLOCK_STARTED  

#### 函数 standard_ExecutorStart  

调用 setupDtxTransaction， 将 MyTmGxactLocal->state 设置为 DTX_STATE_ACTIVE_DISTRIBUTED  
调用 CdbDispatchPlan->cdbdisp_dispatchX->cdbdisp_buildPlanQueryParms->mppTxnOptions 设置 options |= GP_OPT_NEED_DTX，之后发送给 QE  

#### 函数doNotifyingOnePhaseCommit  

设置MyTmGxactLocal->state 状态为DTX_STATE_NOTIFYING_ONE_PHASE_COMMIT  
调用currentDtxDispatchProtocolCommand发送DTX_PROTOCOL_COMMAND_COMMIT_ONEPHASE给QE  

#### 函数ClearTransactionState  

调用ProcArrayEndGxact->resetGxact将MyTmGxactLocal->state变更为DTX_STATE_NONE  

#### 函数finishDistributedTransactionContext  

将DistributedTransactionContext状态变更为DTX_CONTEXT_LOCAL_ONLY  

### QE  

#### 函数 setupQEDtxContext  

设置 DistributedTransactionContext 状态为 DTX_CONTEXT_QE_TWO_PHASE_IMPLICIT_WRITER  

#### 函数StartTransactionCommand  

设置 CurrentTransactionState->blockState 为 TBLOCK_INPROGRESS  

#### 函数performDtxProtocolCommitOnePhase  

调用 CommitTransactionCommand，CurrentTransactionState->blockState 状态更改为 TBLOCK_DEFAULT  
设置 DistributedTransactionContext 状态为 DTX_CONTEXT_LOCAL_ONLY  
