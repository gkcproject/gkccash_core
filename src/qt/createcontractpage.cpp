#include "createcontractpage.h"
#include "ui_createcontractpage.h"
#include <qt/walletmodel.h>
#include <qt/clientmodel.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/abifunctionfield.h>

#include <qt/execrpccommand.h>
#include <qt/contractabi.h>

#include <qt/bitcoinunits.h>
#include <qt/sendcoinsdialog.h>
#include <qt/contractresult.h>

#include <QDebug>

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

CreateContractPage::CreateContractPage(QWidget *parent) : QWidget(parent),ui(new Ui::CreateContractPage)
{
    ui->setupUi(this);
    std::string platformName;
    platformStyle = PlatformStyle::instantiate(QString::fromStdString(platformName));
    if (!platformStyle)
        platformStyle = PlatformStyle::instantiate("other");

    assert(platformStyle);

    //------------create---------------
    QStringList lstMandatory;
    lstMandatory.append(PARAM_BYTECODE);
    QStringList lstOptional;
    lstOptional.append(PARAM_GASLIMIT);
    lstOptional.append(PARAM_GASPRICE);
    lstOptional.append(PARAM_SENDER);
    QMap<QString, QString> lstTranslations;
    lstTranslations[PARAM_BYTECODE] = ui->create_byteCodeText->toPlainText();
    lstTranslations[PARAM_GASLIMIT] = ui->create_gasLimitSpinBox->text();
    lstTranslations[PARAM_GASPRICE] = ui->create_gasPriceEdit->text();
    lstTranslations[PARAM_SENDER] = ui->create_senderAddress->text();
    m_ABIFunctionField = new ABIFunctionField(platformStyle, ABIFunctionField::Create, ui->create_scrollArea);
    ui->create_scrollArea->setWidget(m_ABIFunctionField);

    m_execRPCCommand = new ExecRPCCommand(PRC_COMMAND, lstMandatory, lstOptional, lstTranslations, this);
    m_contractABI = new ContractABI();

    connect(ui->create_btn, SIGNAL(clicked()), SLOT(on_createContractClicked()));
    connect(ui->create_byteCodeText, SIGNAL(textChanged()), SLOT(on_updateCreateButton()));
    connect(ui->create_abiText, SIGNAL(textChanged()), SLOT(on_newContractABI()));
    //------------create---------------


    //------------sendto---------------
    QStringList sendto_lstMandatory;
    sendto_lstMandatory.append(SENDTO_PARAM_ADDRESS);
    sendto_lstMandatory.append(SENDTO_PARAM_DATAHEX);
    QStringList sendto_lstOptional;
    sendto_lstOptional.append(SENDTO_PARAM_AMOUNT);
    sendto_lstOptional.append(SENDTO_PARAM_GASLIMIT);
    sendto_lstOptional.append(SENDTO_PARAM_GASPRICE);
    sendto_lstOptional.append(SENDTO_PARAM_SENDER);
    QMap<QString, QString> sendto_lstTranslations;
    sendto_lstTranslations[SENDTO_PARAM_ADDRESS] = ui->sendto_byteCodeText->toPlainText();
    sendto_lstTranslations[SENDTO_PARAM_AMOUNT] = ui->sendto_amountText->text();
    sendto_lstTranslations[SENDTO_PARAM_GASLIMIT] = ui->sendto_gasLimitSpinBox->text();
    sendto_lstTranslations[SENDTO_PARAM_GASPRICE] = ui->sendto_gasPriceEdit->text();
    sendto_lstTranslations[SENDTO_PARAM_SENDER] = ui->sendto_senderAddress->text();
    m_ABIFunctionField_sendto = new ABIFunctionField(platformStyle, ABIFunctionField::SendTo, ui->sendto_scrollArea);

    ui->sendto_scrollArea->setWidget(m_ABIFunctionField_sendto);

    m_execRPCCommand_sendto = new ExecRPCCommand(SENDTO_PRC_COMMAND, sendto_lstMandatory, sendto_lstOptional, sendto_lstTranslations, this);
    m_contractABI_sendto = new ContractABI();


    connect(ui->sendto_btn, SIGNAL(clicked()), SLOT(on_sendToContractClicked()));
    connect(ui->sendto_byteCodeText, SIGNAL(textChanged(QString)), SLOT(on_updateSendToContractButton()));
    connect(ui->sendto_abiText, SIGNAL(textChanged()), SLOT(on_newContractABI_sendto()));
    connect(m_ABIFunctionField_sendto, SIGNAL(functionChanged()), SLOT(on_functionChanged()));
    connect(ui->sendto_byteCodeText, SIGNAL(textChanged(QString)), SLOT(on_contractAddressChanged()));


    //------------sendto---------------


    //-------------call----------------
    QStringList CALL_lstMandatory;
    CALL_lstMandatory.append(CALL_PARAM_ADDRESS);
    CALL_lstMandatory.append(CALL_PARAM_DATAHEX);
    QStringList CALL_lstOptional;
    CALL_lstOptional.append(CALL_PARAM_SENDER);
    QMap<QString, QString> CALL_lstTranslations;
    CALL_lstTranslations[CALL_PARAM_ADDRESS] = ui->call_byteCodeText->toPlainText();
    CALL_lstTranslations[CALL_PARAM_SENDER] = ui->call_senderAddress->text();



    m_ABIFunctionField_call = new ABIFunctionField(platformStyle, ABIFunctionField::Call, ui->call_scrollArea);
    ui->call_scrollArea->setWidget(m_ABIFunctionField_call);
    m_execRPCCommand_call = new ExecRPCCommand(CALL_PRC_COMMAND, CALL_lstMandatory, CALL_lstOptional, CALL_lstTranslations, this);
    m_contractABI_call = new ContractABI();


    connect(ui->call_btn, SIGNAL(clicked()), SLOT(on_callContractClicked()));
    connect(ui->call_byteCodeText, SIGNAL(textChanged(QString)), SLOT(on_updateCallContractButton()));
    connect(ui->call_abiText, SIGNAL(textChanged()), SLOT(on_newContractABI_call()));

    //-------------call----------------





}


