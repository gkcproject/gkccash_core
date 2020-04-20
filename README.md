GKC Core integration/staging repository
=====================================

[![Build Status](https://travis-ci.org/GKC-Project/GKC.svg?branch=master)](https://travis-ci.org/GKC-Project/GKC) [![GitHub version](https://badge.fury.io/gh/GKC-Project%2FGKC.svg)](https://badge.fury.io/gh/GKC-Project%2FGKC)

GKC is a cutting edge cryptocurrency, with many features not available in most other cryptocurrencies.
- Anonymized transactions using coin mixing technology, we call it _Obfuscation_.
- Fast transactions featuring guaranteed zero confirmation transactions, we call it _SwiftTX_.
- Decentralized blockchain voting providing for consensus based advancement of the current Masternode
  technology used to secure the network and provide the above features, each Masternode is secured
  with a collateral of 10K GKC.

More information at [gkc.io](http://www.gkc.io) Visit our ANN thread at [BitcoinTalk](http://www.bitcointalk.org/index.php?topic=5080811)

### Download
#### Ubuntu 18.04
https://github.com/GKC-Project/GKC/releases/download/v2.0.0/gkc-qml-2.0.0-ubuntu18.04.zip
#### Windows 7,10 (64-bit)
https://github.com/GKC-Project/GKC/releases/download/v2.0.0/gkc-qml-2.0.0-win.zip
#### Android 4.4+
https://github.com/GKC-Project/GKC/releases/download/v2.0.0/app-release-1.0.apk
#### IOS
https://itunes.apple.com/cn/app/gkcwallet/id1441621155?l=zh&ls=1&mt=8


### Coin Specs
<table>
<tr><td>Algo</td><td>scrypt</td></tr>
<tr><td>Block Time</td><td>60 Seconds</td></tr>
<tr><td>Difficulty Retargeting</td><td>Every Block</td></tr>
<tr><td>Max Coin Supply (PoW Phase)</td><td>10,000,014,390 GKC</td></tr>
<tr><td>Max Coin Supply (PoS Phase)</td><td>Infinite</td></tr>
<tr><td>Premine</td><td>10,000,000,000 GKC</td></tr>
</table>

### Reward Distribution

<table>
<th colspan=4>Genesis Block</th>
<tr><th>Block Height</th><th>Reward Amount</th><th>Notes</th></tr>
<tr><td>1-400000</td><td>3,452,400,000 GKC</td><td>Pow Rewards</td></tr>
</table>

### PoW Rewards Breakdown

<table>
<th>Block Height</th><th>Reward</th><th>Masternodes</th><th>Miner</th><th>Budget</th>
<tr><td>1-80000</td><td>21920 GKC</td><td>50% (10960 GKC)</td><td>50% (10960 GKC)</td><td>N/A</td></tr>
<tr><td>80001-160000</td><td>10960 GKC</td><td>50% (5480 GKC)</td><td>50% (5480 GKC)</td><td>N/A</td></tr>
<tr><td>160001-240000</td><td>5480 GKC</td><td>50% (2740 GKC)</td><td>50% (2740 GKC)</td><td>N/A</td></tr>
<tr><td>240001-320000</td><td>2740 GKC</td><td>50% (1370 GKC)</td><td>50% (1370 GKC)</td><td>N/A</td></tr>
<tr><td>320001-400000</td><td>1370 GKC</td><td>50% (685 GKC)</td><td>50% (685 GKC)</td><td>N/A</td></tr>
</table>

### PoS + PoW Rewards Breakdown

<table>
<th>Phase</th><th>Block Height</th><th>Reward</th><th>Masternodes & Stakers</th><th>Miner</th><th>Budget</th>
<tr><td>Phase 0</td><td>400001-1976800</td><td>685 GKC</td><td>60% (411 GKC)</td><td>40% (274 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 1</td><td>1976801-3553600</td><td>570.83333333 GKC</td><td>60% (342.49999999 GKC)</td><td>40% (228.33333334 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 2</td><td>3553601-5130400</td><td>475.69444444 GKC</td><td>60% (285.41666666 GKC)</td><td>40% (190.27777778 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 3</td><td>5130401-6707200</td><td>396.41203703 GKC</td><td>60% (237.84722222 GKC)</td><td>40% (158.56481481 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 4</td><td>6707201-8284000</td><td>330.34336419 GKC</td><td>60% (198.20601851 GKC)</td><td>40% (132.13734568 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 5</td><td>8284001-9860800</td><td>275.28613683 GKC</td><td>60% (165.17168209 GKC)</td><td>40% (110.11445474 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 6</td><td>9860801-11437600</td><td>229.40511402 GKC</td><td>60% (137.64306841 GKC)</td><td>40% (91.76204561 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 7</td><td>11437601-13014400</td><td>191.17092835 GKC</td><td>60% (114.70255701 GKC)</td><td>40% (76.46837134 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 8</td><td>13014401-14591200</td><td>159.30910696 GKC</td><td>60% (95.58546417 GKC)</td><td>40% (63.72364279 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 9</td><td>14591201-16168000</td><td>132.75758913 GKC</td><td>60% (79.65455347 GKC)</td><td>40% (53.10303566 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 10</td><td>16168001-17744800</td><td>110.63132427 GKC</td><td>60% (66.37879456 GKC)</td><td>40% (44.25252971 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 11</td><td>17744801-19321600</td><td>92.19277023 GKC</td><td>60% (55.31566213 GKC)</td><td>40% (36.87710810 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 12</td><td>19321601-20898400</td><td>76.82730852 GKC</td><td>60% (46.09638511 GKC)</td><td>40% (30.73092341 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 13</td><td>20898401-22475200</td><td>64.02275710 GKC</td><td>60% (38.41365426 GKC)</td><td>40% (25.60910284 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 14</td><td>22475201-24052000</td><td>53.35229758 GKC</td><td>60% (32.01137854 GKC)</td><td>40% (21.34091904 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 15</td><td>24052001-25628800</td><td>44.46024799 GKC</td><td>60% (26.67614879 GKC)</td><td>40% (17.78409920 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 16</td><td>25628801-27205600</td><td>37.05020665 GKC</td><td>60% (22.23012399 GKC)</td><td>40% (14.82008266 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 17</td><td>27205601-28782400</td><td>30.87517221 GKC</td><td>60% (18.52510332 GKC)</td><td>40% (12.35006889 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 18</td><td>28782401-30359200</td><td>25.72931017 GKC</td><td>60% (15.43758610 GKC)</td><td>40% (10.29172407 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 19</td><td>30359201-31936000</td><td>21.44109181 GKC</td><td>60% (12.86465508 GKC)</td><td>40% (8.57643673 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 20</td><td>31936001-33512800</td><td>17.86757651 GKC</td><td>60% (10.72054590 GKC)</td><td>40% (7.14703061 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 21</td><td>33512801-35089600</td><td>14.88964709 GKC</td><td>60% (8.93378825 GKC)</td><td>40% (5.95585884 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 22</td><td>35089601-36666400</td><td>12.40803924 GKC</td><td>60% (7.44482354 GKC)</td><td>40% (4.96321570 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 23</td><td>36666401-38243200</td><td>10.34003270 GKC</td><td>60% (6.20401962 GKC)</td><td>40% (4.13601308 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 24</td><td>38243201-39820000</td><td>8.61669392 GKC</td><td>60% (5.17001635 GKC)</td><td>40% (3.44667757 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 25</td><td>39820001-41396800</td><td>7.18057826 GKC</td><td>60% (4.30834695 GKC)</td><td>40% (2.87223131 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 26</td><td>41396801-42973600</td><td>5.98381522 GKC</td><td>60% (3.59028913 GKC)</td><td>40% (2.39352609 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 27</td><td>42973601-44550400</td><td>4.98651268 GKC</td><td>60% (2.99190760 GKC)</td><td>40% (1.99460508 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 28</td><td>44550401-46127200</td><td>4.15542723 GKC</td><td>60% (2.49325633 GKC)</td><td>40% (1.66217090 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 29</td><td>46127201-47704000</td><td>3.46285603 GKC</td><td>60% (2.07771361 GKC)</td><td>40% (1.38514242 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 30</td><td>47704001-49280800</td><td>2.88571335 GKC</td><td>60% (1.73142801 GKC)</td><td>40% (1.15428534 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 31</td><td>49280801-50857600</td><td>2.40476113 GKC</td><td>60% (1.44285667 GKC)</td><td>40% (0.96190446 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 32</td><td>50857601-52434400</td><td>2.00396761 GKC</td><td>60% (1.20238056 GKC)</td><td>40% (0.80158705 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 33</td><td>52434401-54011200</td><td>1.66997300 GKC</td><td>60% (1.00198380 GKC)</td><td>40% (0.66798920 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 34</td><td>54011201-55588000</td><td>1.39164417 GKC</td><td>60% (0.83498650 GKC)</td><td>40% (0.55665767 GKC)</td><td>N/A</td></tr>
<tr><td>Phase 35</td><td>55588001-57164800</td><td>1.15970347 GKC</td><td>60% (0.69582208 GKC)</td><td>40% (0.46388139 GKC)</td><td>N/A</td></tr>
<tr><td>Phase X</td><td>57164801-Infinite</td><td>0 GKC</td><td>60% (0 GKC)</td><td>40% (0 GKC)</td><td>N/A</td></tr>
</table>
