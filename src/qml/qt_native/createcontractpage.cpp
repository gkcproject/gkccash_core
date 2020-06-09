#include "createcontractpage.h"
#include "walletmodel.h"
#include "clientmodel.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "abifunctionfield.h"

#include "execrpccommand.h"
#include "contractabi.h"

#include "bitcoinunits.h"
#include "sendcoinsdialog.h"
#include "contractresult.h"
#include "contractconfig.h"

#include <tuple>

namespace CreateContract_NS
{
// Contract data names
static const QString PRC_COMMAND = "createcontract";
static const QString PARAM_BYTECODE = "bytecode";
static const QString PARAM_GASLIMIT = "gaslimit";
static const QString PARAM_GASPRICE = "gasprice";
static const QString PARAM_SENDER = "sender";

static const CAmount SINGLE_STEP = 0.00000001*COIN;
static const CAmount HIGH_GASPRICE = 0.001*COIN;
}
using namespace CreateContract_NS;


namespace SendToContract_NS
{
// Contract data names
static const QString SENDTO_PRC_COMMAND = "sendtocontract";
static const QString SENDTO_PARAM_ADDRESS = "address";
static const QString SENDTO_PARAM_DATAHEX = "datahex";
static const QString SENDTO_PARAM_AMOUNT = "amount";
static const QString SENDTO_PARAM_GASLIMIT = "gaslimit";
static const QString SENDTO_PARAM_GASPRICE = "gasprice";
static const QString SENDTO_PARAM_SENDER = "sender";

}
using namespace SendToContract_NS;


namespace CallContract_NS
{
// Contract data names
static const QString CALL_PRC_COMMAND = "callcontract";
static const QString CALL_PARAM_ADDRESS = "address";
static const QString CALL_PARAM_DATAHEX = "datahex";
static const QString CALL_PARAM_SENDER = "sender";
}
using namespace CallContract_NS;

CreateContractPage::CreateContractPage(WalletModel *parent):m_model(parent)
{

    //    std::string platformName;
    //    platformStyle = PlatformStyle::instantiate(QString::fromStdString(platformName));
    //    if (!platformStyle)
    //        platformStyle = PlatformStyle::instantiate("other");

    //    assert(platformStyle);

    //    //------------create---------------
    QStringList lstMandatory;
    lstMandatory.append(PARAM_BYTECODE);
    QStringList lstOptional;
    lstOptional.append(PARAM_GASLIMIT);
    lstOptional.append(PARAM_GASPRICE);
    lstOptional.append(PARAM_SENDER);
    QMap<QString, QString> lstTranslations;
    lstTranslations[PARAM_BYTECODE] = "";
    lstTranslations[PARAM_GASLIMIT] = "";
    lstTranslations[PARAM_GASPRICE] = "";
    lstTranslations[PARAM_SENDER] = "";


    m_execRPCCommand = new ExecRPCCommand(PRC_COMMAND, lstMandatory, lstOptional, lstTranslations, this);
    m_createContractABI = new ContractABI();


    //    //------------create---------------


    //    //------------sendto---------------
    QStringList sendto_lstMandatory;
    sendto_lstMandatory.append(SENDTO_PARAM_ADDRESS);
    sendto_lstMandatory.append(SENDTO_PARAM_DATAHEX);
    QStringList sendto_lstOptional;
    sendto_lstOptional.append(SENDTO_PARAM_AMOUNT);
    sendto_lstOptional.append(SENDTO_PARAM_GASLIMIT);
    sendto_lstOptional.append(SENDTO_PARAM_GASPRICE);
    sendto_lstOptional.append(SENDTO_PARAM_SENDER);
    QMap<QString, QString> sendto_lstTranslations;
    sendto_lstTranslations[SENDTO_PARAM_ADDRESS] = "";
    sendto_lstTranslations[SENDTO_PARAM_AMOUNT] = "";
    sendto_lstTranslations[SENDTO_PARAM_GASLIMIT] = "";
    sendto_lstTranslations[SENDTO_PARAM_GASPRICE] = "";
    sendto_lstTranslations[SENDTO_PARAM_SENDER] = "";

    m_execRPCCommand_sendto = new ExecRPCCommand(SENDTO_PRC_COMMAND, sendto_lstMandatory, sendto_lstOptional, sendto_lstTranslations, this);
    m_sendtoContractABI = new ContractABI();




    //    //------------sendto---------------


    //    //-------------call----------------
    QStringList CALL_lstMandatory;
    CALL_lstMandatory.append(CALL_PARAM_ADDRESS);
    CALL_lstMandatory.append(CALL_PARAM_DATAHEX);
    QStringList CALL_lstOptional;
    CALL_lstOptional.append(CALL_PARAM_SENDER);
    QMap<QString, QString> CALL_lstTranslations;
    CALL_lstTranslations[CALL_PARAM_ADDRESS] = "";
    CALL_lstTranslations[CALL_PARAM_SENDER] = "";

    m_execRPCCommand_call = new ExecRPCCommand(CALL_PRC_COMMAND, CALL_lstMandatory, CALL_lstOptional, CALL_lstTranslations, this);
    m_callContractABI = new ContractABI();




}

