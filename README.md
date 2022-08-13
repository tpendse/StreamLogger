<p align="center" style="font-size: 30px">
    << <strong>StreamLogger</strong> >>
</p>

<p align="center">
Use a logger as a stream to make logging easier
</p>


## Logger as a stream
The most annoying thing about loggers is text formatting. If you have data of different data types you need to use `sprintf`s or `stringstream`s convert to  `string`s.
This is even more annoying since modern C++ suggests you stream data to a file pointer anyways. 
Wouldn't it be nice to stream whatever you want, like `cout` & `cerr` to a logger instead? Where the logger would append timestamps and details to messages? 

This is where `StreamLogger` comes in . . .

***

### How do I use it?

Include the header & souce in your project then include `Logger.h`

```cpp
#include "Logger.h"
```

Create a new logger instance

```cpp
using namespace StreamLogger;
Logger logger;
```

By default, `Logger` will create a log file named `StreamLogger.log` in the executing directory. If you need the log file in a particular dirctory & name, pass it to the constructor

```cpp
const std::string logFilePath = "LogFile/Goes/Here.log"
Logger logger(logfilePath);
```

`StreamLogger` provides 3 types of log levels: Info, Warning and Error
Log what you need by invoking the appropriate method

```cpp
unsigned int count = 0;
logger.Info()  << "This is message #" << ++count << std::endl;
logger.Warn()  << "This is message #" << ++count << std::endl;
logger.Error() << "This is message #" << ++count << std::endl;
```

Which produces the output

```
-------------------------------------------------------
StreamLogger Log : Dated 2022-08-09 16:30:24
-------------------------------------------------------

[16:30:24] [INFO ] : This is message #1
[16:30:24] [WARN ] : This is message #2
[16:30:24] [ERROR] : This is message #3
```

</br>

### Disabling the logger

To avoid convoluted code to enable & disable logging, enable or disable logging entirely by

```cpp
logger.Info() << "This will be logged ... " << std::endl;
logger.SetEnabled(false);
logger.Error() << "This will not be logged!" << std::endl;
```

Which produces the output

```
-------------------------------------------------------
StreamLogger Log : Dated 2022-08-09 16:30:24
-------------------------------------------------------

[16:30:24] [INFO ] : This will be logged ... 
```

</br>

### Multiple sessions

On every logger construction, writing to existing log file appends logs

```cpp
/* Scope for First session */
{
    Logger loggerTheFirst;
    loggerTheFirst.Info() << "This is session #1 << std::endl;
}

/* Sleep for 30 s */

/* Scope for Second session */
{
    Logger loggerTheSecond;
    loggerTheSecond.Warn() << "This is the next session! << std::endl;
}
```

Which produces the output

```cpp
-------------------------------------------------------
StreamLogger Log : Dated 2022-08-09 16:30:24
-------------------------------------------------------

[16:30:24] [INFO ] : This is session #1

-------------------------------------------------------
StreamLogger Log : Dated 2022-08-09 16:30:54
-------------------------------------------------------

[16:30:54] [WARN ] : This is the next session! 
```

</br>

### Multithreading

Stream Logger is thread safe internally. However, (currently) logging does happen on the caller thread.
