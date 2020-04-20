#include "UnitTest.h"
#include "CoinReleasePlan.h"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace crp;

void TestFirstPowBlock(){
	Height h = 1;
	CoinReleasePlan plan;
	BlockValue v = plan.GetBlockValue(h);
	assert(v.miner == 2350*WAN);
	assert(v.miner == v.Total());
}
void TestStablePowBlock(){
	Height begin = 2, back = 3000;
	CoinReleasePlan plan;
	BlockValue v = plan.GetBlockValue(begin);
	assert(v.miner == 170*COIN);
	assert(v.miner == v.Total());
}
void TestPow(){
	TestFirstPowBlock();
	TestStablePowBlock();
}
void TestPos(){
	CoinReleasePlan plan;
	Height begin = 3001, back = 1299000;
	assert(plan.pos.heightRange.Begin()==begin);
	assert(plan.pos.heightRange.Back()==back);
	assert(plan.InPosHeightRange(begin));
	assert(plan.InPosHeightRange(back));
	assert((back-begin+1)%43200 == 0);
	assert(plan.IsLastHeightOfSeason(back));
	BlockValue v1 = plan.GetBlockValue(begin);
	assert(v1.miner == 120000000);
	assert(v1.entrust == 660000000);
	assert(v1.masternode == 60000000);
	assert(v1.topFirst == 0);
	assert(v1.topSecond == 0);
	BlockValue v2 = plan.GetBlockValue(back);
	assert(v2.miner == 120000000);
	assert(v2.entrust == 660000000);
	assert(v2.masternode == 60000000);
	assert(v2.topFirst == 78000*COIN);
	assert(v2.topSecond == 78000*COIN);
	assert(plan.IsLastHeightOfSeason(begin+1*43200-1));
	assert(plan.IsLastHeightOfSeason(begin+2*43200-1));
	assert(plan.IsLastHeightOfSeason(begin+3*43200-1));
	int n = 1 + rand()%30;
	assert(1 <= n && n <= 30);
	Height randHeight =            begin+n*43200-1;
	assert(plan.IsLastHeightOfSeason(randHeight));
	assert(plan.IsLastHeightOfSeason(begin+30*43200-1));
	BlockValue v3 = plan.GetBlockValue(randHeight);
	assert(v3 == v2);
}
void TestEnd(){
	Height begin = 1299001;
	CoinReleasePlan plan;
	BlockValue v = plan.GetBlockValue(begin);
	assert(v.miner == 0);
	assert(v.miner == v.Total());
}
void TestPlan(){
	TestPow();
	TestPos();
	TestEnd();
}
void TestAgentNum1_27(){
	CoinAmount topFirst = 78000*COIN;
	CoinAmount topSecond = 78000*COIN;
	CoinAmount total= topFirst + topSecond;
	std::vector<std::pair<Agentid,BlockAmount>> blockCountVec;
	const int N = 1 + rand()%27;
	assert(1 <= N && N <= 27);
	for(int i=0; i<N; i++){
		blockCountVec.push_back(make_pair(i,100-i));
	}
	std::vector<std::pair<Agentid,CoinAmount>> distVec = GetMonthlyDistribution(blockCountVec,topFirst,topSecond);
	assert(distVec.size() == N);
	assert(distVec[0].first == 0 && distVec[0].second == total/N + total%N);
	for(int i=1; i<N; i++){
		assert(distVec[i].first == i && distVec[i].second == total/N);
	}

	CoinAmount totalReward = 0;
	//cout << "top" << "\t" << "agent" << "\t" << "blockCount" << "\t" << "reward" << endl;
	for(int i=0; i<distVec.size(); i++){
		totalReward += distVec[i].second;
		//cout << i+1 << "\t" << distVec[i].first << "\t" << blockCountVec[i].second << "\t" << distVec[i].second << endl;
	}
	assert(total == totalReward);
}
void TestAgentNum28_99(){
	CoinAmount topFirst = 78000*COIN;
	CoinAmount topSecond = 78000*COIN;
	std::vector<std::pair<Agentid,BlockAmount>> blockCountVec;
	const int N1 = 27;
	const int N2 = 1 + rand() % (99-N1);
	const int N = N1 + N2;
	assert(28 <= N && N <= 99);
	CoinAmount topSecondCountTotal = 0;
	for(int i=0; i<N; i++){
		blockCountVec.push_back(make_pair(i,100-i));
		if(i>=N1)
			topSecondCountTotal += blockCountVec[i].second;
	}
	std::vector<std::pair<Agentid,CoinAmount>> distVec = GetMonthlyDistribution(blockCountVec,topFirst,topSecond);
	assert(distVec.size() == N);
	CoinAmount topFirstTotal = 0, topSecondTotal = 0;
	for(int i=0; i<N; i++){
		if(i<N1)
			topFirstTotal += distVec[i].second;
		else
			topSecondTotal += distVec[i].second;
	}
	assert(topFirstTotal == topFirst);
	assert(topFirstTotal == topSecond);
	CoinAmount top29_99Total = 0;
	for(int i=1; i<N2; i++){
		CoinAmount amount = topSecond * (blockCountVec[N1+i].second * 1.0 / topSecondCountTotal);
		assert(distVec[N1+i].second == amount);
		top29_99Total += amount;
	}
	CoinAmount top28Amount = topSecond - top29_99Total;
	assert(distVec[N1].second == top28Amount);
}
void TestAgentNum100(){
	CoinAmount topFirst = 78000*COIN;
	CoinAmount topSecond = 78000*COIN;
	std::vector<std::pair<Agentid,BlockAmount>> blockCountVec;
	const int N = 100;
	for(int i=0; i<N; i++){
		blockCountVec.push_back(make_pair(i,100-i));
	}
	std::vector<std::pair<Agentid,CoinAmount>> distVec = GetMonthlyDistribution(blockCountVec,topFirst,topSecond);
	assert(distVec.size() == 99);

	CoinAmount topFirstTotal = 0, topSecondTotal = 0;
	//cout << "top" << "\t" << "agent" << "\t" << "blockCount" << "\t" << "reward" << endl;
	for(int i=0; i<distVec.size(); i++){
		if(i < 27)
			topFirstTotal += distVec[i].second;
		else
			topSecondTotal += distVec[i].second;
		//cout << i+1 << "\t" << distVec[i].first << "\t" << blockCountVec[i].second << "\t" << distVec[i].second << endl;
	}
	assert(topFirstTotal == topFirst);
	assert(topSecondTotal == topSecond);
}
void TestDistribution(){
	TestAgentNum1_27();
	TestAgentNum28_99();
	TestAgentNum100();
}
void TestSorter(){
	std::vector<std::pair<crp::Agentid,crp::BlockAmount>> blockCountVec;
	const int N = 20;
	for(int i=0; i<N; i++){
		BlockAmount amount = 1+rand()%100;
		blockCountVec.push_back(std::make_pair(i,amount));
		//cout << i << "," << amount << endl;
	}
	std::sort(blockCountVec.begin(),blockCountVec.end(),crp::LeftAmountGreater);
	for(int i=1; i<N; i++){
		//cout << i << "," << blockCountVec[i].second << endl;
		assert(blockCountVec[i].second <= blockCountVec[i-1].second);
	}
}
void crp::DoUnitTest(){
	TestPlan();
	TestDistribution();
	TestSorter();
}