quint64 CreateContractPage::getDefaultGasLimitOpCreate()
{
    return DEFAULT_GAS_LIMIT_OP_CREATE;
}

quint64 CreateContractPage::getDefaultGasLimitOpSend()
{
    return DEFAULT_GAS_LIMIT_OP_SEND;
}


void CreateContractPage::on_newContractABI_call()
{
    //    std::string json_data = ui->call_abiText->toPlainText().toStdString();
    //    if(!m_contractABI_call->loads(json_data))
    //    {
    //        m_contractABI_call->clean();
    //       // ui->textEditInterface->setIsValidManually(false);
    //    }
    //    else
    //    {
    //        //ui->textEditInterface->setIsValidManually(true);
    //    }
    //    m_ABIFunctionField_call->setContractABI(m_contractABI_call);

    //    on_updateCallContractButton();
}

void CreateContractPage::on_updateCallContractButton()
{
    //    int func = m_ABIFunctionField_call->getSelectedFunction();
    //    bool enabled = true;
    //    if(ui->call_byteCodeText->toPlainText().isEmpty())
    //    {
    //        enabled = false;
    //    }
    //    enabled &= ui->tabview->currentIndex() == 2;

    //    ui->call_btn->setEnabled(enabled);
}


QString CreateContractPage::setParamsData(FunctionABI &function, QVariantList paramValues)
{
    QString params = "<b>Params:</b><br>";

    // Don't show empty list
    if(function.inputs.size() == 0)
    {
        return params;
    }

    // Add rows with params and values sent
    int i = 0;
    for(std::vector<ParameterABI>::const_iterator param = function.inputs.begin(); param != function.inputs.end(); ++param)
    {
        QString thisParam = "";


        thisParam += QString("&nbsp;&nbsp;&nbsp;<b>%2 %1</b><br>").arg(QString::fromStdString(param->name)).arg(QString::fromStdString(param->type));

        QStringList listValues = paramValues[i].toStringList();

        if(listValues.size() > 0)
        {
            for(int j = 0; j < listValues.count(); j++)
            {
                thisParam += QString("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%1<br>").arg(listValues[j]);
            }

        }
        i++;

        params += thisParam;
    }

    return params;

}


