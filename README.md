# Nohup Manager

This is an app that allows you to easily manage nohup processes that are currently running.

You can add, list, and kill processes that were run with nohup.

It tries to find child processes and kill them first, bubbling up to the root process if it is still running.

## How to use

Run with the `-h` argument to see the help menu.

```
--help (-h)     : Print the help menu
--kill (-k) {PID} : Pass a PID to kill child processes and then the root process
--add (-a) {PID}  : Pass a PID to add the corresponding process to the list
--name (-n) {name}: Used with the --add flag to add a name or description to a process
--list (-l)      : Print the list of active added processes (inactive processes are removed from the list automatically)
--remove-record (-rr) {PID} : Remove a process record from the list
```

### Examples

To add a process:

```sh
nhman -a 23412
```

To add a process with a name:

```sh
nhman -a 23412 -n "Some name"
```

To list processes (inactive processes are dropped from the list automatically):

```sh
nhman -l
```

To kill a process:

```sh
nhman -k 23412
```

To remove an active process from the list:

```sh
nhman -rr 23412
```

## How to build

Prerequisites: `gcc` and `make`
Create the release build (the executable will be placed at `./build/release/nhman` and `./nhman`):

```sh
make all
```

Create the debug build (the executable will be placed at `./build/debug/nhman_debug` and `./nhman_debug`):

```sh
make debug
```

To remove all build files:

```sh
To remove all build files:
```
