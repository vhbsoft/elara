//tcp_decompression_switch.click
//tcp decompression switch

out :: Queue(10000000)->
//Print("->eth1", CONTENTS 'NONE')->
	      ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
//Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	class[1]->out;
	class[0]->Strip(14)->cip :: CheckIPHeader(CHECKSUM false);

	cip[1]->Unstrip(14)->out;
	cip[0]->ipclass :: IPClassifier(tcp, -);

	ipclass[1]->Unstrip(14)->out;
	ipclass[0]->decomp :: TcpDecompression;

	decomp[1]->Unstrip(14)->out;
	decomp[0]->Unstrip(14)->
	//Print("decompress", CONTENTS 'NONE')->
				out;

FromDevice(eth1, PROMISC true)->
//Print("eth1->", CONTENTS 'NONE')->
	Queue->
	//Print("->eth0", CONTENTS 'NONE')->
		ToDevice(eth0);

