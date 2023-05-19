import re
import sys

data_file = sys.argv[1]

time_under_100 = {}
time_idle = {}

execs = {}

task_executor_map = {}
host_exec_map = {}

start_task_line_pattern = re.compile(r'Start task (\w+) on vm (\w+) at (\d+(\.\d+)?([Ee][+-]?\d+)?)')
end_task_line_pattern = re.compile(r'Task (\w+) ended at (\d+(\.\d+)?([Ee][+-]?\d+)?)')
turn_on_host_line_pattern = re.compile(r'Start vm (\w+) on host (\w+) with (\d+) cores at (\d+(\.\d+)?([Ee][+-]?\d+)?)')
turn_off_host_line_pattern = re.compile(r'Turn off vm (\w+) at ([+-]?\d+(\.\d+)?([Ee][+-]?\d+)?)')

def parse_start_task_line(match):
    parsed_line = {}

    parsed_line['event_type'] = 'start_task'
    parsed_line['task'] = match.group(1)
    parsed_line['executor'] = match.group(2)
    parsed_line['time'] = float(match.group(3))

    task_executor_map[match.group(1)] = match.group(2)

    return parsed_line

def parse_end_task_line(match):
    parsed_line = {}

    parsed_line['event_type'] = 'end_task'
    parsed_line['task'] = match.group(1)
    parsed_line['executor'] = task_executor_map[match.group(1)]
    parsed_line['time'] = float(match.group(2))

    return parsed_line

def parse_turn_on_host_line(match):
    parsed_line = {}

    parsed_line['event_type'] = 'turn_on_host'
    parsed_line['executor'] = match.group(1)
    parsed_line['host'] = match.group(2)
    parsed_line['cores'] = int(match.group(3))
    parsed_line['time'] = float(match.group(4))

    host_exec_map[match.group(2)] = match.group(1)

    return parsed_line

def parse_turn_off_host_line(match):
    parsed_line = {}

    parsed_line['event_type'] = 'turn_off_host'
    parsed_line['executor'] = host_exec_map[match.group(1)]
    parsed_line['time'] = float(match.group(2))

    return parsed_line

def parse_line(line):
    for pattern, process_func in regex_patterns:
        match = re.search(pattern, line)
        if match:
            return process_func(match)
    
    return None

regex_patterns = [
    (start_task_line_pattern, lambda match: parse_start_task_line(match)),
    (end_task_line_pattern, lambda match: parse_end_task_line(match)),
    (turn_on_host_line_pattern, lambda match: parse_turn_on_host_line(match)),
    (turn_off_host_line_pattern, lambda match: parse_turn_off_host_line(match))
]

def main():
    with open(data_file) as f:
        lines = f.readlines()
        for line in lines:
            parsed_line = parse_line(line)

            if parsed_line == None:
                pass
            elif parsed_line['event_type'] == 'start_task':
                executor = parsed_line['executor']
                time_now = parsed_line['time']

                time_under_100[executor] += time_now - execs[executor]['idle_since']

                execs[executor]['idle_cores'] -= 1
                idle_since = execs[executor]['times'].pop()
                if execs[executor]['idle_cores'] == 0:
                    time_idle[executor] += time_now - idle_since

            elif parsed_line['event_type'] == 'end_task':
                executor = parsed_line['executor']
                time_now = parsed_line['time']

                if execs[executor]['idle_cores'] == 0:
                    execs[executor]['idle_since'] = time_now

                execs[executor]['idle_cores'] += 1
                execs[executor]['times'].append(time_now)

            elif parsed_line['event_type'] == 'turn_on_host':
                executor = parsed_line['executor']
                time_now = parsed_line['time']
                n_cores = parsed_line['cores']

                if executor not in time_under_100:
                    time_under_100[executor] = 0

                if executor not in time_idle:
                    time_idle[executor] = 0

                if executor not in execs:
                    execs[executor] = {}
                    execs[executor]['times'] = []
                    execs[executor]['idle_cores'] = n_cores

                execs[executor]['idle_since'] = time_now

                for i in range(0, n_cores):
                    execs[executor]['times'].append(time_now)

            elif parsed_line['event_type'] == 'turn_off_host':
                executor = parsed_line['executor']
                time_now = parsed_line['time']

                while execs[executor]['times']:
                    start_idle_time = execs[executor]['times'].pop()
                    time_idle[executor] += time_now - start_idle_time;
                
                time_under_100[executor] += time_now - execs[executor]['idle_since']

        for executor in execs:
            print('Executor ' + executor + ' spent ' + str(round(time_under_100[executor], 3)) + 's under 100%')
            print('Executor ' + executor + ' cores spent ' + str(round(time_idle[executor], 3)) + 's idle')
        
if __name__ == "__main__":
    main()