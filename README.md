# arpdump

Usage:

```plain
usage: arpdump --interface=string [options] ... 
options:
  -i, --interface        interface to use (string)
  -s, --send             send mode
  -w, --sniff            sniff mode
  -a, --attack           attack mode
  -c, --netcut           use netcut in attack mode
  -r, --reply            send arp reply in send mode
  -D, --dest-ip          ip address of the device to send in send or attack mode (string [=])
  -X, --dest-mac         ether address of the device to send in send or attack mode (string [=])
  -S, --src-ip           sender ip in arp packet in send or attack mode (string [=])
  -C, --src-mac          sender ether addr in arp packet in send or attack mode (string [=])
  -T, --interval-time    inverval time to send packet in attack mode (int [=10])
  -?, --help             print this message
```
