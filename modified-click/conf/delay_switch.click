// delay_switch.click
// delay switch

out :: Queue(10000000)->BandwidthShaper(1000000)->
//Print("->eth1", CONTENTS 'NONE')->
		  ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
//Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(42/0000, 42/1111, -);

	class[2]->out;
	
	// microsecond precision
	class[0]->DelayShaper[0.000001]->out
	class[1]->DelayShaper[1.000000]->out

FromDevice(eth1, PROMISC true)->
//Print("eth1->", CONTENTS 'NONE')->
		  Queue->
		  //Print("->eth0", CONTENTS 'NONE')->
		  	   ToDevice(eth0)