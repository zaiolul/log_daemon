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
LD_LIBRARY_PATH=../sdk/build/lib ./src/log_daemon [PRODUCT_ID] [DEVICE_ID] [DEVICE_SECRET]
```

To remove all installed files:
```
make clean
```
