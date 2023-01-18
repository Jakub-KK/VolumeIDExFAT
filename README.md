# VolumeIdExFAT
## A Windows command-line tool to set the serial number of an exFAT volume.

```
Usage: VolumeIdExFAT.exe [-nobanner] [-verbose] <drive>: <XXXX-XXXX>

  <drive>:     drive letter of the target volume, ":" must be included
  <XXXX-XXXX>  new serial number, X is hexadecimal digit
  -nobanner    optional, do not display the startup banner
  -verbose     optional, display a detailed progress information
```

This program will change the serial number of an exFAT volume.

Administrator privileges are not needed when operating on "ejectable" volumes (such as USB removable devices).

Before making any changes the exFAT filesystem boot records will be checked for consistency, if an invalid state is detected the program will abort (try using 'chkdsk' program to fix the volume).

## Releases
The latest version can be downloaded from the [releases](https://github.com/Jakub-KK/VolumeIDExFAT/releases/latest).

## :warning: WARNING :warning:
IMPROPER USE OF THIS SOFTWARE MAY RESULT IN DATA LOSS. DO NOT REMOVE USB DEVICES WHEN USING THIS SOFTWARE. DO NOT TURN OFF OR CUT THE POWER TO THE DEVICE HOSTING THE VOLUME. ALWAYS HAVE A BACKUP OF DATA ON THE VOLUME YOU OPERATE ON.

## Other operating systems

For Linux there is [exfatprogs](https://github.com/exfatprogs/exfatprogs) where the ```tune.exfat -I 0x12345678 /dev/sda1``` command will set the new serial number for an exFAT volume.

## Other file systems

Microsoft publishes [VolumeID](https://learn.microsoft.com/en-us/sysinternals/downloads/volumeid) utility which works on FAT and NTFS volumes.

## License

This software is freely distributable under the terms of the MIT
license. Please refer to the [LICENSE.txt](LICENSE.txt) file for additional details.

Copyright (C) 2023 Jakub Kopeć-Kawka (MIT License)
