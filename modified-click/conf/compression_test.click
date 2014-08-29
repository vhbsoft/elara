// compression_test.click
// test compression/decompression element functionality

ipclass :: IPClassifier(dst udp port 0000, dst tcp port 0000, -);

out :: Queue(10000000)->BandwidthShaper(1000000)->
    	Discard;

FromDevice(eth0, PROMISC true)->
        //Print("eth0->", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	// Remove Ethernet header
	class[0]->Strip(14)->cip :: CheckIPHeader(CHECKSUM false);
	class[1]->out;

	cip[0]->ipclass;
	cip[1]->Unstrip(14)->out;

	// Remove IP and UDP/TCP header
	ipclass[0]->Strip(20)->Strip(8)->udpcomp :: Compression;
	ipclass[1]->Strip(20)->Strip(20)->tcpcomp :: Compression;
	ipclass[2]->Unstrip(14)->
		Print("do not compress", CONTENTS 'NONE')->
		out;

	udpcomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)->out;
	udpcomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
		Print("compress udp", CONTENTS 'NONE')->
		Strip(14)->Strip(20)->Strip(8)->udpdecomp :: Decompression;
		
		udpdecomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)->out;
		udpdecomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
			Print("decompress udp", CONTENTS 'NONE')->
			out;

	tcpcomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)->out;
	tcpcomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
		Print("compress tcp", CONTENTS 'NONE')->
		Strip(14)->Strip(20)->Strip(20)->udpdecomp :: Decompression;

		tcpdecomp[1]->Unstrip(8)->Unstrip(20)->Unstrip(14)->out;
		tcpdecomp[0]->Unstrip(8)->Unstrip(20)->Unstrip(14)->
			Print("decompress udp", CONTENTS 'NONE')->
			out;
		