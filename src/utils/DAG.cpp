#include "DAG.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

DAG::DAG(std::string file_name) {
    std::ifstream input_file(file_name);

    std::string json_str((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    nlohmann::json j = nlohmann::json::parse(json_str);

    auto jobs = j["workflow"]["jobs"];

    for(const auto &job : jobs) {
        std::string name = job["name"];
        std::list<std::string> parents = job["parents"];
        
        std::cout << "Job name: " << name << std::endl;
        std::cout << "Parents: ";
        for (const auto& parent : parents) {
            std::cout << parent << " ";
        }
        std::cout << std::endl;
    }
}

void DAG::concludeTask(std::string task_name) {
    if(this->tasks.count(task_name) == 0) return;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    for(auto &t : task.get()->get_childs()) {
        if(this->tasks.count(task_name) != 0) {
            std::unique_ptr<Task> &child = this->tasks[t];
            child.get()->remove_dependency(task_name);
        }
    }
}
