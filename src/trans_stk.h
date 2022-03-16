#pragma once

namespace libra {



__device__ void lock(int* mutex) {
    while (atomicCAS((int*)mutex, 0, 1) != 0){
    }
  }

  __device__ void unlock(int* mutex) {
    atomicExch((int*)mutex, 0);
  }


__device__ void print_stk(CallStack* _stk,  Pattern* pat){
    printf("iter:");
    for(int i=0; i<PAT_SIZE; i++){
        printf("%d ", _stk->iter[i]);
    }

    printf("\nuiter:");
    for(int i=0; i<PAT_SIZE; i++){
        printf("%d ", _stk->uiter[i]);
    }
    printf("\nslot_size:");

    for(int i=0; i<PAT_SIZE; i++){
        for(int j=pat->rowptr[i]; j<pat->rowptr[i+1]; j++){
            for(int k=0; k<UNROLL; k++){
                printf("%d ", _stk->slot_size[j][k]);
            }
            printf(", ");
        }
        printf("\n");
    }
    printf("\n");
}


__device__
bool trans_layer0(CallStack& _target_stk, CallStack& _cur_stk, Pattern* pat){
    int left_task = JOB_CHUNK_SIZE - _target_stk.iter[0] - UNROLL;
    if(left_task<=0) return false;
    memcpy((_cur_stk.slot_storage[pat->rowptr[0]]), (_target_stk.slot_storage[pat->rowptr[0]]),  GRAPH_DEGREE*UNROLL*sizeof(graph_node_t));
    _cur_stk.iter[0] = _target_stk.iter[0]+UNROLL;
}


__device__
bool trans_layerk_in_unroll_slot(CallStack& _target_stk, CallStack& _cur_stk,  Pattern* pat, int _k){

        if(_target_stk.level<=_k) return false;
        int num_left_task = _target_stk.slot_size[pat->rowptr[_k]][_target_stk.uiter[_k]] - _target_stk.iter[_k]-UNROLL;

        if(num_left_task<=0) return false;
        int target_left_task = num_left_task/2;
        int cur_left_task = num_left_task - target_left_task;
        int stealed_start_idx_in_target = _target_stk.iter[_k]+UNROLL+target_left_task;
        
        //graph_node_t (*slot_storage)[UNROLL][GRAPH_DEGREE] = _cur_stk.slot_storage;
        //_cur_stk = _target_stk;
        //_cur_stk.slot_storage = slot_storage;
        //printf("k:%d, slot_size:%d, iter:%d, target_left_task:%d, cur_left_task:%d, stealed_start_idx_in_target:%d\n", 
        //       _k, _target_stk.slot_size[pat->rowptr[_k]][_target_stk.uiter[_k]], _target_stk.iter[_k], 
        //        target_left_task, cur_left_task, stealed_start_idx_in_target);

        for(int i=0; i<=_k; i++){
            memcpy((_cur_stk.slot_storage[pat->rowptr[i]]), (_target_stk.slot_storage[pat->rowptr[i]]),  (pat->rowptr[i+1]-pat->rowptr[i])*GRAPH_DEGREE*UNROLL*sizeof(graph_node_t));
        }
        
        //Copy 
        for(int l=0; l<_k; l++){
            _cur_stk.iter[l]=_target_stk.iter[l];
            _cur_stk.uiter[l]=_target_stk.uiter[l];
            for(int s = pat->rowptr[l]; s<pat->rowptr[l+1]; s++){
               for(int u = 0; u<UNROLL; u++){
                   if(l<_k)  {
                       if(l>=0 && s==pat->rowptr[l] && u==_target_stk.uiter[l]) 
                          //printf("l:%d, _target_stk.slot_size:%d, iter:%d\n",  l, _target_stk.slot_size[s][u], _target_stk.iter[l]);
                         _cur_stk.slot_size[s][u] = 0;
                         //_cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
                   }
                    else _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
               }
            }
        }
        
        //copy
        _cur_stk.uiter[_k] = _target_stk.uiter[_k];
        for(int s =  pat->rowptr[_k]; s<pat->rowptr[_k+1]; s++){
            for(int u = 0; u<UNROLL; u++){
                if(u<=_target_stk.uiter[_k])  _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
                else                          _cur_stk.slot_size[s][u] = 0;
            }
        }

        //copy
        for(int l=_k+1; l<PAT_SIZE-1; l++){
            _cur_stk.iter[l] = 0;
            _cur_stk.uiter[l] = 0;
            for(int s = pat->rowptr[l]; s<pat->rowptr[l+1]; s++){
                for(int u = 0; u<UNROLL; u++){
                   _cur_stk.slot_size[s][u] = 0;
                }
            }
        }
        _target_stk.slot_size[pat->rowptr[_k]][_target_stk.uiter[_k]] = stealed_start_idx_in_target;
        _cur_stk.iter[_k] = stealed_start_idx_in_target; 
        _cur_stk.level = _k+1;
        _cur_stk.start_level = _target_stk.start_level;
        return true;   
}

   __device__
    bool trans_skt(CallStack* _all_stk, CallStack* _cur_stk, Pattern* pat, int* _mutex){
        //int local_mutex = 0;
        int max_left_task = 0;
        int stk_idx = -1;
        int at_level = -1;
        //int task_level = -1;
        
        for(int level = 1; level<4; level++){
            for(int i=0; i<NWARPS_PER_BLOCK; i++){
                if(i==threadIdx.x / WARP_SIZE) continue;
                //printf("slot_size 0:%d\n", _all_stk[i].slot_size[0][1]);
               // continue;
                if(level==0){
                    int left_task = JOB_CHUNK_SIZE - _all_stk[i].iter[0] - UNROLL;
                    if(left_task>0){
                        //printf("Warp:%d, left_task:%d\n", i, left_task);
                    }
                }
                else if(level>0 && _all_stk[i].level > level){
                    int left_task = _all_stk[i].slot_size[pat->rowptr[level]][_all_stk[i].uiter[level]] - (_all_stk[i].iter[level]+UNROLL);
                    if(left_task>max_left_task){
                      max_left_task = left_task;
                      stk_idx = i;
                      at_level = level;
                    }
                }
                else{
                    continue;
                }
            }
            if(stk_idx!=-1) break;
        }
        
        
        if(stk_idx!=-1){
            bool res = false;
            lock(&_mutex[threadIdx.x / WARP_SIZE]);
            lock(&_mutex[stk_idx]);
            //__threadfence_block();

           
            if(at_level==0){
               //res = trans_layer0(_all_stk[stk_idx], *_cur_stk);
            }
            else{
                //if(stk_idx==0)
                //printf("In warp:%d, stk_idx:%d, max_left_task:%d, _all_stk[stk_idx].level:%d, at_level:%d\n", threadIdx.x / WARP_SIZE, stk_idx, max_left_task, _all_stk[stk_idx].level, at_level);
                //if(threadIdx.x / WARP_SIZE ==4)
                //printf("In warp:%d, stk_idx:%d, _all_stk.uiter[1]:%d, _all_stk[stk_idx].level:%d, at_level:%d\n", threadIdx.x / WARP_SIZE, stk_idx, _all_stk[stk_idx].uiter[1], _all_stk[stk_idx].level, at_level);

              res = trans_layerk_in_unroll_slot(_all_stk[stk_idx], *_cur_stk, pat, at_level); 
            }
            
            unlock(&_mutex[threadIdx.x / WARP_SIZE]);
            unlock(&_mutex[stk_idx]);
            return res;
            //return false;
        }
        return false;
    }
}