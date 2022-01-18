#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

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

napi_value SayHello(napi_env env, napi_callback_info info)
{
    printf("Hello2\n");
    return NULL;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;

    napi_value fn;
    status = napi_create_function(env, NULL, 0, SayHello, NULL, &fn);

    napi_value fn2;
    status = napi_create_function(env, NULL, 0, make_zombie, NULL, &fn2);
    if (status != napi_ok)
        return NULL;

    napi_value fn3;
    status = napi_create_function(env, NULL, 0, kill_zombie, NULL, &fn3);
    if (status != napi_ok)
        return NULL;

    status = napi_set_named_property(env, exports, "kill_zombie", fn3);

    status = napi_set_named_property(env, exports, "make_zombie", fn2);

    status = napi_set_named_property(env, exports, "sayHello", fn);
    if (status != napi_ok)
        return NULL;

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)