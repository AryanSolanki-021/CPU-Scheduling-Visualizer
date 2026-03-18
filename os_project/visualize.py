import matplotlib.pyplot as plt

FILE = "output.txt"

# ------------------ PARSE OUTPUT ------------------ #
def parse_output(file_path):
    data = {}

    with open(file_path, "r") as f:
        lines = [line.strip() for line in f if line.strip()]

    i = 0
    while i < len(lines):
        if lines[i].startswith("==="):
            algo = lines[i].replace("=", "").strip()
            i += 2  # skip header

            pid, at, bt, ct, tat, wt = [], [], [], [], [], []

            while i < len(lines) and not lines[i].startswith("==="):
                parts = list(map(int, lines[i].split()))
                pid.append(parts[0])
                at.append(parts[1])
                bt.append(parts[2])
                ct.append(parts[3])
                tat.append(parts[4])
                wt.append(parts[5])
                i += 1

            data[algo] = {
                "pid": pid,
                "arrival": at,
                "burst": bt,
                "completion": ct,
                "turnaround": tat,
                "waiting": wt
            }
        else:
            i += 1

    return data


# ------------------ COMBINED DASHBOARD ------------------ #
def plot_algorithm_dashboard(algo, d):
    fig, axs = plt.subplots(3, 1, figsize=(10, 10))

    # -------- 1. WT vs TAT -------- #
    x = range(len(d["pid"]))
    axs[0].bar(x, d["waiting"], label="Waiting Time")
    axs[0].bar(x, d["turnaround"], bottom=d["waiting"], label="Turnaround Time")

    axs[0].set_xticks(x)
    axs[0].set_xticklabels([f"P{p}" for p in d["pid"]])
    axs[0].set_title(f"{algo} - Time Analysis")
    axs[0].set_ylabel("Time")
    axs[0].legend()
    axs[0].grid(axis='y')

    # -------- 2. Completion Timeline -------- #
    axs[1].plot(d["pid"], d["completion"], marker='o')

    for i in range(len(d["pid"])):
        axs[1].text(d["pid"][i], d["completion"][i], str(d["completion"][i]))

    axs[1].set_title("Completion Timeline")
    axs[1].set_xlabel("Process ID")
    axs[1].set_ylabel("Completion Time")
    axs[1].grid()

    # -------- 3. Pseudo Gantt -------- #
    for i in range(len(d["pid"])):
        start = d["completion"][i] - d["burst"][i]
        axs[2].barh(f"P{d['pid'][i]}", d["burst"][i], left=start)

        axs[2].text(start + d["burst"][i] / 2, i, f"P{d['pid'][i]}",
                    ha='center', va='center')

    axs[2].set_title("Process Execution (Approx)")
    axs[2].set_xlabel("Time")
    axs[2].set_ylabel("Processes")
    axs[2].grid(axis='x')

    # -------- FINAL TOUCH -------- #
    fig.suptitle(f"{algo} - Complete Visualization", fontsize=14)
    plt.tight_layout()

    plt.show()


# ------------------ GRAPH: COMPARISON ------------------ #
def plot_comparison(data):
    algos = list(data.keys())
    avg_wt = []
    avg_tat = []

    for algo in algos:
        d = data[algo]
        avg_wt.append(sum(d["waiting"]) / len(d["waiting"]))
        avg_tat.append(sum(d["turnaround"]) / len(d["turnaround"]))

    x = range(len(algos))

    plt.figure()
    plt.bar(x, avg_wt, label="Avg Waiting Time")
    plt.bar(x, avg_tat, bottom=avg_wt, label="Avg Turnaround Time")

    plt.xticks(x, algos)
    plt.ylabel("Time")
    plt.title("Comparison of Scheduling Algorithms")
    plt.legend()
    plt.grid(axis='y')

    plt.show()


# ------------------ MAIN ------------------ #
def main():
    data = parse_output(FILE)

    if not data:
        print("No data found in output.txt")
        return

    # -------- ONE WINDOW PER ALGORITHM -------- #
    for algo in data:
        print(f"Showing dashboard for: {algo}")
        plot_algorithm_dashboard(algo, data[algo])

    # -------- FINAL COMPARISON -------- #
    plot_comparison(data)


if __name__ == "__main__":
    main()