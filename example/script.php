<?php

echo "Hello from user: " . posix_getpwuid(posix_getuid())['name'] . " " . $argv[1] ?? "", "\n";
return 0;
