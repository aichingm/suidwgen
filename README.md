# SUIDWGEN

_setuid wrapper generator_

## Requirements

* gcc
* sudo
* openssl (optional for hash protection) 

## Installation

No installation needed, just checkout this repository!

## Generating a wrapper

To generate a wrapper run:

```shell
make warpper SCRIPT=super-secure.php INTERPRETER=/usr/bin/php
```

You can configure a bunch of checks done by the wrapper by passing flags to `make` : see the section below. After the first step you will have three new files. Named  `super-secure.php-root` ,     `super-secure.php-root-wrapper.c`  and  `super-secure.php-root-wrapper`.

Now you have to prepare the script you want to be run as privileged user, this can be done via `make`:

```shell
make secure SCRIPT=super-secure.php
```

_Note: you can change the owner and group of the script and wrapper by passing `USER=someuser` to `make`_ 

or manually:

```shell
chmod 700 [super-secure.php] #this will prevent normal users from running and changing the script
chown root:root [super-secure.php] # root or any other privileged user:group
```

__NOTE:__ it is super important that normal users are unable to change the script!

After this is done you have to configure the wrapper:

```shell
chmod 711 [super-secure.php-root-wrapper]
chmod a+s [super-secure.php-root-wrapper] # this will set the uid bit
chmod u+rwx [super-secure.php-root-wrapper]
chown root:root [super-secure.php-root-wrapper] # root or any other privileged user:group
```

Now you should be able to call `./super-secure.php-wrapper` with your unprivileged user and run your `[super-secure.php]` script as root or any other user. 

### Flags & Wrapper Checks

You can configure the wrapper by passing flags like this:

```shell
make wrapper CHECK_SCRIPT_NOT_WRITABLE=1
```

The flags are:

* CHECK_SCRIPT_NOT_WRITABLE `1` or `0`: Check if the script is not writable by any one but the owner.
*  CHECK_SCRIPT_NOT_READABLE `1` or `0` : Check if the script is not readable by any one but the owner.
* CHECK_SCRIPT_ONLY_USR_EXEC `1` or `0`: Check if the script is only executable by the owner.
* CHECK_SAME_OWN  `1` or `0`: Check if the script and the wrapper have the same owner.
* CHECK_SAME_GRP  `1` or `0`: Check if the script and the wrapper have the same group.
* CHECK_NOT_ROOT_U  `1` or `0`: Check if the owner is not root, this is disabled by default.
* CHECK_NOT_ROOT_G  `1` or `0`: Check if the group is not root, this is disabled by default.
* CHECK_HASH  `1` or `0`: Check if the hash of the file matches a precalculated hash.
* CHECK_MAX_ARGS `1` or `0`: Check the number of arguments passed to the script.
* MAX_ARGS  a number `0` to `1000`: This settings depends on __CHECK_MAX_ARGS__. Limit the number of arguments passed to the script to a positive integer. 
* UNSET_ENV `1` or `0`: Unset the environment for the script.
* __DEBUG__ `1` or `0`: This is disabled by default. Set it to 1 to get some error logging.
* __USER__ a `username` Change the user under which the wrapper will be executed. Default is `root`.

### Example 

Run:

```shell
make all SCRIPT="example/script.php" INTERPRETER=/usr/bin/php ; ./example/script.php-root-wrapper ";)"
```

## Wrapper Return Codes

* 0 Everything is cool

* 1: More then MAX_ARGS arguments are passed to the wrapper.

* 4: Failed to set the uid.

* 5: Failed to set the gid.

* 6: The script is writable by other users then the owner.

* 7: The script is readable by other users then the owner.

* 8: The script is executable by other users then the owner.

* 9: The owners of the script and the wrapper are differing.

* 10: The group of the script and the wrapper are differing.

* 11: The wrapper does not run as user root.

* 12: The wrapper does not run as group root.

* 13: Failed to hash the script.

* 14: The hash of the script and the stored hash in the wrapper are differing. 

* 15: Failed to open the script for reading.

* \> 100: Use this error range indicate errors thrown from the script. 

* \> 200

  ​

  ​