void CreateContractPage::on_newContractABI_call()
{
    std::string json_data = ui->call_abiText->toPlainText().toStdString();
    if(!m_contractABI_call->loads(json_data))
    {
        m_contractABI_call->clean();
       // ui->textEditInterface->setIsValidManually(false);
    }
    else
    {
        //ui->textEditInterface->setIsValidManually(true);
    }
    m_ABIFunctionField_call->setContractABI(m_contractABI_call);

    on_updateCallContractButton();
}

void CreateContractPage::on_updateCallContractButton()
{
    int func = m_ABIFunctionField_call->getSelectedFunction();
    bool enabled = true;
    if(ui->call_byteCodeText->toPlainText().isEmpty())
    {
        enabled = false;
    }
    enabled &= ui->tabview->currentIndex() == 2;

    ui->call_btn->setEnabled(enabled);
}


void CreateContractPage::on_callContractClicked()
{

    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int func = m_ABIFunctionField_call->getSelectedFunction();

    // Append params to the list
    ExecRPCCommand::appendParam(lstParams, CALL_PARAM_ADDRESS, ui->call_byteCodeText->toPlainText());
    ExecRPCCommand::appendParam(lstParams, CALL_PARAM_DATAHEX, toDataHex_Call(func, errorMessage));
    ExecRPCCommand::appendParam(lstParams, CALL_PARAM_SENDER, ui->call_senderAddress->text());

    // Execute RPC command line
    if(errorMessage.isEmpty() && m_execRPCCommand_call->exec(lstParams, result, resultJson, errorMessage))
    {

        QMessageBox::information(this, tr("Result"), resultJson);

//        ContractResult *widgetResult = new ContractResult(NULL);
//        widgetResult->setResultData(result, m_contractABI_call->functions[func], m_ABIFunctionField_call->getParamsValues(), ContractResult::CallResult);


//        int position = ui->tabview->count() - 1;
//        m_results_call = position == 1 ? 1 : m_results_call + 1;

//        ui->tabview->addTab(widgetResult,tr("Result %1").arg(m_results_call));
//        ui->tabview->setCurrentIndex(position);




    }
    else
    {
        QMessageBox::warning(this, tr("Call contract"), errorMessage);
    }

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
    bool payable = isFunctionPayable();
    ui->sendto_amountText->setEnabled(payable);
    if(!payable)
    {
        ui->sendto_amountText->clear();
    }
}

bool CreateContractPage::isFunctionPayable()
{
    int func = m_ABIFunctionField_sendto->getSelectedFunction();
    if(func < 0) return true;
    FunctionABI function = m_contractABI_sendto->functions[func];
    return function.payable;
}

void CreateContractPage::on_updateSendToContractButton()
{
    int func = m_ABIFunctionField_sendto->getSelectedFunction();
    bool enabled = true;
    if(ui->sendto_byteCodeText->toPlainText().isEmpty())
    {
        enabled = false;
    }
    enabled &= ui->tabview->currentIndex() == 1;

    ui->sendto_btn->setEnabled(enabled);
}


