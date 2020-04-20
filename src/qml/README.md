<h1>GKC正式版 2.3.6</h1>

<h4>您現在正在使用的是GKC正式版，本版本現在有一些已知BUG和部分細微功能缺失。如果您在使用中發現有任何未知Bug，或者對程式有其他建議，請聯繫開發人員，請不要在公開場合討論和評價BUG，謝謝！</h4>


<h4>開發者</h4>
<ul>
	<li>GKC核心研發團隊</li>
</ul>

<h2>更新日誌</h2>

<ul>
	<h5>vx.x.x</h5>
	<li>1 修復DPOS交易共識檢查問題</li>
	<li>2 錢包保存廣告資訊</li>
	<li>3 修改共識：從137105高度以後開始檢查贖回交易中的委託交易的鎖定狀態</li>

	<h5>v2.3.6</h5>
	<li>1 修復DPOS統計回滾問題</li>
	<li>2 修復普通交易可以將委託OUTPUT作為INPUT的共識檢查問題</li>

	<h5>v2.3.5</h5>
	<li>1 定期備份錢包檔</li>
	<li>2 擴展RPC介面支援智慧合約功能</li>
	<li>3 添加BBS廣告功能。廣告費永久銷毀。</li>
	<li>4 委託命令添加可選參數指定選為input的UTXO最小面值</li>
	<li>5 修改共識：高度331601(大約在2020年09月03日)以後委託數量下限減半(50 GKC)，權益節點抵押數量減半(1500 GKC)</li>
	<li>6 修改共識：排放計畫（塊獎勵、委託分紅等），每年減半</li>
	<li>7 修復getqueryexplorer命令返回結果中無法區分備註txout和合約txout的問題</li>
	<li>8 修復不能查詢創世塊coinbase交易的問題</li>
	<li>9 修復 listaccounts 命令查詢帳戶餘額時可能有負數的問題</li>

	<h5>v2.3.4</h5>
	<li>1 記憶體優化</li>
	<li>2 修改RPC命令'selectutxotosendmany'可指定找零地址</li>

	<h5>v2.3.3</h5>
	<li>1 修復塊流覽器上查看到coinstake交易中數量為0的OUTPUT問題（續）</li>

	<h5>v2.3.2</h5>
	<li>1 修復新塊收益地址問題</li>
	<li>2 修復塊流覽器上查看到coinstake交易中數量為0的OUTPUT問題</li>

	<h5>v2.3.1</h5>
	<li>1 修改指定礦工收益地址功能的開啟高度為74001</li>

	<h5>v2.3.0</h5>
	<li>1 通過設定檔指定新塊收益地址</li>
	<li>2 修改賽季獎勵機制，按賽季累計有效委託量、出塊數量、網路品質等因素加權分配</li>
	<li>3 添加72小時出塊統計</li>
	<li>4 添加節點賽季排名</li>
	<li>5 修復賽季資料統計相關BUG</li>

	<h5>v2.2.1</h5>
	<li>1 修復礦工挖不出塊的問題</li>

	<h5>v2.2.0</h5>
	<li>1 新增DPOS狀態顯示、調整首頁的顯示</li>
	<li>2 Help->About GKC Core頁面新增塊流覽器連結和下載連結</li>
	<li>3 修復coinstake交易的檢查BUG</li>
	<li>4 DPOS統計資料緩存優化</li>

	<h5>v2.1.13</h5>
	<li>1 修改RPC介面 getstakingstatus 查詢可用於挖礦的UTXO數量。</li>
	<li>2 修改RPC介面 getagent 查詢節點最近24小時出塊數量。</li>
	<li>3 修改RPC介面 getdposnameofblock 查詢節點ID和名稱。</li>
	<li>4 添加RPC介面 getcurrentseason 查詢賽季獎勵統計資料。</li>

	<h5>v2.1.12</h5>
	<li>1 修復賽季獎勵BUG 資料溢出</li>

	<h5>v2.1.11</h5>
	<li>1 修復同步卡塊問題，緩存多個高度的DPOS統計資料</li>
	<li>2 添加RPC命令 posminer，POS礦工挖礦動態開關</li>
	<li>3 交易白名單添加 1dc50daeda604027e781c1000c995adaae123fe22aeb6ff88812ce0ea4d6c732</li>
	<li>4 移除部分礦工調試日誌，添加或移除其他調試日誌</li>
	<li>5 添加啟動配置選項 banappnode=1/0 和 RPC banappnode (on/off mode)，拒絕APP節點連接開關</li>

	<h5>v2.1.10</h5>
	<li>1 添加調試日誌。通過配置 debug=miner 開啟。</li>
	<li>2 修復挖礦UTXO數量太少，不出塊的問題</li>

	<h5>v2.1.9</h5>
	<li>1 修復同步卡塊在33860高度的問題</li>
	<li>2 VIN數量限制512,交易尺寸限制200K</li>
	<li>3 允許192.168網段建網</li>
	<li>4 添加RPC selectutxotosendmany</li>
	<li>5 調試日誌</li>

	<h5>v2.1.8</h5>
	<li>1 添加交易白名單</li>

	<h5>v2.1.5</h5>
	<li>1 修復礦工打包錯誤</li>
	<li>2 添加block#33972調試日誌</li>

	<h5>v2.1.4</h5>
	<li>1 添加檢查點 33972</li>

	<h5>v2.1.3</h5>
	<li>1 添加調試日誌，配置 debug=pos 生效</li>
	<li>2 增加檢查點 31000, ..., 37050</li>

	<h5>v2.1.2</h5>
	<li>1 添加檢查點 1,10,100,1000,10000,20000,30000</li>

	<h5>v2.1.1</h5>
	<li>1 修復餘額顯示問題</li>
	<li>2 擴展rpc命令 getblockheader 和 listagents</li>

	<h5>v2.1.0</h5>
	<li>1 高度34600開始，修改礦工收益位址</li>
	<li>2 修改賽季獎勵為每31天發一次</li>
	<li>3 修復賽季獎勵分配演算法：安慰獎沒有從排放計畫中取的問題</li>
	<li>4 修改賽季獎勵分配演算法：按月委託量排序，按月委託量分配獎勵</li>
</ul>

