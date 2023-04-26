#ifndef DAG_H
#define DAG_H

class DAG {
public:
    DAG(std::string file_name);
    void concludeTask(std::string task); 
private:
    std::map<std::string, Task> tasks;    
};



#endif