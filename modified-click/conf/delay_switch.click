// delay_switch.click
// delay switch

out :: Queue(100)->BandwidthShaper(5000000)->
//Print("->eth1", CONTENTS 'NONE')->
		  ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
//Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(50/0000, 50/1111, -); // 50 is offset to skip headers
	
	class[2]->out;
	
	// microsecond precision
	class[0]->DelayShaper[0.000001]->out // 0.000001 is 1 microsecond
	class[1]->DelayShaper[0.001000]->out // 0.001 is 1 milisecond

FromDevice(eth1, PROMISC true)->
//Print("eth1->", CONTENTS 'NONE')->
		  Queue->
		  //Print("->eth0", CONTENTS 'NONE')->
		  	   ToDevice(eth0)