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
            turnaround_time = int(match.group(5))
            if 6 <= pid <= 11:
                result.append((pid, run_time, turnaround_time))

print("pid\trun_time\tturnaround_time")
total_turnaround = 0
for pid, run_time, turnaround_time in result:
    print(f"{pid}\t{run_time}\t\t{turnaround_time}")
    total_turnaround += turnaround_time

print(f"\n总周转时间：{total_turnaround} 时钟周期")