void CreateContractPage::callContractClicked(int functionIndex,
                                             QString contractAddress,
                                             QString senderAddress,
                                             QVariantList paramList)
{

    //    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;

    int func = m_callFunctionArray.at(functionIndex);


    FunctionABI function = m_callContractABI->functions[func];

    //    // Append params to the list
    ExecRPCCommand::appendParam(lstParams, CALL_PARAM_ADDRESS, contractAddress);
    ExecRPCCommand::appendParam(lstParams, CALL_PARAM_DATAHEX, toDataHex_Call(func, errorMessage ,paramList));
    ExecRPCCommand::appendParam(lstParams, CALL_PARAM_SENDER, senderAddress);

    //    // Execute RPC command line
    if(errorMessage.isEmpty() && m_execRPCCommand_call->exec(lstParams, result, resultJson, errorMessage))
    {
        QVariantMap variantMap = result.toMap();
        QString resultStr = "";
        QVariantMap executionResultMap = variantMap.value("executionResult").toMap();

        resultStr += "<b>address</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("address").toString() + "<br>";
        resultStr += "<b>Function</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QString::fromStdString(function.name) + "()<br>";
        resultStr += "<b>sender</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("sender").toString() + "<br>";

        resultStr += setParamsData(function,paramList);


        std::string rawData = executionResultMap.value("output").toString().toStdString();
        std::vector<std::vector<std::string>> values;
        std::vector<ParameterABI::ErrorType> errors;
        if(function.abiOut(rawData, values, errors))
        {
            QString Result = "<b>Result:</b><br>";

            if(values.size() > 0)
            {

                for(size_t i = 0; i < values.size(); i++)
                {
                    QString thisValue = "";

                    thisValue += QString("&nbsp;&nbsp;&nbsp;<b>%2 %1</b><br>").arg(QString::fromStdString(function.outputs[i].name)).arg(QString::fromStdString(function.outputs[i].type));

                    std::vector<std::string> listValues = values[i];
                    if(listValues.size() > 0)
                    {
                        for(size_t j = 0; j < listValues.size(); j++)
                        {
                            thisValue += QString("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%1<br>").arg(QString::fromStdString(listValues[j]));
                        }
                    }

                    Result += thisValue;
                }

                resultStr += Result;
            }
        }
        else
        {
            emit notifyContractResult("Call contract",true,function.errorMessage(errors, false),1,"");
            return;
        }



        m_lastResult = resultStr;


        emit notifyContractResult("Call contract",false,errorMessage,1,resultStr);

    }
    else
    {
        emit notifyContractResult("Call contract",true,errorMessage,1,"");

    }


}

QString CreateContractPage::gethash160AddressFromGKCAddress(QString gkcAddress)
{
    CBitcoinAddress dest(gkcAddress.toStdString());
    if (!dest.IsValid()) {
        return "";
    }

    CKeyID keyID;
    dest.GetKeyID(keyID);

    return QString::fromStdString(keyID.ToStringReverseEndian());
}


QString CreateContractPage::getBCKDatas(int mode,QString senderAddress)
{
    return "";


}

bool CreateContractPage::isValidContractAddress()
{
    return true;
    // ui->sendto_byteCodeText->checkValidity();
    // return ui->lineEditContractAddress->isValid();
}

void CreateContractPage::on_contractAddressChanged()
{
    //    if(isValidContractAddress() && m_contractModel)
    //    {
    //        QString contractAddress = ui->sendto_byteCodeText->toPlainText();
    //        if(m_contractModel->lookupAddress(contractAddress) > -1)
    //        {
    //            QString contractAbi = m_contractModel->abiForAddress(contractAddress);
    //            if(ui->sendto_abiText->toPlainText() != contractAbi)
    //            {
    //                ui->sendto_abiText->setText(m_contractModel->abiForAddress(contractAddress));
    //            }
    //        }
    //    }
}

void CreateContractPage::on_functionChanged()
{
    //    bool payable = isFunctionPayable();
    //    ui->sendto_amountText->setEnabled(payable);
    //    if(!payable)
    //    {
    //        ui->sendto_amountText->clear();
    //    }
}


void CreateContractPage::on_updateSendToContractButton()
{
    //    int func = m_ABIFunctionField_sendto->getSelectedFunction();
    //    bool enabled = true;
    //    if(ui->sendto_byteCodeText->toPlainText().isEmpty())
    //    {
    //        enabled = false;
    //    }
    //    enabled &= ui->tabview->currentIndex() == 1;

    //    ui->sendto_btn->setEnabled(enabled);
}


