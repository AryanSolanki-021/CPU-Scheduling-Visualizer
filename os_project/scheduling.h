#ifndef SCHEDULING_H
#define SCHEDULING_H
 
#include <stdio.h>
 
#define MAX 50
 
// ---------- Save Output ----------
void save_to_file(char algo[], int n, int pid[], int at[], int bt[], int ct[], int tat[], int wt[])
{
    FILE *fp = fopen("output.txt", "a");
 
    fprintf(fp, "\n=== %s ===\n", algo);
    fprintf(fp, "PID\tAT\tBT\tCT\tTAT\tWT\n");
 
    for (int i = 0; i < n; i++)
        fprintf(fp, "%d\t%d\t%d\t%d\t%d\t%d\n",
                pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);
 
    fclose(fp);
}
 
// ---------- Gantt Chart Printer ----------
// gantt_seq[]  : sequence of PIDs executed (e.g. {1, 3, 2, 1, ...})
// gantt_time[] : timeline markers         (e.g. {0, 2, 4, 10, 16, ...})
// len          : number of segments (gantt_time has len+1 entries)
void print_gantt(int gantt_seq[], int gantt_time[], int len)
{
    printf("\nGantt Chart:\n");
 
    // Top border
    printf(" ");
    for (int i = 0; i < len; i++)
    {
        int seg = gantt_time[i + 1] - gantt_time[i];
        for (int j = 0; j < seg * 2 + 1; j++)
            printf("-");
        printf(" ");
    }
    printf("\n|");
 
    // Process labels (centred inside each segment)
    for (int i = 0; i < len; i++)
    {
        int seg = gantt_time[i + 1] - gantt_time[i];
        int width = seg * 2 + 1;
        int label_len = (gantt_seq[i] >= 10) ? 4 : 3; // "P10" vs "P1 "
        int left  = (width - label_len) / 2;
        int right = width - label_len - left;
 
        for (int j = 0; j < left; j++)  printf(" ");
        printf("P%d", gantt_seq[i]);
        for (int j = 0; j < right; j++) printf(" ");
        printf("|");
    }
    printf("\n ");
 
    // Bottom border
    for (int i = 0; i < len; i++)
    {
        int seg = gantt_time[i + 1] - gantt_time[i];
        for (int j = 0; j < seg * 2 + 1; j++)
            printf("-");
        printf(" ");
    }
    printf("\n");
 
    // Time markers
    printf("%d", gantt_time[0]);
    for (int i = 0; i < len; i++)
    {
        int seg = gantt_time[i + 1] - gantt_time[i];
        int width = seg * 2 + 2;          // space up to next marker
        int t     = gantt_time[i + 1];
        int digits = (t >= 100) ? 3 : (t >= 10) ? 2 : 1;
 
        for (int j = 0; j < width - digits; j++)
            printf(" ");
        printf("%d", t);
    }
    printf("\n");
}
 
// ---------- FCFS ----------
void fcfs(int n, int pid[], int at[], int bt[])
{
    int ct[MAX], tat[MAX], wt[MAX];
    int time = 0;
 
    // Gantt arrays: one segment per process
    int gantt_seq[MAX];
    int gantt_time[MAX + 1];
    int glen = 0;
 
    for (int i = 0; i < n; i++)
    {
        if (time < at[i])
            time = at[i];
 
        gantt_time[glen] = time;
        gantt_seq[glen]  = pid[i];
        glen++;
 
        time += bt[i];
        ct[i] = time;
        tat[i] = ct[i] - at[i];
        wt[i]  = tat[i] - bt[i];
    }
    gantt_time[glen] = time;
 
    printf("\n--- FCFS ---\n");
    printf("PID AT BT CT TAT WT\n");
 
    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);
 
    print_gantt(gantt_seq, gantt_time, glen);
 
    save_to_file("FCFS", n, pid, at, bt, ct, tat, wt);
}
 
// ---------- SJF ----------
void sjf(int n, int pid[], int at[], int bt[])
{
    int ct[MAX], tat[MAX], wt[MAX];
    int completed[MAX] = {0};
    int time = 0, count = 0;
 
    // Gantt arrays: one segment per process (order determined by scheduler)
    int gantt_seq[MAX];
    int gantt_time[MAX + 1];
    int glen = 0;
 
    while (count < n)
    {
        int min = 9999, index = -1;
 
        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && !completed[i] && bt[i] < min)
            {
                min   = bt[i];
                index = i;
            }
        }
 
        if (index == -1)
        {
            time++;
        }
        else
        {
            gantt_time[glen] = time;
            gantt_seq[glen]  = pid[index];
            glen++;
 
            time += bt[index];
            ct[index]        = time;
            completed[index] = 1;
            count++;
        }
    }
    gantt_time[glen] = time;
 
    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i]  = tat[i] - bt[i];
    }
 
    printf("\n--- SJF Non-Preemptive ---\n");
    printf("PID AT BT CT TAT WT\n");
 
    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);
 
    print_gantt(gantt_seq, gantt_time, glen);
 
    save_to_file("SJF Non-Preemptive", n, pid, at, bt, ct, tat, wt);
}
 
