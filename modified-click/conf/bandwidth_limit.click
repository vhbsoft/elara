//bandwidth_limit.click
//BandwidthShaper(RATE)  is bps so 48kbps -> 48000
FromDevice(eth0, PROMISC true)->Queue->BandwidthShaper(1000000)->ToDevice(eth1)
FromDevice(eth1, PROMISC true)->Queue->BandwidthShaper(1000000)->ToDevice(eth0)

