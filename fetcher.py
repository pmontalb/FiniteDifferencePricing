import os
from subprocess import Popen, PIPE
import pandas as pd
from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
plt.style.use("classic")

ROOT_DIR = os.getcwd()
VERSIONS = ["Release", "Release GCC-7.1", "Optimized GCC-7.1", "Graphite GCC-7.1",
            "Release Clang-4.0", "Optimized Clang-4.0", "Polly Clang-4.0"]

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

    generate_png_cmd = "gprof2dot --format=callgrind --output=out.dot "

    callgrind_file = [f for f in listdir(ROOT_DIR) if isfile(join(ROOT_DIR, f)) and f.endswith(".1")]
    if len(callgrind_file) != 1:
        raise ValueError("Callgrind generated " + str(len(callgrind_file)) + " files!")
    generate_png_cmd += callgrind_file[0]

    os.popen(generate_png_cmd)
    os.popen("dot -Tpng out.dot -o callgrind.png")
    os.popen("rm -f callgrind.out*")

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

    columns = ["Instructions"]
    if profile:
        columns.append("Average Time Per Option (ms)")
        columns.append("Option Per Second")

    columns = ["Version"] + columns
    df = pd.DataFrame(columns=columns)

    for version in VERSIONS:
        df.loc[len(df)] = [version] + run_version(version, n_iterations, n_divs, smoothing, acceleration, method, profile)

    df["Dividends"] = n_divs
    df["Smoothing"] = smoothing
    df["Acceleration"] = acceleration
    df["Threads"] = method
    df = df.set_index("Version")

    return df


def summary(n_iterations, method="SingleThreaded", profile=False):

    smoothing = False
    acceleration = False
    def worker():
        _df = run_all(n_iterations, 0, smoothing, acceleration, method=method, profile=profile)
        for n_divs in range(1, 9):
            _df = _df.append(run_all(n_iterations, n_divs, smoothing, acceleration, method=method, profile=profile))

        return _df

    df = worker()

    smoothing = True
    acceleration = False
    df = df.append(worker())

    smoothing = False
    acceleration = True
    df = df.append(worker())

    smoothing = True
    acceleration = True
    df = df.append(worker())

    os.chdir(ROOT_DIR)
    df.to_csv("summary.csv")
    print(df)


def plot_version(group_df, version, y):
    version_df = group_df.get_group(version)

    version_df_no_improvements = version_df[(version_df["Smoothing"] == False) & (version_df["Acceleration"] == False)]
    version_df_smoothing = version_df[(version_df["Smoothing"] == True) & (version_df["Acceleration"] == False)]
    version_df_acceleration = version_df[(version_df["Smoothing"] == False) & (version_df["Acceleration"] == True)]
    version_df_all = version_df[(version_df["Smoothing"] == True) & (version_df["Acceleration"] == True)]

    try:
        is_subplot = len(y) == 1
    except AttributeError:
        is_subplot = False

    ax = version_df_no_improvements.plot(y=y, x="Dividends", color='b', label="No Improvements", subplots=is_subplot)
    version_df_smoothing.plot(y=y, x="Dividends", ax=ax, color='g', label="Smoothing", subplots=is_subplot)
    version_df_acceleration.plot(y=y, x="Dividends", ax=ax, color='k', label="Acceleration", subplots=is_subplot)
    version_df_all.plot(y=y, x="Dividends", ax=ax, color='r', label="All", subplots=is_subplot)

    plt.title(version)

    plt.show()


def plot(contain_profile_info=False):
    df = pd.DataFrame.from_csv("summary.csv")
    group_df = df.groupby("Version")

    version = "Release GCC-7.1"
    if contain_profile_info:
        y = ["Instructions", "Option Per Second"]
    else:
        y = "Instructions"
    plot_version(group_df, version, y)


if __name__ == "__main__":
    summary(n_iterations=10000, method="SingleThreaded", profile=True)



