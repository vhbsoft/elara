//simple_switch.click
//simple switch

FromDevice(eth0, PROMISC true)->
//Print("eth0->eth1", CONTENTS 'NONE')->
				Queue(10000000)->ToDevice(eth1)
FromDevice(eth1, PROMISC true)->
//Print("eth1->eth0", CONTENTS 'NONE')->
				Queue(10000000)->ToDevice(eth0)