QString CreateContractPage::toDataHex_Sendto(int func, QString& errorMessage, QVariantList &paramList, bool forBCK)
{
    if(func == -1)
    {
        std::string defSelector = FunctionABI::defaultSelector();
        return QString::fromStdString(defSelector);
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = getValuesVector(paramList);


    FunctionABI function = forBCK?m_bckSendToContractABI->functions[func]:m_sendtoContractABI->functions[func];

    std::vector<ParameterABI::ErrorType> errors;

    if(function.abiIn(values, strData, errors))
    {
        return QString::fromStdString(strData);
    }
    else
    {
        errorMessage = function.errorMessage(errors, true);
    }

    return "";
}


QString CreateContractPage::toDataHex_Call(int func, QString& errorMessage,QVariantList &paramList,bool forBCK)
{
    if(func == -1)
    {
        return "";
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = getValuesVector(paramList);

    FunctionABI function = forBCK?m_bckCallContractABI->functions[func]:m_callContractABI->functions[func];

    std::vector<ParameterABI::ErrorType> errors;
    if(function.abiIn(values, strData, errors))
    {

        return QString::fromStdString(strData);
    }
    else
    {
        errorMessage = function.errorMessage(errors, true);
    }
    return "";
}


std::vector<std::vector<std::string>> CreateContractPage::getValuesVector(QVariantList &paramList)
{
    std::vector<std::vector<std::string>> result;
    for (int i=0; i<paramList.size(); i++)
    {
        std::vector<std::string> itemParam;
        QVariantList qlistVlaues = paramList[i].value<QVariantList>();

        for(int j=0; j<qlistVlaues.size(); j++)
        {
            itemParam.push_back(qlistVlaues.at(j).toString().toUtf8().data());
        }
        result.push_back(itemParam);
    }


    return result;
}

void CreateContractPage::bckFunctions(int mode,
                                      QString gasLimitStr,
                                      int gasPriceUint,
                                      QString gasPriceStr,
                                      QString amountStr,
                                      int amountUint,
                                      QString senderAddress,
                                      QVariantList paramList)
{


    WalletModel::UnlockContext ctx(m_model->requestUnlock());
    if(!ctx.isValid())
    {
        return;
    }

    //    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int unit = m_model->getOptionsModel()->getDisplayUnit();
    uint64_t gasLimit = gasLimitStr.toULongLong();
    CAmount gasPrice = 0;
    BitcoinUnits::parse(gasPriceUint, gasPriceStr, &gasPrice);
    int functionIndex = -1;


    CAmount amount_ = 0;
    BitcoinUnits::parse(amountUint, amountStr, &amount_);


    QString resStr = "";

    if(mode == 0)
        resStr = "buy\\(.*";
    else if(mode == 1)
    {
        resStr = "sell\\(.*";
        QVariant var(paramList.at(0).toString());
        QVariantList list_in;
        list_in.append(var);
        QVariant var2(list_in);
        paramList.clear();
        paramList.append(var2);
    }
    else if(mode == 2)
        resStr = "transfer\\(.*";
    else if(mode == 3)
        resStr = "transferFrom\\(.*";
    else
        return;



    functionIndex = m_bckSendToFunctionList.indexOf(QRegExp(resStr));


    int func = m_bckSendToFunctionArray.at(functionIndex);


    //    // Check for high gas price
    if(gasPrice > HIGH_GASPRICE)
    {
        QString message = tr("The Gas Price is too high, are you sure you want to possibly spend a max of %1 for this transaction?");
        emit notifyBCKResult("High Gas price",true,message.arg(BitcoinUnits::formatWithUnit(unit, gasLimit * gasPrice)),1,"");
    }


    //    // Append params to the list
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_ADDRESS, QString::fromStdString(Params().getBCKContractAddress()));

    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_DATAHEX, toDataHex_Sendto(func, errorMessage,paramList,true));

    QString amount = isFunctionPayable(functionIndex,true) ? BitcoinUnits::format(0, amount_, false, BitcoinUnits::separatorNever) : "0";

    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_AMOUNT, amount);


    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_GASLIMIT, QString::number(gasLimit));

    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_GASPRICE, BitcoinUnits::format(0, gasPrice, false, BitcoinUnits::separatorNever));

    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_SENDER, senderAddress);


    //        // Execute RPC command line
    if(errorMessage.isEmpty() && m_execRPCCommand_sendto->exec(lstParams, result, resultJson, errorMessage))
    {
        QVariantMap variantMap = result.toMap();
        QString resultStr = "";

        resultStr += "<b>txid</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("txid").toString() + "<br>";
        resultStr += "<b>sender</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("sender").toString() + "<br>";
        resultStr += "<b>hash160</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("hash160").toString() + "<br>";

        m_lastResult = resultStr;


        emit notifyBCKResult("Result",false,errorMessage,1,resultStr);

    }
    else
    {
        emit notifyBCKResult("Result",true,errorMessage,1,"");

    }



}




