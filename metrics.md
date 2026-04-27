# Metrics after every updates

## Init
+ Build  
```
RAM:   [=         ]  13.9% (used 45532 bytes from 327680 bytes)  
Flash: [=======   ]  65.7% (used 861285 bytes from 1310720 bytes)
```
+ File System 
```
LittleFS: 32768 / 1441792 bytes
```


## After adding OTA logic and UI
+ Build  
```
RAM:   [=         ]  14.0% (used 45724 bytes from 327680 bytes)
Flash: [=======   ]  66.2% (used 867077 bytes from 1310720 bytes)
```
+ File System 
```
LittleFS: 36864 / 1441792 bytes
```


## After minify Web UI
+ Build  
```
RAM:   [=         ]  14.0% (used 45724 bytes from 327680 bytes)
Flash: [=======   ]  66.2% (used 867077 bytes from 1310720 bytes)
```
+ File System 
```
LittleFS: 36864 / 1441792 bytes
```
Note: 
+ Nothing changed?
+ 36864 bytes -> 36 KB -> 9 blocks 
+ Files shrink, but Buddy Allocation return the same blocks?


## Write inline JS and delete app.js
+ Build  
```
RAM:   [=         ]  14.0% (used 45724 bytes from 327680 bytes)
Flash: [=======   ]  66.2% (used 867077 bytes from 1310720 bytes)
```
+ File System 
```
LittleFS: 32768 / 1441792 bytes
```
Note: 
+ File size reduced (back to initial size) --> Cross the block boundary
+ 32768 bytes -> 32 KB -> 8 blocks 

