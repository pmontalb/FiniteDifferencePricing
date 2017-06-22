import os
from subprocess import Popen, PIPE
import pandas as pd

ROOT_DIR = os.getcwd()


def run_profiler(n_iterations, n_divs, smoothing=True, acceleration=True, method="SingleThreaded"):
    cmd = "./FiniteDifferencePricing -profile"
    cmd += " -iter " + str(n_iterations)
    cmd += " -divs " + str(n_divs)

    if smoothing:
        cmd += " -smooth"
    if acceleration:
        cmd += " -acc"

    if method == "SingleThreaded":
        cmd += " -method single"
    elif method == "MultiThreaded":
        cmd += " -method multi"
    else:
        raise NotImplementedError()

    p = Popen(cmd, shell=True, stdout=PIPE)
    out, _ = p.communicate()

    lines = out.decode().split("\n")

    avg_time_line = lines[3].split(":")[-1].strip()
    avg_opt_sec_line = lines[5].split(":")[-1].strip()

    return avg_time_line, avg_opt_sec_line


def run_callgrind(n_divs, smoothing=True, acceleration=True):
    cmd = "valgrind --tool=callgrind --instr-atstart=no ./FiniteDifferencePricing -profile -iter 100"
    cmd += " -divs " + str(n_divs)

    if smoothing:
        cmd += " -smooth"
    if acceleration:
        cmd += " -acc"

    cmd += " -method single"

    p = Popen(cmd, shell=True, stderr=PIPE, stdout=PIPE)
    _, out = p.communicate()

    lines = out.decode().split("\n")
    os.popen('rm -f callgrind.out*')

    return lines[8].split(":")[-1].strip()


def run_version(version, n_iterations, n_divs, smoothing=True, acceleration=True, method="SingleThreaded", profile=False):
    wd = ROOT_DIR + "/" + version + "/"
    os.chdir(wd)

    instruction_reads = run_callgrind(n_divs, smoothing, acceleration)

    ret = [instruction_reads]
    if profile:
        avg_time_line, avg_opt_sec_line = run_profiler(n_iterations, n_divs, smoothing, acceleration, method)
        ret.append(avg_time_line)
        ret.append(avg_opt_sec_line)

    return ret


def run_all(n_iterations, n_divs, smoothing=True, acceleration=True, method="SingleThreaded", profile=False):
    versions = ["Release", "Release GCC-7.1",   "Optimized GCC-7.1",
                           "Release Clang-4.0", "Optimized Clang-4.0"]

    columns = ["Instructions"]
    if profile:
        columns.append("Average Time Per Option")
        columns.append("Option Per Second")

    columns = ["Version"] + columns
    df = pd.DataFrame(columns=columns)

    for version in versions:
        df.loc[len(df)] = [version] + run_version(version, n_iterations, n_divs, smoothing, acceleration, method, profile)

    df = df.set_index("Version")
    print(df)

if __name__ == "__main__":
    run_all(5000, 0, smoothing=False, acceleration=True, method="SingleThreaded", profile=False)


