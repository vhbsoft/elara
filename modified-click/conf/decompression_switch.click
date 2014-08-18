// simple_decomp.click
// simple decompression switch

ipclass :: IPClassifier(dst udp port 0000, dst tcp port 0000, -);

out :: Queue(10000000)->
        //Print("->eth1", CONTENTS 'NONE')->
	ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
	//Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->cip :: CheckIPHeader(CHECKSUM false);
	class[1]->out;

	cip[0]->ipclass;
	cip[1]->Unstrip(14)->out;

	// Remove IP and UDP/TCP header
	ipclass[0]->Strip(20)->Strip(8)->udpdecomp :: Decompression;
	ipclass[1]->Strip(20)->Strip(20)->tcpdecomp :: Decompression;
	ipclass[2]->Unstrip(14)->
	        Print("do not decompress", CONTENTS 'NONE')->
		out;

	udpdecomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)->out;
	udpdecomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
	        //Print("decompress udp", CONTENTS 'NONE')->
		out;

	tcpdecomp[1]->Unstrip(20)->Unstrip(20)->Unstrip(14)->out;
	tcpdecomp[0]->Unstrip(20)->Unstrip(20)->Unstrip(14)->
	        Print("decompress tcp", CONTENTS 'NONE')->
		out;

FromDevice(eth1, PROMISC true)->
	//Print("eth1->", CONTENTS 'NONE')->
	Queue->
		//Print("->eth0", CONTENTS 'NONE')->
		ToDevice(eth0);

