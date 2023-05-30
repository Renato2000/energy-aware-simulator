#include "Hosts.h"

#include <iostream>

#include "pugixml.hpp"

Hosts::Hosts(const std::string platform_file) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(platform_file.c_str());

    if (result)
    {
        std::cout << "XML file parsed successfully." << std::endl;

        // get the root node
        pugi::xml_node platform = doc.child("platform");

        // loop over all the host nodes
        for (pugi::xml_node host = platform.child("zone").child("host"); host; host = host.next_sibling("host"))
        {
            // get the id, core, and host attributes
            std::string id = host.attribute("id").value();
            std::string core = host.attribute("core").value();

            if (id != "master" && id != "data_server") {
                this->hosts[id] = std::stoi(core);
                this->available_cores[id] = std::stoi(core);
            }
        }
    }
    else
    {
        std::cout << "XML file parsing failed. Error description: " << result.description() << std::endl;
    }
}

const std::vector<std::string> Hosts::get_hosts() {
    std::vector<std::string> ret;

    for (const auto &it : this->hosts) {
        ret.push_back(it.first);
    }

    return ret;
}

int Hosts::get_host_cores(std::string host) {
    return this->hosts[host];
}

void Hosts::use_core(std::string host) {
    this->available_cores[host] -= 1;
}

void Hosts::free_core(std::string host) {
    this->available_cores[host] += 1;
}

int Hosts::get_available_cores(std::string host) {
    return this->available_cores[host];
}
