import time
from typing import *  # use type hints to make signatures clear

import snakefish


# the function that will be executed on a snakefish thread
def f() -> List[int]:
    print("f() will sleep for 500 ms")
    time.sleep(0.5)
    return [i for i in range(200)]  # return a large list


# spawn a snakefish thread
t = snakefish.Thread(f)
t.start()

# try to join the thread and time it
start = time.time()
while True:
    if t.try_join():
        print("try_join() successful")
        break
    else:
        print("try_join() unsuccessful, sleep for 100ms")
        time.sleep(0.1)
print("try_join() took %s sec" % (time.time() - start))

# check exit status and get the return value
assert (t.get_exit_status() == 0)
print("thread exit status:", t.get_exit_status())
print("result of f():", t.get_result())

# release resources
t.dispose()
