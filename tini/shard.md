![image](https://user-images.githubusercontent.com/23253540/147585620-ee5b49e3-c947-4fce-a4c2-560a6578248e.png)

## 背景
收到告警通知, ⚠️ 容器线程数异常（PID上限为15K，超过15K则无法新建进程）⚠️ 。该服务会定时通过 puppeteer 进行一些页面性能收集的任务，为什么残留了这么多进程没有正常退出？

进入终端调试后，发现了大量的 chrome defunct processes 🧟‍♀️🧟‍♂️ 僵尸进程。于是尝试在 puppeteer issue [Zombie Process problem. #1825](https://github.com/puppeteer/puppeteer/issues/1825) 中找一找答案。

![451a52f0f90eeeb19449b8dfa8bb92c20651f5b4](https://user-images.githubusercontent.com/23253540/147537580-776565a9-208a-4508-8554-95287ebb058a.png)

## 尝试解决
按照 puppeteer issue 中的建议，在 browser.close() 后，新增了 ps.kill 去杀死可能会残留的相关进程。
```js
await page.close();
await browser.close();
const psLookup = await ps.lookup({ pid: borwserPID });

for (let proc of psLookup) {
  if (_.has(proc, 'pid')) {
      await ps.kill(proc.pid, 'SIGKILL');
  }
}
```
然后又过了几天，又收到了告警通知，即本次并未解决该问题。最后又通过运行 puppeteer 时加上 --single-process 参数和定时调用 kill -9 [pid] 去杀死僵尸进程等方法都以失败告终 ❌ 。

```js
const chromeFlags = [
    '--headless',
    '--no-sandbox',
    "--disable-gpu",
    "--single-process",
    "--no-zygote"
]
```

## 僵尸进程
正当大家困惑的时候，同学 a 发来了一篇文章 [一次 Docker 容器内大量僵尸进程排查分析](https://mp.weixin.qq.com/s/85ZigYHVBtu6LDTtqpesVw)，文章中进行了详细的科普，此时才真正认识了僵尸进程。

到这里给我的体会是，如果遇见了一筹莫展的问题，不妨先仔细了解一下该问题的***定义与介绍***。它的基础概念是什么？造成的本质原因是什么？***了解完前因后果后或许能够事半功倍***。

> 僵尸进程 - 维基百科: 在类UNIX系统中，僵尸进程是指完成执行（通过exit系统调用，或运行时发生致命错误或收到终止信号所致），但在操作系统的进程表中仍然存在其进程控制块，处于"终止状态"的进程。这发生于子进程需要保留表项以允许其父进程读取子进程的退出状态：一旦退出态通过wait系统调用读取，僵尸进程条目就从进程表中删除，称之为"回收"（reaped）。正常情况下，进程直接被其父进程wait并由系统回收。进程长时间保持僵尸状态一般是错误的并导致资源泄漏。

通俗的来讲，就像下面的程序一样。当子进程调用 exit 函数退出了，但是父进程没有给它收尸，于是它变成了杀不死的🧟‍♀️🧟‍♂️ ，因为它早就已经死了，现在只是在进程列表中占了一个坑位而已。

当该僵尸进程的父进程退出后，它就会被托管到 PID 为 1 的进程上面，通常 PID 为 1 的进程会扮演收尸的角色。
但是当 Node.js 为 PID 1 的进程时，不会进行收尸，从而导致了大量的僵尸进程的问题。
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

  printf("pid %d\n", getpid());
  int child_pid = fork();
  if (child_pid == 0) {
    printf("-----in child process:  %d\n", getpid());
    exit(0);
  } else {
    sleep(1000000);
  }
  return 0;
}
```
## 解决办法
当 Docker 中第一个运行的程序为 node xxx.js 时 Node 就成为了 PID 为 1 的进程，所以说问题的解决办法可以是让有能力收尸的进程为第一个运行的程序。

在 [Docker and Node.js Best Practices](https://github.com/nodejs/docker-node/blob/main/docs/BestPractices.md) 中官方也给出了解决方案
1. 通过 docker 加上 --init 参数使得有一个 init 进程为 PID 为 1
2. 通过 Tini 作为容器去运行 Node 程序

> Node.js was not designed to run as PID 1 which leads to unexpected behaviour when running inside of Docker. For example, a Node.js process running as PID 1 will not respond to SIGINT (CTRL-C) and similar signals. As of Docker 1.13, you can use the --init flag to wrap your Node.js process with a lightweight init system that properly handles running as PID 1.

```bash
docker run -it --init node
```

> You can also include Tini directly in your Dockerfile, ensuring your process is always started with an init wrapper.

## Tini
现在让我们通过 [Tini](https://github.com/krallin/tini) 来学习了一下收尸技术，可通过下面的方式让 Tini 去代理运行 Node 程序，使得 Node 成为 Tini 的子进程。
```bash
# Add Tini
ENV TINI_VERSION v0.19.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini
ENTRYPOINT ["/tini", "--"]

# Run your program under Tini
CMD ["/your/program", "-and", "-its", "arguments"]
# or docker run your-image /your/program ...
```
通过仔细阅读 Tini 的代码，我判断核心的收尸技术就是这个 ***waitpid 函数*** 了，其实在僵尸进程的定义中就有了如何收尸，所以先了解基础概念是非常重要的。
```c
int reap_zombies(const pid_t child_pid, int* const child_exitcode_ptr) {
	pid_t current_pid;
	int current_status;

	while (1) {
		current_pid = waitpid(-1, &current_status, WNOHANG);

		switch (current_pid) {

			case -1:
				if (errno == ECHILD) {
					PRINT_TRACE("No child to wait");
					break;
				}
				PRINT_FATAL("Error while waiting for pids: '%s'", strerror(errno));
				return 1;

			case 0:
				PRINT_TRACE("No child to reap");
				break;

			default:
				/* A child was reaped. Check whether it's the main one. If it is, then
				 * set the exit_code, which will cause us to exit once we've reaped everyone else.
				 */
				PRINT_DEBUG("Reaped child with pid: '%i'", current_pid);
				if (current_pid == child_pid) {
					// ...
				} else if (warn_on_reap > 0) {
					PRINT_WARNING("Reaped zombie process with pid=%i", current_pid);
				}

				// Check if other childs have been reaped.
				continue;
		}

		/* If we make it here, that's because we did not continue in the switch case. */
		break;
	}

	return 0;
}
```
当然 Tini 作为父进程还有其他的优点，比如
1. 会把接收到的信号转发给其代理运行的子进程，代码实现可见 wait_and_forward_signal 函数
2. 代理运行的子进程异常退出后，它也会自动退出，代码实现可见 reap_zombies 函数

## 复现与定案
当我们学到核心的收尸技术后，就可以来揭发完整的案发现场了 ~

![image](https://user-images.githubusercontent.com/23253540/147583717-1f7591fa-e1fd-4a22-bf36-16792de6df31.png)


### 1. Docker 运行 node xxx.js
```bash
➜  ~ docker run -t -i -v /test/tini:/test 97f7595bf6c4 node /test/main.js
```
Tini 是一个 C 程序，这里先把 Tini 核心实现的代码复制过来，接着用 Node.js C++ 插件的方式来调用 C 这部分的代码

我们的 main.js 程序对外暴露了两个接口，来完成本次实验
* /make_zombie: 调用 make_zombie 函数产生一个僵尸进程
* /kill_zombie: 调用 kill_zombie 函数收掉一个僵尸进程
```js
// /test/mian.js

const http = require("http");
const { exec } = require("child_process");
const tini = require("./build/Release/addon.node");

const server = http.createServer((req, res) => {
  if (req.url === "/make_zombie") {
    console.log("make_zombie >>>");
    exec("node /test/make_zombie.js", () => {});
    res.end("hello");
  } else if (req.url === "/kill_zombie") {
    console.log("kill_zombie >>>");
    console.log(tini.kill_zombie());
    res.end("hello");
  }
});

server.listen(3000);
```

### 2. Node 程序的 PID 会是 1
✅ 可见 Node 成为了 PID 为 1 的进程
```bash
➜  ~ docker exec -it 83a67a46ec13 /bin/bash
[root@83a67a46ec13 /]# ps -ef
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 16:53 pts/0    00:00:00 node /test/main.js
root        14     0  1 16:53 pts/1    00:00:00 /bin/bash
root        28    14  0 16:53 pts/1    00:00:00 ps -ef
[root@83a67a46ec13 /]#
```

### 3. 制造一个僵尸
✅ 子进程调用 exit 退出，父进程不收尸，使其顺利成为一具僵尸
```bash
[root@83a67a46ec13 /]# curl localhost:3000/make_zombie
hello[root@83a67a46ec13 /]# ps -ef
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 16:53 pts/0    00:00:00 node /test/main.js
root        14     0  0 16:53 pts/1    00:00:00 /bin/bash
root        31     1  0 16:55 pts/0    00:00:00 node /test/make_zom
root        38    31  0 16:55 pts/0    00:00:00 [node] <defunct>
root        39    14  0 16:55 pts/1    00:00:00 ps -ef
[root@83a67a46ec13 /]#
```
产生僵尸的代码为
```c
napi_value make_zombie(napi_env env, napi_callback_info info)
{
    printf("pid %d\n", getpid());
    int child_pid = fork();
    if (child_pid == 0)
    {
        printf("-----in child process:  %d\n", getpid());
        exit(0);
    }
    else
    {
        sleep(1000000);
    }
    return NULL;
}
```
✅ 杀死僵尸进程的父进程，它就被 PID 为 1 的进程托管
```bash
[root@83a67a46ec13 /]# kill -9 31
[root@83a67a46ec13 /]# ps -ef
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 16:57 pts/0    00:00:00 node /test/main.js
root        14     0  0 16:57 pts/1    00:00:00 /bin/bash
root        38     1  0 16:59 pts/0    00:00:00 [node] <defunct>
root        40    14  0 17:07 pts/1    00:00:00 ps -ef
[root@83a67a46ec13 /]#
```

### 4. 收尸
✅  kill -9 杀不死僵尸进程, 符合预期
```bash
[root@83a67a46ec13 /]# kill -9 38
[root@83a67a46ec13 /]# ps -ef
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 16:57 pts/0    00:00:00 node /test/main.js
root        14     0  0 16:57 pts/1    00:00:00 /bin/bash
root        38     1  0 16:59 pts/0    00:00:00 [node] <defunct>
root        41    14  0 17:09 pts/1    00:00:00 ps -ef
```
✅ 使用 waitpid 函数去收尸，僵尸进程消失
```bash
[root@83a67a46ec13 /]# curl localhost:3000/kill_zombie
hello[root@83a67a46ec13ps -ef
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 16:57 pts/0    00:00:00 node /test/main.js
root        14     0  0 16:57 pts/1    00:00:00 /bin/bash
root        44    14  0 17:10 pts/1    00:00:00 ps -ef
```
真正收尸的代码为下面，并且通过 Node-api 把本次收尸进程的 id 和 status 返回给了 js 调用方。
```c
napi_value kill_zombie(napi_env env, napi_callback_info info)
{
    int current_pid;
    int current_status;
    napi_status status;

    current_pid = waitpid(-1, &current_status, WNOHANG);
    napi_value n_pid;
    napi_value n_status;
    status = napi_create_int32(env, current_pid, &n_pid);
    assert(status == napi_ok);

    status = napi_create_int32(env, current_status, &n_status);
    assert(status == napi_ok);

    napi_value obj;
    status = napi_create_object(env, &obj);
    assert(status == napi_ok);

    status = napi_set_named_property(env, obj, "pid", n_pid);
    assert(status == napi_ok);

    status = napi_set_named_property(env, obj, "status", n_status);
    assert(status == napi_ok);

    return obj;
}
```
可见通过我们逐步的复盘，一切也都验证了我们最初的猜想。

## 小结
其实僵尸进程的产生也是 puppeteer 程序的一个 bug, Node.js 不去处理也是情理之中，因为很难判断用户真正的行为，况且还要写一堆副作用的代码。

最后我们通过 docker --init 使用一个 init 进程去解决，这样进程间互相解藕，各司其职显得优雅一点。这也算践行了sidecar 模式吧 ~