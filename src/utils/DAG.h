#ifndef DAG_H
#define DAG_H

#include "Task.h"

#include <string>
#include <map>

class DAG {
public:
    DAG(std::string file_name);
    void concludeTask(std::string task); 
private:
    std::map<std::string, Task> tasks;    
};



#endif