# Defense Guide — Codexion

## Questions about the project in general

**What is Codexion?**
It is a concurrency simulation written in C based on the Dining Philosophers problem. There are N coders sitting in a circle, each needing two USB dongles to compile, and there are only N dongles in total. The challenge is to coordinate access to the dongles without deadlocks or burnouts.

**What is a burnout?**
If a coder does not start compiling within `time_to_burnout` milliseconds since their last compilation or since the start of the simulation, they die. The simulation stops and "burned out" is printed.

**When does the simulation end?**
In two cases — when a coder burns out, or when all coders have compiled at least `number_of_compiles_required` times.

**Why is it a variant of the Dining Philosophers problem?**
Philosophers need two forks to eat, coders need two dongles to compile. The problem is the same — limited shared resources among multiple concurrent entities.

---

## Questions about threads

**What is a thread?**
An independent execution thread within the same process. It shares memory with other threads but has its own execution stack.

**How many threads does your program have?**
N coder threads + 1 monitor thread = N+1 threads in total.

**How do you create threads?**
With `pthread_create`. Each coder is a thread running `coder_routine`. The monitor runs `monitor_routine`.

**How do you wait for them to finish?**
With `pthread_join`. Without join, main could finish before the threads.

---

## Questions about mutex

**What is a mutex?**
A mutual exclusion lock. Only one thread can hold it at a time. The rest wait until it is released.

**How many mutexes do you have and what does each one protect?**
- `log_lock` → protects all printf calls
- `env->state_lock` → protects `simulation_over`
- `dongle->mutex` → protects each individual dongle
- `coder->state_lock` → protects `last_compile_time` and `compile_count`

**What happens if you don't use a mutex?**
Data race — two threads access the same memory simultaneously, the result is unpredictable or corrupted.

**What is a deadlock?**
When two threads mutually block each other waiting for a resource the other holds. For example, coder 1 holds the left dongle and waits for the right, coder 2 holds the right and waits for the left — both wait forever.

**How did you prevent deadlock?**
Coders with even IDs do a brief `usleep` at startup. This breaks the symmetry — not everyone tries to grab their left dongle at the same time, ensuring at least one coder can always make progress.

---

## Questions about condition variables

**What is `pthread_cond_wait`?**
Releases the mutex and puts the thread to sleep until another thread notifies it with signal or broadcast. When it wakes up, it automatically re-locks the mutex.

**Why do you use `while` instead of `if` with `cond_wait`?**
Because of spurious wakeups — the system can wake a thread for no reason. With `while` the thread always rechecks the condition when it wakes. With `if` it could continue even if the condition is not met.

**When do you use `cond_timedwait`?**
When the dongle is in cooldown. Instead of waiting indefinitely, the coder calculates exactly when the cooldown expires and sleeps until that precise moment.

**When do you use `broadcast` and when `signal`?**
`broadcast` — when releasing a dongle, because multiple coders may be waiting and all must recheck.
`signal` — would only wake one, which may not be the most prioritized.

---

## Questions about the scheduler

**What is FIFO?**
First In First Out — the dongle is given to the coder who requested it first. The priority is the arrival timestamp.

**What is EDF?**
Earliest Deadline First — the dongle is given to the coder with the nearest deadline, i.e., the one closest to burning out. The priority is `last_compile_time + time_to_burnout`.

**Where does the behavior change between FIFO and EDF?**
Only in `get_priority` — the value inserted as priority in the heap during push. Everything else is identical.

**Why is EDF better than FIFO?**
Because it prioritizes the most urgent coders, reducing burnouts. With FIFO a coder could be about to die but wait behind others who arrived earlier.

---

## Questions about the heap

**Why did you implement a heap?**
To efficiently manage each dongle's waiting queue. The heap always provides the highest priority element in O(1) without traversing the entire queue.

**What is a min-heap?**
A binary tree where the parent is always smaller than its children. The minimum is always at the root — index 0 of the array.

**How does `sift_up` work?**
After inserting a node at the end, it moves it up by comparing it with its parent and swapping if smaller, until reaching the root or finding a smaller parent.

**How does `sift_down` work?**
After extracting the root, it places the last element there and moves it down by comparing it with the smaller of its children, until it has no children or is smaller than both.

**What does each heap node store?**
`coder_id` — who the coder is, and `priority` — their ordering value (timestamp for FIFO, deadline for EDF).

---

## Questions about the monitor

**What does the monitor do?**
It is a separate thread that checks two conditions every 500 microseconds — whether any coder has exceeded `time_to_burnout`, and whether all coders have compiled enough.

**Why 500 microseconds?**
The subject requires burnout detection within ≤10ms. With `usleep(500)` the monitor checks every 0.5ms, giving a margin of 20 checks per 10ms window.

**How does the simulation stop cleanly?**
The monitor sets `simulation_over = 1` and broadcasts to all dongles. Coders blocked in `cond_wait` wake up, check `sim_is_over` and exit their loop.

---

## Questions about time

**What is `gettimeofday`?**
A system function that returns the current time in seconds (`tv_sec`) and microseconds (`tv_usec`) since January 1, 1970.

**Why do you convert to milliseconds?**
Because all program parameters are in milliseconds. It is simpler to work in a single unit.

**What is `get_timestamp`?**
Returns the milliseconds elapsed since the simulation started — `get_time_in_ms() - start_time`.

**Why do you need `ms_to_timespec`?**
`pthread_cond_timedwait` requires a `struct timespec` with absolute time in seconds and nanoseconds. `ms_to_timespec` converts the cooldown milliseconds to the format that `cond_timedwait` understands.

---

## Edge cases — possible trick questions

**What happens with 1 coder?**
With only one dongle they can never hold two, so they can never compile. The monitor detects the burnout and terminates.

**What happens if `dongle_cooldown` is 0?**
No cooldown period. The dongle is available immediately after being released. Works correctly.

**What happens if `number_of_compiles_required` is 0?**
The simulation should end immediately since all coders have already compiled 0 times. Verify that your `check_all_compiled` handles this correctly.

**Are there memory leaks?**
No — Valgrind confirms 0 leaks. All mutexes are destroyed in `cleanup`, all heaps are freed with `pqueue_free`, and the coder and dongle arrays are freed with `free`.