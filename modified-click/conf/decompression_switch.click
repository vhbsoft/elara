//simple_decomp.click
//simple decompression switch


out :: Queue(10000000)->
//Print("->eth1", CONTENTS 'NONE')->
	      ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
//Print("eth0->", CONTENTS 'NONE')->
				class :: Classifier(12/0800, -);
	class[1]->out;
	class[0]->Strip(14)->cip :: CheckIPHeader(CHECKSUM false);

	cip[1]->Unstrip(14)->out;
	cip[0]->ipclass :: IPClassifier(udp, -);

	ipclass[1]->Unstrip(14)->out;
	ipclass[0]->decomp :: Decompression;

	decomp[1]->Unstrip(14)->out;
	decomp[0]->Unstrip(14)->
//Print("decompress", CONTENTS 'NONE')->
				out;
		   //SetIPChecksum->udpsum :: SetUDPChecksum;

	//udpsum[0]->Unstrip(14)->out;
	//udpsum[1]->Unstrip(14)->out;

FromDevice(eth1, PROMISC true)->
//Print("eth1->", CONTENTS 'NONE')->
				Queue->
//Print("->eth0", CONTENTS 'NONE')->
				       ToDevice(eth0);

