<h1>GKC Release Version 2.3.6</h1>

<h4>You are now using release version of GKC. There are several missing functions and bugs already known. If you found any new bugs，or you have any advice, feel free to contact us. Please DO NOT discuss in public. Thanks！</h4>


<h4>Developers</h4>
<ul>
	<li>GKC Developers</li>
</ul>

<h1>Change Log</h1>

<ul>
	<h5> vx.x.x </ h5>
	<li> 1 Fix DPOS transaction consensus check issue </li>
	<li> 2 Wallet saves advertising information </li>
	<li> 3 Consensus modification: From the height of 137105, the locked state of the entrusted transaction in the redemption transaction will be checked </li>

	<h5> v2.3.6</h5>
	<li> 1 Fix DPOS statistical rollback issue </li>
	<li> 2 Fix the consensus check problem that ordinary transactions can use OUTPUT of entrust tx as it's INPUT </li>

	<h5>v2.3.5</h5>
	<li> 1 Regularly back up wallet files </li>
	<li> 2 Extended RPC interface to support smart contract functions </li>
	<li> 3 Add BBS advertising function. Advertising fees are permanently destroyed. </li>
	<li> 4 The commission command adds optional parameters to specify the minimum face value of UTXO selected as input </li>
	<li> 5 Consensus revision: height 331601 (approximately on September 3, 2020), the lower limit of the number of commissions will be halved (50 GKC), and the number of equity node mortgages will be halved (1500 GKC) </li>
	<li> 6 Revise consensus: Emission plan (block rewards, commission dividends, etc.), halved every year </li>
	<li> 7 Fix the problem that the remark txout and contract txout cannot be distinguished in the result returned by the getqueryexplorer command </li>
	<li> 8 Fix the problem that the coinbase transaction of the genesis block cannot be queried </li>
	<li> 9 Fix the problem that the listaccounts command may have negative numbers when querying account balances </li>

	<h5>v2.3.4</h5>
	<li> 1 Memory optimization</li>
	<li> 2 Modify the RPC command 'selectutxotosendmany' to specify the change address</li>
	
	<h5>v2.3.3</h5>
	<li> 1 Fix the issue of 0 OUTPUT in coinstake transactions viewed on the blockchain explorer (continued)</li>

	<h5>v2.3.2</h5>
	<li> 1 Fix the issue of new block revenue addresses </li>
	<li> 2 Fix the issue of 0 OUTPUT in coinstake transactions viewed on the blockchain explorer </li>

	<h5>v2.3.1</h5>
	<li>1 Modify the opening height of the specified miner income address function to 74001</li>

	<h5>v2.3.0</h5>
	<li> 1 Specify the new block revenue address through the configuration file </li>
	<li> 2 Modify the season reward mechanism, weighted distribution according to factors such as the cumulative effective commission quantity, block production quantity, network quality and other factors of the season </li>
	<li> 3 Add 72-hour block statistics </li>
	<li> 4 Add node season rankings </li>
	<li> 5 Fix bugs related to season statistics </li>

	<h5>v2.2.1</h5>
	<li>1 Fixed miner error</li>

	<h5>v2.2.0</h5>
	<li>1 Add dpos status display, Adjustment display of index page.</li>
	<li>2 Add url of explorer and download page.</li>
	<li>3 Fix bug of coinstake.</li>
	<li>4 Optimize DPOS statistics data cache.</li>

	<h5>v2.1.13</h5>
	<li>Modify and add RPCs of Season Reward:</li>
	<li>1 Add 'stakablecoins' in result of rpc getstakingstatus</li>
	<li>2 Add 'lastest_24_hour_block_num' field in result of rpc getagent</li>
	<li>3 Add 'name' and 'nodeid' field in result of rpc getdposnameofblock</li>
	<li>4 Add rpc getcurrentseason for season reward</li>

	<h5>v2.1.12</h5>
	<li>1 fix bug of Season Reward
</ul>