QString CreateContractPage::toDataHex_Sendto(int func, QString& errorMessage)
{
    if(func == -1 || m_ABIFunctionField_sendto == NULL || m_contractABI_sendto == NULL)
    {
        std::string defSelector = FunctionABI::defaultSelector();
        return QString::fromStdString(defSelector);
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = m_ABIFunctionField_sendto->getValuesVector();
    FunctionABI function = m_contractABI_sendto->functions[func];
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


QString CreateContractPage::toDataHex_Call(int func, QString& errorMessage)
{
    if(func == -1 || m_ABIFunctionField_call == NULL || m_contractABI_call == NULL)
    {
        return "";
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = m_ABIFunctionField_call->getValuesVector();
    FunctionABI function = m_contractABI_call->functions[func];
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

void CreateContractPage::on_sendToContractClicked()
{

    WalletModel::UnlockContext ctx(m_model->requestUnlock());
    if(!ctx.isValid())
    {
        return;
    }

    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int unit = m_model->getOptionsModel()->getDisplayUnit();
    uint64_t gasLimit = ui->sendto_gasLimitSpinBox->value();
    CAmount gasPrice = (int64_t)(ui->sendto_gasPriceEdit->text().toDouble()*COIN);
    int func = m_ABIFunctionField_sendto->getSelectedFunction();

    // Check for high gas price
    if(gasPrice > HIGH_GASPRICE)
    {
        QString message = tr("The Gas Price is too high, are you sure you want to possibly spend a max of %1 for this transaction?");
        if(QMessageBox::question(this, tr("High Gas price"), message.arg(BitcoinUnits::formatWithUnit(unit, gasLimit * gasPrice))) == QMessageBox::No)
            return;
    }

    // Append params to the list
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_ADDRESS, ui->sendto_byteCodeText->toPlainText());
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_DATAHEX, toDataHex_Sendto(func, errorMessage));
    QString amount = isFunctionPayable() ? BitcoinUnits::format(unit, (int64_t)(ui->sendto_amountText->text().toDouble()*COIN), false, BitcoinUnits::separatorNever) : "0";
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_AMOUNT, amount);
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_GASLIMIT, QString::number(gasLimit));
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_GASPRICE, BitcoinUnits::format(unit, gasPrice, false, BitcoinUnits::separatorNever));
    ExecRPCCommand::appendParam(lstParams, SENDTO_PARAM_SENDER, ui->sendto_senderAddress->text());

    QString questionString = tr("Are you sure you want to send to the contract: <br /><br />");
    questionString.append(tr("<b>%1</b>?")
                          .arg(ui->sendto_byteCodeText->toPlainText()));

    SendConfirmationDialog confirmationDialog(tr("Confirm sending to contract."), questionString, 3, this);
    confirmationDialog.exec();
    QMessageBox::StandardButton retval = (QMessageBox::StandardButton)confirmationDialog.result();
    if(retval == QMessageBox::Yes)
    {
        // Execute RPC command line
        if(errorMessage.isEmpty() && m_execRPCCommand_sendto->exec(lstParams, result, resultJson, errorMessage))
        {

            QMessageBox::information(this, tr("Result"), resultJson);

//            ContractResult *widgetResult = new ContractResult(NULL);
//            widgetResult->setResultData(result, FunctionABI(), m_ABIFunctionField_sendto->getParamsValues(), ContractResult::SendToResult);

//            int position = ui->tabview->count() - 1;
//            m_results_sendto = position == 1 ? 1 : m_results_sendto + 1;

//            ui->tabview->addTab(widgetResult,tr("Result %1").arg(m_results_sendto));
//            ui->tabview->setCurrentIndex(position);

        }
        else
        {
            QMessageBox::warning(this, tr("Send to contract"), errorMessage);
        }
    }

}



CreateContractPage::~CreateContractPage()
{
    delete ui;
}

void CreateContractPage::on_newContractABI()
{
    QColor red("red");
    QColor gray("#303030");
    qDebug()<<__func__<<" 1";

    std::string json_data = ui->create_abiText->toPlainText().toStdString();
    if(!m_contractABI->loads(json_data))
    {
        m_contractABI->clean();
        // ui->create_abiText->setTextBackgroundColor(red);
        qDebug()<<__func__<<" 2";

    }
    else
    {
        qDebug()<<__func__<<" 3";

        //  ui->create_abiText->setTextBackgroundColor(gray);
    }
    qDebug()<<__func__<<" 4";

    m_ABIFunctionField->setContractABI(m_contractABI);
    qDebug()<<__func__<<" 5";


    on_updateCreateButton();
}

void CreateContractPage::on_newContractABI_sendto()
{
    QColor red("red");
    QColor gray("#303030");
    qDebug()<<__func__<<" 1";

    std::string json_data = ui->sendto_abiText->toPlainText().toStdString();
    if(!m_contractABI_sendto->loads(json_data))
    {
        m_contractABI_sendto->clean();
        // ui->create_abiText->setTextBackgroundColor(red);
        qDebug()<<__func__<<" 2";

    }
    else
    {
        qDebug()<<__func__<<" 3";

        //  ui->create_abiText->setTextBackgroundColor(gray);
    }
    qDebug()<<__func__<<" 4";

    m_ABIFunctionField_sendto->setContractABI(m_contractABI_sendto);
    qDebug()<<__func__<<" 5";


    on_updateSendToContractButton();
}





