// compression_test.click
// test compression/decompression element functionality

FromDevice(eth0, PROMISC true)->
	//Print("New packet")->
	//Print("\nNew packet\n", -1)->
	class1 :: Classifier(12/0800, -);
class1[0]->
	Strip(14)->
		//Print("Strip ethernet")->
		//Print("Strip ethernet\n", -1)->
		comp :: Compression;
comp[0]->
	Print("Compress")->
	//Print("Compress\n", -1)->
	Unstrip(14)->
		//Print("Unstrip ethernet")->
		//Print("Unstrip ethernet\n", -1)->
		class2 :: Classifier(12/0800, -);
class2[0]->
	Strip(14)->
		//Print("Strip ethernet")->
		//Print("Strip ethernet\n", -1)->
		decomp :: Decompression;
decomp[0]->
	Print("Decompress")->
	//Print("Decompress\n", -1)->
	Unstrip(14)->
		//Print("Unstrip ethernet")->
		//Print("Unstrip ethernet\n", -1)->
		Print("Discard", 0)->
		Discard;

class1[1]->Discard;
comp[1]->
	Print("compression failed!", 0)->
	Discard;

class2[1]->Discard;
decomp[1]->
	Print("decompression failed!", 0)->
	Discard;
