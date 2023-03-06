# log_daemon

Builds the Tuya IoT core SDK and the daemon program files, creates program executable.
```
git clone https://github.com/zaiolul/log_daemon
cd log_daemon
make
```
The program executable is located in src directory of the project

To run:
```
LD_LIBRARY_PATH=./sdk/build/lib ./src/log_daemon -p [PRODUCT_ID] -d [DEVICE_ID] -s [DEVICE_SECRET]

or check usage:
LD_LIBRARY_PATH=./sdk/build/lib ./src/log_daemon --help

```

To remove all installed files:
```
make clean
```
