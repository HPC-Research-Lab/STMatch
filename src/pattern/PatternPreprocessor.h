#ifndef PATTERN_PREPROCESSOR_H_
#define PATTERN_PREPROCESSOR_H_

#include "compiler/TrieCompiler.h"
#include "pattern/StlPattern.h"
#include "pattern/CPattern.h"

namespace libra{

struct PatternDataManager{
    PatternDataManager(){}
    void read_file(const char* _filename){
        stl_pattern_.readfile(_filename);
        trie_compiler_.compile(stl_pattern_); 
        trie_compiler_.print();
    }

    void set_pattern_info(CPattern* _pattern){
        trie_compiler_.write_stk_info(*_pattern);
    }
    size_t total_size(){
        return sizeof(CPattern);
    }

    StlPattern stl_pattern_;
    TrieCompiler trie_compiler_;
};


class PatternPreprocessor{
public:
    PatternPreprocessor(){}
    ~PatternPreprocessor(){_free();}

    void readfile(const char* _filename){
        pm_.read_file(_filename);
    }

    Pattern* to_gpu(){
        return (Pattern*)gpu_pattern_data_;
    }

    CPattern* cpu_data(){
        //if(!cpu_pattern_data_){
        //    cpu_pattern_data_ = malloc(pm_.total_size());
        //}
        pm_.set_pattern_info(&pat);
        return &pat;
    }

    CUdeviceptr gpu_data(){
        cpu_data();
        if(!gpu_pattern_data_){
            checkCudaErrors( cuMemAlloc(&gpu_pattern_data_, pm_.total_size()) );
            checkCudaErrors (cuMemcpyHtoD(gpu_pattern_data_, &pat, pm_.total_size()) );
        }
        return gpu_pattern_data_;
    }

    void _free(){
        if(cpu_pattern_data_) free(cpu_pattern_data_);
        if(gpu_pattern_data_) cuMemFree(gpu_pattern_data_);
    }

private:

    CPattern pat;
    PatternDataManager pm_;
    void* cpu_pattern_data_ = nullptr;
    CUdeviceptr gpu_pattern_data_ = 0;
};

}

#endif