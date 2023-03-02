# log_daemon

Builds the Tuya IoT core SDK and the program files, creates program executable.
```
git clone https://github.com/zaiolul/log_daemon
cd log_daemon
make
```
The program executable is located in src directory of the project

To run:
```
LD_LIBRARY_PATH=../../build/lib ./src/log_daemon [PRODUCT_ID] [DEVICE_ID] [DEVICE_SECRET]
```

To remove all installed files:
```
make clean
```

Removes only the SDK build files:
```
make clean-sdk
```

Removes the program executable:
```
make clean-src
```