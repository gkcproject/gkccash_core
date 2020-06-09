/****************************************************************************
** Meta object code from reading C++ file 'walletmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/walletmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'walletmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WalletModel_t {
    QByteArrayData data[177];
    char stringdata0[2532];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WalletModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WalletModel_t qt_meta_stringdata_WalletModel = {
    {
QT_MOC_LITERAL(0, 0, 11), // "WalletModel"
QT_MOC_LITERAL(1, 12, 14), // "balanceChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 7), // "CAmount"
QT_MOC_LITERAL(4, 36, 7), // "balance"
QT_MOC_LITERAL(5, 44, 18), // "unconfirmedBalance"
QT_MOC_LITERAL(6, 63, 15), // "immatureBalance"
QT_MOC_LITERAL(7, 79, 12), // "totalBalance"
QT_MOC_LITERAL(8, 92, 15), // "zerocoinBalance"
QT_MOC_LITERAL(9, 108, 26), // "unconfirmedZerocoinBalance"
QT_MOC_LITERAL(10, 135, 23), // "immatureZerocoinBalance"
QT_MOC_LITERAL(11, 159, 16), // "watchOnlyBalance"
QT_MOC_LITERAL(12, 176, 18), // "watchUnconfBalance"
QT_MOC_LITERAL(13, 195, 20), // "watchImmatureBalance"
QT_MOC_LITERAL(14, 216, 14), // "entrustChanged"
QT_MOC_LITERAL(15, 231, 14), // "cachedEntrusts"
QT_MOC_LITERAL(16, 246, 13), // "cachedProfits"
QT_MOC_LITERAL(17, 260, 23), // "encryptionStatusChanged"
QT_MOC_LITERAL(18, 284, 6), // "status"
QT_MOC_LITERAL(19, 291, 19), // "notificationChanged"
QT_MOC_LITERAL(20, 311, 5), // "index"
QT_MOC_LITERAL(21, 317, 5), // "title"
QT_MOC_LITERAL(22, 323, 4), // "time"
QT_MOC_LITERAL(23, 328, 4), // "text"
QT_MOC_LITERAL(24, 333, 6), // "nblock"
QT_MOC_LITERAL(25, 340, 4), // "link"
QT_MOC_LITERAL(26, 345, 6), // "author"
QT_MOC_LITERAL(27, 352, 5), // "nhash"
QT_MOC_LITERAL(28, 358, 13), // "requireUnlock"
QT_MOC_LITERAL(29, 372, 17), // "syncStatusChanged"
QT_MOC_LITERAL(30, 390, 12), // "notifyBackup"
QT_MOC_LITERAL(31, 403, 10), // "hideWindow"
QT_MOC_LITERAL(32, 414, 7), // "message"
QT_MOC_LITERAL(33, 422, 5), // "style"
QT_MOC_LITERAL(34, 428, 9), // "coinsSent"
QT_MOC_LITERAL(35, 438, 8), // "CWallet*"
QT_MOC_LITERAL(36, 447, 6), // "wallet"
QT_MOC_LITERAL(37, 454, 18), // "SendCoinsRecipient"
QT_MOC_LITERAL(38, 473, 9), // "recipient"
QT_MOC_LITERAL(39, 483, 11), // "transaction"
QT_MOC_LITERAL(40, 495, 12), // "showProgress"
QT_MOC_LITERAL(41, 508, 9), // "nProgress"
QT_MOC_LITERAL(42, 518, 22), // "notifyWatchonlyChanged"
QT_MOC_LITERAL(43, 541, 14), // "fHaveWatchonly"
QT_MOC_LITERAL(44, 556, 21), // "notifyMultiSigChanged"
QT_MOC_LITERAL(45, 578, 13), // "fHaveMultiSig"
QT_MOC_LITERAL(46, 592, 22), // "pollBalanceChanged_sig"
QT_MOC_LITERAL(47, 615, 10), // "traySignal"
QT_MOC_LITERAL(48, 626, 8), // "badWords"
QT_MOC_LITERAL(49, 635, 15), // "existingAddress"
QT_MOC_LITERAL(50, 651, 20), // "addAddressSuccessful"
QT_MOC_LITERAL(51, 672, 7), // "address"
QT_MOC_LITERAL(52, 680, 12), // "updateStatus"
QT_MOC_LITERAL(53, 693, 17), // "updateTransaction"
QT_MOC_LITERAL(54, 711, 17), // "updateAddressBook"
QT_MOC_LITERAL(55, 729, 5), // "label"
QT_MOC_LITERAL(56, 735, 6), // "isMine"
QT_MOC_LITERAL(57, 742, 7), // "purpose"
QT_MOC_LITERAL(58, 750, 7), // "pubCoin"
QT_MOC_LITERAL(59, 758, 6), // "isUsed"
QT_MOC_LITERAL(60, 765, 19), // "updateWatchOnlyFlag"
QT_MOC_LITERAL(61, 785, 18), // "updateMultiSigFlag"
QT_MOC_LITERAL(62, 804, 18), // "pollBalanceChanged"
QT_MOC_LITERAL(63, 823, 18), // "emitBalanceChanged"
QT_MOC_LITERAL(64, 842, 18), // "updateContractBook"
QT_MOC_LITERAL(65, 861, 3), // "abi"
QT_MOC_LITERAL(66, 865, 21), // "checkForInvalidTokens"
QT_MOC_LITERAL(67, 887, 11), // "backupCheck"
QT_MOC_LITERAL(68, 899, 16), // "getTxDescription"
QT_MOC_LITERAL(69, 916, 3), // "row"
QT_MOC_LITERAL(70, 920, 11), // "caculateSum"
QT_MOC_LITERAL(71, 932, 4), // "rows"
QT_MOC_LITERAL(72, 937, 13), // "exportClicked"
QT_MOC_LITERAL(73, 951, 10), // "chooseType"
QT_MOC_LITERAL(74, 962, 3), // "idx"
QT_MOC_LITERAL(75, 966, 10), // "chooseDate"
QT_MOC_LITERAL(76, 977, 16), // "dateRangeChanged"
QT_MOC_LITERAL(77, 994, 8), // "fromDate"
QT_MOC_LITERAL(78, 1003, 6), // "toDate"
QT_MOC_LITERAL(79, 1010, 13), // "changedPrefix"
QT_MOC_LITERAL(80, 1024, 6), // "prefix"
QT_MOC_LITERAL(81, 1031, 13), // "changedAmount"
QT_MOC_LITERAL(82, 1045, 6), // "amount"
QT_MOC_LITERAL(83, 1052, 14), // "getlockedCoins"
QT_MOC_LITERAL(84, 1067, 12), // "GetAllProfit"
QT_MOC_LITERAL(85, 1080, 9), // "getNodeID"
QT_MOC_LITERAL(86, 1090, 20), // "GetRedeemDescription"
QT_MOC_LITERAL(87, 1111, 13), // "GetRedeemTXID"
QT_MOC_LITERAL(88, 1125, 14), // "GetEntrustSure"
QT_MOC_LITERAL(89, 1140, 6), // "nodeID"
QT_MOC_LITERAL(90, 1147, 6), // "number"
QT_MOC_LITERAL(91, 1154, 4), // "uint"
QT_MOC_LITERAL(92, 1159, 15), // "RedeemOperation"
QT_MOC_LITERAL(93, 1175, 4), // "txid"
QT_MOC_LITERAL(94, 1180, 16), // "EntrustOperation"
QT_MOC_LITERAL(95, 1197, 6), // "nodeid"
QT_MOC_LITERAL(96, 1204, 22), // "SendAdvertiseOperation"
QT_MOC_LITERAL(97, 1227, 10), // "GetMessage"
QT_MOC_LITERAL(98, 1238, 11), // "InitEntrust"
QT_MOC_LITERAL(99, 1250, 17), // "GetMyAgentUserNum"
QT_MOC_LITERAL(100, 1268, 14), // "GetMyUTXOCount"
QT_MOC_LITERAL(101, 1283, 17), // "GetMyAgentEntrust"
QT_MOC_LITERAL(102, 1301, 18), // "GetDopsStateString"
QT_MOC_LITERAL(103, 1320, 14), // "GetStakingStat"
QT_MOC_LITERAL(104, 1335, 21), // "GetMyMasternodeStatus"
QT_MOC_LITERAL(105, 1357, 18), // "UpdateMainNodeList"
QT_MOC_LITERAL(106, 1376, 12), // "formatAmount"
QT_MOC_LITERAL(107, 1389, 4), // "unit"
QT_MOC_LITERAL(108, 1394, 9), // "getAmount"
QT_MOC_LITERAL(109, 1404, 11), // "currentUnit"
QT_MOC_LITERAL(110, 1416, 9), // "direction"
QT_MOC_LITERAL(111, 1426, 6), // "factor"
QT_MOC_LITERAL(112, 1433, 12), // "getClipBoard"
QT_MOC_LITERAL(113, 1446, 4), // "type"
QT_MOC_LITERAL(114, 1451, 12), // "setClipBoard"
QT_MOC_LITERAL(115, 1464, 7), // "variant"
QT_MOC_LITERAL(116, 1472, 13), // "getFeePerkilo"
QT_MOC_LITERAL(117, 1486, 13), // "alreadyShowed"
QT_MOC_LITERAL(118, 1500, 7), // "version"
QT_MOC_LITERAL(119, 1508, 14), // "getFieldAmount"
QT_MOC_LITERAL(120, 1523, 10), // "amountText"
QT_MOC_LITERAL(121, 1534, 17), // "addAddressByWords"
QT_MOC_LITERAL(122, 1552, 10), // "addressStr"
QT_MOC_LITERAL(123, 1563, 12), // "rescanWallet"
QT_MOC_LITERAL(124, 1576, 15), // "validateAddress"
QT_MOC_LITERAL(125, 1592, 14), // "validateAmount"
QT_MOC_LITERAL(126, 1607, 15), // "sendtoAddresses"
QT_MOC_LITERAL(127, 1623, 9), // "getReadMe"
QT_MOC_LITERAL(128, 1633, 9), // "isTestNet"
QT_MOC_LITERAL(129, 1643, 19), // "getEncryptionStatus"
QT_MOC_LITERAL(130, 1663, 18), // "setWalletEncrypted"
QT_MOC_LITERAL(131, 1682, 7), // "passStr"
QT_MOC_LITERAL(132, 1690, 16), // "changePassphrase"
QT_MOC_LITERAL(133, 1707, 10), // "oldPassStr"
QT_MOC_LITERAL(134, 1718, 10), // "newPassStr"
QT_MOC_LITERAL(135, 1729, 15), // "setWalletLocked"
QT_MOC_LITERAL(136, 1745, 6), // "locked"
QT_MOC_LITERAL(137, 1752, 13), // "anonymizeOnly"
QT_MOC_LITERAL(138, 1766, 12), // "backupWallet"
QT_MOC_LITERAL(139, 1779, 8), // "filename"
QT_MOC_LITERAL(140, 1788, 17), // "checkBackupStatus"
QT_MOC_LITERAL(141, 1806, 28), // "UpdateNotificationRecordList"
QT_MOC_LITERAL(142, 1835, 4), // "from"
QT_MOC_LITERAL(143, 1840, 5), // "count"
QT_MOC_LITERAL(144, 1846, 21), // "GetNotificationRecord"
QT_MOC_LITERAL(145, 1868, 20), // "GetNotificationTitle"
QT_MOC_LITERAL(146, 1889, 21), // "GetEntructDescription"
QT_MOC_LITERAL(147, 1911, 25), // "notificationRecords_Proxy"
QT_MOC_LITERAL(148, 1937, 22), // "NotificationListProxy*"
QT_MOC_LITERAL(149, 1960, 19), // "redeemRecords_Proxy"
QT_MOC_LITERAL(150, 1980, 23), // "EntrustRecordListProxy*"
QT_MOC_LITERAL(151, 2004, 20), // "entrustRecords_Proxy"
QT_MOC_LITERAL(152, 2025, 14), // "nodeList_Proxy"
QT_MOC_LITERAL(153, 2040, 21), // "EntrustNodeListProxy*"
QT_MOC_LITERAL(154, 2062, 18), // "mainNodeList_Proxy"
QT_MOC_LITERAL(155, 2081, 18), // "MainNodeListProxy*"
QT_MOC_LITERAL(156, 2100, 22), // "transaction_ProxyModel"
QT_MOC_LITERAL(157, 2123, 23), // "TransactionFilterProxy*"
QT_MOC_LITERAL(158, 2147, 10), // "syncStatus"
QT_MOC_LITERAL(159, 2158, 30), // "transaction_ProxyModelOverView"
QT_MOC_LITERAL(160, 2189, 26), // "receivingAddressProxyModel"
QT_MOC_LITERAL(161, 2216, 19), // "AddressFilterProxy*"
QT_MOC_LITERAL(162, 2236, 24), // "sendingAddressProxyModel"
QT_MOC_LITERAL(163, 2261, 25), // "recentRequestsFilterProxy"
QT_MOC_LITERAL(164, 2287, 26), // "RecentRequestsFilterProxy*"
QT_MOC_LITERAL(165, 2314, 19), // "contractfilterproxy"
QT_MOC_LITERAL(166, 2334, 20), // "ContractFilterProxy*"
QT_MOC_LITERAL(167, 2355, 12), // "contractPage"
QT_MOC_LITERAL(168, 2368, 19), // "CreateContractPage*"
QT_MOC_LITERAL(169, 2388, 16), // "tokenfilterproxy"
QT_MOC_LITERAL(170, 2405, 17), // "TokenFilterProxy*"
QT_MOC_LITERAL(171, 2423, 14), // "tokenItemModel"
QT_MOC_LITERAL(172, 2438, 15), // "TokenItemModel*"
QT_MOC_LITERAL(173, 2454, 16), // "coinControlProxy"
QT_MOC_LITERAL(174, 2471, 17), // "CoinControlProxy*"
QT_MOC_LITERAL(175, 2489, 20), // "masternodetableproxy"
QT_MOC_LITERAL(176, 2510, 21) // "MasterNodeTableProxy*"

    },
    "WalletModel\0balanceChanged\0\0CAmount\0"
    "balance\0unconfirmedBalance\0immatureBalance\0"
    "totalBalance\0zerocoinBalance\0"
    "unconfirmedZerocoinBalance\0"
    "immatureZerocoinBalance\0watchOnlyBalance\0"
    "watchUnconfBalance\0watchImmatureBalance\0"
    "entrustChanged\0cachedEntrusts\0"
    "cachedProfits\0encryptionStatusChanged\0"
    "status\0notificationChanged\0index\0title\0"
    "time\0text\0nblock\0link\0author\0nhash\0"
    "requireUnlock\0syncStatusChanged\0"
    "notifyBackup\0hideWindow\0message\0style\0"
    "coinsSent\0CWallet*\0wallet\0SendCoinsRecipient\0"
    "recipient\0transaction\0showProgress\0"
    "nProgress\0notifyWatchonlyChanged\0"
    "fHaveWatchonly\0notifyMultiSigChanged\0"
    "fHaveMultiSig\0pollBalanceChanged_sig\0"
    "traySignal\0badWords\0existingAddress\0"
    "addAddressSuccessful\0address\0updateStatus\0"
    "updateTransaction\0updateAddressBook\0"
    "label\0isMine\0purpose\0pubCoin\0isUsed\0"
    "updateWatchOnlyFlag\0updateMultiSigFlag\0"
    "pollBalanceChanged\0emitBalanceChanged\0"
    "updateContractBook\0abi\0checkForInvalidTokens\0"
    "backupCheck\0getTxDescription\0row\0"
    "caculateSum\0rows\0exportClicked\0"
    "chooseType\0idx\0chooseDate\0dateRangeChanged\0"
    "fromDate\0toDate\0changedPrefix\0prefix\0"
    "changedAmount\0amount\0getlockedCoins\0"
    "GetAllProfit\0getNodeID\0GetRedeemDescription\0"
    "GetRedeemTXID\0GetEntrustSure\0nodeID\0"
    "number\0uint\0RedeemOperation\0txid\0"
    "EntrustOperation\0nodeid\0SendAdvertiseOperation\0"
    "GetMessage\0InitEntrust\0GetMyAgentUserNum\0"
    "GetMyUTXOCount\0GetMyAgentEntrust\0"
    "GetDopsStateString\0GetStakingStat\0"
    "GetMyMasternodeStatus\0UpdateMainNodeList\0"
    "formatAmount\0unit\0getAmount\0currentUnit\0"
    "direction\0factor\0getClipBoard\0type\0"
    "setClipBoard\0variant\0getFeePerkilo\0"
    "alreadyShowed\0version\0getFieldAmount\0"
    "amountText\0addAddressByWords\0addressStr\0"
    "rescanWallet\0validateAddress\0"
    "validateAmount\0sendtoAddresses\0getReadMe\0"
    "isTestNet\0getEncryptionStatus\0"
    "setWalletEncrypted\0passStr\0changePassphrase\0"
    "oldPassStr\0newPassStr\0setWalletLocked\0"
    "locked\0anonymizeOnly\0backupWallet\0"
    "filename\0checkBackupStatus\0"
    "UpdateNotificationRecordList\0from\0"
    "count\0GetNotificationRecord\0"
    "GetNotificationTitle\0GetEntructDescription\0"
    "notificationRecords_Proxy\0"
    "NotificationListProxy*\0redeemRecords_Proxy\0"
    "EntrustRecordListProxy*\0entrustRecords_Proxy\0"
    "nodeList_Proxy\0EntrustNodeListProxy*\0"
    "mainNodeList_Proxy\0MainNodeListProxy*\0"
    "transaction_ProxyModel\0TransactionFilterProxy*\0"
    "syncStatus\0transaction_ProxyModelOverView\0"
    "receivingAddressProxyModel\0"
    "AddressFilterProxy*\0sendingAddressProxyModel\0"
    "recentRequestsFilterProxy\0"
    "RecentRequestsFilterProxy*\0"
    "contractfilterproxy\0ContractFilterProxy*\0"
    "contractPage\0CreateContractPage*\0"
    "tokenfilterproxy\0TokenFilterProxy*\0"
    "tokenItemModel\0TokenItemModel*\0"
    "coinControlProxy\0CoinControlProxy*\0"
    "masternodetableproxy\0MasterNodeTableProxy*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WalletModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      83,   14, // methods
      17,  724, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,   10,  429,    2, 0x06 /* Public */,
      14,    3,  450,    2, 0x06 /* Public */,
      17,    1,  457,    2, 0x06 /* Public */,
      19,    8,  460,    2, 0x06 /* Public */,
      28,    0,  477,    2, 0x06 /* Public */,
      29,    0,  478,    2, 0x06 /* Public */,
      30,    0,  479,    2, 0x06 /* Public */,
      31,    0,  480,    2, 0x06 /* Public */,
      32,    3,  481,    2, 0x06 /* Public */,
      34,    3,  488,    2, 0x06 /* Public */,
      40,    2,  495,    2, 0x06 /* Public */,
      42,    1,  500,    2, 0x06 /* Public */,
      44,    1,  503,    2, 0x06 /* Public */,
      46,    0,  506,    2, 0x06 /* Public */,
      47,    1,  507,    2, 0x06 /* Public */,
      48,    0,  510,    2, 0x06 /* Public */,
      49,    0,  511,    2, 0x06 /* Public */,
      50,    1,  512,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      52,    0,  515,    2, 0x0a /* Public */,
      53,    0,  516,    2, 0x0a /* Public */,
      54,    5,  517,    2, 0x0a /* Public */,
      54,    3,  528,    2, 0x0a /* Public */,
      60,    1,  535,    2, 0x0a /* Public */,
      61,    1,  538,    2, 0x0a /* Public */,
      62,    0,  541,    2, 0x0a /* Public */,
      63,    0,  542,    2, 0x0a /* Public */,
      64,    4,  543,    2, 0x0a /* Public */,
      66,    0,  552,    2, 0x08 /* Private */,
      67,    0,  553,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      68,    1,  554,    2, 0x02 /* Public */,
      70,    1,  557,    2, 0x02 /* Public */,
      72,    0,  560,    2, 0x02 /* Public */,
      73,    1,  561,    2, 0x02 /* Public */,
      75,    1,  564,    2, 0x02 /* Public */,
      76,    2,  567,    2, 0x02 /* Public */,
      79,    1,  572,    2, 0x02 /* Public */,
      81,    1,  575,    2, 0x02 /* Public */,
      83,    0,  578,    2, 0x02 /* Public */,
      84,    0,  579,    2, 0x02 /* Public */,
      85,    1,  580,    2, 0x02 /* Public */,
      86,    1,  583,    2, 0x02 /* Public */,
      87,    1,  586,    2, 0x02 /* Public */,
      88,    3,  589,    2, 0x02 /* Public */,
      92,    1,  596,    2, 0x02 /* Public */,
      94,    3,  599,    2, 0x02 /* Public */,
      96,    3,  606,    2, 0x02 /* Public */,
      97,    0,  613,    2, 0x02 /* Public */,
      98,    0,  614,    2, 0x02 /* Public */,
      99,    0,  615,    2, 0x02 /* Public */,
     100,    0,  616,    2, 0x02 /* Public */,
     101,    0,  617,    2, 0x02 /* Public */,
     102,    0,  618,    2, 0x02 /* Public */,
     103,    0,  619,    2, 0x02 /* Public */,
     104,    0,  620,    2, 0x02 /* Public */,
     105,    0,  621,    2, 0x02 /* Public */,
     106,    2,  622,    2, 0x02 /* Public */,
     106,    1,  627,    2, 0x22 /* Public | MethodCloned */,
     108,    5,  630,    2, 0x02 /* Public */,
     108,    4,  641,    2, 0x22 /* Public | MethodCloned */,
     112,    1,  650,    2, 0x02 /* Public */,
     114,    1,  653,    2, 0x02 /* Public */,
     116,    0,  656,    2, 0x02 /* Public */,
     117,    1,  657,    2, 0x02 /* Public */,
     119,    2,  660,    2, 0x02 /* Public */,
     121,    1,  665,    2, 0x02 /* Public */,
     123,    0,  668,    2, 0x02 /* Public */,
     124,    1,  669,    2, 0x02 /* Public */,
     125,    2,  672,    2, 0x02 /* Public */,
     126,    0,  677,    2, 0x02 /* Public */,
     127,    0,  678,    2, 0x02 /* Public */,
     128,    0,  679,    2, 0x02 /* Public */,
     129,    0,  680,    2, 0x02 /* Public */,
     130,    1,  681,    2, 0x02 /* Public */,
     132,    2,  684,    2, 0x02 /* Public */,
     135,    3,  689,    2, 0x02 /* Public */,
     135,    2,  696,    2, 0x22 /* Public | MethodCloned */,
     135,    1,  701,    2, 0x22 /* Public | MethodCloned */,
     138,    1,  704,    2, 0x02 /* Public */,
     140,    0,  707,    2, 0x02 /* Public */,
     141,    3,  708,    2, 0x02 /* Public */,
     144,    1,  715,    2, 0x02 /* Public */,
     145,    1,  718,    2, 0x02 /* Public */,
     146,    1,  721,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, 0x80000000 | 3,    4,   15,   16,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   20,   21,   22,   23,   24,   25,   26,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,   21,   32,   33,
    QMetaType::Void, 0x80000000 | 35, 0x80000000 | 37, QMetaType::QByteArray,   36,   38,   39,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   21,   41,
    QMetaType::Void, QMetaType::Bool,   43,
    QMetaType::Void, QMetaType::Bool,   45,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   51,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool, QMetaType::QString, QMetaType::Int,   51,   55,   56,   57,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int,   58,   59,   18,
    QMetaType::Void, QMetaType::Bool,   43,
    QMetaType::Void, QMetaType::Bool,   45,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Int,   51,   55,   65,   18,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::QString, QMetaType::Int,   69,
    QMetaType::QString, QMetaType::QVariantList,   71,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   74,
    QMetaType::Void, QMetaType::Int,   74,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   77,   78,
    QMetaType::Void, QMetaType::QString,   80,
    QMetaType::Void, QMetaType::QString,   82,
    QMetaType::LongLong,
    0x80000000 | 3,
    QMetaType::QString, QMetaType::Int,   69,
    QMetaType::QString, QMetaType::Int,   69,
    QMetaType::QString, QMetaType::Int,   69,
    QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   89,   90,   91,
    QMetaType::Bool, QMetaType::QString,   93,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString, QMetaType::Int,   95,   82,   91,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString, QMetaType::QString,   21,   25,   23,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Double,
    QMetaType::QString,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::QString, QMetaType::LongLong, QMetaType::Int,   82,  107,
    QMetaType::QString, QMetaType::LongLong,   82,
    QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Int, QMetaType::Int,  109,  107,   23,  110,  111,
    QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Int,  109,  107,   23,  110,
    QMetaType::QVariant, QMetaType::QString,  113,
    QMetaType::Void, QMetaType::QVariant,  115,
    QMetaType::LongLong,
    QMetaType::Bool, QMetaType::QString,  118,
    QMetaType::LongLong, QMetaType::Int, QMetaType::QString,   91,  120,
    QMetaType::Void, QMetaType::QString,  122,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::QString,   51,
    QMetaType::LongLong, QMetaType::Int, QMetaType::QString,  109,   23,
    QMetaType::Void,
    QMetaType::QString,
    QMetaType::Bool,
    QMetaType::Int,
    QMetaType::Bool, QMetaType::QString,  131,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,  133,  134,
    QMetaType::Bool, QMetaType::Bool, QMetaType::QString, QMetaType::Bool,  136,  131,  137,
    QMetaType::Bool, QMetaType::Bool, QMetaType::QString,  136,  131,
    QMetaType::Bool, QMetaType::Bool,  136,
    QMetaType::Bool, QMetaType::QString,  139,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,  113,  142,  143,
    QMetaType::QString, QMetaType::Int,   20,
    QMetaType::QString, QMetaType::Int,   20,
    QMetaType::QString, QMetaType::Int,   69,

 // properties: name, type, flags
     147, 0x80000000 | 148, 0x00095409,
     149, 0x80000000 | 150, 0x00095409,
     151, 0x80000000 | 150, 0x00095409,
     152, 0x80000000 | 153, 0x00095409,
     154, 0x80000000 | 155, 0x00095409,
     156, 0x80000000 | 157, 0x00095409,
     158, QMetaType::QString, 0x00495003,
     159, 0x80000000 | 157, 0x00095409,
     160, 0x80000000 | 161, 0x00095409,
     162, 0x80000000 | 161, 0x00095409,
     163, 0x80000000 | 164, 0x00095409,
     165, 0x80000000 | 166, 0x00095409,
     167, 0x80000000 | 168, 0x00095409,
     169, 0x80000000 | 170, 0x00095409,
     171, 0x80000000 | 172, 0x00095409,
     173, 0x80000000 | 174, 0x00095409,
     175, 0x80000000 | 176, 0x00095409,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       0,
       5,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

       0        // eod
};

void WalletModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WalletModel *_t = static_cast<WalletModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->balanceChanged((*reinterpret_cast< const CAmount(*)>(_a[1])),(*reinterpret_cast< const CAmount(*)>(_a[2])),(*reinterpret_cast< const CAmount(*)>(_a[3])),(*reinterpret_cast< const CAmount(*)>(_a[4])),(*reinterpret_cast< const CAmount(*)>(_a[5])),(*reinterpret_cast< const CAmount(*)>(_a[6])),(*reinterpret_cast< const CAmount(*)>(_a[7])),(*reinterpret_cast< const CAmount(*)>(_a[8])),(*reinterpret_cast< const CAmount(*)>(_a[9])),(*reinterpret_cast< const CAmount(*)>(_a[10]))); break;
        case 1: _t->entrustChanged((*reinterpret_cast< const CAmount(*)>(_a[1])),(*reinterpret_cast< const CAmount(*)>(_a[2])),(*reinterpret_cast< const CAmount(*)>(_a[3]))); break;
        case 2: _t->encryptionStatusChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->notificationChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QString(*)>(_a[6])),(*reinterpret_cast< QString(*)>(_a[7])),(*reinterpret_cast< QString(*)>(_a[8]))); break;
        case 4: _t->requireUnlock(); break;
        case 5: _t->syncStatusChanged(); break;
        case 6: _t->notifyBackup(); break;
        case 7: _t->hideWindow(); break;
        case 8: _t->message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 9: _t->coinsSent((*reinterpret_cast< CWallet*(*)>(_a[1])),(*reinterpret_cast< SendCoinsRecipient(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3]))); break;
        case 10: _t->showProgress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->notifyWatchonlyChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->notifyMultiSigChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->pollBalanceChanged_sig(); break;
        case 14: _t->traySignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->badWords(); break;
        case 16: _t->existingAddress(); break;
        case 17: _t->addAddressSuccessful((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->updateStatus(); break;
        case 19: _t->updateTransaction(); break;
        case 20: _t->updateAddressBook((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 21: _t->updateAddressBook((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 22: _t->updateWatchOnlyFlag((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->updateMultiSigFlag((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->pollBalanceChanged(); break;
        case 25: _t->emitBalanceChanged(); break;
        case 26: _t->updateContractBook((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 27: _t->checkForInvalidTokens(); break;
        case 28: _t->backupCheck(); break;
        case 29: { QString _r = _t->getTxDescription((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 30: { QString _r = _t->caculateSum((*reinterpret_cast< QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 31: _t->exportClicked(); break;
        case 32: _t->chooseType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->chooseDate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 34: _t->dateRangeChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 35: _t->changedPrefix((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 36: _t->changedAmount((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 37: { qint64 _r = _t->getlockedCoins();
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = std::move(_r); }  break;
        case 38: { CAmount _r = _t->GetAllProfit();
            if (_a[0]) *reinterpret_cast< CAmount*>(_a[0]) = std::move(_r); }  break;
        case 39: { QString _r = _t->getNodeID((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 40: { QString _r = _t->GetRedeemDescription((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 41: { QString _r = _t->GetRedeemTXID((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 42: { QString _r = _t->GetEntrustSure((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 43: { bool _r = _t->RedeemOperation((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 44: { bool _r = _t->EntrustOperation((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 45: { bool _r = _t->SendAdvertiseOperation((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 46: { QString _r = _t->GetMessage();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 47: _t->InitEntrust(); break;
        case 48: { int _r = _t->GetMyAgentUserNum();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 49: { int _r = _t->GetMyUTXOCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 50: { double _r = _t->GetMyAgentEntrust();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 51: { QString _r = _t->GetDopsStateString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 52: { bool _r = _t->GetStakingStat();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 53: { bool _r = _t->GetMyMasternodeStatus();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 54: _t->UpdateMainNodeList(); break;
        case 55: { QString _r = _t->formatAmount((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 56: { QString _r = _t->formatAmount((*reinterpret_cast< qint64(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 57: { QString _r = _t->getAmount((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 58: { QString _r = _t->getAmount((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 59: { QVariant _r = _t->getClipBoard((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 60: _t->setClipBoard((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 61: { qint64 _r = _t->getFeePerkilo();
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = std::move(_r); }  break;
        case 62: { bool _r = _t->alreadyShowed((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 63: { qint64 _r = _t->getFieldAmount((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = std::move(_r); }  break;
        case 64: _t->addAddressByWords((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 65: _t->rescanWallet(); break;
        case 66: { bool _r = _t->validateAddress((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 67: { qint64 _r = _t->validateAmount((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = std::move(_r); }  break;
        case 68: _t->sendtoAddresses(); break;
        case 69: { QString _r = _t->getReadMe();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 70: { bool _r = _t->isTestNet();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 71: { int _r = _t->getEncryptionStatus();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 72: { bool _r = _t->setWalletEncrypted((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 73: { bool _r = _t->changePassphrase((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 74: { bool _r = _t->setWalletLocked((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 75: { bool _r = _t->setWalletLocked((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 76: { bool _r = _t->setWalletLocked((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 77: { bool _r = _t->backupWallet((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 78: { bool _r = _t->checkBackupStatus();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 79: _t->UpdateNotificationRecordList((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 80: { QString _r = _t->GetNotificationRecord((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 81: { QString _r = _t->GetNotificationTitle((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 82: { QString _r = _t->GetEntructDescription((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (WalletModel::*_t)(const CAmount & , const CAmount & , const CAmount & , const CAmount & , const CAmount & , const CAmount & , const CAmount & , const CAmount & , const CAmount & , const CAmount & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::balanceChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(const CAmount & , const CAmount & , const CAmount & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::entrustChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::encryptionStatusChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(int , QString , QString , QString , QString , QString , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::notificationChanged)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::requireUnlock)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::syncStatusChanged)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::notifyBackup)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::hideWindow)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(const QString & , const QString & , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::message)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(CWallet * , SendCoinsRecipient , QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::coinsSent)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(const QString & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::showProgress)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::notifyWatchonlyChanged)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::notifyMultiSigChanged)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::pollBalanceChanged_sig)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::traySignal)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::badWords)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::existingAddress)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (WalletModel::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletModel::addAddressSuccessful)) {
                *result = 17;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
        case 8:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< AddressFilterProxy* >(); break;
        case 15:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CoinControlProxy* >(); break;
        case 11:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ContractFilterProxy* >(); break;
        case 3:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EntrustNodeListProxy* >(); break;
        case 2:
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< EntrustRecordListProxy* >(); break;
        case 4:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< MainNodeListProxy* >(); break;
        case 16:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< MasterNodeTableProxy* >(); break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< NotificationListProxy* >(); break;
        case 10:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RecentRequestsFilterProxy* >(); break;
        case 13:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TokenFilterProxy* >(); break;
        case 7:
        case 5:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TransactionFilterProxy* >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        WalletModel *_t = static_cast<WalletModel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< NotificationListProxy**>(_v) = _t->notificationRecordsProxy; break;
        case 1: *reinterpret_cast< EntrustRecordListProxy**>(_v) = _t->redeemRecordsProxy; break;
        case 2: *reinterpret_cast< EntrustRecordListProxy**>(_v) = _t->entrustRecordsProxy; break;
        case 3: *reinterpret_cast< EntrustNodeListProxy**>(_v) = _t->entrustNodeListProxy; break;
        case 4: *reinterpret_cast< MainNodeListProxy**>(_v) = _t->mainNodeListProxy; break;
        case 5: *reinterpret_cast< TransactionFilterProxy**>(_v) = _t->transactionProxyModel; break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->m_syncStatus; break;
        case 7: *reinterpret_cast< TransactionFilterProxy**>(_v) = _t->transactionProxyModelOverView; break;
        case 8: *reinterpret_cast< AddressFilterProxy**>(_v) = _t->receivingAddressProxyModel_; break;
        case 9: *reinterpret_cast< AddressFilterProxy**>(_v) = _t->sendingAddressProxyModel_; break;
        case 10: *reinterpret_cast< RecentRequestsFilterProxy**>(_v) = _t->recentRequestsFilterProxy_; break;
        case 11: *reinterpret_cast< ContractFilterProxy**>(_v) = _t->contractfilterproxy_; break;
        case 12: *reinterpret_cast< CreateContractPage**>(_v) = _t->contractPage_; break;
        case 13: *reinterpret_cast< TokenFilterProxy**>(_v) = _t->tokenfilterproxy_; break;
        case 14: *reinterpret_cast< TokenItemModel**>(_v) = _t->tokenItemModel_; break;
        case 15: *reinterpret_cast< CoinControlProxy**>(_v) = _t->coinControlProxy_; break;
        case 16: *reinterpret_cast< MasterNodeTableProxy**>(_v) = _t->masternodetableproxy_; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        WalletModel *_t = static_cast<WalletModel *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 6:
            if (_t->m_syncStatus != *reinterpret_cast< QString*>(_v)) {
                _t->m_syncStatus = *reinterpret_cast< QString*>(_v);
                Q_EMIT _t->syncStatusChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject WalletModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WalletModel.data,
      qt_meta_data_WalletModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *WalletModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WalletModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WalletModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WalletModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 83)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 83;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 83)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 83;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 17;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void WalletModel::balanceChanged(const CAmount & _t1, const CAmount & _t2, const CAmount & _t3, const CAmount & _t4, const CAmount & _t5, const CAmount & _t6, const CAmount & _t7, const CAmount & _t8, const CAmount & _t9, const CAmount & _t10)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)), const_cast<void*>(reinterpret_cast<const void*>(&_t9)), const_cast<void*>(reinterpret_cast<const void*>(&_t10)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WalletModel::entrustChanged(const CAmount & _t1, const CAmount & _t2, const CAmount & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WalletModel::encryptionStatusChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WalletModel::notificationChanged(int _t1, QString _t2, QString _t3, QString _t4, QString _t5, QString _t6, QString _t7, QString _t8)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WalletModel::requireUnlock()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void WalletModel::syncStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void WalletModel::notifyBackup()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void WalletModel::hideWindow()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void WalletModel::message(const QString & _t1, const QString & _t2, unsigned int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void WalletModel::coinsSent(CWallet * _t1, SendCoinsRecipient _t2, QByteArray _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void WalletModel::showProgress(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void WalletModel::notifyWatchonlyChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void WalletModel::notifyMultiSigChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void WalletModel::pollBalanceChanged_sig()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void WalletModel::traySignal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void WalletModel::badWords()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void WalletModel::existingAddress()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void WalletModel::addAddressSuccessful(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
