1. 对前一个程序稍作修改，这次使用 waitpid()而我是 wait()。谁谁谁谁 waitpid()会 有用？编写一个调用 fork()的程序。在调用 fork()之前，让主进程访问一个变量（例如 x） 并将其值设置为某个值（例如 100）。子进程中的变量有什么值？当子进程和父进程都改变x 的值时，变量会发生什么？

   ```c
   #include <stdio.h>
   #include <unistd.h>
   
   void main() {
       int pid = -1;
       int x = 100;
       pid = fork();
   
       if (pid < 0) {
           printf("Error!");
       } else if (pid == 0) {
           printf("This is child, my pid is %d, my init_value of x is %d \n", pid, x);
           x = 2222;
           printf("This is child, my pid is %d, my change_value of x is %d \n", pid, x);
       } else if (pid > 0) {
           printf("This is father, my pid is %d, my value of x is %d \n", pid, x);
           x = 3333;
           printf("This is father, my pid is %d, my change_value of x is %d \n", pid, x);
       }
   }
   ```

   ```
   This is father, my pid is 32697, my value of x is 100 
   This is father, my pid is 32697, my change_value of x is 3333 
   This is child, my pid is 0, my init_value of x is 100 
   This is child, my pid is 0, my change_value of x is 2222 
   ```

   **Answer: 子进程和父进程的初始x值是相等的，改变x值两两互不影响，各改变各的。**

   

2. 编写一个打开文件的程序（使用 open()系统调用），然后调用 fork()创建一个新进程。 子进程和父进程都可以访问 open()返回的文件描述符吗？当它我并发（即同时）写入文件时，会发生什么？

```c
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void main() {
    int f, vid;
    f = open("./text.txt", O_RDWR);
    vid = fork();
    if (vid == 0) {
        printf("This is file %d from child\n", f);
    } else if (vid > 0) {
        printf("This is file %d from father\n", f);
    } else {
        printf("Error!");
    }
    if (vid == 0) {
        write(f, "This is text from child\n", 20);
    } else if (vid > 0) {
        write(f, "This is text from father\n", 25);
    } else {
        printf("Error!");
    }
    close(f);
}

```

**Answer: 子进程和父进程都可以访问返回的文件描述符。同时写入时，不会发生错误，但是会有写入的先后顺序。**



3. 使用 fork()编写另一个程序。子进程应打印“hello”，父进程应打印“goodbye”。你应该尝试确保子进程始终先打印。你能否不在父进程调用 wait()而做到这一点呢？

```c
#include <stdio.h>
#include <unistd.h>

void main(){
    int vid;
    vid = vfork();
    if (vid == 0){
        printf("Hello\n");
        _exit(0);
    }else if(vid > 0){
        printf("goodbye\n");
    }
}
```

**Answer: 使用vfork()，vfork产生的子进程与父进程共享数据段，并且子进程运行，当子进程调用_exit(0)后，父进程才会继续执行。若子进程未调用 _exit(0)，会导致程序的死锁。**



4. 编写一个调用 fork()的程序，然后调用某种形式的 exec()来运行程序/bin/ls。看看是否可以尝试 exec()的所有变体，包括 execl()、execle()、execlp()、execv()、execvp()和 execvP()。

```c
#include <stdio.h>
#include <unistd.h>

void main(int argc, char *argv[], char *envp[]) {
    int vid;
    vid = vfork();
    if (vid == 0) {
        printf("This is child\n");
    } else if (vid > 0) {
        char *argv[] = {"ls", "-l", NULL};
        char *path = "/bin/ls";

        execl(path, "ls", "-l", NULL);
        execlp("ls", "-l", NULL);
        execle(path, "ls", "-l", NULL, envp);

        execv(path, argv);
        execvp(path, argv);
        execve(path, argv, envp);

    }
}

```

**Answer: 他们实现的功能是一样的，但是需要的参数形式不同，为了满足不同的需求。**



5. 现在编写一个程序，在父进程中使用 wait()，等待子进程完成。wait()返回什么？如果你在子进程中使用 wait()会发生什么？

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void main(){
    int vid, pid, status;
    vid = fork();
    if(vid == 0){
        printf("i am child, my id is %d\n", vid);
        pid = wait(&status);
        printf("alright, i am child, i am using the wait(), its namber is %d\n", pid);
    }else if (vid > 0){
        printf("i am father, my id is %d,it's time to wait child...\n", vid);
        pid = wait(&status);
        printf("alright, i am father, my child is over, its namber is %d\n", pid);
    }
}
```

**Answer: 在父进程中返回调用子进程的id，在子进程中会返回-1 。**



6. 对前一个程序稍作修改，这次使用 waitpid()而不是 wait()。什么时候 waitpid()会有用？

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void main() {
    int vid, pid, status;
    vid = fork();
    if (vid == 0) {
        printf("i am child, my id is %d\n", vid);
        pid = wait(&status);
        printf("alright, i am child, i am using the wait(), its namber is %d\n", pid);
    } else if (vid > 0) {
        printf("i am father, my id is %d,it's time to wait child...\n", vid);
        pid = waitpid(vid, &status, 0);
        printf("alright, i am father, my child is over, its namber is %d\n", pid);
    }
}
```

*  pid>0时，只等待进程ID等于pid的子进程，不管其它已经有多少子进程运行结束退出了，只要指定的子进程还没有结束，waitpid就会一直等下去。
* pid=-1时，等待任何一个子进程退出，没有任何限制，此时waitpid和wait的作用一模一样。

* pid=0时，等待同一个进程组中的任何子进程，如果子进程已经加入了别的进程组，waitpid不会对它做任何理睬。

* pid<-1时，等待一个指定进程组中的任何子进程，这个进程组的ID等于pid的绝对值。

​	**Answer: waitpid可以等待特定的子进程结束之后再继续运行。**



7. 编写一个创建子进程的程序，然后在子进程中关闭标准输出（STDOUT_FILENO）。 如果子进程在关闭描述符后调用 printf()打印输出，会发生什么？

```c
#include <stdio.h>
#include <unistd.h>

void main() {
    int vid;
    vid = fork();
    if (vid == 0) {
        printf("This is child\n");
        close(1);
        printf("Already close the stdout");
    } else if (vid > 0) {
        printf("This is father\n");
    }
}
```

**Answer: 在关闭标准输出后，子进程其后的内容不会被打印出来。**



8. 编写一个程序，创建两个子进程，并使用 pipe()系统调用，将一个子进程的标准输出连接到另一个子进程的标准输入。

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void main() {
    int p[2], vid, backup;
    char inbuf[19];
    if (pipe(p) < 0) exit(1);
    vid = fork();
    if (vid == 0) {
        printf("This is child\n");
        dup2(1, backup);
        close(1);
        printf("Try\n");
        write(p[1], "trans_imformation\n", 19);
        dup2(backup, 1);
        printf("used pip\n");
    } else if (vid > 0) {
        wait(NULL);
        read(p[0], inbuf, 19);
        printf("%s\n", inbuf);
        printf("This is father\n");
    }
}
```
