1. 用以下标志运行程序：./process-run.py -l 5:100,5:100。CPU 利用率（CPU 使用时间的百分比）应该是多少？为什么你知道这一点？利用 -c 标记查看你的答案是否正确。

```
./process-run.py -l 5:100,5:100
```

```
Produce a trace of what would happen when you run these processes: 
Process 0
  cpu
  cpu
  cpu
  cpu
  cpu

Process 1
  cpu
  cpu
  cpu
  cpu
  cpu

Important behaviors:
System will switch when the current process is FINISHED or ISSUES AN IO
After IOs, the process issuing the IO will run LATER (when it is its turn)
```

```
./process-run.py -l 5:100,5:100 -c
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2        RUN:cpu         READY             1          
  3        RUN:cpu         READY             1          
  4        RUN:cpu         READY             1          
  5        RUN:cpu         READY             1          
  6           DONE       RUN:cpu             1          
  7           DONE       RUN:cpu             1          
  8           DONE       RUN:cpu             1          
  9           DONE       RUN:cpu             1          
10           DONE       RUN:cpu             1          
```

**Answer：CPU利用率为100%，因为process0和process1都没有进行io操作。**



2. 现在用这些标志运行：./process-run.py -l 4:100,1:0。这些标志指定了一个包含 4 条指令的进程（都要使用 CPU），并且只是简单地发出 I/O 并等待它完成。完成这两个进程需要多长时间？利用-c 检查你的答案是否正确。

```
./process-run.py -l 4:100,1:0
```

```
Produce a trace of what would happen when you run these processes:
Process 0
  cpu
  cpu
  cpu
  cpu

Process 1
  io
  io_done

Important behaviors:
  System will switch when the current process is FINISHED or ISSUES AN IO
  After IOs, the process issuing the IO will run LATER (when it is its turn)
```

```
./process-run.py -l 4:100,1:0 -c
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2        RUN:cpu         READY             1          
  3        RUN:cpu         READY             1          
  4        RUN:cpu         READY             1          
  5           DONE        RUN:io               1          
  6           DONE       WAITING                           1
  7           DONE       WAITING                           1
  8           DONE       WAITING                           1
  9           DONE       WAITING                           1
10           DONE       WAITING                           1
11*         DONE   RUN:io_done             1
```

**Answer: 需要11个时钟周期。**



3. 现在交换进程的顺序：./process-run.py -l 1:0,4:100。现在发生了什么？交换顺序是否重要？为什么？同样，用-c 看看你的答案是否正确。

```
./process-run.py -l 1:0,4:100
```

```
Produce a trace of what would happen when you run these processes:
Process 0
  io
  io_done

Process 1
  cpu
  cpu
  cpu
  cpu

Important behaviors:
  System will switch when the current process is FINISHED or ISSUES AN IO
  After IOs, the process issuing the IO will run LATER (when it is its turn)
```

```
./process-run.py -l 1:0,4:100 -c
```

```
Time        PID: 0        PID: 1               CPU           IOs
  1         RUN:io         READY                1          
  2        WAITING       RUN:cpu             1             1
  3        WAITING       RUN:cpu             1             1
  4        WAITING       RUN:cpu             1             1
  5        WAITING       RUN:cpu             1             1
  6        WAITING          DONE                             1
  7*   RUN:io_done          DONE            1          
```

**Answer: 交换进程顺序后，cpu启动process0的io的操作后，process1的cpu操作同时与io操作并行进行，最终耗时7个时钟周期。现在，process0和process1是并行执行的。**



4. 现在探索另一些标志。一个重要的标志是-S，它决定了当进程发出 I/O 时系统如何反应。将标志设置为SWITCH_ON_END，在进程进行 I/O 操作时，系统将不会切换到另一 个进程，而是等待进程完成。当你运行以下两个进程时，会发生什么情况？一个执行 I/O，另一个执行 CPU 工作。（-l 1:0,4:100 -c -S SWITCH_ON_END）

```
./process-run.py -l 1:0,4:100 -c -S SWITCH_ON_END
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1         RUN:io         READY             1          
  2        WAITING         READY                           1
  3        WAITING         READY                           1
  4        WAITING         READY                           1
  5        WAITING         READY                           1
  6        WAITING         READY                           1
  7*   RUN:io_done      READY             1          
  8           DONE       RUN:cpu             1          
  9           DONE       RUN:cpu             1          
 10           DONE       RUN:cpu             1          
 11           DONE       RUN:cpu             1          
```

**Answer: 现在，在cpu执行启动process0的io操作后，进入等待状态，不进行process1的cpu并行运行，最终耗时11个时钟周期。**



5. 现在，运行相同的进程，但切换行为设置，在等待I/O 时切换到另一个进程（-l 1:0,4:100 -c -S SWITCH_ON_IO）。现在会发生什么？利用-c 来确认你的答案是否正确。

```
./process-run.py -l 1:0,4:100 -c -S SWITCH_ON_IO
```

