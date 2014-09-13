// ppp_decompression.click
// decompression switch, only compresses ip packets

out :: Queue(10000000)->
        //Print("->eth1", CONTENTS 'NONE')->
	ToDevice(eth1);

FromDevice(eth0, PROMISC true)->
	//Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->decomp :: Decompression;
	class[1]->
	        Print("do not decompress", CONTENTS 'NONE')->
		out;

	decomp[1]->Unstrip(14)->Print ("decompression failed!", CONTENTS 'NONE')->Discard;
	decomp[0]->Unstrip(14)->
		Print("decompress", CONTENTS 'NONE')->
		out;

FromDevice(eth1, PROMISC true)->
	//Print("eth1->", CONTENTS 'NONE')->
	Queue->
		//Print("->eth0", CONTENTS 'NONE')->
		ToDevice(eth0);

