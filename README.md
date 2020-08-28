GKC Core integration/staging repository
=====================================

[![Build Status](https://travis-ci.org/GKC-Project/GKC.svg?branch=master)](https://travis-ci.org/GKC-Project/GKC) [![GitHub version](https://badge.fury.io/gh/GKC-Project%2FGKC.svg)](https://badge.fury.io/gh/GKC-Project%2FGKC)

GKC is a cutting-edge cryptocurrency with many functions that most other cryptocurrencies do not have.

1. The POS function design with entrusted PO is adopted, and the number of DPOS nodes is not limited. Currently, there are more than 110 effective DPOS nodes;
2. Use sigma protocol for anonymous transactions;
3. Fast transactions with guaranteed zero confirmation transactions, which we call SwiftTX;
4. Decentralized blockchain voting provides consensus for the advanced consensus-based Masternode technology, which is used to protect the network and provide the above functions. Each Masternode has a 10K GKC mortgage;
5. Integrated EVM virtual, with Turing complete smart contract.

### Build
See depends/README.md


### Coin Specs
<table>
<tr><td>Algo</td><td>scrypt</td></tr>
<tr><td>Block Time</td><td>60 Seconds</td></tr>
<tr><td>Difficulty Retargeting</td><td>Every Block</td></tr>
<tr><td>PoW Coin Supply</td><td>23,509,830 GKC</td></tr>
<tr><td>PoS Coin Supply</td><td>8,900,500 GKC</td></tr>
<tr><td>Season Supply</td><td>3,681,600 GKC</td></tr>
<tr><td>Total Supply</td><td>36,091,930 GKC</td></tr>
</table>




### PoW Rewards Breakdown

<table>
<th>First Block</th>		<th>Last Block</th> 	<th>Miner            </th> 	<th>Total            </th>
<tr> <td>          1</td> 	<td>         1</td> 	<td>23000000.00000000</td> 	<td>23000000.00000000</td> </tr>
<tr> <td>          2</td> 	<td>      3000</td> 	<td>     170.00000000</td> 	<td>  509830.00000000</td> </tr>
<tr> <td>      Total</td> 	<td>          </td> 	<td>                 </td> 	<td>23509830.00000000</td> </tr>
</table>


### PoS Rewards Breakdown


<table>
<th>First_Block</th> 	<th>Last_Block</th> <th>Miner</th> 		<th>Masternodes</th>	<th>Entrust</th>		<th>Fund</th>			<th>Total</th> 				<th>Acc</th> 
<tr><td>    3001</td>	<td>250000	</td>	<td>1.20000000</td>	<td>0.60000000</td>		<td>6.60000000</td>		<td>0.00000000</td> 	<td>2074800.00000000</td> 	<td>2074800.00000000</td> 
<tr><td>  250001</td>	<td>538680	</td>	<td>1.20000000</td>	<td>0.48000000</td>		<td>6.60000000</td>		<td>0.35000000</td> 	<td>2390270.40000000</td> 	<td>4465070.40000000</td> 
<tr><td>  538681</td>	<td>1074360	</td>	<td>0.60000000</td>	<td>0.24000000</td>		<td>3.30000000</td>		<td>0.17500000</td> 	<td>2217715.20000000</td> 	<td>6682785.60000000</td> 
<tr><td> 1074361</td>	<td>1610040	</td>	<td>0.30000000</td>	<td>0.12000000</td>		<td>1.65000000</td>		<td>0.08750000</td> 	<td>1108857.60000000</td> 	<td>7791643.20000000</td> 
<tr><td> 1610041</td>	<td>2145720	</td>	<td>0.15000000</td>	<td>0.06000000</td>		<td>0.82500000</td>		<td>0.04375000</td> 	<td> 554428.80000000</td> 	<td>8346072.00000000</td> 
<tr><td> 2145721</td>	<td>2681400	</td>	<td>0.07500000</td>	<td>0.03000000</td>		<td>0.41250000</td>		<td>0.02187500</td> 	<td> 277214.40000000</td> 	<td>8623286.40000000</td> 
<tr><td> 2681401</td>	<td>3217080	</td> 	<td>0.03750000</td>	<td>0.01500000</td>		<td>0.20625000</td>		<td>0.01093750</td>		<td> 138607.20000000</td> 	<td>8761893.60000000</td> 
<tr><td> 3217081</td>	<td>3752760	</td> 	<td>0.01875000</td>	<td>0.00750000</td>		<td>0.10312500</td>		<td>0.00546875</td> 	<td>  69303.60000000</td> 	<td>8831197.20000000</td> 
<tr><td> 3752761</td>	<td>4288440	</td> 	<td>0.00937500</td>	<td>0.00375000</td>		<td>0.05156250</td>		<td>0.00273438</td> 	<td>  34651.80000000</td> 	<td>8865849.00000000</td> 
<tr><td> 4288441</td>	<td>4824120	</td> 	<td>0.00468750</td>	<td>0.00187500</td>		<td>0.02578125</td>		<td>0.00136719</td> 	<td>  17325.90000000</td> 	<td>8883174.90000000</td> 
<tr><td> 4824121</td>	<td>5359800	</td> 	<td>0.00234375</td>	<td>0.00093750</td>		<td>0.01289063</td>		<td>0.00068359</td> 	<td>   8662.95000000</td> 	<td>8891837.85000000</td> 
<tr><td> 5359801</td>	<td>5895480	</td> 	<td>0.00117188</td>	<td>0.00046875</td>		<td>0.00644531</td>		<td>0.00034180</td> 	<td>   4331.47500000</td> 	<td>8896169.32500000</td> 
<tr><td> 5895481</td>	<td>6431160	</td>	<td>0.00058594</td>	<td>0.00023438</td>		<td>0.00322266</td>		<td>0.00017090</td> 	<td>   2165.73750000</td> 	<td>8898335.06250000</td> 
<tr><td> 6431161</td>	<td>6966840	</td>	<td>0.00029297</td>	<td>0.00011719</td>		<td>0.00161133</td>		<td>0.00008545</td> 	<td>   1082.86875000</td> 	<td>8899417.93125000</td> 
<tr><td> 6966841</td>	<td>7502520	</td>	<td>0.00014648</td>	<td>0.00005859</td>		<td>0.00080566</td>		<td>0.00004272</td> 	<td>    541.43437500</td> 	<td>8899959.36562500</td> 
<tr><td> 7502521</td>	<td>8038200	</td>	<td>0.00007324</td>	<td>0.00002930</td>		<td>0.00040283</td>		<td>0.00002136</td> 	<td>    270.71718750</td> 	<td>8900230.08281250</td> 
<tr><td> 8038201</td>	<td>8573880	</td>	<td>0.00003662</td>	<td>0.00001465</td>		<td>0.00020142</td>		<td>0.00001068</td> 	<td>    135.35859375</td> 	<td>8900365.44140625</td> 
<tr><td> 8573881</td>	<td>9109560	</td>	<td>0.00001831</td>	<td>0.00000732</td>		<td>0.00010071</td>		<td>0.00000534</td> 	<td>     67.67929688</td>	<td>8900433.12070312</td> 
<tr><td> 9109561</td>	<td>9645240	</td>	<td>0.00000916</td>	<td>0.00000366</td>		<td>0.00005035</td>		<td>0.00000267</td> 	<td>     33.83964844</td> 	<td>8900466.96035156</td> 
<tr><td> 9645241</td>	<td>10180920</td>	<td>0.00000458</td>	<td>0.00000183</td>		<td>0.00002518</td>		<td>0.00000134</td> 	<td>     16.91982422</td>	<td>8900483.88017578</td> 
<tr><td>10180921</td>	<td>10716600</td>	<td>0.00000229</td>	<td>0.00000092</td>		<td>0.00001259</td>		<td>0.00000067</td> 	<td>      8.45991211</td>	<td>8900492.34008789</td> 
<tr><td>10716601</td>	<td>11252280</td>	<td>0.00000114</td>	<td>0.00000046</td>		<td>0.00000629</td>		<td>0.00000033</td> 	<td>      4.22995605</td> 	<td>8900496.57004394</td> 
<tr><td>11252281</td>	<td>11787960</td>	<td>0.00000057</td>	<td>0.00000023</td>		<td>0.00000315</td>		<td>0.00000017</td> 	<td>      2.11497803</td> 	<td>8900498.68502197</td> 
<tr><td>11787961</td>	<td>12323640</td>	<td>0.00000029</td>	<td>0.00000011</td>		<td>0.00000157</td>		<td>0.00000008</td> 	<td>      1.05748901</td> 	<td>8900499.74251098</td> 
<tr><td>12323641</td>	<td>12859320</td>	<td>0.00000014</td>	<td>0.00000006</td>		<td>0.00000079</td>		<td>0.00000004</td> 	<td>      0.52874451</td> 	<td>8900500.27125549</td> 
<tr><td>12859321</td>	<td>13395000</td>	<td>0.00000007</td>	<td>0.00000003</td>		<td>0.00000039</td>		<td>0.00000002</td> 	<td>      0.26437225</td> 	<td>8900500.53562775</td> 
<tr><td>13395001</td>	<td>13930680</td>	<td>0.00000004</td>	<td>0.00000001</td>		<td>0.00000020</td>		<td>0.00000001</td> 	<td>      0.13218613</td> 	<td>8900500.66781387</td> 
<tr><td>13930681</td>	<td>14466360</td>	<td>0.00000002</td>	<td>0.00000001</td>		<td>0.00000010</td>		<td>0.00000001</td> 	<td>      0.06609306</td> 	<td>8900500.73390693</td> 
<tr><td>14466361</td>	<td>15002040</td>	<td>0.00000001</td>	<td>0.00000000</td>		<td>0.00000005</td>		<td>0.00000000</td> 	<td>      0.03304653</td> 	<td>8900500.76695347</td> 
<tr><td> 15002041</td>	<td>INF		</td>	<td>0.00000000</td>	<td>0.00000000</td>		<td>0.00000002</td>		<td>0.00000000</td> 	<td>      0.00000000</td> 	<td>8900500.76695347</td> 
</table>

### Season Rewards Breakdown

<table>
<th>First Block</th>	<th>Last Block</th>		<th>Season</th>		<th>Reward</th>				<th>Total</th>				<th>Acc</th>
<tr><td>    3001</td> 	<td>  538680</td> 		<td> 6</td> 		<td>156000.00000000</td> 	<td>936000.00000000</td> 	<td> 936000.00000000</td> 
<tr><td>  250001</td> 	<td>  538680</td> 		<td> 6</td> 		<td>145600.00000000</td> 	<td>873600.00000000</td> 	<td>1809600.00000000</td> 
<tr><td>  538681</td> 	<td>1 074360</td> 		<td>12</td> 		<td> 78000.00000000</td> 	<td>936000.00000000</td> 	<td>2745600.00000000</td> 
<tr><td> 1074361</td> 	<td> 1610040</td> 		<td>12</td> 		<td> 39000.00000000</td> 	<td>468000.00000000</td> 	<td>3213600.00000000</td> 
<tr><td> 1610041</td> 	<td> 2145720</td> 		<td>12</td> 		<td> 19500.00000000</td> 	<td>234000.00000000</td> 	<td>3447600.00000000</td> 
<tr><td> 2145721</td> 	<td> 2681400</td> 		<td>12</td> 		<td>  9750.00000000</td> 	<td>117000.00000000</td> 	<td>3564600.00000000</td> 
<tr><td> 2681401</td> 	<td> 3217080</td> 		<td>12</td> 		<td>  4875.00000000</td> 	<td> 58500.00000000</td> 	<td>3623100.00000000</td> 
<tr><td> 3217081</td> 	<td> 3752760</td> 		<td>12</td> 		<td>  2437.50000000</td> 	<td> 29250.00000000</td> 	<td>3652350.00000000</td> 
<tr><td> 3752761</td> 	<td> 4288440</td> 		<td>12</td> 		<td>  1218.75000000</td> 	<td> 14625.00000000</td> 	<td>3666975.00000000</td> 
<tr><td> 4288441</td> 	<td> 4824120</td> 		<td>12</td> 		<td>   609.37500000</td> 	<td>  7312.50000000</td> 	<td>3674287.50000000</td> 
<tr><td> 4824121</td> 	<td> 5359800</td> 		<td>12</td> 		<td>   304.68750000</td> 	<td>  3656.25000000</td> 	<td>3677943.75000000</td> 
<tr><td> 5359801</td> 	<td> 5895480</td> 		<td>12</td> 		<td>   152.34375000</td> 	<td>  1828.12500000</td> 	<td>3679771.87500000</td> 
<tr><td> 5895481</td> 	<td> 6431160</td> 		<td>12</td> 		<td>    76.17187500</td> 	<td>   914.06250000</td> 	<td>3680685.93750000</td> 
<tr><td> 6431161</td> 	<td> 6966840</td> 		<td>12</td> 		<td>    38.08593750</td> 	<td>   457.03125000</td> 	<td>3681142.96875000</td> 
<tr><td> 6966841</td> 	<td> 7502520</td> 		<td>12</td> 		<td>    19.04296875</td> 	<td>   228.51562500</td> 	<td>3681371.48437500</td> 
<tr><td> 7502521</td> 	<td> 8038200</td> 		<td>12</td> 		<td>     9.52148438</td> 	<td>   114.25781250</td> 	<td>3681485.74218750</td> 
<tr><td> 8038201</td> 	<td> 8573880</td> 		<td>12</td> 		<td>     4.76074219</td> 	<td>    57.12890625</td> 	<td>3681542.87109375</td> 
<tr><td> 8573881</td> 	<td> 9109560</td> 		<td>12</td> 		<td>     2.38037109</td> 	<td>    28.56445313</td> 	<td>3681571.43554687</td> 
<tr><td> 9109561</td> 	<td> 9645240</td> 		<td>12</td> 		<td>     1.19018555</td> 	<td>    14.28222656</td> 	<td>3681585.71777344</td> 
<tr><td> 9645241</td> 	<td>10180920</td> 		<td>12</td> 		<td>     0.59509277</td> 	<td>     7.14111328</td> 	<td>3681592.85888672</td> 
<tr><td>10180921</td> 	<td>10716600</td> 		<td>12</td> 		<td>     0.29754639</td> 	<td>     3.57055664</td> 	<td>3681596.42944336</td> 
<tr><td>10716601</td> 	<td>11252280</td> 		<td>12</td> 		<td>     0.14877319</td> 	<td>     1.78527832</td> 	<td>3681598.21472168</td> 
<tr><td>11252281</td> 	<td>11787960</td> 		<td>12</td> 		<td>     0.07438660</td> 	<td>     0.89263916</td> 	<td>3681599.10736084</td> 
<tr><td>11787961</td> 	<td>12323640</td> 		<td>12</td> 		<td>     0.03719330</td> 	<td>     0.44631958</td> 	<td>3681599.55368042</td> 
<tr><td>12323641</td> 	<td>12859320</td> 		<td>12</td> 		<td>     0.01859665</td> 	<td>     0.22315979</td> 	<td>3681599.77684021</td> 
<tr><td>12859321</td> 	<td>13395000</td> 		<td>12</td> 		<td>     0.00929832</td> 	<td>     0.11157990</td> 	<td>3681599.88842010</td> 
<tr><td>13395001</td> 	<td>13930680</td> 		<td>12</td> 		<td>     0.00464916</td> 	<td>     0.05578995</td> 	<td>3681599.94421005</td> 
<tr><td>13930681</td> 	<td>14466360</td> 		<td>12</td> 		<td>     0.00232458</td> 	<td>     0.02789497</td> 	<td>3681599.97210503</td> 
<tr><td>14466361</td> 	<td>15002040</td> 		<td>12</td> 		<td>     0.00116229</td> 	<td>     0.01394749</td> 	<td>3681599.98605251</td> 
<tr><td>15002041</td> 	<td>15537720</td> 		<td>12</td> 		<td>     0.00058115</td> 	<td>     0.00697374</td> 	<td>3681599.99302626</td> 
<tr><td>15537721</td> 	<td>16073400</td> 		<td>12</td> 		<td>     0.00029057</td> 	<td>     0.00348687</td> 	<td>3681599.99651313</td> 
<tr><td>16073401</td> 	<td>16609080</td> 		<td>12</td> 		<td>     0.00014529</td> 	<td>     0.00174344</td> 	<td>3681599.99825656</td> 
<tr><td>16609081</td> 	<td>17144760</td> 		<td>12</td> 		<td>     0.00007264</td> 	<td>     0.00087172</td> 	<td>3681599.99912828</td> 
<tr><td>17144761</td> 	<td>17680440</td> 		<td>12</td> 		<td>     0.00003632</td> 	<td>     0.00043586</td> 	<td>3681599.99956414</td> 
<tr><td>17680441</td> 	<td>18216120</td> 		<td>12</td> 		<td>     0.00001816</td> 	<td>     0.00021793</td> 	<td>3681599.99978207</td> 
<tr><td>18216121</td> 	<td>18751800</td> 		<td>12</td> 		<td>     0.00000908</td> 	<td>     0.00010896</td> 	<td>3681599.99989104</td> 
<tr><td>18751801</td> 	<td>19287480</td> 		<td>12</td> 		<td>     0.00000454</td> 	<td>     0.00005448</td> 	<td>3681599.99994552</td> 
<tr><td>19287481</td> 	<td>19823160</td> 		<td>12</td> 		<td>     0.00000227</td> 	<td>     0.00002724</td> 	<td>3681599.99997276</td> 
<tr><td>19823161</td> 	<td>20358840</td> 		<td>12</td> 		<td>     0.00000114</td> 	<td>     0.00001362</td> 	<td>3681599.99998638</td> 
<tr><td>20358841</td> 	<td>20894520</td> 		<td>12</td> 		<td>     0.00000057</td> 	<td>     0.00000681</td> 	<td>3681599.99999319</td> 
<tr><td>21430201</td> 	<td>21965880</td> 		<td>12</td> 		<td>     0.00000014</td> 	<td>     0.00000170</td> 	<td>3681599.99999830</td> 
<tr><td>21965881</td> 	<td>22501560</td> 		<td>12</td> 		<td>     0.00000007</td> 	<td>     0.00000085</td> 	<td>3681599.99999915</td> 
<tr><td>22501561</td> 	<td>23037240</td> 		<td>12</td> 		<td>     0.00000004</td> 	<td>     0.00000043</td> 	<td>3681599.99999957</td> 
<tr><td>23037241</td> 	<td>23572920</td> 		<td>12</td> 		<td>     0.00000002</td> 	<td>     0.00000021</td> 	<td>3681599.99999979</td> 
<tr><td>24108601</td> 	<td>     INF</td> 		<td>12</td> 		<td>     0.00000000</td> 	<td>     0.00000000</td> 	<td>3681599.99999989</td> 
</table>
