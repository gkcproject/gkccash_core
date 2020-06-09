// Copyright (c) 2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>
#include "wallet/bip39.h"
#include <string>
#include <algorithm>
#include "util.h"
#include "wallet.h"

BOOST_AUTO_TEST_SUITE(bip39_tests)

BOOST_AUTO_TEST_CASE(test_generate_mnemonic) {
	SecureString m12 = Mnemonic::mnemonic_generate(128);
	SecureString m24 = Mnemonic::mnemonic_generate(256);
	BOOST_CHECK(std::count(m12.begin(),m12.end(),' ')==11); // 12 words
	BOOST_CHECK(std::count(m24.begin(),m24.end(),' ')==23); // 24 words
	BOOST_CHECK(Mnemonic::mnemonic_check(m12));
	BOOST_CHECK(Mnemonic::mnemonic_check(m24));
}

BOOST_AUTO_TEST_CASE(test_get_mnemonic) {
	const std::string mcode = "pony indicate dinner clown trend make scissors keep elite across antique wire typical citizen laundry lecture replace toss walk valley cook extend basket uncover";
	SoftSetArg("-mnemonic",mcode);
	CWallet wallet("wallet.dat");
	wallet.GenerateNewMnemonic();
    MnemonicContainer mContainer = wallet.GetMnemonicContainer();
    SecureString mnemonic;
    BOOST_CHECK(mContainer.GetMnemonic(mnemonic));
	std::string result = mnemonic.c_str();
	BOOST_CHECK(result==mcode);
}

BOOST_AUTO_TEST_CASE(test_mnemonic_check) {
	const SecureString a = "pony indicate dinner clown trend make scissors keep elite across antique wire typical citizen laundry lecture replace toss walk valley cook extend basket uncover";
	const SecureString b = "1 2 3";
	const SecureString c = "1 2 3 4 5 6 7 8 9 10 11 12";
	const SecureString d = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24";
	const SecureString e = "";
	BOOST_CHECK(Mnemonic::mnemonic_check(a));
	BOOST_CHECK(!Mnemonic::mnemonic_check(b));
	BOOST_CHECK(!Mnemonic::mnemonic_check(c));
	BOOST_CHECK(!Mnemonic::mnemonic_check(d));
	BOOST_CHECK(!Mnemonic::mnemonic_check(e));
}


BOOST_AUTO_TEST_SUITE_END()
