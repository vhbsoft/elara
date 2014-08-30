// compression_test.click
// test compression/decompression element functionality

FromDevice(eth0, PROMISC true)->
	//Print("\nNew packet\n", -1)->
	Print("New packet")->
	class1 :: Classifier(12/0800, -);
class1[0]->
	Strip(14)->
		//Print("Strip ethernet\n", -1)->
		Print("Strip ethernet")->
		MarkIPHeader()->ipclass1 :: IPClassifier(tcp, -);

ipclass1[0]->
	Strip(40)->
		Print("Strip IP/TCP\n", -1)->
		//Print("Strip IP/TCP")->
		tcpcomp :: Compression;
tcpcomp[0]->
	Print("Compress\n", -1)->
	//Print("Compress")->
	Unstrip(40)->
		//Print("Unstrip IP/TCP\n", -1)->
		Print("Unstrip IP/TCP")->
		Unstrip(14)->
			//Print("Unstrip ethernet\n", -1)->
			Print("Unstrip ethernet")->
			class2 :: Classifier(12/0800, -);
class2[0]->
	Strip(14)->
		//Print("Strip ethernet\n", -1)->
		Print("Strip ethernet")->
		MarkIPHeader()->ipclass2 :: IPClassifier(tcp, -);
ipclass2[0]->
	Strip(40)->
		Print("Strip IP/TCP\n", -1)->
		//Print("Strip IP/TCP")->
		tcpdecomp :: Decompression;
tcpdecomp[0]->
	Print("Decompress\n", -1)->
	//Print("Decompress")->
	Unstrip(40)->
		//Print("Unstrip IP/TCP\n", -1)->
		Print("Unstrip IP/TCP")->
		Unstrip(14)->
			//Print("Unstrip ethernet\n", -1)->
			Print("Unstrip ethernet")->
			Print("Discard", 0)->
			Discard;

class1[1]->Discard;
ipclass1[1]->
	//Print("ipclass1[1]->", 0)->	
	Discard;
tcpcomp[1]->
	Print("compression failed!", 0)->
	Discard;

class2[1]->Discard;
ipclass2[1]->Discard;
tcpdecomp[1]->
	Print("decompression failed!", 0)->
	Discard;
