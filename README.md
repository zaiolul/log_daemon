# log_daemon

Builds the Tuya IoT core SDK and the program files, creates program binary.
```
git clone https://github.com/zaiolul/log_daemon
cd log_daemon
make
```

To run:
```
LD_LIBRARY_PATH=../../build/lib ./log_daemon [PRODUCT_ID] [DEVICE_ID] [DEVICE_SECRET]
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