void CreateContractPage::sendToContractClicked(int functionIndex,
                                               QString gasLimitStr,
                                               int gasPriceUint,
                                               QString gasPriceStr,
                                               QString contractAddress,
                                               QString amountStr,
                                               int amountUint,
                                               QString senderAddress,
                                               QVariantList paramList
                                               )
{

    WalletModel::UnlockContext ctx(m_model->requestUnlock());
    if(!ctx.isValid())
    {
        return;
    }

    //    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int unit = m_model->getOptionsModel()->getDisplayUnit();
    uint64_t gasLimit = gasLimitStr.toULongLong();
    CAmount gasPrice = 0;
    BitcoinUnits::parse(gasPriceUint, gasPriceStr, &gasPrice);

    CAmount amount_ = 0;
    BitcoinUnits::parse(amountUint, amountStr, &amount_);

    int func = m_sendtoFunctionArray.at(functionIndex);


    //    // Check for high gas price
    if(gasPrice > HIGH_GASPRICE)
    {
        QString message = tr("The Gas Price is too high, are you sure you want to possibly spend a max of %1 for this transaction?");
        emit notifyContractResult("High Gas price",true,message.arg(BitcoinUnits::formatWithUnit(unit, gasLimit * gasPrice)),1,"");
    }

    //    // Append params to the list
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_ADDRESS, contractAddress);
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_DATAHEX, toDataHex_Sendto(func, errorMessage,paramList));
    QString amount = isFunctionPayable(functionIndex) ? BitcoinUnits::format(0, amount_, false, BitcoinUnits::separatorNever) : "0";
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_AMOUNT, amount);
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_GASLIMIT, QString::number(gasLimit));
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_GASPRICE, BitcoinUnits::format(0, gasPrice, false, BitcoinUnits::separatorNever));
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_SENDER, senderAddress);


    //        // Execute RPC command line
    if(errorMessage.isEmpty() && m_execRPCCommand_sendto->exec(lstParams, result, resultJson, errorMessage))
    {
        QVariantMap variantMap = result.toMap();
        QString resultStr = "";

        resultStr += "<b>txid</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("txid").toString() + "<br>";
        resultStr += "<b>sender</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("sender").toString() + "<br>";
        resultStr += "<b>hash160</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("hash160").toString() + "<br>";

        m_lastResult = resultStr;

        emit notifyContractResult("Send to contract",false,errorMessage,1,resultStr);

    }
    else
    {
        emit notifyContractResult("Send to contract",true,errorMessage,1,"");

    }


}

QString CreateContractPage::getLastResult()
{
    return m_lastResult;
}



CreateContractPage::~CreateContractPage()
{
    //    delete ui;
}

bool CreateContractPage::updateContractABI(int mode,const QString &abiStr)
{
    ContractABI* m_contractABI;
    QStringList *functionList;
    QVector<int> *functionArray;
    switch (mode) {
    case Create:
        m_contractABI = m_createContractABI;
        functionList = &m_createFunctionList;
        functionArray = &m_createFunctionArray;
        break;
    case SendTo:
        m_contractABI = m_sendtoContractABI;
        functionList = &m_sendtoFunctionList;
        functionArray = &m_sendtoFunctionArray;
        break;
    case Call:
        m_contractABI = m_callContractABI;
        functionList = &m_callFunctionList;
        functionArray = &m_callFunctionArray;
        break;
    case BCKCall:
        m_contractABI = m_bckCallContractABI;
        functionList = &m_bckCallFunctionList;
        functionArray = &m_bckCallFunctionArray;
        break;
    case BCKSendTo:
        m_contractABI = m_bckSendToContractABI;
        functionList = &m_bckSendToFunctionList;
        functionArray = &m_bckSendToFunctionArray;
        break;

    default:
        break;
    }



    std::string json_data = abiStr.toStdString();
    if(!m_contractABI->loads(json_data))
    {
        functionList->clear();
        m_contractABI->clean();

        if(mode != BCKCall && mode != BCKSendTo)
        {
            emit createFunctionListChanged();
            emit sendtoFunctionListChanged();
            emit callFunctionListChanged();
        }

        return false;
    }
    else
    {

        RenewContractFunctions(m_contractABI,mode == BCKCall?Call:(mode == BCKSendTo?SendTo:(ContractMode)mode),functionList,functionArray, (mode == BCKCall || mode == BCKSendTo)?true:false);
        return true;
    }

    //   m_ABIFunctionField->setContractABI(m_contractABI);
}


