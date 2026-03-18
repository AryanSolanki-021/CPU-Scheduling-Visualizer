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

// ---------- FCFS ----------
void fcfs(int n, int pid[], int at[], int bt[])
{
    int ct[MAX], tat[MAX], wt[MAX];
    int time = 0;

    for (int i = 0; i < n; i++)
    {
        if (time < at[i])
            time = at[i];

        time += bt[i];
        ct[i] = time;
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n--- FCFS ---\n");
    printf("PID AT BT CT TAT WT\n");

    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);

    save_to_file("FCFS", n, pid, at, bt, ct, tat, wt);
}

// ---------- SJF ----------
void sjf(int n, int pid[], int at[], int bt[])
{
    int ct[MAX], tat[MAX], wt[MAX];
    int completed[MAX] = {0};
    int time = 0, count = 0;

    while (count < n)
    {
        int min = 9999, index = -1;

        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && !completed[i] && bt[i] < min)
            {
                min = bt[i];
                index = i;
            }
        }

        if (index == -1)
            time++;
        else
        {
            time += bt[index];
            ct[index] = time;
            completed[index] = 1;
            count++;
        }
    }

    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n--- SJF Non-Preemptive ---\n");
    printf("PID AT BT CT TAT WT\n");

    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);

    save_to_file("SJF Non-Preemptive", n, pid, at, bt, ct, tat, wt);
}

// ---------- SRTN ----------
void srtn(int n, int pid[], int at[], int bt[])
{
    int rt[MAX], ct[MAX], tat[MAX], wt[MAX];
    int time = 0, completed = 0;

    for (int i = 0; i < n; i++)
        rt[i] = bt[i];

    while (completed != n)
    {
        int min = 9999, index = -1;

        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && rt[i] > 0 && rt[i] < min)
            {
                min = rt[i];
                index = i;
            }
        }

        if (index == -1)
        {
            time++;
            continue;
        }

        rt[index]--;
        time++;

        if (rt[index] == 0)
        {
            ct[index] = time;
            completed++;
        }
    }

    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n--- SRTN ---\n");
    printf("PID AT BT CT TAT WT\n");

    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);

    save_to_file("SRTN", n, pid, at, bt, ct, tat, wt);
}

// ---------- Round Robin ----------
void round_robin(int n, int pid[], int at[], int bt[], int tq, int cs)
{
    int rt[MAX], ct[MAX], tat[MAX], wt[MAX];
    int time = 0, completed = 0;

    for (int i = 0; i < n; i++)
        rt[i] = bt[i];

    while (completed < n)
    {
        int done = 1;

        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && rt[i] > 0)
            {
                done = 0;

                if (rt[i] > tq)
                {
                    time += tq;
                    rt[i] -= tq;
                    time += cs;
                }
                else
                {
                    time += rt[i];
                    rt[i] = 0;
                    ct[i] = time;
                    completed++;
                    if (completed < n)
                        time += cs;
                }
            }
        }

        if (done)
            time++;
    }

    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n--- Round Robin ---\n");
    printf("PID AT BT CT TAT WT\n");

    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], ct[i], tat[i], wt[i]);

    save_to_file("Round Robin", n, pid, at, bt, ct, tat, wt);
}

// ---------- Priority Non-Preemptive ----------
void priority_np(int n, int pid[], int at[], int bt[], int pr[])
{
    int ct[MAX], tat[MAX], wt[MAX];
    int completed[MAX] = {0};
    int time = 0, count = 0;

    while (count < n)
    {
        int best = 9999, index = -1;

        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && !completed[i] && pr[i] < best)
            {
                best = pr[i];
                index = i;
            }
        }

        if (index == -1)
            time++;
        else
        {
            time += bt[index];
            ct[index] = time;
            completed[index] = 1;
            count++;
        }
    }

    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n--- Priority Non-Preemptive ---\n");
    printf("PID AT BT PR CT TAT WT\n");

    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], pr[i], ct[i], tat[i], wt[i]);

    save_to_file("Priority Non-Preemptive", n, pid, at, bt, ct, tat, wt);
}

// ---------- Priority Preemptive ----------
void priority_p(int n, int pid[], int at[], int bt[], int pr[])
{
    int rt[MAX], ct[MAX], tat[MAX], wt[MAX];
    int time = 0, completed = 0;

    for (int i = 0; i < n; i++)
        rt[i] = bt[i];

    while (completed != n)
    {
        int best = 9999, index = -1;

        for (int i = 0; i < n; i++)
        {
            if (at[i] <= time && rt[i] > 0 && pr[i] < best)
            {
                best = pr[i];
                index = i;
            }
        }

        if (index == -1)
        {
            time++;
            continue;
        }

        rt[index]--;
        time++;

        if (rt[index] == 0)
        {
            ct[index] = time;
            completed++;
        }
    }

    for (int i = 0; i < n; i++)
    {
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];
    }

    printf("\n--- Priority Preemptive ---\n");
    printf("PID AT BT PR CT TAT WT\n");

    for (int i = 0; i < n; i++)
        printf("%d %d %d %d %d %d %d\n",
               pid[i], at[i], bt[i], pr[i], ct[i], tat[i], wt[i]);

    save_to_file("Priority Preemptive", n, pid, at, bt, ct, tat, wt);
}

#endif