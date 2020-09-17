#include <cstdio>
#include <map>

using namespace std;

const int TEST_HEIGHT = 100;
const int OFFSET = 10;
const int MIN_REWARD = 5;

class UserStatistics
{
public:
	int joinHeight,entrustAmount,dealDivide,dividend;
	
	void init()
	{
		joinHeight = entrustAmount = dealDivide = dividend = 0;
	}
	
	UserStatistics()
	{
		init();
	}
	
	void Entrust(int height, int amount)
	{
		joinHeight = height, entrustAmount = amount;
	}
	
	void Deprive()
	{
		entrustAmount = 0;
	}

	bool IsActive() const
	{
		return entrustAmount > 0 || dealDivide - dividend >= MIN_REWARD;
	}
};

class Entrustment
{
public:
	map<int,UserStatistics> userEntrusts;

	int ExpectedReward(int height) const
	{
		if(height < OFFSET || height < 1)
			return 0;
		int baseHeight = height-OFFSET;
		int prevHeight = height-1;
		if(userEntrusts.count(baseHeight) == 0 || userEntrusts.count(prevHeight) == 0)
			return 0;
		const UserStatistics& base = userEntrusts.at(baseHeight);
		const UserStatistics& prev = userEntrusts.at(prevHeight);
		if(baseHeight < prev.joinHeight)
			return 0;
		int deal = base.dealDivide - prev.dividend;
		if(deal < MIN_REWARD)
			return 0;
		return deal;
	}
	
	int DealDivide(int height) const
	{
		if(height < 1)
			return 0;
		int prevHeight = height-1;
		if(userEntrusts.count(prevHeight) == 0)
			return 0;
		const UserStatistics& prev = userEntrusts.at(prevHeight);
		return 2 * prev.entrustAmount/100;
	}
	
	void Erase(int height)
	{
		userEntrusts.erase(height);
	}
};

int main ()
{
	Entrustment e;
	printf("height,joinHeight,entrustAmount,dealDivide,dividend,expectedReward\n");
	for(int i=0; i<TEST_HEIGHT; i++)
	{
		int h = i;
		UserStatistics& u = e.userEntrusts[h];
		if(h > 0)
		{
			if(e.userEntrusts.count(h-1) > 0)
			{
				u = e.userEntrusts[h-1];
			}
		}
		
		switch(h)
		{
		case 2:
			u.Entrust(h,100);
			break;
		case 20:
			u.Deprive();
			break;
		case 35:
			u.Entrust(h,200);
			break;
		case 40:
			u.Deprive();
			break;
		case 42:
			u.Entrust(h,100);
			break;
		case 50:
			u.Deprive();
			break;
		case 60:
			u.Entrust(h,100);
			break;
		case 80:
			u.Deprive();
			break;
		}
		
		u.dealDivide += e.DealDivide(h);
		u.dividend += e.ExpectedReward(i);

		if(!u.IsActive()){
			e.Erase(h);
		}
		
		if(e.userEntrusts.count(h) > 0) {
			const UserStatistics& tempU = e.userEntrusts[h];
			printf("%d,%d,%d, %d,%d, %d\n",h,tempU.joinHeight,tempU.entrustAmount,tempU.dealDivide,tempU.dividend,e.ExpectedReward(i));
		} else {
			printf("%d,-,-, -,-, %d\n",h,e.ExpectedReward(i));
		}
	}
	return 0;
}

/* Output:
******************************************************************
height,joinHeight,entrustAmount,dealDivide,dividend,expectedReward
0,-,-, -,-, 0
1,-,-, -,-, 0
2,2,100, 0,0, 0
3,2,100, 2,0, 0
4,2,100, 4,0, 0
5,2,100, 6,0, 0
6,2,100, 8,0, 0
7,2,100, 10,0, 0
8,2,100, 12,0, 0
9,2,100, 14,0, 0
10,2,100, 16,0, 0
11,2,100, 18,0, 0
12,2,100, 20,0, 0
13,2,100, 22,0, 0
14,2,100, 24,0, 0
15,2,100, 26,6, 6
16,2,100, 28,6, 0
17,2,100, 30,6, 0
18,2,100, 32,12, 6
19,2,100, 34,12, 0
20,2,0, 36,12, 0
21,2,0, 36,18, 6
22,2,0, 36,18, 0
23,2,0, 36,18, 0
24,2,0, 36,24, 6
25,2,0, 36,24, 0
26,2,0, 36,24, 0
27,2,0, 36,30, 6
28,2,0, 36,30, 0
29,2,0, 36,30, 0
30,-,-, -,-, 6
31,-,-, -,-, 0
32,-,-, -,-, 0
33,-,-, -,-, 0
34,-,-, -,-, 0
35,35,200, 0,0, 0
36,35,200, 4,0, 0
37,35,200, 8,0, 0
38,35,200, 12,0, 0
39,35,200, 16,0, 0
40,35,0, 20,0, 0
41,35,0, 20,0, 0
42,42,100, 20,0, 0
43,42,100, 22,0, 0
44,42,100, 24,0, 0
45,42,100, 26,0, 0
46,42,100, 28,0, 0
47,42,100, 30,0, 0
48,42,100, 32,0, 0
49,42,100, 34,0, 0
50,42,0, 36,0, 0
51,42,0, 36,0, 0
52,42,0, 36,20, 20
53,42,0, 36,20, 0
54,42,0, 36,20, 0
55,42,0, 36,26, 6
56,42,0, 36,26, 0
57,42,0, 36,26, 0
58,-,-, -,-, 6
59,-,-, -,-, 0
60,60,100, 0,0, 0
61,60,100, 2,0, 0
62,60,100, 4,0, 0
63,60,100, 6,0, 0
64,60,100, 8,0, 0
65,60,100, 10,0, 0
66,60,100, 12,0, 0
67,60,100, 14,0, 0
68,60,100, 16,0, 0
69,60,100, 18,0, 0
70,60,100, 20,0, 0
71,60,100, 22,0, 0
72,60,100, 24,0, 0
73,60,100, 26,6, 6
74,60,100, 28,6, 0
75,60,100, 30,6, 0
76,60,100, 32,12, 6
77,60,100, 34,12, 0
78,60,100, 36,12, 0
79,60,100, 38,18, 6
80,60,0, 40,18, 0
81,60,0, 40,18, 0
82,60,0, 40,24, 6
83,60,0, 40,24, 0
84,60,0, 40,24, 0
85,60,0, 40,30, 6
86,60,0, 40,30, 0
87,60,0, 40,30, 0
88,-,-, -,-, 6
89,-,-, -,-, 0
90,-,-, -,-, 0
91,-,-, -,-, 0
92,-,-, -,-, 0
93,-,-, -,-, 0
94,-,-, -,-, 0
95,-,-, -,-, 0
96,-,-, -,-, 0
97,-,-, -,-, 0
98,-,-, -,-, 0
99,-,-, -,-, 0
******************************************************************
*/