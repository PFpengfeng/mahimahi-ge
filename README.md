# mahimahi with GE-channel mode
mahimahi: a web performance measurement toolkit

For origin mahimahi, mm-loss only support random loss packet mode. However, GE-channel is the more common packet loss mode. I build mm-ge-loss to support GE-channel.

Usage:
```shell
mm-ge-loss g2b_rate,b2g_rate,lr_good,lr_bad [command]
```

g2b_rate: the probability for good state to bad state

b2g_rate: the probability for bad state to good state

lr_good: packet loss rate in good state

lr_bad: packet loss rate in bad state

Currently, the loss is set to uplink only.