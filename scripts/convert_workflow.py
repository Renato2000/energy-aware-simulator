import json
import sys

in_json = sys.argv[1]
out_json = sys.argv[2]

machines = {}

workflow = {}

with open(in_json) as f:
    json_data = f.read()

    data = json.loads(json_data)

    workflow['name'] = data['name']
    workflow['workflow'] = {}
    workflow['workflow']['jobs'] = []
    workflow['workflow']['machines'] = []
    workflow['workflow']['makespan'] = data['workflow']['makespanInSeconds']

    # Read machines info
    for machine in data['workflow']['machines']:
        machine_name = machine['nodeName']
        machine_cpu_count = machine['cpu']['count']
        machine_cpu_speed = machine['cpu']['speed']
        machines[machine_name] = {}
        machines[machine_name]['cpu_count'] = machine_cpu_count
        machines[machine_name]['cpu_speed'] = machine_cpu_speed

    # Read tasks info
    for task in data['workflow']['tasks']:
        task_name = task['name']
        task_type = task['type']
        task_parents = task['parents']
        task_files = task['files']
        task_machine = task['machine']
        task_num_cores = 1
        if 'cores' in task:
            task_num_cores = task['cores']
        task_runtime = (task['runtimeInSeconds'] * task['avgCPU']) / (100 * task_num_cores)
        task_runtime = task_runtime * machines[task['machine']]['cpu_speed']
        workflow['workflow']['jobs'].append({
            'name': task_name,
            'type': task_type,
            'parents': task_parents,
            'files': [],
            'machine': task_machine,
            'avgCPU': 0,
            'memory': 0,
            'runtime': task_runtime
        })

    result = json.dumps(workflow)
    with open(out_json, 'w') as f:
        f.write(result)

    
