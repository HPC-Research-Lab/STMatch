#pragma once

namespace libra {



__device__ void lock(int* mutex) {
    while (atomicCAS((int*)mutex, 0, 1) != 0){
    }
  }

  __device__ void unlock(int* mutex) {
    atomicExch((int*)mutex, 0);
  }

/*
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
*/

/*
__device__
bool trans_layer0(CallStack& _target_stk, CallStack& _cur_stk, Pattern* pat){
    int left_task = JOB_CHUNK_SIZE - _target_stk.iter[0] - _target_stk.uiter[1]-1;
    if(left_task<=0) return false;
    memcpy((_cur_stk.slot_storage[pat->rowptr[0]]), (_target_stk.slot_storage[pat->rowptr[0]]),  GRAPH_DEGREE*UNROLL*sizeof(graph_node_t));
    _cur_stk.iter[0] = _target_stk.iter[0]+_target_stk.uiter[1]+1;
    _cur_stk.uiter[0] = _target_stk.uiter[0];
}
*/


__device__
bool trans_layer(int _stk_idx, CallStack& _target_stk, CallStack& _cur_stk, Pattern* _pat, int _k){
    if(_target_stk.level<=_k) return false;

    int num_left_task = _target_stk.slot_size[_pat->rowptr[_k]][_target_stk.uiter[_k]] - 
                                                   (_target_stk.iter[_k]+_target_stk.uiter[_k+1]+1);
    if(num_left_task<=0) return false;
    
    int target_left_task = num_left_task/2;
    
    int stealed_start_idx_in_target = _target_stk.iter[_k] + _target_stk.uiter[_k+1] + 1 + num_left_task/2;
    
    //graph_node_t (*slot_storage)[UNROLL][GRAPH_DEGREE] = _cur_stk.slot_storage;
    //_cur_stk = _target_stk;
    //_cur_stk.slot_storage = slot_storage;
   //printf("_cur:%d, _stk_idx:%d, _k:%d,  _target_stk.level:%d\n", 
   //        threadIdx.x / WARP_SIZE, 
   //        _stk_idx,
   //        _k,  
   //        _target_stk.level);
    
   // printf("slot_size:%d, uiter:%d,  iter:%d, uiter_k+1:%d, slot_size_k+1:%d, stealed_start_idx_in_target:%d\n", 
    //      _target_stk.slot_size[_pat->rowptr[_k]][_target_stk.uiter[_k]], 
    //       _target_stk.uiter[_k], 
    //       _target_stk.iter[_k], 
    //       _target_stk.slot_size[_pat->rowptr[_k+1]][_target_stk.uiter[_k+1]], 
    //       _target_stk.uiter[_k+1], stealed_start_idx_in_target
   // );
    for(int i=0; i<=_k; i++){
         memcpy((_cur_stk.slot_storage[_pat->rowptr[i]]), (_target_stk.slot_storage[_pat->rowptr[i]]),  (_pat->rowptr[i+1]-_pat->rowptr[i])*GRAPH_DEGREE*UNROLL*sizeof(graph_node_t));
    }
   
    //Copy 

    for(int l=0; l<_k; l++){
        _cur_stk.iter[l]=_target_stk.iter[l];
        _cur_stk.uiter[l]=_target_stk.uiter[l];
        for(int s = _pat->rowptr[l]; s<_pat->rowptr[l+1]; s++){
            if(s>_pat->rowptr[l]){
                for(int u = 0; u<UNROLL; u++){
                    _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
                }
            }
            else{
                for(int u = 0; u<UNROLL; u++){
                    if(u<_target_stk.uiter[l])  {
                        _cur_stk.slot_size[s][u] = 0; //_target_stk.slot_size[s][u];
                    }
                    else if(u==_target_stk.uiter[l]){
                        _cur_stk.slot_size[s][u] = _target_stk.iter[l]; //_target_stk.slot_size[s][u];
                    }
                    else {
                         _cur_stk.slot_size[s][u] = 0;
                    }
                }
            }
        }
    }

    //copy
    for(int i=stealed_start_idx_in_target-_target_stk.iter[_k]; i<UNROLL; i++){
        _target_stk.slot_size[_pat->rowptr[_k+1]][i]=0;
    }


 
    for(int s =  _pat->rowptr[_k]; s<_pat->rowptr[_k+1]; s++){
        if(s==_pat->rowptr[_k]){
            for(int u = 0; u<UNROLL; u++){
                if(u==_target_stk.uiter[_k])  _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
                else                          _cur_stk.slot_size[s][u] = 0;
            }
        }
        else{
            for(int u = 0; u<UNROLL; u++){
                _cur_stk.slot_size[s][u] = _target_stk.slot_size[s][u];
             }
        }
    }

    _cur_stk.uiter[_k] = _target_stk.uiter[_k];
    _cur_stk.iter[_k] = stealed_start_idx_in_target; 
    _target_stk.slot_size[_pat->rowptr[_k]][_target_stk.uiter[_k]] = stealed_start_idx_in_target;
    //copy
    for(int l=_k+1; l<PAT_SIZE-1; l++){
        _cur_stk.iter[l] = 0;
        _cur_stk.uiter[l] = 0;
        for(int s = _pat->rowptr[l]; s<_pat->rowptr[l+1]; s++){
            for(int u = 0; u<UNROLL; u++){
                _cur_stk.slot_size[s][u] = 0;
            }
        }
    }
    _cur_stk.iter[PAT_SIZE-1] = 0;
    _cur_stk.uiter[PAT_SIZE-1] = 0;
    for(int u = 0; u<UNROLL; u++){
        _cur_stk.slot_size[_pat->rowptr[PAT_SIZE-1]][u] = 0;
    }
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
            //if(level==2) continue;
            for(int i=0; i<NWARPS_PER_BLOCK; i++){
                
                if(i==threadIdx.x / WARP_SIZE) continue;
                lock(&_mutex[i]);
                //printf("slot_size 0:%d\n", _all_stk[i].slot_size[0][1]);
               // continue;
               
                if(_all_stk[i].level > level){
                    int left_task = _all_stk[i].slot_size[pat->rowptr[level]][_all_stk[i].uiter[level]] - 
                                                   (_all_stk[i].iter[level]+_all_stk[i].uiter[level+1]+1);
                    if(left_task>max_left_task){
                      max_left_task = left_task;
                      stk_idx = i;
                      at_level = level;
                    }
                }
                else{
                    unlock(&_mutex[i]);
                    continue;
                }
                unlock(&_mutex[i]);
            }
            if(stk_idx!=-1) break;
        }
        
        
        if(stk_idx!=-1){
            //printf("stk_idx:%d\n", stk_idx);
            int res = -1;
            lock(&_mutex[threadIdx.x / WARP_SIZE]);
            lock(&_mutex[stk_idx]);
            //__threadfence_block();
            res = trans_layer(stk_idx, _all_stk[stk_idx], *_cur_stk, pat, at_level);
            
            unlock(&_mutex[threadIdx.x / WARP_SIZE]);
            unlock(&_mutex[stk_idx]);
            return res;
        }
        return false;
    }
}