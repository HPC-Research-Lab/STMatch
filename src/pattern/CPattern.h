#ifndef C_PATTERN_H_
#define C_PATTERN_H_

#include "config.h"

namespace libra{

struct CPattern{

    pattern_node_t nnodes = 0;
    label_t vertex_label[PAT_SIZE];
    pattern_node_t partial[PAT_SIZE][PAT_SIZE];
    set_op_t set_ops[PAT_SIZE][PAT_SIZE];
};

typedef CPattern Pattern;

}

#endif