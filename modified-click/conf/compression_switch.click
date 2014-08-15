// simple_comp.click
// simple compression switch


out :: Queue(10000000)->BandwidthShaper(1000000)->
        //Print("->eth1", CONTENTS 'NONE')->
    	ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
        //Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->cip :: CheckIPHeader(CHECKSUM false);
	class[1]->out;

	cip[0]->ipclass :: IPClassifier(udp, tcp, -);
	cip[1]->Unstrip(14)->out;

	// Remove IP and UDP/TCP header
	ipclass[0]->Strip(20)->Strip(8)->udpcomp :: Compression;
	ipclass[1]->Strip(20)->Strip(20)->tcpcomp :: Compression;
	ipclass[2]->Unstrip(14)->out;

	udpcomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)-out;	
	udpcomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
		//Print("compress", CONTENTS 'NONE')->
	        out;

	tcpcomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)-out;
	tcpcomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
		//Print("compress", CONTENTS 'NONE')->
	        out;

FromDevice(eth1, PROMISC true)->
	//Print("eth1->", CONTENTS 'NONE')->
	Queue->
		//Print("->eth0", CONTENTS 'NONE')->
       		ToDevice(eth0);
