//simple_comp.click
//simple compression switch

//out1 :: Unstrip(14)->Queue(1000000)->BandwidthShaper(1000000)->ToDevice(eth1)

loopout :: Unstrip(14)->Queue(100)->BandwidthShaper(1000000)->ToDevice(lo)
FromDevice(eth0, PROMISC true)->
	class1 :: Classifier(12/0800, -);

	class1[1]->Strip(14)->loopout;
	class1[0]->Strip(14)->cip1 :: CheckIPHeader(CHECKSUM false);

	cip1[1]->loopout;
	cip1[0]->ipclass1 :: IPClassifier(udp, -);

	ipclass1[1]->loopout;
	ipclass1[0]->comp :: Compression;

	comp[1]->loopout;
	comp[0]->SetIPChecksum->IPPrint(CONTENTS 'NONE')->
		udpsum1 :: SetUDPChecksum;

	udpsum1[1]->loopout;
	udpsum1[0]->loopout;

out :: Unstrip(14)->Queue->ToDevice(eth1)
FromDevice(lo, PROMISC true)->
	class2 :: Classifier(12/0800, -);

	class2[1]->Strip(14)->out;
	class2[0]->Strip(14)->cip2 :: CheckIPHeader(CHECKSUM false);

	cip2[1]->out;
	cip2[0]->ipclass2 :: IPClassifier(udp, -);

	ipclass2[1]->out;
	ipclass2[0]->decomp :: Decompression;

	decomp[1]->out;
	decomp[0]->
		StoreIPAddress(131.179.192.56, dst)->
	SetIPChecksum->IPPrint(CONTENTS 'NONE')->udpsum2 :: SetUDPChecksum;

	udpsum2[1]->out;
	udpsum2[0]->out;

//FromDevice(eth1, PROMISC true)->Queue->ToDevice(eth0);

/*
out1 :: Unstrip(14)->Queue->Print("ToDevice:", CONTENTS 'NONE')->ToDevice(eth1)
FromDevice(eth0, PROMISC true)->Print("FromDevice:", CONTENTS 'NONE')->
	class :: Classifier(12/0800, -);

	class[1]->Strip(14)->out1;
	class[0]->Strip(14)->cip :: CheckIPHeader(CHECKSUM false);

	cip[1]->out1;
	cip[0]->ipclass :: IPClassifier(udp, -);

	ipclass[1]->out1;
	ipclass[0]->IPPrint(MAXLENGTH -1, CONTENTS 'HEX')->Compression->IPPrint(MAXLENGTH -1, CONTENTS 'HEX')->out1;
FromDevice(eth1, PROMISC true)->Queue->ToDevice(eth0);
*/

