#include "DAG.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

DAG::DAG(std::string file_name) {
    std::ifstream input_file(file_name);

    std::string json_str((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    nlohmann::json j = nlohmann::json::parse(json_str);

    auto jobs = j["workflow"]["jobs"];

    for (const auto &job : jobs) {
        std::string name = job["name"];
        std::list<std::string> parents = job["parents"];
        float runtime = job["runtime"];
       
        std::unique_ptr<Task> task = std::make_unique<Task>(Task(name, parents, runtime));

        this->tasks.insert(std::make_pair(name, std::move(task)));

        for (auto &parent : parents) {
            this->tasks[parent]->add_child(name);
        }
 
        //std::cout << "Job name: " << name << std::endl;
        //std::cout << "Runtime " << runtime << std::endl;
        //std::cout << "Num Parents: " << parents.size() << std::endl;
    }
}

void DAG::run_task(std::string task_name) {
    if (this->tasks.count(task_name) == 0) return;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    
}

void DAG::complete_task(std::string task_name) {
    if (this->tasks.count(task_name) == 0) return;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    for (auto &t : task.get()->get_childs()) {
        if (this->tasks.count(task_name) != 0) {
            std::unique_ptr<Task> &child = this->tasks[t];
            child.get()->remove_dependency(task_name);
        }
    }
}

float DAG::get_expected_runtime(std::string task_name) {
    if (this->tasks.count(task_name) == 0) return 0.0;

    std::unique_ptr<Task> &task = this->tasks[task_name];
    return task.get()->get_expected_runtime();
}

bool DAG::is_priority(std::string task_name) {
    if (this->tasks.count(task_name) == 0) return false;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    for (auto &t : task.get()->get_childs()) {
        if (this->tasks.count(task_name) != 0) {
            std::unique_ptr<Task> &child = this->tasks[t];
            if (child->get_number_dependencies() == 1) return true;
        }
    }

    return false;
}

void DAG::set_start_time(std::string task_name, float start_time) {
    if (this->tasks.count(task_name) == 0) return;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    task.get()->set_start_time(start_time);
}

float DAG::get_start_time(std::string task_name) {
    if (this->tasks.count(task_name) == 0) return 0;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    return task.get()->get_start_time();
}

float DAG::get_blevel(std::string task_name) {
    if (this->tasks.count(task_name) == 0) return 0;

    if (this->tasks[task_name]->get_blevel() > 0) return this->tasks[task_name]->get_blevel();

    std::unique_ptr<Task> &task = this->tasks[task_name];

    float cost = task->get_expected_runtime();
    float max = 0;

    for (auto &child_name : task->get_childs()) {
        //std::cout << child_name <<  " is child of " << task_name << std::endl;
        float child_cost = this->get_blevel(child_name);
        if (child_cost > max) max = child_cost;
    }

    float blevel = cost + max;
    task->set_blevel(blevel);

//    std::cout << "blevel for task: " << task_name << " is: " << blevel << std::endl; 

    return blevel;
}
