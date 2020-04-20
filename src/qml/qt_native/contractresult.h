#ifndef CONTRACTRESULT_H
#define CONTRACTRESULT_H

#include <QObject>

class FunctionABI;


class ContractResult: public QObject
{
    Q_OBJECT

public:
    enum ContractTxType{
        CreateResult,
        SendToResult,
        CallResult
    };
    explicit ContractResult();
    ~ContractResult();
    void setResultData(QVariant result, FunctionABI function, QList<QStringList> paramValues, ContractTxType type);

private:
   // Ui::ContractResult *ui;
//    void setParamsData(FunctionABI function, QList<QStringList> paramValues);
//    void updateCreateResult(QVariant result);
//    void updateSendToResult(QVariant result);
//    void updateCallResult(QVariant result, FunctionABI function, QList<QStringList> paramValues);
};

#endif // CONTRACTRESULT_H
