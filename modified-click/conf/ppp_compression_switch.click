// ppp_compression.click
// compression switch, only compresses ip packets

out :: Queue(10000000)->BandwidthShaper(1000000)->
        //Print("->eth1", CONTENTS 'NONE')->
    	ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
        //Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->comp :: Compression;
	class[1]->
		Print("do not compress", CONTENTS 'NONE')->
		out;

	comp[1]->Unstrip(14)->Print("compression failed!", CONTENTS 'NONE')->Discard;
	comp[0]->Unstrip(14)->
		Print("compress", CONTENTS 'NONE')->
	        out;

FromDevice(eth1, PROMISC true)->
	//Print("eth1->", CONTENTS 'NONE')->
	Queue->
		//Print("->eth0", CONTENTS 'NONE')->
       		ToDevice(eth0);