```
Time        PID: 0          PID: 1              CPU           IOs
  1         RUN:io         READY                1          
  2        WAITING       RUN:cpu             1             1
  3        WAITING       RUN:cpu             1             1
  4        WAITING       RUN:cpu             1             1
  5        WAITING       RUN:cpu             1             1
  6        WAITING          DONE                             1
  7*   RUN:io_done        DONE              1          
```

**Answer: 现在，在cpu执行启动process0的io操作后，process1的cpu操作同时与io操作并行进行，最终耗时7个时钟周期。**



6. 另一个重要的行为是 I/O 完成时要做什么。利用-I IO_RUN_LATER，当 I/O 完成时， 发出它的进程不一定马上运行。相反，当时运行的进程一直运行。当你运行这个进程组合时会发生什么？（./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p）系统资源是否被有效利用？

```
./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p
```

```
Time        PID: 0        PID: 1        PID: 2        PID: 3           CPU           IOs
  1         RUN:io         READY         READY         READY               1          
  2        WAITING       RUN:cpu         READY         READY             1             1
  3        WAITING       RUN:cpu         READY         READY             1             1
  4        WAITING       RUN:cpu         READY         READY             1             1
  5        WAITING       RUN:cpu         READY         READY             1             1
  6        WAITING       RUN:cpu         READY         READY             1             1
  7*         READY          DONE       RUN:cpu         READY             1          
  8          READY          DONE       RUN:cpu         READY             1          
  9          READY          DONE       RUN:cpu         READY             1          
 10          READY          DONE       RUN:cpu         READY             1          
 11          READY          DONE       RUN:cpu         READY             1          
 12          READY          DONE          DONE       RUN:cpu             1          
 13          READY          DONE          DONE       RUN:cpu             1          
 14          READY          DONE          DONE       RUN:cpu             1          
 15          READY          DONE          DONE       RUN:cpu             1          
 16          READY          DONE          DONE       RUN:cpu             1          
 17    RUN:io_done          DONE          DONE          DONE             1          
 18         RUN:io          DONE          DONE          DONE             1          
 19        WAITING          DONE          DONE          DONE                           1
 20        WAITING          DONE          DONE          DONE                           1
 21        WAITING          DONE          DONE          DONE                           1
 22        WAITING          DONE          DONE          DONE                           1
 23        WAITING          DONE          DONE          DONE                           1
 24*   RUN:io_done          DONE          DONE          DONE             1          
 25         RUN:io          DONE          DONE          DONE             1          
 26        WAITING          DONE          DONE          DONE                           1
 27        WAITING          DONE          DONE          DONE                           1
 28        WAITING          DONE          DONE          DONE                           1
 29        WAITING          DONE          DONE          DONE                           1
 30        WAITING          DONE          DONE          DONE                           1
 31*   RUN:io_done          DONE          DONE          DONE             1          

Stats: Total Time 31
Stats: CPU Busy 21 (67.74%)
Stats: IO Busy  15 (48.39%)
```

**Answer: 在process0的第一次io操作结束后，迟迟获取不到cpu的控制权，以至于要等待所有的进程完成cpu操作后才能继续io操作，浪费了系统资源，最终耗时31个时钟周期。**



7. 现在运行相同的进程，但使用-I IO_RUN_IMMEDIATE 设置，该设置立即运行发出 I/O 的进程。这种行为有何不同？为什么运行一个刚刚完成 I/O 的进程会是一个好主意？

```
./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_IMMEDIATE -c -p
```

```
Time        PID: 0        PID: 1        PID: 2        PID: 3           CPU           IOs
  1         RUN:io         READY         READY         READY             1          
  2        WAITING       RUN:cpu         READY         READY             1             1
  3        WAITING       RUN:cpu         READY         READY             1             1
  4        WAITING       RUN:cpu         READY         READY             1             1
  5        WAITING       RUN:cpu         READY         READY             1             1
  6        WAITING       RUN:cpu         READY         READY             1             1
  7*   RUN:io_done          DONE         READY         READY             1          
  8         RUN:io          DONE         READY         READY             1          
  9        WAITING          DONE       RUN:cpu         READY             1             1
 10        WAITING          DONE       RUN:cpu         READY             1             1
 11        WAITING          DONE       RUN:cpu         READY             1             1
 12        WAITING          DONE       RUN:cpu         READY             1             1
 13        WAITING          DONE       RUN:cpu         READY             1             1
 14*   RUN:io_done          DONE          DONE         READY             1          
 15         RUN:io          DONE          DONE         READY             1          
 16        WAITING          DONE          DONE       RUN:cpu             1             1
 17        WAITING          DONE          DONE       RUN:cpu             1             1
 18        WAITING          DONE          DONE       RUN:cpu             1             1
 19        WAITING          DONE          DONE       RUN:cpu             1             1
 20        WAITING          DONE          DONE       RUN:cpu             1             1
 21*   RUN:io_done          DONE          DONE          DONE             1          

Stats: Total Time 21
Stats: CPU Busy 21 (100.00%)
Stats: IO Busy  15 (71.43%)
```

