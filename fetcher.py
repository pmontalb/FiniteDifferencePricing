import os
from subprocess import Popen, PIPE
from numpy import nan
import pandas as pd
from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt
plt.style.use("classic")

ROOT_DIR = os.getcwd()
VERSIONS = ["Release",
            "Release GCC-7.1",   "Optimized GCC-7.1",   "Graphite GCC-7.1",
            "Release Clang-4.0", "Optimized Clang-4.0", "Polly Clang-4.0",
            "Release Intel",     "Optimized Intel"]


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


def run_callgrind(n_divs, smoothing=True, acceleration=True, version=None):
    # clean out previous runs
    os.popen("rm -f callgrind.out*")
    os.popen("rm -f *.dot")

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

    # Generate call graph
    if version is not None:
        generate_png_cmd = "$HOME/anaconda3/bin/gprof2dot --format=callgrind --output=out.dot "

        callgrind_file = [f for f in listdir(os.getcwd()) if isfile(join(os.getcwd(), f)) and f.endswith(".1")]
        if len(callgrind_file) != 1:
            raise ValueError("Callgrind generated " + str(len(callgrind_file)) + " files!")
        generate_png_cmd += callgrind_file[0]

        Popen(generate_png_cmd, shell=True).communicate()
        version = str(version).replace(" ", "\\")
        os.popen("dot -Tpng out.dot -o " + ROOT_DIR + "/Results/" + version + ".png")

    # clean out
    os.popen("rm -f callgrind.out*")
    os.popen("rm -f *.dot")

    return lines[8].split(":")[-1].strip()


def run_version(version, n_iterations, n_divs, smoothing=True, acceleration=True, method="SingleThreaded", profile=False):
    wd = ROOT_DIR + "/" + version + "/"
    os.chdir(wd)

    if method == "SingleThreaded":
        instruction_reads = run_callgrind(n_divs, smoothing, acceleration, version)
    else:
        instruction_reads = nan

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


def recompile_all():
    for version in VERSIONS:
        wd = ROOT_DIR + "/" + version + "/"
        os.chdir(wd)
        p = Popen("make clean && make all -j8", shell=True)
        p.communicate()


def summary(n_iterations, method="SingleThreaded", profile=False, recompile=False):

    if recompile:
        recompile_all()

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

    os.chdir(ROOT_DIR + "/Results")
    df.to_csv("summary" + method + ".csv")
    print(df)


def plot_version(group_df, version, y):
    version_df = group_df.get_group(version)

    version_df_no_improvements = version_df[(version_df["Smoothing"] == False) & (version_df["Acceleration"] == False)]
    version_df_smoothing = version_df[(version_df["Smoothing"] == True) & (version_df["Acceleration"] == False)]
    version_df_acceleration = version_df[(version_df["Smoothing"] == False) & (version_df["Acceleration"] == True)]
    version_df_all = version_df[(version_df["Smoothing"] == True) & (version_df["Acceleration"] == True)]

    try:
        is_subplot = len(y) > 1
    except AttributeError:
        is_subplot = False

    ax = version_df_no_improvements.plot(y=y, x="Dividends", color='b', label="No Improvements", subplots=is_subplot, layout=(1, 2))
    version_df_smoothing.plot(y=y, x="Dividends", ax=ax, color='g', label="Smoothing", subplots=is_subplot)
    version_df_acceleration.plot(y=y, x="Dividends", ax=ax, color='k', label="Acceleration", subplots=is_subplot)
    version_df_all.plot(y=y, x="Dividends", ax=ax, color='r', label="All", subplots=is_subplot)

    fig = plt.figure(1)
    fig.suptitle(version)

    plt.show()


def plot_version_comparisons(n_divs, smoothing, acceleration, method="SingleThreaded"):
    os.chdir(ROOT_DIR + "/Results/")
    df = pd.DataFrame.from_csv("summary" + method + ".csv")
    group_df = df.groupby("Version")

    points = pd.DataFrame()
    columns = ["Instructions", "Option Per Second"]
    for version in VERSIONS:
        version_df = group_df.get_group(version)

        version_df = version_df[(version_df["Smoothing"] == smoothing) &
                                (version_df["Acceleration"] == acceleration) &
                                (version_df["Dividends"] == n_divs)]

        points = points.append(version_df[columns])

    print(points)
    ax = points.plot(y=columns, x=points.index,
                     kind='bar', width=.25, sort_columns=True,
                     subplots=True, layout=(1, 2), legend=False)
    #ax = points.plot(y= kind='bar', width=.5, legend=False)
    for _ax in ax[0]:
        _ax.set_xticklabels(_ax.xaxis.get_majorticklabels(), rotation=45)
    fig = plt.figure(1)
    fig.suptitle("{:d} Dividends".format(n_divs))
    plt.show()



def plot(contain_profile_info=True, method="SingleThreaded"):
    os.chdir(ROOT_DIR + "/Results/")
    df = pd.DataFrame.from_csv("summary" + method + ".csv")
    group_df = df.groupby("Version")

    version = "Release GCC-7.1"
    if contain_profile_info:
        y = ["Instructions", "Option Per Second"]
    else:
        y = "Instructions"
    plot_version(group_df, version, y)


if __name__ == "__main__":
    os.chdir(ROOT_DIR + "/Results/")
    df = pd.DataFrame.from_csv("summarySingleThreaded.csv")
    group_df = df.groupby("Version")
    intel_df = group_df.get_group("Release Intel")
    del intel_df["Average Time Per Option (ms)"]
    del intel_df["Option Per Second"]
    del intel_df["Threads"]
    print(intel_df)
    #plot_version_comparisons(n_divs=0, smoothing=True, acceleration=True, method="MultiThreaded")
    #plot(contain_profile_info=True)
    #summary(n_iterations=10000, method="SingleThreaded", profile=True, recompile=False)