// ---------- SRTN ----------
void srtn(int n, int pid[], int at[], int bt[])
{
    int rt[MAX], ct[MAX], tat[MAX], wt[MAX];
    int time = 0, completed = 0;
 
    for (int i = 0; i < n; i++)
        rt[i] = bt[i];
 
    // Gantt: track tick-by-tick, then compress consecutive same-PID segments
    int gantt_seq[MAX * 100];
    int gantt_time[MAX * 100 + 1];
    int glen      = 0;
    int prev_pid  = -1;
 
    while (completed != n)
    {
        int min = 9999, index = -1;
 
        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && rt[i] > 0 && rt[i] < min)
            {
                min   = rt[i];
                index = i;
            }
        }
 
        if (index == -1)
        {
            // CPU idle — record as PID 0
            if (prev_pid != 0)
            {
                gantt_time[glen] = time;
                gantt_seq[glen]  = 0;
                glen++;
                prev_pid = 0;
            }
            time++;
            continue;
        }
 
        // New segment only when process changes
        if (pid[index] != prev_pid)
        {
            gantt_time[glen] = time;
            gantt_seq[glen]  = pid[index];
            glen++;
            prev_pid = pid[index];
        }
 
        rt[index]--;
        time++;
 
        if (rt[index] == 0)
        {
            ct[index] = time;
            completed++;
        }
    }
    gantt_time[glen] = time;
 
    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i]  = tat[i] - bt[i];
    }
 
    printf("\n--- SRTN ---\n");
    printf("PID AT BT CT TAT WT\n");
 
    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);
 
    print_gantt(gantt_seq, gantt_time, glen);
 
    save_to_file("SRTN", n, pid, at, bt, ct, tat, wt);
}
 
// ---------- Round Robin ----------
void round_robin(int n, int pid[], int at[], int bt[], int tq, int cs)
{
    int rt[MAX], ct[MAX], tat[MAX], wt[MAX];
    int time = 0, completed = 0;
 
    for (int i = 0; i < n; i++)
        rt[i] = bt[i];
 
    // Gantt: each slice (or partial slice) is one segment
    int gantt_seq[MAX * 100];
    int gantt_time[MAX * 100 + 1];
    int glen = 0;
 
    while (completed < n)
    {
        int done = 1;
 
        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && rt[i] > 0)
            {
                done = 0;
 
                gantt_time[glen] = time;
                gantt_seq[glen]  = pid[i];
                glen++;
 
                if (rt[i] > tq)
                {
                    time   += tq;
                    rt[i]  -= tq;
                    time   += cs;
                }
                else
                {
                    time  += rt[i];
                    rt[i]  = 0;
                    ct[i]  = time;
                    completed++;
                    if (completed < n)
                        time += cs;
                }
            }
        }
 
        if (done)
            time++;
    }
    gantt_time[glen] = time;
 
    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i]  = tat[i] - bt[i];
    }
 
    printf("\n--- Round Robin ---\n");
    printf("PID AT BT CT TAT WT\n");
 
    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);
 
    print_gantt(gantt_seq, gantt_time, glen);
 
    save_to_file("Round Robin", n, pid, at, bt, ct, tat, wt);
}
 
// ---------- Priority Non-Preemptive ----------
void priority_np(int n, int pid[], int at[], int bt[], int pr[])
{
    int ct[MAX], tat[MAX], wt[MAX];
    int completed[MAX] = {0};
    int time = 0, count = 0;
 
    // Gantt arrays: one segment per process (order determined by scheduler)
    int gantt_seq[MAX];
    int gantt_time[MAX + 1];
    int glen = 0;
 
    while (count < n)
    {
        int best = 9999, index = -1;
 
        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && !completed[i] && pr[i] < best)
            {
                best  = pr[i];
                index = i;
            }
        }
 
        if (index == -1)
        {
            time++;
        }
        else
        {
            gantt_time[glen] = time;
            gantt_seq[glen]  = pid[index];
            glen++;
 
            time            += bt[index];
            ct[index]        = time;
            completed[index] = 1;
            count++;
        }
    }
    gantt_time[glen] = time;
 
    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i]  = tat[i] - bt[i];
    }
 
    printf("\n--- Priority Non-Preemptive ---\n");
    printf("PID AT BT PR CT TAT WT\n");
 
    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], pr[i], ct[i], tat[i], wt[i]);
 
    print_gantt(gantt_seq, gantt_time, glen);
 
    save_to_file("Priority Non-Preemptive", n, pid, at, bt, ct, tat, wt);
}
 
// ---------- Priority Preemptive ----------
void priority_p(int n, int pid[], int at[], int bt[], int pr[])
{
    int rt[MAX], ct[MAX], tat[MAX], wt[MAX];
    int time = 0, completed = 0;
 
    for (int i = 0; i < n; i++)
        rt[i] = bt[i];
 
    // Gantt: track tick-by-tick, compress consecutive same-PID segments
    int gantt_seq[MAX * 100];
    int gantt_time[MAX * 100 + 1];
    int glen     = 0;
    int prev_pid = -1;
 
    while (completed != n)
    {
        int best = 9999, index = -1;
 
        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && rt[i] > 0 && pr[i] < best)
            {
                best  = pr[i];
                index = i;
            }
        }
 
        if (index == -1)
        {
            // CPU idle — record as PID 0
            if (prev_pid != 0)
            {
                gantt_time[glen] = time;
                gantt_seq[glen]  = 0;
                glen++;
                prev_pid = 0;
            }
            time++;
            continue;
        }
 
        // New segment only when process changes
        if (pid[index] != prev_pid)
        {
            gantt_time[glen] = time;
            gantt_seq[glen]  = pid[index];
            glen++;
            prev_pid = pid[index];
        }
 
        rt[index]--;
        time++;
 
        if (rt[index] == 0)
        {
            ct[index] = time;
            completed++;
        }
    }
    gantt_time[glen] = time;
 
    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i]  = tat[i] - bt[i];
    }
 
    printf("\n--- Priority Preemptive ---\n");
    printf("PID AT BT PR CT TAT WT\n");
 
    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], pr[i], ct[i], tat[i], wt[i]);
 
    print_gantt(gantt_seq, gantt_time, glen);
 
    save_to_file("Priority Preemptive", n, pid, at, bt, ct, tat, wt);
}
 
#endif
