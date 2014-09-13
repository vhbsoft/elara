// compression_switch.click
// Compresses everything after the transport headers for the specified destination ports 

ipclass :: IPClassifier(dst udp port 0, dst tcp port 0, -);

out :: Queue(10000000)->BandwidthShaper(1000000)->
        //Print("->eth1", CONTENTS 'NONE')->
    	ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
        //Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->MarkIPHeader()->ipclass;
	class[1]->out;

	ipclass[0]->udpcomp :: UdpCompression;
	ipclass[1]->tcpcomp :: TcpCompression;
	ipclass[2]->Unstrip(14)->
		Print("do not compress", CONTENTS 'NONE')->
		out;

	udpcomp[1]->Unstrip(14)->Print ("udp compression failed!", CONTENTS 'NONE')->Discard;
	udpcomp[0]->Unstrip(14)->
		Print("compress udp", CONTENTS 'NONE')->
	        out;

	tcpcomp[1]->Unstrip(14)->Print ("tcp compression failed!", CONTENTS 'NONE')->Discard;
	tcpcomp[0]->Unstrip(14)->
		Print("compress tcp", CONTENTS 'NONE')->
	        out;

FromDevice(eth1, PROMISC true)->
	//Print("eth1->", CONTENTS 'NONE')->
	Queue->
		//Print("->eth0", CONTENTS 'NONE')->
       		ToDevice(eth0);