**Answer: 现在，io操作可以和cpu执行并行执行，不浪费cpu资源，最终耗时21个时钟周期。**



8. 现在运行一些随机生成的进程，例如-s 1 -l 3:50,3:50, -s 2 -l 3:50,3:50, -s 3 -l 3:50,3:50。 看看你是否能预测追踪记录会如何变化？当你使用-I IO_RUN_IMMEDIATE 与-I IO_RUN_LATER时会发生什么？当你使用-S SWITCH_ON_IO与-S SWITCH_ON_END时会发生什么？

使用-I IO_RUN_IMMEDIATE

```
./process-run.py -s 1 -l 3:50,3:50, -s 2 -l 3:50,3:50, -s 3 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -c
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2         RUN:io         READY             1          
  3        WAITING        RUN:io             1             1
  4        WAITING       WAITING                           2
  5        WAITING       WAITING                           2
  6        WAITING       WAITING                           2
  7        WAITING       WAITING                           2
  8*   RUN:io_done       WAITING             1             1
  9*         READY   RUN:io_done             1          
 10          READY        RUN:io             1          
 11        RUN:cpu       WAITING             1             1
 12           DONE       WAITING                           1
 13           DONE       WAITING                           1
 14           DONE       WAITING                           1
 15           DONE       WAITING                           1
 16*          DONE   RUN:io_done             1          
 17           DONE       RUN:cpu             1  
```

使用-I IO_RUN_LATER

```
./process-run.py -s 1 -l 3:50,3:50, -s 2 -l 3:50,3:50, -s 3 -l 3:50,3:50 -I IO_RUN_LATER -c
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2         RUN:io         READY             1          
  3        WAITING        RUN:io             1             1
  4        WAITING       WAITING                           2
  5        WAITING       WAITING                           2
  6        WAITING       WAITING                           2
  7        WAITING       WAITING                           2
  8*   RUN:io_done       WAITING             1             1
  9*       RUN:cpu         READY             1          
 10           DONE   RUN:io_done             1          
 11           DONE        RUN:io             1          
 12           DONE       WAITING                           1
 13           DONE       WAITING                           1
 14           DONE       WAITING                           1
 15           DONE       WAITING                           1
 16           DONE       WAITING                           1
 17*          DONE   RUN:io_done             1          
 18           DONE       RUN:cpu             1          
```

使用-S SWITCH_ON_IO

```
./process-run.py -s 1 -l 3:50,3:50, -s 2 -l 3:50,3:50, -s 3 -l 3:50,3:50 -S SWITCH_ON_IO -c
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2         RUN:io         READY             1          
  3        WAITING        RUN:io             1             1
  4        WAITING       WAITING                           2
  5        WAITING       WAITING                           2
  6        WAITING       WAITING                           2
  7        WAITING       WAITING                           2
  8*   RUN:io_done       WAITING             1             1
  9*       RUN:cpu         READY             1          
 10           DONE   RUN:io_done             1          
 11           DONE        RUN:io             1          
 12           DONE       WAITING                           1
 13           DONE       WAITING                           1
 14           DONE       WAITING                           1
 15           DONE       WAITING                           1
 16           DONE       WAITING                           1
 17*          DONE   RUN:io_done             1          
 18           DONE       RUN:cpu             1
```

使用-S SWITCH_ON_END

```
./process-run.py -s 1 -l 3:50,3:50, -s 2 -l 3:50,3:50, -s 3 -l 3:50,3:50 -S SWITCH_ON_END -c
```

```
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1          
  2         RUN:io         READY             1          
  3        WAITING         READY                           1
  4        WAITING         READY                           1
  5        WAITING         READY                           1
  6        WAITING         READY                           1
  7        WAITING         READY                           1
  8*   RUN:io_done         READY             1          
  9        RUN:cpu         READY             1          
 10           DONE        RUN:io             1          
 11           DONE       WAITING                           1
 12           DONE       WAITING                           1
 13           DONE       WAITING                           1
 14           DONE       WAITING                           1
 15           DONE       WAITING                           1
 16*          DONE   RUN:io_done             1          
 17           DONE        RUN:io             1          
 18           DONE       WAITING                           1
 19           DONE       WAITING                           1
 20           DONE       WAITING                           1
 21           DONE       WAITING                           1
 22           DONE       WAITING                           1
 23*          DONE   RUN:io_done             1          
 24           DONE       RUN:cpu             1
```

**Answer: 当使用SWITCH_ON_IO 和SWITCH_ON_END参数时，加了SWITCH_ON_END后io操作和cpu执行不能同时运行，使得process0和precess1的io操作不能并行执行。**
**当使用-I IO_RUN_IMMEDIATE 和-I IO_RUN_LATER参数时，加了-I IO_RUN_IMMEDIATE优先执行后续还有io操作的进程，使得io操作和cpu操作可以并行执行，使得cpu资源利用最大化。**
