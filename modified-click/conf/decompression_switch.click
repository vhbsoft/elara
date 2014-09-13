// decompression_switch.click
// decompresses everything after the transport header for specified destination ports

ipclass :: IPClassifier(dst udp port 0, dst tcp port 0, -);

out :: Queue(10000000)->
        //Print("->eth1", CONTENTS 'NONE')->
	ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
	//Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->MarkIPHeader()->ipclass;
	class[1]->out;

	ipclass[0]->udpdecomp :: UdpDecompression;
	ipclass[1]->tcpdecomp :: TcpDecompression;
	ipclass[2]->Unstrip(14)->
	        Print("do not decompress", CONTENTS 'NONE')->
		out;

	udpdecomp[1]->Unstrip(14)->Print ("udp decompression failed!", CONTENTS 'NONE')->Discard;
	udpdecomp[0]->Unstrip(14)->
		Print("decompress udp", CONTENTS 'NONE')->
		out;

	tcpdecomp[1]->Unstrip(14)->Print ("tcp decompression failed!", CONTENTS 'NONE')->Discard;
	tcpdecomp[0]->Unstrip(14)->
	        Print("decompress tcp", CONTENTS 'NONE')->
		out;

FromDevice(eth1, PROMISC true)->
	//Print("eth1->", CONTENTS 'NONE')->
	Queue->
		//Print("->eth0", CONTENTS 'NONE')->
		ToDevice(eth0);