void CreateContractPage::RenewContractFunctions(ContractABI* abi,ContractMode mode,QStringList *functionList,QVector<int> *functionArray,bool forBCK)
{
    if(abi != NULL)
    {
        // Populate the control with functions


        std::vector<FunctionABI> functions = abi->functions;
        functionList->clear();
        functionArray->clear();
        bool bFieldCreate = mode == Create;
        bool bFieldCall = mode == Call;
        bool bFieldSendTo = mode == SendTo;
        bool bFieldFunc = bFieldCall || bFieldSendTo;
        for (int func = 0; func < (int)functions.size(); ++func)
        {
            const FunctionABI &function = functions[func];
            bool bTypeConstructor = function.type == "constructor";
            bool bTypeEvent = function.type == "event";
            bool bTypeDefault = function.type == "default";
            bool bIsConstant = function.constant;
            if((bFieldCreate && !bTypeConstructor) ||
                    (bFieldFunc && bTypeConstructor) ||
                    (bFieldFunc && bTypeEvent) ||
                    (bFieldCall && !bIsConstant && !bTypeDefault) ||
                    (bFieldSendTo && bIsConstant && !bTypeDefault))
            {
                continue;
            }

            //            ABIParamsField *abiParamsField = new ABIParamsField(m_platformStyle, this);
            //            abiParamsField->updateParamsField(function);

            //            m_paramsField->addWidget(abiParamsField);
            QString funcName = QString::fromStdString(function.name);
            QString funcSelector = QString::fromStdString(function.selector());

            functionList->append(QString(funcName + "(" + funcSelector + ")"));

            functionArray->append(func);

        }


        if(!forBCK)
        {
            emit createFunctionListChanged();
            emit sendtoFunctionListChanged();
            emit callFunctionListChanged();
        }
    }


}


bool CreateContractPage::isFunctionPayable(int index,bool forBCK)
{
    if(index < 0) return true;

    std::vector<FunctionABI> functions = forBCK?m_bckSendToContractABI->functions: m_sendtoContractABI->functions;

    const FunctionABI &function = functions[forBCK?m_bckSendToFunctionArray.at(index):m_sendtoFunctionArray.at(index)];

    return function.payable;
}


QVariantList CreateContractPage::functionChanged(int mode,int currentIndex)
{
    QVariantList functionParamList;

    QVariantList paramNameList;
    QVariantList paramTypeList;
    QVariantList paramExpandingList;
    QVariantList paramInputCountList;
    QVariantList paramRegexpStrList;
    QVector<int> *functionArray;


    if(currentIndex == -1) return QVariantList();

    std::vector<FunctionABI> functions;
    ContractABI* abi;

    switch (mode)
    {
    case Create:
        abi = m_createContractABI;
        functionArray = &m_createFunctionArray;
        break;
    case SendTo:
        abi = m_sendtoContractABI;
        functionArray = &m_sendtoFunctionArray;
        break;
    case Call:
        abi = m_callContractABI;
        functionArray = &m_callFunctionArray;
        break;
    default:
        break;
    }

    functions = abi->functions;

    if(functionArray->size()==0)
        return QVariantList();

    const FunctionABI &function = functions[functionArray->at(currentIndex)];



    for(std::vector<ParameterABI>::const_iterator param = function.inputs.begin(); param != function.inputs.end(); ++param)
    {
        ParameterABI param_ = *param;

        std::tuple<std::string, std::string, bool, int, std::string> tuple_ = ParseParams(param_);

        paramNameList.append(QString::fromStdString(static_cast<std::string>(std::get<0>(tuple_))));
        paramTypeList.append(QString::fromStdString(static_cast<std::string>(std::get<1>(tuple_))));
        paramExpandingList.append(std::get<2>(tuple_));
        paramInputCountList.append(std::get<3>(tuple_));
        paramRegexpStrList.append(QString::fromStdString(static_cast<std::string>(std::get<4>(tuple_))));

    }


    functionParamList.append(paramNameList);
    functionParamList.append(paramTypeList);
    functionParamList.append(paramExpandingList);
    functionParamList.append(paramInputCountList);
    functionParamList.append(paramRegexpStrList);


    return functionParamList;
}


