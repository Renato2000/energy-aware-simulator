#include "EnergyMeter.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(energy_meter, "Log category for EnergyMeter");

/**
 * @brief Constructor
 *
 * @param wms: the WMS that uses this power meter
 * @param hostnames: the list of metered hosts, as hostnames
 * @param measurement_period: the measurement period
 */
EnergyMeter::EnergyMeter(wrench::WMS *wms,
                       const std::vector<std::string> &hostnames,
                       std::shared_ptr<ClusterInfo> cluster_info,
                       double measurement_period) :
        Service(wms->hostname, "energy_meter", "energy_meter"),
        wms(wms),
        cluster_info(cluster_info),
        measurement_period(measurement_period) {
    // sanity checks
    if (hostnames.empty()) {
        throw std::invalid_argument("EnergyMeter::EnergyMeter(): no host to meter!");
    }
    if (measurement_period < 1) {
        throw std::invalid_argument("EnergyMeter::EnergyMeter(): measurement period must be at least 1 second");
    }
    for (auto const &h : hostnames) {
        if (not wrench::S4U_Simulation::hostExists(h)) {
            throw std::invalid_argument("EnergyMeter::EnergyMeter(): unknown host " + h);
        }
    }

    this->time_to_next_measurement = 0.0;
}

/**
 * @brief Compare the start time between two workflow tasks
 *
 * @param lhs: pointer to a workflow task
 * @param rhs: pointer to a workflow task
 *
 * @return whether the start time of the left-hand-side workflow tasks is earlier
 */
bool EnergyMeter::TaskStartTimeComparator::operator()(wrench::WorkflowTask *&lhs, wrench::WorkflowTask *&rhs) {
    return lhs->getStartDate() < rhs->getStartDate();
}

/**
 * @brief Main method of the daemon that implements the EnergyMeter
 * @return 0 on success
 */
int EnergyMeter::main() {
    wrench::TerminalOutput::setThisProcessLoggingColor(wrench::TerminalOutput::COLOR_YELLOW);

    WRENCH_INFO("New Energy Meter starting (%s)", this->mailbox_name.c_str());

    /** Main loop **/
    while (true) {
        wrench::S4U_Simulation::computeZeroFlop();

        double current_time = wrench::Simulation::getCurrentSimulatedDate();

        if (current_time >= this->time_to_next_measurement) {
            // separate tasks per host
            std::map<std::string, std::set<wrench::WorkflowTask *>> tasks_per_host;

            for (auto task : this->wms->getWorkflow()->getTasks()) {

                // only process running tasks
                if (task->getStartDate() != -1 && task->getEndDate() == -1) {
                    if (tasks_per_host.find(task->getExecutionHost()) == tasks_per_host.end()) {
                        std::set<wrench::WorkflowTask *> tasks_set;
                        tasks_set.insert(task);
                        tasks_per_host.insert(std::make_pair(task->getExecutionHost(), tasks_set));
                    } else {
                        tasks_per_host[task->getExecutionHost()].insert(task);
                    }
                }
            }

  //          std::cout << "New round" << std::endl;
            // compute power consumption
            for (auto &key_value : tasks_per_host) {
                this->computePowerMeasurements(key_value.first, key_value.second);
            }

            // update time to next measurement
            this->time_to_next_measurement = current_time + this->measurement_period;
        }

        // stop meter
        if (!this->processNextMessage(this->measurement_period)) {
            break;
        }
    }

    WRENCH_INFO("Energy Meter Manager terminating");
    return 0;
}

/**
 * @brief Obtain the current power consumption of a host and will add SimulationTimestampEnergyConsumption to
 *          simulation output if can_record is set to true
 *
 * @param hostname: the host name
 * @param tasks: list of WorkflowTask running on the host
 * @param record_as_time_stamp: bool signaling whether or not to record a SimulationTimestampEnergyConsumption object
 *
 * @throw std::invalid_argument
 */
void EnergyMeter::computePowerMeasurements(const std::string &hostname,
                                          std::set<wrench::WorkflowTask *> &tasks) {

    double consumption = 0.5; //wrench::S4U_Simulation::getMinPowerConsumption(hostname);

    int used_cores = cluster_info->get_host_cores(hostname) - cluster_info->get_available_cores(hostname);

    //std::cout << "Total cores: " << cluster_info->get_host_cores(hostname) << std::endl;
    //std::cout << "Available cores: " << cluster_info->get_available_cores(hostname) << std::endl;
    //std::cout << "Used cores: " << used_cores << std::endl;

    if (used_cores == 1) consumption += 18;
    else if (used_cores == 2) consumption += 28;
    else if (used_cores == 3) consumption += 38;
    else if (used_cores == 4) consumption += 48;
    else if (used_cores == 5) consumption += 55.8;
    else if (used_cores == 6) consumption += 63;
    else if (used_cores == 7) consumption += 63.8;
    else if (used_cores == 8) consumption += 65;
    else if (used_cores == 9) consumption += 67.3;
    else if (used_cores == 10) consumption += 69;
    else if (used_cores == 11) consumption += 70;
    else if (used_cores == 12) consumption += 71;

    this->cluster_info->add_energy(consumption);
    // this->simulation->getOutput().addTimestampEnergyConsumption("energy_meter__" + hostname, consumption);

    //std::cout << "Host: " << hostname << " is using " << used_cores << " cores and is consuming " << consumption << " watts of power" << std::endl;
//    std::cout << hostname << " " << consumption << std::endl;
}

/**
 * @brief Process the next message
 * @return true if the daemon should continue, false otherwise
 *
 * @throw std::runtime_error
 */
bool EnergyMeter::processNextMessage(double timeout) {
    std::shared_ptr<wrench::SimulationMessage> message = nullptr;

    try {
        message = wrench::S4U_Mailbox::getMessage(this->mailbox_name, timeout);
    } catch (std::shared_ptr<wrench::NetworkError> &cause) {
        return true;
    }

    if (message == nullptr) { WRENCH_INFO("Got a NULL message... Likely this means we're all done. Aborting!");
        return false;
    }

    WRENCH_INFO("Power Meter got a %s message", message->getName().c_str());

    if (auto msg = dynamic_cast<wrench::ServiceStopDaemonMessage *>(message.get())) {
        // There shouldn't be any need to clean any state up
        return false;

    } else {
        throw std::runtime_error(
                "EnergyMeter::waitForNextMessage(): Unexpected [" + message->getName() + "] message");
    }
}

/**
 * @brief Stop the power meter
 *
 * @throw WorkflowExecutionException
 * @throw std::runtime_error
 */
void EnergyMeter::stop() {
    try {
        wrench::S4U_Mailbox::putMessage(this->mailbox_name, new wrench::ServiceStopDaemonMessage("", 0.0));
    } catch (std::shared_ptr<wrench::NetworkError> &cause) {
        throw wrench::WorkflowExecutionException(cause);
    }
}