void CreateContractPage::on_updateCreateButton()
{
    qDebug()<<__func__<<" 1";

    bool enabled = true;
    if(ui->create_byteCodeText->toPlainText().isEmpty())
    {
        enabled = false;
    }
    qDebug()<<__func__<<" 2";

    enabled &= ui->tabview->currentIndex() == 0;
    qDebug()<<__func__<<" 3";

    ui->create_btn->setEnabled(enabled);
}

void CreateContractPage::setModel(WalletModel *_model)
{
    m_model = _model;
    // m_contractModel = m_model->getContractTableModel();

}

QString CreateContractPage::toDataHex(int func, QString& errorMessage)
{
    if(func == -1 || m_ABIFunctionField == NULL || m_contractABI == NULL)
    {
        return "";
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = m_ABIFunctionField->getValuesVector();
    FunctionABI function = m_contractABI->functions[func];
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


void CreateContractPage::on_createContractClicked()
{
    qDebug()<<__func__<<" on_createContractClicked";

    WalletModel::UnlockContext ctx(m_model->requestUnlock());
    if(!ctx.isValid())
    {
        return;
    }

    qDebug()<<__func__<<" 1";
    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int unit = m_model->getOptionsModel()->getDisplayUnit();

    qDebug()<<__func__<<" 2";

    uint64_t gasLimit = ui->create_gasLimitSpinBox->value();

    qDebug()<<__func__<<" gasLimit:"<<gasLimit;

    //FixMe: check if this convert has overflow problem?
    CAmount gasPrice = (int64_t)(ui->create_gasPriceEdit->text().toDouble()*COIN);

    qDebug()<<__func__<<" gasPrice:"<<gasPrice;

    int func = m_ABIFunctionField->getSelectedFunction();

    qDebug()<<__func__<<" func:"<<func;

    // Check for high gas price
    if(gasPrice > HIGH_GASPRICE)
    {
        QString message = tr("The Gas Price is too high, are you sure you want to possibly spend a max of %1 for this transaction?");
        if(QMessageBox::question(this, tr("High Gas price"), message.arg(BitcoinUnits::formatWithUnit(unit, gasLimit * gasPrice))) == QMessageBox::No)
            return;
    }



    // Append params to the list
    QString bytecode = ui->create_byteCodeText->toPlainText() + toDataHex(func, errorMessage);

    qDebug()<<__func__<<" bytecode:"<<bytecode;


    ExecRPCCommand::appendParam(lstParams, PARAM_BYTECODE, bytecode);
    ExecRPCCommand::appendParam(lstParams, PARAM_GASLIMIT, QString::number(gasLimit));
    ExecRPCCommand::appendParam(lstParams, PARAM_GASPRICE, BitcoinUnits::format(unit, gasPrice, false, BitcoinUnits::separatorNever));
    ExecRPCCommand::appendParam(lstParams, PARAM_SENDER, ui->create_senderAddress->text());


    qDebug()<<__func__<<" lstParams:"<<lstParams;

    QString questionString = tr("Are you sure you want to create contract? <br />");



    SendConfirmationDialog confirmationDialog(tr("Confirm contract creation."), questionString, 3, this);
    confirmationDialog.exec();
    QMessageBox::StandardButton retval = (QMessageBox::StandardButton)confirmationDialog.result();
    if(retval == QMessageBox::Yes)
    {
        // Execute RPC command line
        if(errorMessage.isEmpty() && m_execRPCCommand->exec(lstParams, result, resultJson, errorMessage))
        {
            qDebug()<<__func__<<" errorMessage:"<<errorMessage;


            QMessageBox::information(this, tr("Result"), resultJson);


//            ContractResult *widgetResult = new ContractResult(NULL);

//            qDebug()<<__func__<<" widgetResult:"<<result;

//            widgetResult->setResultData(result, FunctionABI(), QList<QStringList>(), ContractResult::CreateResult);
//            //ui->stackedWidget->addWidget(widgetResult);
//            int position = ui->tabview->count() - 1;
//            m_results = position == 1 ? 1 : m_results + 1;

//            qDebug()<<__func__<<" m_results:"<<m_results;

//            ui->tabview->addTab(widgetResult,tr("Result %1").arg(m_results));

//            qDebug()<<__func__<<" add:";

//            ui->tabview->setCurrentIndex(position);

//            qDebug()<<__func__<<" end:";

        }
        else
        {
            QMessageBox::warning(this, tr("Create contract"), errorMessage);
        }
    }

}
