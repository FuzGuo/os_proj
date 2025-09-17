import re

log_file = "process.log"

pattern = re.compile(
    r"pid=(\d+),.*state=E, time=(\d+), run_time=(\d+), wait_time=(\d+), turnaround_time=(\d+)"
)

result = []

with open(log_file, "r") as f:
    for line in f:
        match = pattern.search(line)
        if match:
            pid = int(match.group(1))
            run_time = int(match.group(3))
            wait_time = int(match.group(4))
            turnaround_time = int(match.group(5))
            result.append((pid, run_time, wait_time, turnaround_time))

print("pid\trun_time\twait_time\tturnaround_time")
for pid, run_time, wait_time, turnaround_time in result:
    print(f"{pid}\t{run_time}\t\t{wait_time}\t\t{turnaround_time}")