std::tuple<std::string, std::string, bool, int, std::string> CreateContractPage::ParseParams(const ParameterABI &param)
{
    std::string paramName = param.name;
    std::string paramType = param.type;
    bool expanding = false;
    int inputCount = 0;
    std::string regexpStr = "";



    if(param.decodeType().isList())
    {
        if(param.decodeType().isDynamic())
        {
            expanding = true;
        }
        else
        {
            //fixed array e.g.:byte32[3]
            inputCount = param.decodeType().length();
        }
    }

    regexpStr =  ParameterABI::getRegularExpession(param.decodeType()).toStdString();


    std::tuple<std::string, std::string, bool, int, std::string> tuple_ = std::make_tuple(paramName,paramType,expanding,inputCount,regexpStr);

    return tuple_;
}





void CreateContractPage::on_updateCreateButton()
{

}

QString CreateContractPage::toDataHex(int func, QString& errorMessage, QVariantList &paramList)
{
    if(func == -1)
    {
        return "";
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = getValuesVector(paramList);

    FunctionABI function = m_createContractABI->functions[func];

    std::vector<ParameterABI::ErrorType> errors;
    if(function.abiIn(values, strData, errors))
    {
        return QString::fromStdString(strData);
    }
    else
    {
        errorMessage = function.errorMessage(errors, true);
    }
    return "";
}


void CreateContractPage::createContractClicked(QString gasLimitStr,
                                               int gasPriceUint,
                                               QString gasPriceStr,
                                               QString byteCode,
                                               QString senderAddress,
                                               QVariantList paramList
                                               )
{

    WalletModel::UnlockContext ctx(m_model->requestUnlock());
    if(!ctx.isValid())
    {
        return;
    }

    //    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int unit = m_model->getOptionsModel()->getDisplayUnit();



    uint64_t gasLimit = gasLimitStr.toULongLong();
    CAmount gasPrice = 0;
    BitcoinUnits::parse(gasPriceUint, gasPriceStr, &gasPrice);





    //FixMe: check if this convert has overflow problem?
    int func = m_createFunctionArray.size() > 0 ? m_createFunctionArray.at(0) : -1;


    if(gasPrice > HIGH_GASPRICE)
    {
        QString message = tr("The Gas Price is too high, are you sure you want to possibly spend a max of %1 for this transaction?");
        emit notifyContractResult("High Gas price",true,message.arg(BitcoinUnits::formatWithUnit(unit, gasLimit * gasPrice)),1,"");
    }



    //    // Append params to the list
	byteCode = byteCode.trimmed();
    byteCode = byteCode + toDataHex(func, errorMessage,paramList);


    ExecRPCCommand::appendParam(lstParams, PARAM_BYTECODE, byteCode);
    ExecRPCCommand::appendParam(lstParams, PARAM_GASLIMIT, QString::number(gasLimit));
    ExecRPCCommand::appendParam(lstParams, PARAM_GASPRICE, BitcoinUnits::format(0, gasPrice, false, BitcoinUnits::separatorNever));
    ExecRPCCommand::appendParam(lstParams, PARAM_SENDER, senderAddress);


    //        // Execute RPC command line
    if(errorMessage.isEmpty() && m_execRPCCommand->exec(lstParams, result, resultJson, errorMessage))
    {
        QVariantMap variantMap = result.toMap();
        QString resultStr = "";

        resultStr += "<b>txid</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("txid").toString() + "<br>";
        resultStr += "<b>sender</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("sender").toString() + "<br>";
        resultStr += "<b>hash160</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("hash160").toString() + "<br>";
        resultStr += "<b>address</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + variantMap.value("address").toString() + "<br>";

        m_lastResult = resultStr;

        emit notifyContractResult("Create contract",false,errorMessage,1,resultStr);

    }
    else
    {
        emit notifyContractResult("Create contract",true,errorMessage,1,"");

    }

}
