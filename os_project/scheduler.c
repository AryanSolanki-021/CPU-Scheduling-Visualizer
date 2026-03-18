#include <stdio.h>
#include <stdlib.h>
#include "scheduling.h"

#define MAX 50

int main()
{
    // Clear file ONCE at start
    FILE *fp = fopen("output.txt", "w");
    fclose(fp);
    int n, choice;
    int pid[MAX], at[MAX], bt[MAX], pr[MAX];
    printf("\nEnter number of processes: ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        pid[i] = i + 1;
        printf("\nProcess %d\n", pid[i]);
        printf("Arrival Time: ");
        scanf("%d", &at[i]);
        printf("Burst Time: ");
        scanf("%d", &bt[i]);
    }

    while (1)
    {
        printf("\n===== CPU Scheduling Simulator =====\n");
        printf("1. FCFS\n");
        printf("2. SJF (Non-Preemptive)\n");
        printf("3. SRTN\n");
        printf("4. Round Robin\n");
        printf("5. Priority (Non-Preemptive)\n");
        printf("6. Priority (Preemptive)\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 7)
        {
            printf("Exiting program...\n");
            break;
        }

        switch (choice)
        {
        case 1:
            fcfs(n, pid, at, bt);
            break;

        case 2:
            sjf(n, pid, at, bt);
            break;

        case 3:
            srtn(n, pid, at, bt);
            break;

        case 4:
        {
            int tq, cs;
            printf("Enter Time Quantum: ");
            scanf("%d", &tq);
            printf("Enter Context Switch Time: ");
            scanf("%d", &cs);
            round_robin(n, pid, at, bt, tq, cs);
            break;
        }

        case 5:
            for (int i = 0; i < n; i++)
            {
                printf("Priority of Process %d: ", pid[i]);
                scanf("%d", &pr[i]);
            }
            priority_np(n, pid, at, bt, pr);
            break;

        case 6:
            for (int i = 0; i < n; i++)
            {
                printf("Priority of Process %d: ", pid[i]);
                scanf("%d", &pr[i]);
            }
            priority_p(n, pid, at, bt, pr);
            break;

        default:
            printf("Invalid choice!\n");
        }
    }

    return 0;
}